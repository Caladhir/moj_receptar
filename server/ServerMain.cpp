#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include <IdContext.hpp>
#include <IdGlobal.hpp>
#include <IdTCPServer.hpp>
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <memory>

static const int kPort = 8080;

class TServerApp : public TObject {
 public:
  std::unique_ptr<TIdTCPServer> S;

  __fastcall TServerApp() {
    S.reset(new TIdTCPServer(nullptr));
    S->DefaultPort = kPort;
    S->OnExecute = ServerExecute;  // ← sad je metoda (closure), pa compajla
  }

  void __fastcall ServerExecute(TIdContext* AContext) {
    auto io = AContext->Connection->IOHandler;
#if defined(IndyTextEncoding_UTF8)
    io->DefStringEncoding = IndyTextEncoding_UTF8();
#endif

    UnicodeString line = Trim(io->ReadLn());

    if (line == L"GET_RECIPES") {
      io->WriteLn(L"OK");
      io->WriteLn(L"Spaghetti|Salad|Burger");
      return;
    }

    if (line.Pos(L"GET_USER_PHOTO ") == 1) {
      int uid = StrToIntDef(line.SubString(16, line.Length()), 0);

      UnicodeString base =
          IncludeTrailingPathDelimiter(ExtractFilePath(ParamStr(0))) +
          L"photos\\";
      UnicodeString fileToSend;

      for (auto ext : {L".jpg", L".jpeg", L".png", L".bmp"}) {
        UnicodeString f = base + IntToStr(uid) + ext;
        if (FileExists(f)) {
          fileToSend = f;
          break;
        }
      }

      if (fileToSend.IsEmpty()) {
        AContext->Connection->IOHandler->WriteLn(L"ERR NOFILE");
      } else {
        std::unique_ptr<TFileStream> fs(
            new TFileStream(fileToSend, fmOpenRead | fmShareDenyNone));
        AContext->Connection->IOHandler->WriteLn(L"STREAM " +
                                                 IntToStr((int)fs->Size));
        AContext->Connection->IOHandler->Write(fs.get(), (int)fs->Size);
      }
      return;
    }

    if (line == L"UPLOAD_RECIPE") {
      UnicodeString hdr = Trim(io->ReadLn());
      if (hdr.Pos(L"STREAM ") != 1) {
        io->WriteLn(L"ERR HEADER");
        return;
      }
      int len = StrToIntDef(hdr.SubString(8, hdr.Length()), 0);

      std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
      io->ReadStream(ms.get(), len, false);
      ms->Position = 0;

      UnicodeString out = ExtractFilePath(ParamStr(0)) + L"uploads\\";
      ForceDirectories(out);
      out += L"recipe_" + FormatDateTime(L"yyyymmdd_hhnnss", Now()) + L".txt";
      std::unique_ptr<TFileStream> fs(new TFileStream(out, fmCreate));
      fs->CopyFrom(ms.get(), 0);

      io->WriteLn(L"OK");
      return;
    }

    io->WriteLn(L"ERR UNKNOWN");
  }
};

int _tmain(int, _TCHAR*[]) {
  try {
    std::unique_ptr<TServerApp> app(new TServerApp());
    app->S->Active = true;

    wprintf(L"MealPlanServer running on port %d. Press ENTER to quit...\n",
            kPort);
    getchar();

    app->S->Active = false;
  } catch (const Exception& e) {
    wprintf(L"Server error: %s\n", e.Message.c_str());
    return 1;
  }
  return 0;
}

