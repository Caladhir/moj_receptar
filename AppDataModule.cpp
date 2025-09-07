#include <vcl.h>
#pragma hdrstop
#include "AppDataModule.h"

#include <Data.DB.hpp>
#include <System.SysUtils.hpp>
#include <System.Variants.hpp>
#include <Vcl.Dialogs.hpp>
#include <memory>
#pragma package(smart_init)
#pragma resource "*.dfm"

TAppDataModule1 *AppDataModule1 = nullptr;

__fastcall TAppDataModule1::TAppDataModule1(TComponent *Owner)
    : TDataModule(Owner) {}

__fastcall TAppDataModule1::~TAppDataModule1() {
  // očisti TEvent ako je kreiran
  if (ReportEvent) {
    delete ReportEvent;
    ReportEvent = nullptr;
  }
}

void __fastcall TAppDataModule1::DataModuleCreate(TObject *Sender) {
  // Sigurna inicijalizacija
  try {
    if (Conn && !Conn->Connected) Conn->Open();
  } catch (...) {
  }

  for (auto ds : {TUsers, TMealPlans, TPlanItems}) {
    try {
      if (ds && !ds->Active) ds->Open();
    } catch (...) {
    }
  }

  // Polja koja moraju biti zapisiva
  if (TMealPlansUserID) TMealPlansUserID->ReadOnly = false;
  if (TPlanItemsPlanID) TPlanItemsPlanID->ReadOnly = false;

  // NIKAKVI filteri na detailu (master-detail sve radi)
  if (TPlanItems) {
    TPlanItems->Filtered = false;
    TPlanItems->Filter = L"";
  }

  ReportEvent = new TEvent(
      /*OwnerSA*/ nullptr,
      /*ManualReset*/ true,
      /*InitialState*/ true,
      /*Name*/ L"",
      /*UseCOMWait*/ false);

  ReportInProgress = false;
}

//
// ============ MEAL PLANS ============
//
void __fastcall TAppDataModule1::TMealPlansNewRecord(TDataSet *DataSet) {
  // Veži novi plan na trenutno odabranog korisnika (ako postoji)
  if (TUsers && !TUsers->IsEmpty()) {
    DataSet->FieldByName(L"UserID")->AsInteger =
        TUsers->FieldByName(L"UserID")->AsInteger;
  }
  DataSet->FieldByName(L"PlanDate")->AsDateTime = Date();
  // PlanName ostavljamo korisniku da unese ručno
}

void __fastcall TAppDataModule1::TMealPlansCalcFields(TDataSet *DataSet) {
  // Izračunaj ItemCount SQL-om (TADOQuery), bez COM Recordset-a
  int cnt = 0;
  try {
    const int pid = DataSet->FieldByName(L"PlanID")->AsInteger;
    std::unique_ptr<TADOQuery> q(new TADOQuery(nullptr));
    q->Connection = Conn;
    q->SQL->Text = L"SELECT COUNT(*) AS Cnt FROM PlanItems WHERE PlanID = :P";
    q->Parameters->ParamByName(L"P")->Value = pid;
    q->Open();
    if (!q->FieldByName(L"Cnt")->IsNull)
      cnt = q->FieldByName(L"Cnt")->AsInteger;
  } catch (...) {
    cnt = 0;
  }
  if (TMealPlansItemCount) TMealPlansItemCount->AsInteger = cnt;
}

//
// ============ PLAN ITEMS (DETAIL) ============
//
void __fastcall TAppDataModule1::TPlanItemsNewRecord(TDataSet *DataSet) {
  // Master-detail veza postavlja PlanID; za svaki slučaj popuni ručno
  if (TMealPlans && !TMealPlans->IsEmpty())
    DataSet->FieldByName(L"PlanID")->AsInteger =
        TMealPlans->FieldByName(L"PlanID")->AsInteger;

  DataSet->FieldByName(L"Servings")->AsInteger = 1;
  DataSet->FieldByName(L"WhenInDay")->AsString = L"Breakfast";
}

void __fastcall TAppDataModule1::TPlanItemsBeforeInsert(TDataSet *DataSet) {
  // Osiguraj da je master postan (da imamo PlanID)
  if (TMealPlans &&
      (TMealPlans->State == dsInsert || TMealPlans->State == dsEdit))
    TMealPlans->Post();
}

void __fastcall TAppDataModule1::TPlanItemsAfterPost(TDataSet *DataSet) {
  if (TMealPlans) TMealPlans->Refresh();  // da se ItemCount preračuna
}
void __fastcall TAppDataModule1::TPlanItemsAfterDelete(TDataSet *DataSet) {
  if (TMealPlans) TMealPlans->Refresh();
}

//
// ============ SORT ============
//
void __fastcall TAppDataModule1::SortUsers(const UnicodeString &orderBy) {
  if (TUsers) TUsers->Sort = orderBy;
}
void __fastcall TAppDataModule1::SortMealPlans(const UnicodeString &orderBy) {
  if (TMealPlans) TMealPlans->Sort = orderBy;
}
void __fastcall TAppDataModule1::SortPlanItems(const UnicodeString &orderBy) {
  if (TPlanItems) TPlanItems->Sort = orderBy;
}

//
// ============ FILTER ============
//
void __fastcall TAppDataModule1::FilterUsersByName(
    const UnicodeString &likeText) {
  if (!TUsers) return;
  const UnicodeString s = likeText.Trim();
  if (s.IsEmpty()) {
    TUsers->Filtered = false;
    TUsers->Filter = L"";
    return;
  }
  TUsers->Filter =
      L"UserName LIKE '%" + s + L"%' OR Surname LIKE '%" + s + L"%'";
  TUsers->Filtered = true;
}

void __fastcall TAppDataModule1::FilterMealPlansByUserID(int userId) {
  if (!TMealPlans) return;
  TMealPlans->Filter = L"UserID=" + IntToStr(userId);
  TMealPlans->Filtered = true;

  // Detail OSTAVLJAMO bez filtera (master-detail veza radi)
  if (TPlanItems) {
    TPlanItems->Filtered = false;
    TPlanItems->Filter = L"";
  }
}

void __fastcall TAppDataModule1::ClearAllFilters() {
  if (TUsers) {
    TUsers->Filtered = false;
    TUsers->Filter = L"";
  }
  if (TMealPlans) {
    TMealPlans->Filtered = false;
    TMealPlans->Filter = L"";
  }
  if (TPlanItems) {
    TPlanItems->Filtered = false;
    TPlanItems->Filter = L"";
  }
}

//
// ============ BLOB helperi ============
//
bool __fastcall TAppDataModule1::LoadUserPhotoFromFile(
    int userId, const UnicodeString &path) {
  if (!TUsers) return false;
  if (!TUsers->Locate(L"UserID", Variant(userId), TLocateOptions()))
    return false;

  std::unique_ptr<TFileStream> fs(
      new TFileStream(path, fmOpenRead | fmShareDenyNone));
  if (!fs) return false;

  TUsers->Edit();
  if (auto *bf = dynamic_cast<TBlobField *>(TUsers->FindField(L"Photo"))) {
    bf->LoadFromStream(fs.get());
    TUsers->Post();
    return true;
  }
  TUsers->Cancel();
  return false;
}

bool __fastcall TAppDataModule1::SaveUserPhotoToFile(
    int userId, const UnicodeString &path) {
  if (!TUsers) return false;
  if (!TUsers->Locate(L"UserID", Variant(userId), TLocateOptions()))
    return false;

  auto *bf = dynamic_cast<TBlobField *>(TUsers->FindField(L"Photo"));
  if (!bf || bf->IsNull) return false;

  std::unique_ptr<TFileStream> fs(new TFileStream(path, fmCreate));
  bf->SaveToStream(fs.get());
  return true;
}

//
// ============ SQL insert u detail (opcionalno) ============
//
bool __fastcall TAppDataModule1::AddPlanItemSQL(int planId,
                                                const UnicodeString &recipeName,
                                                int servings,
                                                const UnicodeString &whenInDay,
                                                const UnicodeString &comment) {
  try {
    std::unique_ptr<TADOCommand> cmd(new TADOCommand(nullptr));
    cmd->Connection = Conn;
    cmd->CommandText =
        L"INSERT INTO PlanItems (PlanID, RecipeName, Servings, WhenInDay, "
        L"Comment) "
        L"VALUES (:P, :R, :S, :W, :C)";
    cmd->Parameters->ParamByName(L"P")->DataType = ftInteger;
    cmd->Parameters->ParamByName(L"R")->DataType = ftWideString;
    cmd->Parameters->ParamByName(L"S")->DataType = ftInteger;
    cmd->Parameters->ParamByName(L"W")->DataType = ftWideString;
    cmd->Parameters->ParamByName(L"C")->DataType = ftWideString;

    cmd->Parameters->ParamByName(L"P")->Value = planId;
    cmd->Parameters->ParamByName(L"R")->Value = recipeName;
    cmd->Parameters->ParamByName(L"S")->Value = servings;
    cmd->Parameters->ParamByName(L"W")->Value = whenInDay;
    cmd->Parameters->ParamByName(L"C")->Value = comment;
    cmd->Execute();

    // Osvježi detail
    if (TPlanItems && TPlanItems->Active) {
      try {
        TPlanItems->Requery();
      } catch (...) {
        TPlanItems->Close();
        TPlanItems->Open();
      }
    }
    if (TMealPlans) TMealPlans->Refresh();

    return true;
  } catch (const Exception &e) {
    Application->MessageBox(
        (L"Greška pri dodavanju stavke:\n" + e.Message).c_str(), L"Greška",
        MB_OK | MB_ICONERROR);
    return false;
  }
}

