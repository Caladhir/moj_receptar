#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include <IdGlobal.hpp>
#include <IdSocketHandle.hpp>
#include <IdUDPServer.hpp>
#include <System.Classes.hpp>
#include <System.NetEncoding.hpp>
#include <System.SysUtils.hpp>
#include <Winapi.Windows.hpp>
#include <memory>

using namespace Idglobal;

static const int kUdpPort = 9090;

// helper: pošalji UTF-8 tekst
static void SendText(TIdSocketHandle* sock, const UnicodeString& host, int port,
                     const UnicodeString& text) {
  TIdBytes bytes = ToBytes(text, IndyTextEncoding_UTF8());
  // SendTo(bytes) overload: (Host, Port, Buffer, Offset, Count, IPVersion)
  sock->SendTo(host, port, bytes, 0, bytes.Length, Id_IPv4);
}

class TUdpServerApp : public TObject {
 public:
  std::unique_ptr<TIdUDPServer> S;

  __fastcall TUdpServerApp() {
    S.reset(new TIdUDPServer(nullptr));

    // Bind na IPv4 (0.0.0.0 = sve adrese) i naš port
    S->Bindings->Clear();
    auto b = S->Bindings->Add();
    b->IP = L"0.0.0.0";
    b->Port = kUdpPort;
    b->IPVersion = Id_IPv4;

    S->DefaultPort = kUdpPort;
    S->OnUDPRead = OnRead;    // handler prije Activiranja
    S->ThreadedEvent = true;  // sigurnije za dulje obrade
  }

  void __fastcall OnRead(TIdUDPListenerThread* /*AThread*/,
                         const TIdBytes AData, TIdSocketHandle* ABinding) {
    try {
      UnicodeString msg = BytesToString(AData, IndyTextEncoding_UTF8());
      msg = Trim(msg);

      // Konzolni log
      wprintf(L"[RX %s:%d] %s\n", ABinding->PeerIP.c_str(), ABinding->PeerPort,
              msg.c_str());

      // 1) jednostavno – PING/PONG
      if (msg == L"PING") {
        SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort, L"PONG");
        return;
      }

      // 2) jednostavno – ACK za promjenu plana
      if (msg.Pos(L"PLAN_UPDATED|") == 1) {
        SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort, L"ACK");
        return;
      }

      // 3) „stream” – thumbnail u Base64
      if (msg.Pos(L"GET_THUMB ") == 1) {
        int uid = StrToIntDef(msg.SubString(11, msg.Length()), 0);

        UnicodeString fn = ExtractFilePath(ParamStr(0)) + L"photos\\" +
                           IntToStr(uid) + L".jpg";
        if (!FileExists(fn)) {
          SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort,
                   L"ERR NOFILE");
          return;
        }

        std::unique_ptr<TFileStream> fs(
            new TFileStream(fn, fmOpenRead | fmShareDenyNone));

        System::Sysutils::TBytes buf;
        buf.Length = (int)fs->Size;
        if (buf.Length) fs->ReadBuffer(&buf[0], buf.Length);

        // C++ Builder: koristimo TNetEncoding bazu – NE stavljati namespace
        // System::
        UnicodeString b64 =
            TNetEncoding::Base64->EncodeBytesToString(&buf[0], buf.Length);

        const int CHUNK = 1000;
        int total = (b64.Length() + CHUNK - 1) / CHUNK;

        SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort,
                 L"B64 BEGIN " + IntToStr(total));

        for (int i = 0; i < total; ++i) {
          int start = i * CHUNK + 1;
          int remain = b64.Length() - (start - 1);
          int take = (remain > CHUNK) ? CHUNK : remain;

          UnicodeString part = b64.SubString(start, take);
          UnicodeString line =
              L"B64 " + IntToStr(i + 1) + L"/" + IntToStr(total) + L" " + part;

          SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort, line);
          ::Sleep(1);  // mali razmak
        }

        SendText(ABinding, ABinding->PeerIP, ABinding->PeerPort, L"B64 END");
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

    app->S->Active = true;  // aktiviramo tek sada

    wprintf(L"MealPlanNotifier (UDP %d). Photos dir: %sphotos\\\n", kUdpPort,
            ExtractFilePath(ParamStr(0)).c_str());
    wprintf(L"Waiting for datagrams ... Press ENTER to quit.\n");

    getchar();  // drži server živim
    app->S->Active = false;
  } catch (const Exception& e) {
    wprintf(L"UDP server error: %s\n", e.Message.c_str());
    return 1;
  }
  return 0;
}

