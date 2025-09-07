#include <vcl.h>
#pragma hdrstop
#include "NutritionForm.h"

#include <System.IOUtils.hpp>
#include <System.SysUtils.hpp>
#include <TypInfo.hpp>
#include <Vcl.Dialogs.hpp>
#include <cstring>  // memset, strncpy, strlen
#include <fstream>

#pragma package(smart_init)
#pragma resource "*.dfm"

TFNutritionForm* FNutritionForm;

// ---------- lokalni prijevod helpers ----------
static void translateForm(
    TForm* Form, const String& Language,
    const std::map<String, std::map<String, String>>& tr) {
  for (int i = 0; i < Form->ComponentCount; ++i) {
    TComponent* C = Form->Components[i];
    auto it = tr.find(C->Name);
    if (it != tr.end()) {
      auto itLang = it->second.find(Language);
      if (itLang != it->second.end()) {
        if (IsPublishedProp(C, L"Caption"))
          SetPropValue(C, L"Caption", itLang->second);
        if (IsPublishedProp(C, L"Text"))
          SetPropValue(C, L"Text", itLang->second);
      }
    }
  }
  auto itCap = tr.find(L"Caption");
  if (itCap != tr.end()) {
    auto itLang = itCap->second.find(Language);
    if (itLang != itCap->second.end()) Form->Caption = itLang->second;
  }
}

static void translateListViewHR(TListView* lv) {
  if (!lv || lv->Columns->Count < 8) return;
  lv->Columns->Items[0]->Caption = L"Recept";
  lv->Columns->Items[1]->Caption = L"Kalorije";
  lv->Columns->Items[2]->Caption = L"Proteini (g)";
  lv->Columns->Items[3]->Caption = L"Ugljikohidrati (g)";
  lv->Columns->Items[4]->Caption = L"Masti (g)";
  lv->Columns->Items[5]->Caption = L"Vlakna (g)";
  lv->Columns->Items[6]->Caption = L"Šeæer (g)";
  lv->Columns->Items[7]->Caption = L"Soli";
}

static void translateListViewEN(TListView* lv) {
  if (!lv || lv->Columns->Count < 8) return;
  lv->Columns->Items[0]->Caption = L"Recipe";
  lv->Columns->Items[1]->Caption = L"Calories";
  lv->Columns->Items[2]->Caption = L"Proteins (g)";
  lv->Columns->Items[3]->Caption = L"Carbs (g)";
  lv->Columns->Items[4]->Caption = L"Fats (g)";
  lv->Columns->Items[5]->Caption = L"Fiber (g)";
  lv->Columns->Items[6]->Caption = L"Sugar (g)";
  lv->Columns->Items[7]->Caption = L"Salts";
}

// ---------- TFNutritionForm ----------
__fastcall TFNutritionForm::TFNutritionForm(TComponent* Owner) : TForm(Owner) {
  BinaryFilePath =
      ExtractFilePath(Application->ExeName) + L"nutrition_data.nut";
  InitializeTranslations();
}

void TFNutritionForm::InitializeTranslations() {
  translation[L"Caption"] = {{L"HR", L"Nutritivne informacije recepata"},
                             {L"EN", L"Recipe Nutrition Information"}};

  translation[L"LblRecipe"] = {{L"HR", L"Recept:"}, {L"EN", L"Recipe:"}};
  translation[L"LblCalories"] = {{L"HR", L"Kalorije:"}, {L"EN", L"Calories:"}};
  translation[L"LblProteins"] = {{L"HR", L"Proteini (g):"},
                                 {L"EN", L"Proteins (g):"}};
  translation[L"LblCarbs"] = {{L"HR", L"Ugljikohidrati (g):"},
                              {L"EN", L"Carbs (g):"}};
  translation[L"LblFats"] = {{L"HR", L"Masti (g):"}, {L"EN", L"Fats (g):"}};
  translation[L"LblFiber"] = {{L"HR", L"Vlakna (g):"}, {L"EN", L"Fiber (g):"}};
  translation[L"LblSugar"] = {{L"HR", L"Šeæer (g):"}, {L"EN", L"Sugar (g):"}};
  translation[L"LblSalts"] = {{L"HR", L"Soli:"}, {L"EN", L"Salts:"}};

  translation[L"BtnAdd"] = {{L"HR", L"Dodaj nutritivne podatke"},
                            {L"EN", L"Add Nutrition Data"}};
  translation[L"BtnEdit"] = {{L"HR", L"Uredi podatke"}, {L"EN", L"Edit Data"}};
  translation[L"BtnDelete"] = {{L"HR", L"Ukloni podatke"},
                               {L"EN", L"Delete Data"}};
}

void __fastcall TFNutritionForm::FormCreate(TObject* Sender) {
  // ListView setup (8 kolona)
  LVNutrition->ViewStyle = vsReport;
  LVNutrition->RowSelect = true;
  LVNutrition->GridLines = true;
  LVNutrition->HideSelection = false;

  LVNutrition->Columns->Clear();
  auto c0 = LVNutrition->Columns->Add();
  c0->Width = 160;  // Recipe
  auto c1 = LVNutrition->Columns->Add();
  c1->Width = 80;  // Calories
  auto c2 = LVNutrition->Columns->Add();
  c2->Width = 100;  // Proteins
  auto c3 = LVNutrition->Columns->Add();
  c3->Width = 120;  // Carbs
  auto c4 = LVNutrition->Columns->Add();
  c4->Width = 90;  // Fats
  auto c5 = LVNutrition->Columns->Add();
  c5->Width = 90;  // Fiber
  auto c6 = LVNutrition->Columns->Add();
  c6->Width = 90;  // Sugar
  auto c7 = LVNutrition->Columns->Add();
  c7->Width = 90;  // Salts

  if (FileExists(BinaryFilePath)) {
    LoadNutritionBinary(BinaryFilePath);
  }
}

void __fastcall TFNutritionForm::FormShow(TObject* Sender) {
  ApplyLanguage();
  RefreshNutritionList();
}

void TFNutritionForm::ApplyLanguage() {
  translateForm(this, CurrentLanguage, translation);
  if (CurrentLanguage == L"HR")
    translateListViewHR(LVNutrition);
  else
    translateListViewEN(LVNutrition);
}

// --------- BINARY CRUD ----------
bool TFNutritionForm::SaveNutritionBinary(const UnicodeString& filePath) {
  try {
    std::ofstream f(AnsiString(filePath).c_str(), std::ios::binary);
    if (!f.is_open()) throw Exception(L"Cannot open file for writing");

    unsigned int count = static_cast<unsigned int>(NutritionData.size());
    f.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& rec : NutritionData) {
      if (rec.IsValid())
        f.write(reinterpret_cast<const char*>(&rec), sizeof(TNutritionInfo));
    }
    f.close();
    return true;
  } catch (const Exception& e) {
    UnicodeString msg =
        (CurrentLanguage == L"HR")
            ? (UnicodeString(L"Greška pri spremanju:\n") + e.Message)
            : (UnicodeString(L"Error while saving:\n") + e.Message);
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Greška" : L"Error";
    Application->MessageBox(msg.c_str(), title, MB_OK | MB_ICONERROR);
    return false;
  }
}

bool TFNutritionForm::LoadNutritionBinary(const UnicodeString& filePath) {
  try {
    if (!FileExists(filePath)) return false;

    std::ifstream f(AnsiString(filePath).c_str(), std::ios::binary);
    if (!f.is_open()) throw Exception(L"Cannot open file for reading");

    unsigned int count = 0;
    f.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (f.fail() || count > 100000)
      throw Exception(L"Invalid or corrupted file");

    NutritionData.clear();
    NutritionData.reserve(count);

    for (unsigned int i = 0; i < count; ++i) {
      TNutritionInfo rec;
      f.read(reinterpret_cast<char*>(&rec), sizeof(TNutritionInfo));
      if (f.fail()) {
        throw Exception(UnicodeString(L"Error reading nutrition record ") +
                        IntToStr((int)(i + 1)));
      }
      if (rec.IsValid()) NutritionData.push_back(rec);
    }

    f.close();
    return true;
  } catch (const Exception& e) {
    UnicodeString msg =
        (CurrentLanguage == L"HR")
            ? (UnicodeString(L"Greška pri uèitavanju:\n") + e.Message)
            : (UnicodeString(L"Error while loading:\n") + e.Message);
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Greška" : L"Error";
    Application->MessageBox(msg.c_str(), title, MB_OK | MB_ICONERROR);
    return false;
  }
}

bool TFNutritionForm::DeleteNutritionRecord(const UnicodeString& recipeName) {
  int idx = FindNutritionIndex(recipeName);
  if (idx >= 0) {
    NutritionData.erase(NutritionData.begin() + idx);
    return SaveNutritionBinary(BinaryFilePath);
  }
  return false;
}

// --------- UI handlers ----------
void __fastcall TFNutritionForm::BtnAddClick(TObject* Sender) {
  if (CombRecipes->ItemIndex < 0) {
    const wchar_t* msg =
        (CurrentLanguage == L"HR") ? L"Odaberite recept." : L"Select a recipe.";
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Info" : L"Info";
    Application->MessageBox(msg, title, MB_OK | MB_ICONINFORMATION);
    return;
  }

  UnicodeString name = CombRecipes->Text;

  if (FindNutritionIndex(name) >= 0) {
    const wchar_t* msg =
        (CurrentLanguage == L"HR")
            ? L"Podaci za ovaj recept veæ postoje. Koristite 'Uredi'."
            : L"Data for this recipe already exists. Use 'Edit'.";
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Info" : L"Info";
    Application->MessageBox(msg, title, MB_OK | MB_ICONINFORMATION);
    return;
  }

  TNutritionInfo rec = GetNutritionFromForm();
  rec.SetRecipeName(name);

  if (!rec.IsValid()) {
    const wchar_t* msg = (CurrentLanguage == L"HR")
                             ? L"Unesite valjane podatke."
                             : L"Please enter valid data.";
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Greška" : L"Error";
    Application->MessageBox(msg, title, MB_OK | MB_ICONWARNING);
    return;
  }

  NutritionData.push_back(rec);
  bool ok = SaveNutritionBinary(BinaryFilePath);
  RefreshNutritionList();
  ClearForm();

  const wchar_t* msg =
      ok ? ((CurrentLanguage == L"HR") ? L"Podaci dodani." : L"Data added.")
         : ((CurrentLanguage == L"HR") ? L"Greška pri spremanju."
                                       : L"Save error.");
  const wchar_t* title =
      ok ? ((CurrentLanguage == L"HR") ? L"Uspjeh" : L"Success")
         : ((CurrentLanguage == L"HR") ? L"Greška" : L"Error");
  Application->MessageBox(msg, title,
                          MB_OK | (ok ? MB_ICONINFORMATION : MB_ICONERROR));
}

void __fastcall TFNutritionForm::BtnEditClick(TObject* Sender) {
  if (!LVNutrition->Selected) {
    const wchar_t* msg =
        (CurrentLanguage == L"HR") ? L"Odaberite zapis." : L"Select a record.";
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Info" : L"Info";
    Application->MessageBox(msg, title, MB_OK | MB_ICONINFORMATION);
    return;
  }

  UnicodeString name = LVNutrition->Selected->Caption;
  int idx = FindNutritionIndex(name);
  if (idx < 0) return;

  TNutritionInfo rec = GetNutritionFromForm();
  rec.SetRecipeName(name);

  if (!rec.IsValid()) {
    const wchar_t* msg =
        (CurrentLanguage == L"HR") ? L"Neispravni podaci." : L"Invalid data.";
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Greška" : L"Error";
    Application->MessageBox(msg, title, MB_OK | MB_ICONWARNING);
    return;
  }

  NutritionData[idx] = rec;
  bool ok = SaveNutritionBinary(BinaryFilePath);
  RefreshNutritionList();
  ClearForm();

  const wchar_t* msg =
      ok ? ((CurrentLanguage == L"HR") ? L"Podaci ažurirani."
                                       : L"Data updated.")
         : ((CurrentLanguage == L"HR") ? L"Greška pri spremanju."
                                       : L"Save error.");
  const wchar_t* title =
      ok ? ((CurrentLanguage == L"HR") ? L"Uspjeh" : L"Success")
         : ((CurrentLanguage == L"HR") ? L"Greška" : L"Error");
  Application->MessageBox(msg, title,
                          MB_OK | (ok ? MB_ICONINFORMATION : MB_ICONERROR));
}

void __fastcall TFNutritionForm::BtnDeleteClick(TObject* Sender) {
  if (!LVNutrition->Selected) {
    const wchar_t* msg =
        (CurrentLanguage == L"HR") ? L"Odaberite zapis." : L"Select a record.";
    const wchar_t* title = (CurrentLanguage == L"HR") ? L"Info" : L"Info";
    Application->MessageBox(msg, title, MB_OK | MB_ICONINFORMATION);
    return;
  }

  UnicodeString name = LVNutrition->Selected->Caption;
  UnicodeString q =
      (CurrentLanguage == L"HR")
          ? (UnicodeString(L"Obrisati nutritivne podatke za:\n") + name + L"?")
          : (UnicodeString(L"Delete nutrition data for:\n") + name + L"?");

  if (Application->MessageBox(
          q.c_str(), (CurrentLanguage == L"HR") ? L"Potvrda" : L"Confirm",
          MB_YESNO | MB_ICONQUESTION) == IDYES) {
    bool ok = DeleteNutritionRecord(name);
    RefreshNutritionList();
    ClearForm();

    const wchar_t* msg =
        ok ? ((CurrentLanguage == L"HR") ? L"Obrisano." : L"Deleted.")
           : ((CurrentLanguage == L"HR") ? L"Greška pri brisanju."
                                         : L"Delete error.");
    const wchar_t* title =
        ok ? ((CurrentLanguage == L"HR") ? L"Uspjeh" : L"Success")
           : ((CurrentLanguage == L"HR") ? L"Greška" : L"Error");
    Application->MessageBox(msg, title,
                            MB_OK | (ok ? MB_ICONINFORMATION : MB_ICONERROR));
  }
}

void __fastcall TFNutritionForm::LVNutritionSelectItem(TObject* Sender,
                                                       TListItem* Item,
                                                       bool Selected) {
  if (Selected && Item) {
    UnicodeString name = Item->Caption;
    int idx = FindNutritionIndex(name);
    if (idx >= 0) LoadNutritionToForm(NutritionData[idx]);
  }
}

// Povezano u DFM-u — zasad bez posebne logike// Povezano u DFM-u — možeš dodati
// validaciju broja Helpers ----------
void TFNutritionForm::RefreshNutritionList() {
  LVNutrition->Items->BeginUpdate();
  LVNutrition->Items->Clear();

  for (const auto& r : NutritionData) {
    TListItem* it = LVNutrition->Items->Add();
    it->Caption = r.GetRecipeName();
    it->SubItems->Add(FloatToStr(r.Calories));
    it->SubItems->Add(FloatToStr(r.Proteins));
    it->SubItems->Add(FloatToStr(r.Carbohydrates));
    it->SubItems->Add(FloatToStr(r.Fats));
    it->SubItems->Add(FloatToStr(r.Fiber));
    it->SubItems->Add(FloatToStr(r.Sugar));
    it->SubItems->Add(FloatToStr(r.Salts));
  }

  LVNutrition->Items->EndUpdate();
}

void TFNutritionForm::ClearForm() {
  CombRecipes->ItemIndex = -1;
  EditCalories->Clear();
  EditProteins->Clear();
  EditCarbs->Clear();
  EditFats->Clear();
  EditFiber->Clear();
  EditSugar->Clear();
  EditSalts->Clear();
}

TNutritionInfo TFNutritionForm::GetNutritionFromForm() const {
  TNutritionInfo info;
  info.Calories = StrToFloatDef(EditCalories->Text, 0.0);
  info.Proteins = StrToFloatDef(EditProteins->Text, 0.0);
  info.Carbohydrates = StrToFloatDef(EditCarbs->Text, 0.0);
  info.Fats = StrToFloatDef(EditFats->Text, 0.0);
  info.Fiber = StrToFloatDef(EditFiber->Text, 0.0);
  info.Sugar = StrToFloatDef(EditSugar->Text, 0.0);
  info.Salts = StrToFloatDef(EditSalts->Text, 0.0);
  return info;
}

void TFNutritionForm::LoadNutritionToForm(const TNutritionInfo& info) {
  EditCalories->Text = FloatToStr(info.Calories);
  EditProteins->Text = FloatToStr(info.Proteins);
  EditCarbs->Text = FloatToStr(info.Carbohydrates);
  EditFats->Text = FloatToStr(info.Fats);
  EditFiber->Text = FloatToStr(info.Fiber);
  EditSugar->Text = FloatToStr(info.Sugar);
  EditSalts->Text = FloatToStr(info.Salts);

  for (int i = 0; i < CombRecipes->Items->Count; ++i) {
    if (CombRecipes->Items->Strings[i] == info.GetRecipeName()) {
      CombRecipes->ItemIndex = i;
      break;
    }
  }
}

int TFNutritionForm::FindNutritionIndex(const UnicodeString& recipeName) const {
  for (size_t i = 0; i < NutritionData.size(); ++i) {
    if (NutritionData[i].GetRecipeName() == recipeName) return (int)i;
  }
  return -1;
}

void TFNutritionForm::SetAvailableRecipes(
    const std::vector<UnicodeString>& recipes) {
  AvailableRecipes = recipes;
  CombRecipes->Items->BeginUpdate();
  CombRecipes->Items->Clear();
  for (const auto& r : recipes) CombRecipes->Items->Add(r);
  CombRecipes->Items->EndUpdate();
}

TNutritionInfo* TFNutritionForm::GetNutritionForRecipe(
    const UnicodeString& recipeName) {
  int idx = FindNutritionIndex(recipeName);
  return (idx >= 0) ? &NutritionData[idx] : nullptr;
}

bool TFNutritionForm::HasNutritionData(const UnicodeString& recipeName) const {
  return FindNutritionIndex(recipeName) >= 0;
}

