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
  if (!bf || bf->IsNull) return;

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
    IdTCPClient1->ConnectTimeout = 3000;
    IdTCPClient1->ReadTimeout = 5000;
    IdTCPClient1->Connect();
    IdTCPClient1->IOHandler->DefStringEncoding = IndyTextEncoding_UTF8();
  }
}

// GET_RECIPES
void __fastcall TFMealPlans::TcpGetRecipes() {
  TcpConnect();
  auto io = IdTCPClient1->IOHandler;
  io->WriteLn(L"GET_RECIPES");
  UnicodeString status = Trim(io->ReadLn());
  if (status == L"OK") {
    UnicodeString list = io->ReadLn();
    ShowMessage(L"Recepti: " + list);  // npr. "Spaghetti|Salad|Burger"
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
    int len = StrToIntDef(hdr.SubString(8, hdr.Length()), 0);
    std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
    io->ReadStream(ms.get(), len, false);
    ms->Position = 0;
    try {
      ImgPhoto->Picture->LoadFromStream(ms.get());
    } catch (...) {
      ms->Position = 0;
      std::unique_ptr<Graphics::TBitmap> bmp(new Graphics::TBitmap());
      bmp->LoadFromStream(ms.get());
      ImgPhoto->Picture->Assign(bmp.get());
    }
  } else {
    ShowMessage(L"Greška: " + hdr);  // npr. ERR NOFILE
  }
}

// UPLOAD_RECIPE (pošalji mali tekst kao stream)
void __fastcall TFMealPlans::TcpUploadRecipe(const UnicodeString &name,
                                             const UnicodeString &category) {
  TcpConnect();
  UnicodeString payload = name + L"|" + category;

  // Pretvori u Indy bajtove (nema konflikta s FastReport TBytes)
  Idglobal::TIdBytes bytes =
      Idglobal::ToBytes(payload, IndyTextEncoding_UTF8());

  auto io = IdTCPClient1->IOHandler;
  io->WriteLn(L"UPLOAD_RECIPE");
  io->WriteLn(L"STREAM " + IntToStr(bytes.Length));
  if (bytes.Length > 0) {
    io->Write(bytes);
  }

  UnicodeString resp = Trim(io->ReadLn());
  ShowMessage(L"Server: " + resp);  // "OK" ili poruka greške
}

void __fastcall TFMealPlans::BtnGetRecipesTCPClick(TObject *) {
  TcpConnect();
  TcpGetRecipes();
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
void __fastcall TFMealPlans::BtnNotifyUDPClick(TObject *) {
  // PING
  UdpSendUtf8(IdUDPClient1, L"PING");
  MemoUDP->Lines->Add(L"UDP→ PING");
  UnicodeString pong =
      IdUDPClient1->ReceiveString();  // koristi ReceiveTimeout iz svojstva
  if (!pong.IsEmpty())
    MemoUDP->Lines->Add(L"UDP← " + pong);
  else
    MemoUDP->Lines->Add(L"UDP timeout (PING).");

  // PLAN_UPDATED |<PlanID>|<timestamp>
  int planId = 0;
  if (AppDataModule1->TMealPlans && !AppDataModule1->TMealPlans->IsEmpty())
    planId = AppDataModule1->TMealPlans->FieldByName(L"PlanID")->AsInteger;

  UnicodeString msg = L"PLAN_UPDATED|" + IntToStr(planId) + L"|" +
                      FormatDateTime(L"yyyy-mm-dd hh:nn:ss", Now());

  UdpSendUtf8(IdUDPClient1, msg);
  MemoUDP->Lines->Add(L"UDP→ " + msg);

  UnicodeString ack = IdUDPClient1->ReceiveString();
  if (!ack.IsEmpty())
    MemoUDP->Lines->Add(L"UDP← " + ack);
  else
    MemoUDP->Lines->Add(L"UDP timeout (ACK).");
}

// THUMBNAIL
void __fastcall TFMealPlans::BtnGetThumbUDPClick(TObject *) {
  auto ds = AppDataModule1->TUsers;
  if (!ds || ds->IsEmpty()) {
    Application->MessageBox(L"Izaberite korisnika.", L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }
  const int uid = ds->FieldByName(L"UserID")->AsInteger;

  // zahtjev
  UdpSendUtf8(IdUDPClient1, L"GET_THUMB " + IntToStr(uid));
  MemoUDP->Lines->Add(L"UDP→ GET_THUMB " + IntToStr(uid));

  // ===== 1) BEGIN =====
  UnicodeString first = IdUDPClient1->ReceiveString();
  if (first.IsEmpty()) {
    MemoUDP->Lines->Add(L"UDP timeout (BEGIN).");
    return;
  }
  if (first.Pos(L"ERR") == 1) {
    ShowMessage(first);
    return;
  }
  if (first.Pos(L"B64 BEGIN ") != 1) {
    MemoUDP->Lines->Add(L"Neočekivan odgovor: " + first);
    return;
  }

  // "B64 BEGIN <total> [<origLen>]"
  int total = 0, origLen = 0;
  {
    int p1 = first.Pos(L" ");  // nakon "B64"
    if (p1 <= 0) {
      MemoUDP->Lines->Add(L"BEGIN split err.");
      return;
    }
    UnicodeString afterB64 =
        first.SubString(p1 + 1, first.Length() - p1);  // "BEGIN <...>"
    int p2 = afterB64.Pos(L" ");                       // nakon "BEGIN"
    if (p2 <= 0) {
      MemoUDP->Lines->Add(L"BEGIN split err.");
      return;
    }
    UnicodeString afterBegin =
        afterB64.SubString(p2 + 1, afterB64.Length() - p2);  // "<total> [orig]"
    int p3 = afterBegin.Pos(L" ");
    if (p3 > 0) {
      total = StrToIntDef(afterBegin.SubString(1, p3 - 1), 0);
      origLen = StrToIntDef(
          afterBegin.SubString(p3 + 1, afterBegin.Length() - p3), 0);
    } else {
      total = StrToIntDef(afterBegin, 0);
    }
  }
  if (total <= 0) {
    MemoUDP->Lines->Add(L"Prazno.");
    return;
  }
  MemoUDP->Lines->Add(L"BEGIN: očekujem " + IntToStr(total) + L" fragmenata…");

  // ===== 2) FRAGMENTI =====
  std::vector<UnicodeString> parts(total + 1);
  int received = 0;
  while (received < total) {
    UnicodeString line = IdUDPClient1->ReceiveString();
    if (line.IsEmpty()) {
      MemoUDP->Lines->Add(L"Fragment timeout.");
      return;
    }
    if (line.Pos(L"B64 ") != 1) {
      MemoUDP->Lines->Add(L"Krivi fragment: " + line);
      return;
    }

    int sp1 = line.Pos(L" ");  // iza "B64"
    if (sp1 <= 0) {
      MemoUDP->Lines->Add(L"Split err.");
      return;
    }
    UnicodeString rest =
        line.SubString(sp1 + 1, line.Length() - sp1);  // "i/n <payload>"
    int sp2 = rest.Pos(L" ");
    if (sp2 <= 0) {
      MemoUDP->Lines->Add(L"Split err.");
      return;
    }

    UnicodeString idxTok = rest.SubString(1, sp2 - 1);  // "i/n"
    int slash = idxTok.Pos(L"/");
    int idx = StrToIntDef(idxTok.SubString(1, slash - 1), 0);

    UnicodeString payload = rest.SubString(sp2 + 1, rest.Length() - sp2);

    if (idx >= 1 && idx <= total) {
      if (parts[idx].IsEmpty()) {
        parts[idx] = payload;
        ++received;
        if ((received % 10) == 0 || received == total)
          MemoUDP->Lines->Add(L"Primljeno " + IntToStr(received) + L"/" +
                              IntToStr(total));
      }
    }
  }

  // ===== 3) END (nebitno, samo pročitamo) =====
  UnicodeString ending = IdUDPClient1->ReceiveString();
  if (ending != L"B64 END")
    MemoUDP->Lines->Add(L"Upozorenje: kraj nije stigao: " + ending);

  // ===== 4) Spoji base64 i dekodiraj =====
  UnicodeString all;
  all.SetLength(0);
  for (int i = 1; i <= total; ++i) all += parts[i];

  // Dekodiranje ISTOM bibliotekom kao na serveru
  System::Sysutils::TBytes raw = TNetEncoding::Base64->DecodeStringToBytes(all);

  // (opcionalno) dijagnostika – snimi što je stiglo
  try {
    std::unique_ptr<TFileStream> dbg(new TFileStream(
        ExtractFilePath(ParamStr(0)) + L"udp_last.jpg", fmCreate));
    if (raw.Length) dbg->WriteBuffer(&raw[0], raw.Length);
  } catch (...) {
  }

  // Učitaj u TImage
  std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
  if (raw.Length) ms->WriteBuffer(&raw[0], raw.Length);
  ms->Position = 0;
  try {
    ImgPhoto->Picture->LoadFromStream(ms.get());  // auto JPEG/PNG
  } catch (...) {
    ms->Position = 0;
    try {
      std::unique_ptr<Vcl::Imaging::Pngimage::TPngImage> png(
          new Vcl::Imaging::Pngimage::TPngImage());
      png->LoadFromStream(ms.get());
      ImgPhoto->Picture->Assign(png.get());
    } catch (...) {
      ShowMessage(L"Primljeni podaci nisu valjana slika.");
      return;
    }
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

