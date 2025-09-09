#include <vcl.h>
#pragma hdrstop
#include "MealPlansForm.h"

#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <IdCoder.hpp>
#include <IdCoderMIME.hpp>
#include <IdGlobal.hpp>
#include <IdTCPClient.hpp>
#include <System.Classes.hpp>
#include <System.NetEncoding.hpp>
#include <System.StrUtils.hpp>
#include <System.SyncObjs.hpp>
#include <System.SysUtils.hpp>
#include <System.Threading.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Winapi.ActiveX.hpp>
#include <Winapi.ShellAPI.hpp>
#include <Winapi.Windows.hpp>
#include <map>
#include <memory>
#include <vector>

#include "AppDataModule.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

static void UdpSendUtf8(TIdUDPClient *cli, const UnicodeString &s) {
  Idglobal::TIdBytes b = Idglobal::ToBytes(s, IndyTextEncoding_UTF8());
  cli->SendBuffer(b);
}

bool __fastcall TFMealPlans::TryBeginParallelLoad() {
  return TInterlocked::CompareExchange(FParallelFlag, 1, 0) == 0;
}
void __fastcall TFMealPlans::EndParallelLoad() {
  TInterlocked::Exchange(FParallelFlag, 0);
}

TFMealPlans *FMealPlans = nullptr;

void __fastcall TFMealPlans::FormShow(TObject *Sender) {
  // kad promijenimo korisnika: filtriraj planove + pokaži sliku
  AppDataModule1->DSUsers->OnDataChange = DSUsersDataChange;
  DSUsersDataChange(nullptr, nullptr);

  IdUDPClient1->Host = L"127.0.0.1";
  IdUDPClient1->Port = 9090;
  IdUDPClient1->IPVersion = Id_IPv4;
  IdUDPClient1->ReceiveTimeout = 2000;

  if (AppDataModule1->TPlanItems)
    AppDataModule1->TPlanItems->AfterPost = PlanItemsAfterPost;
}

//
// ============ USERS ============
//
void __fastcall TFMealPlans::BtnFilterUserClick(TObject *) {
  AppDataModule1->FilterUsersByName(EdUserFilter->Text);
}
void __fastcall TFMealPlans::BtnClearFiltersClick(TObject *) {
  EdUserFilter->Clear();
  AppDataModule1->ClearAllFilters();
  ShowCurrentUserPhoto();
}

void __fastcall TFMealPlans::BtnLoadPhotoClick(TObject *) {
  auto ds = AppDataModule1->TUsers;
  if (!ds || ds->IsEmpty()) return;
  if (OpenDialog1->Execute()) {
    int uid = ds->FieldByName(L"UserID")->AsInteger;
    if (AppDataModule1->LoadUserPhotoFromFile(uid, OpenDialog1->FileName))
      ShowCurrentUserPhoto();
  }
}
void __fastcall TFMealPlans::BtnDeletePhotoClick(TObject *) {
  auto ds = AppDataModule1->TUsers;
  if (!ds || ds->IsEmpty()) return;
  if (auto *bf = dynamic_cast<TBlobField *>(ds->FindField(L"Photo"))) {
    ds->Edit();
    bf->Clear();
    ds->Post();
    ShowCurrentUserPhoto();
  }
}

void __fastcall TFMealPlans::DSUsersDataChange(TObject *, TField *) {
  // ako je neka stavka “napola” i prazna – samo je otkaži
  auto items = AppDataModule1->TPlanItems;
  if (items && (items->State == dsInsert || items->State == dsEdit)) {
    if (Trim(items->FieldByName(L"RecipeName")->AsString).IsEmpty())
      items->Cancel();
    else
      items->Post();
  }

  // filtriraj planove od odabranog korisnika
  auto ds = AppDataModule1->DSUsers->DataSet;
  if (ds && !ds->IsEmpty())
    AppDataModule1->FilterMealPlansByUserID(
        ds->FieldByName(L"UserID")->AsInteger);
  else
    AppDataModule1->TMealPlans->Filtered = false;

  ShowCurrentUserPhoto();
}

void __fastcall TFMealPlans::ShowCurrentUserPhoto() {
  ImgPhoto->Picture = nullptr;

  auto ds = AppDataModule1->TUsers;
  if (!ds || ds->IsEmpty()) return;

  auto *bf = dynamic_cast<TBlobField *>(ds->FindField(L"Photo"));
  if (!bf || bf->IsNull) {
    // Nema slike u bazi → povuci UDP placeholder (bez spamiranja mema)
    const int uid = ds->FieldByName(L"UserID")->AsInteger;
    FetchUdpThumb(uid, /*logToMemo*/ true);
    return;
  }

  std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
  bf->SaveToStream(ms.get());
  ms->Position = 0;
  try {
    ImgPhoto->Picture->LoadFromStream(ms.get());
  } catch (...) {
    ms->Position = 0;
    std::unique_ptr<Graphics::TBitmap> bmp(new Graphics::TBitmap());
    bmp->LoadFromStream(ms.get());
    ImgPhoto->Picture->Assign(bmp.get());
  }
}

//
// ============ MEAL PLANS GRID ============
//
void __fastcall TFMealPlans::GridPlansTitleClick(TColumn *Column) {
  if (Column && Column->Field)
    AppDataModule1->SortMealPlans(Column->Field->FieldName);
}

void __fastcall TFMealPlans::GridItemsExit(TObject *) {
  auto ds = AppDataModule1->TPlanItems;
  if (!ds) return;
  if (ds->State == dsEdit || ds->State == dsInsert) {
    if (Trim(ds->FieldByName(L"RecipeName")->AsString).IsEmpty())
      ds->Cancel();
    else
      ds->Post();
  }
}

// Pomoćno: sigurno osvježi datasets (na UI niti)
void __fastcall TFMealPlans::SafeRefreshDatasets() {
  // MealPlans
  try {
    if (AppDataModule1->TMealPlans && AppDataModule1->TMealPlans->Active) {
      try {
        AppDataModule1->TMealPlans->Requery();
      } catch (...) {
        AppDataModule1->TMealPlans->Close();
        AppDataModule1->TMealPlans->Open();
      }
    }
  } catch (...) {
  }

  // PlanItems
  try {
    if (AppDataModule1->TPlanItems && AppDataModule1->TPlanItems->Active) {
      try {
        AppDataModule1->TPlanItems->Requery();
      } catch (...) {
        AppDataModule1->TPlanItems->Close();
        AppDataModule1->TPlanItems->Open();
      }
    }
  } catch (...) {
  }

  // Users
  try {
    if (AppDataModule1->TUsers && AppDataModule1->TUsers->Active) {
      try {
        AppDataModule1->TUsers->Requery();
      } catch (...) {
        AppDataModule1->TUsers->Close();
        AppDataModule1->TUsers->Open();
      }
    }
  } catch (...) {
  }
}

//
void __fastcall TFMealPlans::GridItemsColEnter(TObject *) {
  auto items = AppDataModule1->TPlanItems;
  if (items && items->Active && items->State == dsBrowse) items->Edit();
}

void __fastcall TFMealPlans::ShowFinalSummary(
    int usersCnt, int plansCnt, int itemsCnt,
    const UnicodeString &whenSummary) {
  UnicodeString s = L"Korisnika: " + IntToStr(usersCnt) + L"  |  Planova: " +
                    IntToStr(plansCnt) + L"  |  Stavki: " + IntToStr(itemsCnt);
  if (!whenSummary.IsEmpty()) s += L"  |  Ukupno obroka: " + whenSummary;
  LblReporting->Caption = s;
}

// Worker konekcija (svaka dretva ima svoju)
static TADOConnection *MakeWorkerConn(const UnicodeString &baseConnStr) {
  TADOConnection *c = new TADOConnection(nullptr);
  c->LoginPrompt = false;
  c->ConnectionString = baseConnStr;
  c->Open();
  return c;
}

struct TSharedState {
  int usersCnt{0};
  int plansCnt{0};
  int itemsCnt{0};
  std::map<UnicodeString, int> whenMap;
  TCriticalSection *cs{nullptr};

  TSharedState() : cs(new TCriticalSection()) {}
  ~TSharedState() { delete cs; }
};

// ============ 11. ZADATAK: 7 paralelnih stvari (3 COUNT + 4 WhenInDay
// writer-a) ============
void __fastcall TFMealPlans::BtnParallelLoadClick(TObject *) {
  if (!TryBeginParallelLoad()) {
    Application->MessageBox(L"Učitavanje je već u tijeku…", L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }

  LblReporting->Caption = L"Učitavam paralelno…";

  auto state = new TSharedState();
  const UnicodeString baseConnStr = AppDataModule1->Conn->ConnectionString;

  // A) Users COUNT
  _di_ITask tA = TTask::Create([=]() {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    try {
      std::unique_ptr<TADOConnection> c(MakeWorkerConn(baseConnStr));
      std::unique_ptr<TADOQuery> q(new TADOQuery(nullptr));
      q->Connection = c.get();
      q->SQL->Text = L"SELECT COUNT(*) AS C FROM Users";
      q->Open();
      int v = q->FieldByName(L"C")->AsInteger;
      state->cs->Enter();
      state->usersCnt = v;
      state->cs->Leave();
    } catch (...) {
      state->cs->Enter();
      state->usersCnt = 0;
      state->cs->Leave();
    }
    CoUninitialize();
  });

  // B) MealPlans COUNT
  _di_ITask tB = TTask::Create([=]() {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    try {
      std::unique_ptr<TADOConnection> c(MakeWorkerConn(baseConnStr));
      std::unique_ptr<TADOQuery> q(new TADOQuery(nullptr));
      q->Connection = c.get();
      q->SQL->Text = L"SELECT COUNT(*) AS C FROM MealPlans";
      q->Open();
      int v = q->FieldByName(L"C")->AsInteger;
      state->cs->Enter();
      state->plansCnt = v;
      state->cs->Leave();
    } catch (...) {
      state->cs->Enter();
      state->plansCnt = 0;
      state->cs->Leave();
    }
    CoUninitialize();
  });

  // C) PlanItems COUNT
  _di_ITask tC = TTask::Create([=]() {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    try {
      std::unique_ptr<TADOConnection> c(MakeWorkerConn(baseConnStr));
      std::unique_ptr<TADOQuery> q(new TADOQuery(nullptr));
      q->Connection = c.get();
      q->SQL->Text = L"SELECT COUNT(*) AS C FROM PlanItems";
      q->Open();
      int v = q->FieldByName(L"C")->AsInteger;
      state->cs->Enter();
      state->itemsCnt = v;
      state->cs->Leave();
    } catch (...) {
      state->cs->Enter();
      state->itemsCnt = 0;
      state->cs->Leave();
    }
    CoUninitialize();
  });

  // D) 4 paralelna writer-a: Breakfast/Lunch/Snack/Dinner
  const std::vector<UnicodeString> whens = {L"Breakfast", L"Lunch", L"Snack",
                                            L"Dinner"};
  std::vector<_di_ITask> tWhen;
  tWhen.reserve(whens.size());

  for (const auto &w : whens) {
    const UnicodeString wcopy = w;  // eksplicitna kopija
    tWhen.push_back(
        TTask::Create([=]() {  // hvata sve po kopiji, uključujući wcopy
          CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
          try {
            std::unique_ptr<TADOConnection> c(MakeWorkerConn(baseConnStr));
            std::unique_ptr<TADOQuery> q(new TADOQuery(nullptr));
            q->Connection = c.get();

            q->SQL->Text =
                L"SELECT COUNT(*) AS C FROM PlanItems WHERE WhenInDay LIKE :W";
            TParameter *p = q->Parameters->ParamByName(L"W");
            p->DataType = ftWideString;
            p->Size = 32;
            p->Value = Trim(wcopy);  // koristi wcopy umjesto w

            q->Open();
            const int cnt = q->FieldByName(L"C")->AsInteger;

            state->cs->Enter();
            state->whenMap[UpperCase(wcopy)] = cnt;  // wcopy
            state->cs->Leave();
          } catch (...) {
            state->cs->Enter();
            state->whenMap[UpperCase(wcopy)] = 0;  // wcopy
            state->cs->Leave();
          }
          CoUninitialize();
        }));
  }

  // Start svih poslova
  tA->Start();
  tB->Start();
  tC->Start();
  for (auto &t : tWhen) t->Start();

  // "Join" + UI update + čišćenje
  _di_ITask fin = TTask::Create([=]() {
    tA->Wait();
    tB->Wait();
    tC->Wait();
    for (auto &t : tWhen) t->Wait();

    // kopija mape bez locka
    std::map<UnicodeString, int> copy;
    state->cs->Enter();
    copy = state->whenMap;
    state->cs->Leave();

    UnicodeString whenSummary;
    for (auto &kv : copy) {
      if (!whenSummary.IsEmpty()) whenSummary += L", ";
      whenSummary += kv.first + L":" + IntToStr(kv.second);
    }

    TThread::Queue(nullptr, [=]() {
      SafeRefreshDatasets();
      ShowFinalSummary(state->usersCnt, state->plansCnt, state->itemsCnt,
                       whenSummary);
      EndParallelLoad();
    });

    delete state;  // oslobodi sve (i kritičnu sekciju)
  });
  fin->Start();
}

int __fastcall TFMealPlans::LaunchPDFViewer(const UnicodeString &pdfPath) {
  if (!FileExists(pdfPath)) return ERROR_FILE_NOT_FOUND;

  SHELLEXECUTEINFO sei = {};
  sei.cbSize = sizeof(sei);
  sei.fMask = SEE_MASK_NOCLOSEPROCESS;  // želimo HANDLE procesa
  sei.lpVerb = L"open";                 // “otvori” default viewerom
  sei.lpFile = pdfPath.c_str();
  sei.nShow = SW_SHOWNORMAL;

  if (!ShellExecuteEx(&sei)) {
    return GetLastError();  // Windows error code kad se nije ni pokrenuo
  }

  // Pričekaj završetak procesa B
  WaitForSingleObject(sei.hProcess, INFINITE);

  DWORD exitCode = 0;
  GetExitCodeProcess(sei.hProcess, &exitCode);
  CloseHandle(sei.hProcess);

  return static_cast<int>(exitCode);  // 0 = OK, ≠0 greška
}

//
// ============ REPORT ============
//
void __fastcall TFMealPlans::BtnReportClick(TObject *) {
  auto users = AppDataModule1->TUsers;
  auto plans = AppDataModule1->TMealPlans;

  if (!users || users->IsEmpty()) {
    Application->MessageBox(L"Izaberite korisnika.", L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }
  if (!plans || plans->IsEmpty()) {
    Application->MessageBox(L"Nema planova za odabranog korisnika.", L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }

  // osiguraj da su planovi filtrirani po odabranom korisniku
  const int uid = users->FieldByName(L"UserID")->AsInteger;
  AppDataModule1->FilterMealPlansByUserID(uid);

  if (!AppDataModule1->ReportEvent ||
      AppDataModule1->ReportEvent->WaitFor(0) != wrSignaled) {
    Application->MessageBox(L"Izvještaj se već generira…", L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }
  AppDataModule1->ReportEvent->ResetEvent();
  AppDataModule1->ReportInProgress = true;

  try {
    // Range-ovi
    frxDBUsers->RangeBegin = rbCurrent;
    frxDBUsers->RangeEnd = reCurrent;
    frxDBMealPlans->RangeBegin = rbFirst;
    frxDBMealPlans->RangeEnd = reLast;
    frxDBPlanItems->RangeBegin = rbFirst;
    frxDBPlanItems->RangeEnd = reLast;

    if (frxReport1->PrepareReport(true)) {
      frxReport1->ShowPreparedReport();
      if (SaveDialog1->Execute()) {
        std::unique_ptr<TfrxPDFExport> pdf(new TfrxPDFExport(nullptr));
        pdf->ShowProgress = false;
        pdf->ShowDialog = false;
        pdf->FileName = SaveDialog1->FileName;
        frxReport1->Export(pdf.get());

        // === 14. zadatak: proces B (PDF viewer) ===
        const int rc = LaunchPDFViewer(SaveDialog1->FileName);
        if (rc != 0) {
          UnicodeString msg =
              L"Greška pri otvaranju PDF-a.\nExit code: " + IntToStr(rc);
          Application->MessageBox(msg.c_str(), L"Greška procesa",
                                  MB_OK | MB_ICONSTOP);
        }
      }
    }
  } __finally {
    AppDataModule1->ReportInProgress = false;
    if (AppDataModule1->ReportEvent) AppDataModule1->ReportEvent->SetEvent();
  }
}

// poveži se na server
void __fastcall TFMealPlans::TcpConnect() {
  if (!IdTCPClient1->Connected()) {
    try {
      IdTCPClient1->ConnectTimeout = 3000;
      IdTCPClient1->ReadTimeout = 5000;
      IdTCPClient1->Connect();
      IdTCPClient1->IOHandler->DefStringEncoding = IndyTextEncoding_UTF8();
    } catch (...) {
      ShowMessage(
          L"Ne mogu se spojiti na TCP server (127.0.0.1:8080). "
          L"Pokreni server pa pokušaj ponovno.");
      throw;
    }
  }
}

void __fastcall TFMealPlans::TcpGetRecipes() {
  TcpConnect();
  auto io = IdTCPClient1->IOHandler;
  io->WriteLn(L"GET_SHARED_ITEMS");

  UnicodeString status = Trim(io->ReadLn());
  if (status == L"OK") {
    UnicodeString countLine = Trim(io->ReadLn());
    int count = StrToIntDef(countLine, 0);
    ShowMessage(L"Broj podijeljenih stavki: " + IntToStr(count));
  } else {
    ShowMessage(L"Greška: " + status);
  }
}

// GET_USER_PHOTO <id>
void __fastcall TFMealPlans::TcpGetUserPhoto(int userId) {
  TcpConnect();
  auto io = IdTCPClient1->IOHandler;
  io->WriteLn(L"GET_USER_PHOTO " + IntToStr(userId));

  UnicodeString hdr = Trim(io->ReadLn());
  if (hdr.Pos(L"STREAM ") == 1) {
    const int len = StrToIntDef(hdr.SubString(8, hdr.Length()), 0);

    std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
    io->ReadStream(ms.get(), len, false);
    ms->Position = 0;

    // 1) Prikaži u ImgPhoto
    try {
      ImgPhoto->Picture->LoadFromStream(ms.get());
    } catch (...) {
      ms->Position = 0;
      std::unique_ptr<Graphics::TBitmap> bmp(new Graphics::TBitmap());
      bmp->LoadFromStream(ms.get());
      ImgPhoto->Picture->Assign(bmp.get());
    }

    // 2) Zapiši u bazu (Users.Photo)
    auto users = AppDataModule1->TUsers;
    if (users && !users->IsEmpty()) {
      auto *bf = dynamic_cast<TBlobField *>(users->FindField(L"Photo"));
      if (bf) {
        users->Edit();
        ms->Position = 0;
        bf->LoadFromStream(ms.get());
        users->Post();
      }
    }

    // 3) Potvrda korisniku i log u memo
    ShowMessage(L"Oporavak slike uspješan. Primljeno " + IntToStr(len) +
                L" B.");
    if (MemoUDP)
      MemoUDP->Lines->Add(L"TCP RESTORE uid=" + IntToStr(userId) + L" ← " +
                          IntToStr(len) + L" B");

  } else {
    // Greška (npr. ERR NOFILE)
    ShowMessage(L"Greška pri oporavku: " + hdr);
    if (MemoUDP)
      MemoUDP->Lines->Add(L"TCP RESTORE ERROR uid=" + IntToStr(userId) +
                          L" ← " + hdr);
  }
}

// UPLOAD_RECIPE (pošalji mali tekst kao stream)
void __fastcall TFMealPlans::TcpUploadRecipe(const UnicodeString &,
                                             const UnicodeString &) {
  // 1) Uhvati trenutno odabranu stavku
  auto items = AppDataModule1->TPlanItems;
  if (!items || items->IsEmpty()) {
    ShowMessage(L"Nema odabrane stavke za dijeljenje.");
    return;
  }
  if (items->State == dsEdit || items->State == dsInsert) {
    if (Trim(items->FieldByName(L"RecipeName")->AsString).IsEmpty())
      items->Cancel();
    else
      items->Post();
  }

  const int itemId = items->FieldByName(L"ItemID")->AsInteger;
  const UnicodeString recipe =
      Trim(items->FieldByName(L"RecipeName")->AsString);
  const UnicodeString when = Trim(items->FieldByName(L"WhenInDay")->AsString);
  const int servings = items->FieldByName(L"Servings")->AsInteger;
  const UnicodeString comment = Trim(items->FieldByName(L"Comment")->AsString);
  UnicodeString planName;
  if (items->FindField(L"PlanNameLkp"))
    planName = Trim(items->FieldByName(L"PlanNameLkp")->AsString);

  // 2) Sastavi payload (jedna linija teksta)
  UnicodeString payload = L"ItemID=" + IntToStr(itemId) + L"|Plan=" + planName +
                          L"|Recipe=" + recipe + L"|When=" + when +
                          L"|Servings=" + IntToStr(servings) + L"|Comment=" +
                          comment;

  Idglobal::TIdBytes bytes =
      Idglobal::ToBytes(payload, IndyTextEncoding_UTF8());

  // 3) Pošalji
  TcpConnect();
  auto io = IdTCPClient1->IOHandler;
  io->WriteLn(L"UPLOAD_SHARED_ITEM");
  io->WriteLn(L"STREAM " + IntToStr(bytes.Length));
  if (bytes.Length > 0) io->Write(bytes);

  UnicodeString resp = Trim(io->ReadLn());  // očekujemo "OK"
  ShowMessage(L"Server: " + resp);
}

void __fastcall TFMealPlans::BtnGetRecipesTCPClick(TObject *) {
  TcpConnect();
  TcpGetRecipes();
}

//
void __fastcall TFMealPlans::BtnBackupPhotoTCPClick(TObject *) {
  auto ds = AppDataModule1->TUsers;
  if (!ds || ds->IsEmpty()) {
    ShowMessage(L"Izaberite korisnika.");
    return;
  }
  const int uid = ds->FieldByName(L"UserID")->AsInteger;

  auto *bf = dynamic_cast<TBlobField *>(ds->FindField(L"Photo"));
  if (!bf || bf->IsNull) {
    ShowMessage(L"Nema slike u bazi za backup.");
    return;
  }

  // pripremi payload (JPG ako možemo, inače RAW)
  std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
  bf->SaveToStream(ms.get());
  ms->Position = 0;

  std::unique_ptr<TMemoryStream> payload(new TMemoryStream());
  try {
    std::unique_ptr<TPicture> pic(new TPicture());
    pic->LoadFromStream(ms.get());
    std::unique_ptr<Vcl::Imaging::Jpeg::TJPEGImage> jpg(
        new Vcl::Imaging::Jpeg::TJPEGImage());
    jpg->Assign(pic->Graphic);
    jpg->CompressionQuality = 90;
    jpg->SaveToStream(payload.get());
  } catch (...) {
    ms->Position = 0;
    payload->CopyFrom(ms.get(), 0);  // RAW fallback
  }
  payload->Position = 0;

  // TCP zahtjev
  TcpConnect();
  TIdIOHandler *io = IdTCPClient1->IOHandler;  // <-- deklaracija 'io'
  io->WriteLn(L"BACKUP_USER_PHOTO " + IntToStr(uid));
  io->WriteLn(L"STREAM " + IntToStr((int)payload->Size));
  if (payload->Size) io->Write(payload.get(), (int)payload->Size, false);

  // odgovor servera
  UnicodeString resp = Trim(io->ReadLn());  // <-- samo JEDNOM definiraj 'resp'
  ShowMessage(L"Backup završio: " +
              resp);  // npr. “OK SAVED” / “OK RAW” / “ERR …”
  if (MemoUDP)
    MemoUDP->Lines->Add(L"TCP BACKUP uid=" + IntToStr(uid) + L" → " + resp);
}

void __fastcall TFMealPlans::BtnGetPhotoTCPClick(TObject *) {
  auto ds = AppDataModule1->TUsers;
  if (!ds || ds->IsEmpty()) {
    Application->MessageBox(L"Izaberite korisnika.", L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }
  const int uid = ds->FieldByName(L"UserID")->AsInteger;
  TcpConnect();
  TcpGetUserPhoto(uid);
}

void __fastcall TFMealPlans::BtnUploadRecipeTCPClick(TObject *) {
  UnicodeString name = L"Demo recept";
  UnicodeString cat = L"General";
  if (AppDataModule1->TMealPlans && !AppDataModule1->TMealPlans->IsEmpty())
    name = AppDataModule1->TMealPlans->FieldByName(L"PlanName")->AsString;

  TcpConnect();
  TcpUploadRecipe(name, cat);
}

// ============ UDP – jednostavne poruke ============
void __fastcall TFMealPlans::PlanItemsAfterPost(TDataSet *ds) {
  try {
    const int itemId = ds->FieldByName(L"ItemID")->AsInteger;
    const UnicodeString ts = FormatDateTime(L"yyyy-mm-dd hh:nn:ss", Now());
    const UnicodeString human =
        L"MealPlanItem br. " + IntToStr(itemId) + L" izmijenjen u " + ts;

    // 1) Pošalji kratku UDP poruku (jedan datagram, bez fragmentacije)
    const UnicodeString wire = L"UPDATE_ITEM " + IntToStr(itemId) + L"|" + ts;
    UdpSendUtf8(IdUDPClient1, wire);

    // 2) Prikaži lokalno u memo
    if (MemoUDP) {
      MemoUDP->Lines->Add(L"UDP→ " + wire);
      // pročitaj (neobavezno) ACK od servera
      UnicodeString ack = IdUDPClient1->ReceiveString();
      if (!ack.IsEmpty()) MemoUDP->Lines->Add(L"UDP← " + ack);
    }
  } catch (...) {
    if (MemoUDP) MemoUDP->Lines->Add(L"(greška pri slanju UPDATE_ITEM)");
  }
}

// THUMBNAIL
void __fastcall TFMealPlans::FetchUdpThumb(int uid, bool logToMemo) {
  try {
    UdpSendUtf8(IdUDPClient1, L"GET_THUMB " + IntToStr(uid));
    if (logToMemo) MemoUDP->Lines->Add(L"UDP→ GET_THUMB " + IntToStr(uid));

    UnicodeString first = IdUDPClient1->ReceiveString();
    if (first.IsEmpty()) {
      if (logToMemo) MemoUDP->Lines->Add(L"UDP timeout (BEGIN).");
      return;
    }
    if (first.Pos(L"ERR") == 1) {
      if (logToMemo) MemoUDP->Lines->Add(first);
      return;
    }
    if (first.Pos(L"B64 BEGIN ") != 1) {
      if (logToMemo) MemoUDP->Lines->Add(L"Neočekivan odgovor: " + first);
      return;
    }

    int total = 0, origLen = 0;
    {
      int p1 = first.Pos(L" ");
      if (p1 <= 0) return;
      UnicodeString afterB64 = first.SubString(p1 + 1, first.Length() - p1);
      int p2 = afterB64.Pos(L" ");
      if (p2 <= 0) return;
      UnicodeString afterBegin =
          afterB64.SubString(p2 + 1, afterB64.Length() - p2);
      int p3 = afterBegin.Pos(L" ");
      if (p3 > 0) {
        total = StrToIntDef(afterBegin.SubString(1, p3 - 1), 0);
        origLen = StrToIntDef(
            afterBegin.SubString(p3 + 1, afterBegin.Length() - p3), 0);
      } else {
        total = StrToIntDef(afterBegin, 0);
      }
    }
    if (total <= 0) return;
    if (logToMemo)
      MemoUDP->Lines->Add(L"BEGIN: očekujem " + IntToStr(total) +
                          L" fragmenata…");

    std::vector<UnicodeString> parts(total + 1);
    int received = 0;
    while (received < total) {
      UnicodeString line = IdUDPClient1->ReceiveString();
      if (line.IsEmpty()) {
        if (logToMemo) MemoUDP->Lines->Add(L"Fragment timeout.");
        return;
      }
      if (line.Pos(L"B64 ") != 1) {
        if (logToMemo) MemoUDP->Lines->Add(L"Krivi fragment: " + line);
        return;
      }

      int sp1 = line.Pos(L" ");
      if (sp1 <= 0) return;
      UnicodeString rest =
          line.SubString(sp1 + 1, line.Length() - sp1);  // "i/n <payload>"
      int sp2 = rest.Pos(L" ");
      if (sp2 <= 0) return;

      UnicodeString idxTok = rest.SubString(1, sp2 - 1);  // "i/n"
      int slash = idxTok.Pos(L"/");
      int idx = StrToIntDef(idxTok.SubString(1, slash - 1), 0);
      UnicodeString payload = rest.SubString(sp2 + 1, rest.Length() - sp2);

      if (idx >= 1 && idx <= total && parts[idx].IsEmpty()) {
        parts[idx] = payload;
        ++received;
        if (logToMemo && ((received % 10) == 0 || received == total))
          MemoUDP->Lines->Add(L"Primljeno " + IntToStr(received) + L"/" +
                              IntToStr(total));
      }
    }

    UnicodeString ending = IdUDPClient1->ReceiveString();
    if (logToMemo && ending != L"B64 END")
      MemoUDP->Lines->Add(L"Upozorenje: kraj nije stigao: " + ending);

    UnicodeString all;
    for (int i = 1; i <= total; ++i) all += parts[i];

    System::Sysutils::TBytes raw =
        TNetEncoding::Base64->DecodeStringToBytes(all);

    // ubacivanje u TIMAGE
    std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
    if (raw.Length) ms->WriteBuffer(&raw[0], raw.Length);
    ms->Position = 0;
    try {
      ImgPhoto->Picture->LoadFromStream(ms.get());
    } catch (...) {
      ms->Position = 0;
      try {
        std::unique_ptr<Vcl::Imaging::Pngimage::TPngImage> png(
            new Vcl::Imaging::Pngimage::TPngImage());
        png->LoadFromStream(ms.get());
        ImgPhoto->Picture->Assign(png.get());
      } catch (...) {
        if (logToMemo)
          MemoUDP->Lines->Add(L"Primljeni podaci nisu valjana slika.");
      }
    }
  } catch (...) {
    if (logToMemo) MemoUDP->Lines->Add(L"Greška pri dohvaćanju thumbnaila.");
  }
}

//
// ============ ADD ITEM BUTTON ============
//
// void __fastcall TFMealPlans::BtnAddItemClick(TObject *) {
//  auto plans = AppDataModule1->TMealPlans;
//  if (plans && (plans->State == dsInsert || plans->State == dsEdit))
//    plans->Post();  // osiguraj PlanID
//
//  auto items = AppDataModule1->TPlanItems;
//  if (!items) return;
//
//  // ne pokušavaj Post/Cancel u browse stanju
//  if (items->State == dsInsert || items->State == dsEdit) {
//    const bool empty =
//        Trim(items->FieldByName(L"RecipeName")->AsString).IsEmpty();
//    if (empty)
//      items->Cancel();
//    else
//      items->Post();
//  }
//
//  if (items->State == dsBrowse) items->Append();
//
//  GridItems->SetFocus();
//  GridItems->SelectedIndex = 2;  // RecipeName kolona
//  GridItems->EditorMode = true;
//}

