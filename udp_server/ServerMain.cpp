#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include <IdGlobal.hpp>
#include <IdSocketHandle.hpp>
#include <IdUDPServer.hpp>
#include <System.Classes.hpp>
#include <System.IOUtils.hpp>
#include <System.NetEncoding.hpp>
#include <System.SysUtils.hpp>
#include <cstdio>  // wprintf
#include <memory>

using namespace Idglobal;

static const int kUdpPort = 9090;
static const int kChunkChars = 900;  // sigurno ispod MTU-a

// ---------- helpers ----------
static void SendText(TIdSocketHandle* sock, const UnicodeString& host, int port,
                     const UnicodeString& text) {
  TIdBytes bytes = ToBytes(text, IndyTextEncoding_UTF8());
  sock->SendTo(host, port, bytes, 0, bytes.Length, Id_IPv4);
}

static UnicodeString ExeDir() {
  return IncludeTrailingPathDelimiter(ExtractFilePath(ParamStr(0)));
}

static UnicodeString FindPhotosPlaceholder() {
  // pokuša: .\photos\placeholder.jpg, ..\photos\..., ..\..\photos\...
  UnicodeString base = ExeDir();
  UnicodeString photo = base + L"photos\\placeholder.jpg";
  if (!FileExists(photo)) photo = base + L"..\\photos\\placeholder.jpg";
  if (!FileExists(photo)) photo = base + L"..\\..\\photos\\placeholder.jpg";
  return photo;
}

static UnicodeString UpdatesDir() {
  // Traženi apsolutni put (kako si naveo u poruci):
  UnicodeString wanted =
      L"C:\\Users\\MobiS PC\\Desktop\\SCHOOL\\4.SEMESTAR\\"
      L"NAPREDNE TEHNIKE PROGRAMIRANJA\\&PROJEKT\\MojReceptar\\"
      L"udp_server\\updates";
  try {
    if (!TDirectory::Exists(wanted)) TDirectory::CreateDirectory(wanted);
    return IncludeTrailingPathDelimiter(wanted);
  } catch (...) {
    // Fallback: .\updates pored exe-a
    UnicodeString local = ExeDir() + L"updates\\";
    if (!TDirectory::Exists(local)) TDirectory::CreateDirectory(local);
    return local;
  }
}

static void AppendUpdateLine(int itemId, const UnicodeString& ts) {
  const UnicodeString line = L"MealPlanItem br. " + IntToStr(itemId) +
                             L" izmijenjen u " + ts + L"\r\n";
  const UnicodeString path = UpdatesDir() + L"update_timestamp.txt";

  // sigurni append (stvori ako ne postoji)
  TFile::AppendAllText(path, line, TEncoding::UTF8);
}

// pošalji datoteku kao Base64 fragmente
// B64 BEGIN <total> <origLen>
// B64 i/n <payload>
// ...
// B64 END
static void SendFileAsB64InChunks(TIdSocketHandle* sock,
                                  const UnicodeString& host, int port,
                                  const UnicodeString& filePath,
                                  int chunkChars) {
  System::DynamicArray<System::Byte> raw = TFile::ReadAllBytes(filePath);
  const int rawLen = raw.Length;

  UnicodeString b64;
  if (rawLen > 0)
    b64 = System::Netencoding::TNetEncoding::Base64->EncodeBytesToString(
        &raw[0], rawLen);
  else
    b64 = L"";

  const int total = (b64.Length() + chunkChars - 1) / chunkChars;
  SendText(sock, host, port,
           L"B64 BEGIN " + IntToStr(total) + L" " + IntToStr(rawLen));

  for (int i = 0; i < total; ++i) {
    const int pos = i * chunkChars + 1;
    const int left = b64.Length() - (pos - 1);
    const int take = (left < chunkChars) ? left : chunkChars;
    const UnicodeString part = b64.SubString(pos, take);
    SendText(sock, host, port,
             L"B64 " + IntToStr(i + 1) + L"/" + IntToStr(total) + L" " + part);
  }

  SendText(sock, host, port, L"B64 END");
}

// ---------- app ----------
class TUdpServerApp : public TObject {
 public:
  std::unique_ptr<TIdUDPServer> S;

  __fastcall TUdpServerApp() {
    S.reset(new TIdUDPServer(nullptr));
    S->Bindings->Clear();
    auto b = S->Bindings->Add();
    b->IP = L"0.0.0.0";
    b->Port = kUdpPort;
    b->IPVersion = Id_IPv4;

    S->DefaultPort = kUdpPort;
    S->ThreadedEvent = true;
    S->OnUDPRead = OnRead;
  }

  // void __fastcall (TIdUDPListenerThread* AThread, const TIdBytes AData,
  // TIdSocketHandle* ABinding)
  void __fastcall OnRead(TIdUDPListenerThread* AThread, const TIdBytes AData,
                         TIdSocketHandle* ABinding) {
    try {
      UnicodeString msg = BytesToString(AData, IndyTextEncoding_UTF8());
      msg = Trim(msg);
      wprintf(L"[RX %s:%d] %s\n", ABinding->PeerIP.c_str(), ABinding->PeerPort,
              msg.c_str());

      // 2) UPDATE_ITEM <id>|<timestamp> → upiši u updates\update_timestamp.txt,
      // odgovori ACK
      if (msg.Pos(L"UPDATE_ITEM ") == 1) {
        // parsiranje: nakon razmaka ide "id|ts"
        UnicodeString rest = msg.SubString(12, msg.Length() - 11);
        int barPos = rest.Pos(L"|");
        if (barPos > 1) {
          const int itemId = StrToIntDef(rest.SubString(1, barPos - 1), 0);
          const UnicodeString ts =
              rest.SubString(barPos + 1, rest.Length() - barPos);
          if (itemId > 0 && !ts.IsEmpty()) {
            AppendUpdateLine(itemId, ts);
            SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort, L"ACK");
            return;
          }
        }
        SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort,
                 L"ERR BAD UPDATE_ITEM");
        return;
      }

      // 3) GET_THUMB <id> → pošalji placeholder.jpg kao B64 fragmente
      if (msg.Pos(L"GET_THUMB ") == 1) {
        UnicodeString photo = FindPhotosPlaceholder();
        if (!FileExists(photo)) {
          SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort,
                   L"ERR NO PLACEHOLDER");
          return;
        }
        SendFileAsB64InChunks(ABinding, ABinding->PeerIP, ABinding->PeerPort,
                              photo, kChunkChars);
        return;
      }

      // nepoznato
      SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort, L"ERR UNKNOWN");
    } catch (const Exception& e) {
      wprintf(L"[ERR] %s\n", e.Message.c_str());
    }
  }
};

int _tmain(int, _TCHAR*[]) {
  try {
    std::unique_ptr<TUdpServerApp> app(new TUdpServerApp());
    app->S->Active = true;

    wprintf(L"MealPlanNotifier (UDP %d)\n", kUdpPort);
    wprintf(L"Updates dir: %s\n", UpdatesDir().c_str());
    wprintf(L"Photos placeholder: %s\n", FindPhotosPlaceholder().c_str());
    wprintf(L"Waiting for datagrams ... Press ENTER to quit.\n");

    getchar();
    app->S->Active = false;
  } catch (const Exception& e) {
    wprintf(L"UDP server error: %s\n", e.Message.c_str());
    return 1;
  }
  return 0;
}

