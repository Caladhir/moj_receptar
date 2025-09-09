#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include <IdContext.hpp>
#include <IdGlobal.hpp>
#include <IdTCPServer.hpp>
#include <System.Classes.hpp>
#include <System.IOUtils.hpp>
#include <System.SysUtils.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <memory>

static const int kPort = 8080;

class TServerApp : public TObject {
 public:
  std::unique_ptr<TIdTCPServer> S;

  __fastcall TServerApp() {
    S.reset(new TIdTCPServer(nullptr));
    S->DefaultPort = kPort;
    S->OnExecute = ServerExecute;
  }

  void __fastcall ServerExecute(TIdContext* AContext) {
    auto io = AContext->Connection->IOHandler;
#if defined(IndyTextEncoding_UTF8)
    io->DefStringEncoding = IndyTextEncoding_UTF8();
#endif

    UnicodeString line = Trim(io->ReadLn());

    // 1) Broj podijeljenih stavki (uploads\shared_item_*.txt)
    if (line == L"GET_SHARED_ITEMS") {
      UnicodeString dir =
          IncludeTrailingPathDelimiter(ExtractFilePath(ParamStr(0))) +
          L"uploads\\";
      ForceDirectories(dir);

      TSearchRec sr;
      int count = 0;
      int res = FindFirst(dir + L"shared_item_*.txt", faAnyFile, sr);
      while (res == 0) {
        if ((sr.Attr & faDirectory) == 0) ++count;
        res = FindNext(sr);
      }
      FindClose(sr);

      io->WriteLn(L"OK");
      io->WriteLn(IntToStr(count));  // samo broj
      return;
    }

    // 2) Backup korisničke fotografije (BLOB → .jpg)
    if (line.Pos(L"BACKUP_USER_PHOTO ") == 1) {
      int uid = StrToIntDef(line.SubString(19, line.Length()), 0);

      UnicodeString hdr = Trim(io->ReadLn());
      if (hdr.Pos(L"STREAM ") != 1) {
        io->WriteLn(L"ERR HEADER");
        return;
      }
      int len = StrToIntDef(hdr.SubString(8, hdr.Length()), 0);
      if (len <= 0) {
        io->WriteLn(L"ERR LEN");
        return;
      }

      std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
      io->ReadStream(ms.get(), len, false);
      ms->Position = 0;

      UnicodeString photos =
          IncludeTrailingPathDelimiter(ExtractFilePath(ParamStr(0))) +
          L"photos\\";
      ForceDirectories(photos);

      try {
        std::unique_ptr<TPicture> pic(new TPicture());
        pic->LoadFromStream(
            ms.get());  // pokušaj učitati bilo koji format (jpg/png/bmp…)
        std::unique_ptr<TJPEGImage> jpg(new TJPEGImage());
        jpg->Assign(pic->Graphic);
        jpg->CompressionQuality = 90;
        jpg->SaveToFile(photos + IntToStr(uid) + L".jpg");
        io->WriteLn(L"OK SAVED");
      } catch (...) {
        // fallback – spremi sirovi stream
        std::unique_ptr<TFileStream> fs(
            new TFileStream(photos + IntToStr(uid) + L".bin", fmCreate));
        ms->Position = 0;
        fs->CopyFrom(ms.get(), 0);
        io->WriteLn(L"OK RAW");
      }
      return;
    }

    // 3) Dohvat backup fotografije (po ekstenzijama)
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
        io->WriteLn(L"ERR NOFILE");
      } else {
        std::unique_ptr<TFileStream> fs(
            new TFileStream(fileToSend, fmOpenRead | fmShareDenyNone));
        io->WriteLn(L"STREAM " + IntToStr((int)fs->Size));
        io->Write(fs.get(), (int)fs->Size);
      }
      return;
    }

    // 4) Upload (tekst kao stream) → uploads\*.txt

    if (line == L"UPLOAD_SHARED_ITEM") {
      UnicodeString hdr = Trim(io->ReadLn());
      if (hdr.Pos(L"STREAM ") != 1) {
        io->WriteLn(L"ERR HEADER");
        return;
      }
      int len = StrToIntDef(hdr.SubString(8, hdr.Length()), 0);

      std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
      if (len > 0) io->ReadStream(ms.get(), len, false);
      ms->Position = 0;

      UnicodeString out =
          IncludeTrailingPathDelimiter(ExtractFilePath(ParamStr(0))) +
          L"uploads\\";
      ForceDirectories(out);
      out +=
          L"shared_item_" + FormatDateTime(L"yyyymmdd_hhnnss", Now()) + L".txt";

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

