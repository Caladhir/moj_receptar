#include <vcl.h>
#pragma hdrstop

#include <Vcl.ComCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#pragma link "Vcl.StdCtrls"
#pragma link "Vcl.ComCtrls"

#include <System.SysUtils.hpp>
#include <TypInfo.hpp>
#include <Vcl.Graphics.hpp>
#include <map>
#include <memory>
#include <vector>

#include "Login_Unit.h"
#include "MealPlansForm.h"
#include "NutritionForm.h"
#include "PopisRecepata.h"
#include "PostavkeForm.h"
#include "RatingForm.h"
#include "ReceptForm.h"
#include "Receptar.h"
#include "Settings.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

AppSettings gSettings;
UnicodeString gIniPath;

TFReceptar *FReceptar;

// ----- PRIJEVOD -----
using std::map;
static map<UnicodeString, map<UnicodeString, UnicodeString>> translation;

static void translateForm(TForm *Form, const String &Language,
                          const map<String, map<String, String>> &tr) {
  for (int i = 0; i < Form->ComponentCount; ++i) {
    TComponent *C = Form->Components[i];
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

static void translateListViewHR(TListView *lv) {
  if (!lv || lv->Columns->Count < 3) return;
  lv->Columns->Items[0]->Caption = L"Naziv recepta";
  lv->Columns->Items[1]->Caption = L"Težina recepta";
  lv->Columns->Items[2]->Caption = L"# Sastojaka";
}
static void translateListViewEN(TListView *lv) {
  if (!lv || lv->Columns->Count < 3) return;
  lv->Columns->Items[0]->Caption = L"Name of the recipe";
  lv->Columns->Items[1]->Caption = L"Difficulty";
  lv->Columns->Items[2]->Caption = L"# Ingredients";
}

__fastcall TFReceptar::TFReceptar(TComponent *Owner) : TForm(Owner) {}

////////////////////// FORM CREATE
void __fastcall TFReceptar::FormCreate(TObject *Sender) {
  gIniPath = ExtractFilePath(Application->ExeName) + "settings.ini";

  // 1) Učitaj postavke
  gSettings.LoadIni(gIniPath);
  gSettings.LoadRegistry();
  if (gSettings.GetRememberUser()) {
    UnicodeString u = gSettings.GetLastUser().Trim();
    if (!u.IsEmpty()) {
      LoggedIn = true;
      CurrentUser = u;
    }
  }

  // 2) Jezik iz settingsa
  currentLanguage = gSettings.GetLanguage();

  // 3) ListView priprema
  if (LVRecipes->Columns->Count < 3) {
    LVRecipes->Columns->Clear();
    LVRecipes->Columns->Add();
    LVRecipes->Columns->Add();
    LVRecipes->Columns->Add();
    LVRecipes->ViewStyle = vsReport;
  }

  // 4) Prijevodi
  translation[L"Caption"] = {{L"HR", L"Moj Receptar"}, {L"EN", L"My Recipes"}};
  translation[L"BtnAdd"] = {{L"HR", L"Dodaj novi recept"},
                            {L"EN", L"Add new recipe"}};
  translation[L"BtnEdit"] = {{L"HR", L"Uredi recept"}, {L"EN", L"Edit recipe"}};
  translation[L"BtnDelete"] = {{L"HR", L"Ukloni recept"},
                               {L"EN", L"Delete recipe"}};
  translation[L"mnuPrijava"] = {{L"HR", L"Prijava…"}, {L"EN", L"Login…"}};
  translation[L"mnuPostavke"] = {{L"HR", L"Postavke…"}, {L"EN", L"Settings…"}};
  translation[L"mnuIzlaz"] = {{L"HR", L"Izlaz"}, {L"EN", L"Exit"}};
  translation[L"mnuData"] = {{L"HR", L"Podaci"}, {L"EN", L"Data"}};
  translation[L"mnuLoadXML"] = {{L"HR", L"Učitaj recepte…"},
                                {L"EN", L"Load recipes…"}};
  translation[L"mnuRatings"] = {{L"HR", L"Ocjene recepata…"},
                                {L"EN", L"Recipe ratings…"}};
  translation[L"mnuNutrition"] = {{L"HR", L"Nutritivne informacije…"},
                                  {L"EN", L"Nutrition information…"}};
  translation[L"mnuMealPlans"] = {{L"HR", L"Planovi prehrane…"},
                                  {L"EN", L"Meal plans…"}};

  DataXmlPath = ExtractFilePath(Application->ExeName) + L"PopisRecepata.xml";
  if (FileExists(DataXmlPath)) {
    LoadRecipesXML(DataXmlPath);
  }

  // 5) Primijeni izgled/geo iz postavki
  gSettings.ApplyToForm(this);

  // 6) Prvi prijevod
  ApplyLanguage();
  RefreshList();

  UpdateLoginLabel();
  UpdateAccess();
}
// ACCESS
void TFReceptar::UpdateAccess() {
  if (LoggedIn) {
    BtnAdd->Enabled = true;
    BtnEdit->Enabled = true;
    BtnDelete->Enabled = true;
    mnuRatings->Enabled = true;
    mnuNutrition->Enabled = true;
    mnuMealPlans->Enabled = true;
  } else {
    BtnAdd->Enabled = false;
    BtnEdit->Enabled = false;
    BtnDelete->Enabled = false;
    mnuRatings->Enabled = false;
    mnuNutrition->Enabled = false;
    mnuMealPlans->Enabled = false;
  }
}

void __fastcall TFReceptar::FormShow(TObject *Sender) {
  ApplyLanguage();

  LVRecipes->OnKeyDown = LVRecipesKeyDown;
  LVRecipes->ReadOnly = true;
  LVRecipes->RowSelect = true;
  LVRecipes->HideSelection = false;
  LVRecipes->ViewStyle = vsReport;

  UpdateLoginLabel();
  UpdateAccess();
}

void TFReceptar::ApplyLanguage() {
  translateForm(this, currentLanguage, translation);
  if (currentLanguage == L"HR")
    translateListViewHR(LVRecipes);
  else
    translateListViewEN(LVRecipes);
  UpdateLoginLabel();
}

void TFReceptar::UpdateLoginLabel() {
  if (LoggedIn && !CurrentUser.Trim().IsEmpty()) {
    LblLogged->Caption = (currentLanguage == L"HR")
                             ? L"Prijavljen: " + CurrentUser
                             : L"Logged in: " + CurrentUser;
    // menij: odjava
    mnuPrijava->Caption = (currentLanguage == L"HR") ? L"Odjava" : L"Logout";
  } else {
    LblLogged->Caption =
        (currentLanguage == L"HR") ? L"Niste prijavljeni" : L"Not signed in";
    // menij: prijava
    mnuPrijava->Caption = (currentLanguage == L"HR") ? L"Prijava…" : L"Login…";
  }
}

void TFReceptar::RefreshList() {
  LVRecipes->Items->BeginUpdate();
  LVRecipes->Items->Clear();
  for (auto &r : Recepti) {
    auto *it = LVRecipes->Items->Add();
    it->Caption = r.Naziv;
    it->SubItems->Add(IntToStr(r.Tezina));
    it->SubItems->Add(IntToStr(r.BrojSastojaka()));
  }
  LVRecipes->Items->EndUpdate();
}

bool TFReceptar::EditRecept(TRecept &r) {
  std::unique_ptr<TFReceptForm> dlg(new TFReceptForm(this));
  gSettings.ApplyLookAndFeel(dlg.get());
  dlg->SetLanguage(currentLanguage);
  dlg->LoadFromRecept(r);
  if (dlg->ShowModal() == mrOk) {
    dlg->SaveToRecept(r);
    return true;
  }
  return false;
}

// DODAJ
void __fastcall TFReceptar::BtnAddClick(TObject *Sender) {
  TRecept r;
  if (EditRecept(r)) {
    Recepti.push_back(r);
    RefreshList();
    if (!DataXmlPath.IsEmpty()) SaveRecipesXML(DataXmlPath);
  }
}

// EDIT
void __fastcall TFReceptar::BtnEditClick(TObject *Sender) {
  if (!LVRecipes->Selected) return;
  int idx = LVRecipes->Selected->Index;
  if (idx >= 0 && idx < (int)Recepti.size()) {
    if (EditRecept(Recepti[idx])) {
      RefreshList();
      if (!DataXmlPath.IsEmpty()) SaveRecipesXML(DataXmlPath);
    }
  }
}

// DELETE
void __fastcall TFReceptar::BtnDeleteClick(TObject *Sender) {
  if (!LVRecipes->Selected) return;

  int idx = LVRecipes->Selected->Index;
  if (idx < 0 || idx >= (int)Recepti.size()) return;

  const UnicodeString q = (currentLanguage == L"HR")
                              ? L"Ukloniti odabrani recept?"
                              : L"Delete selected recipe?";
  if (Application->MessageBox(
          q.c_str(), (currentLanguage == L"HR") ? L"Potvrda" : L"Confirm",
          MB_YESNO | MB_ICONQUESTION) == IDYES) {
    Recepti.erase(Recepti.begin() + idx);
    RefreshList();
    if (!DataXmlPath.IsEmpty()) SaveRecipesXML(DataXmlPath);
  }
}

void __fastcall TFReceptar::LVRecipesKeyDown(TObject *Sender, WORD &Key,
                                             TShiftState) {
  if (Key == VK_DELETE) {
    BtnDeleteClick(Sender);
    Key = 0;
  } else if (Key == VK_RETURN) {
    BtnEditClick(Sender);
    Key = 0;
  }
}

// ------ Prijava ------
void __fastcall TFReceptar::mnuPrijavaClick(TObject *Sender) {
  // Ako je već prijavljen -> klik znači ODJAVA
  if (LoggedIn) {
    LoggedIn = false;
    CurrentUser = L"";

    gSettings.SetRememberUser(false);  // očisti auto-login flag
    gSettings.SetLastUser(L"");  // ili ostavi korisnika ako želiš prefill
    gSettings.SaveRegistry();

    UpdateLoginLabel();
    UpdateAccess();
    return;
  }

  // Inače -> PRIJAVA
  std::unique_ptr<TFLoginForm> dlg(new TFLoginForm(this));
  gSettings.ApplyLookAndFeel(dlg.get());
  dlg->SetLanguage(currentLanguage);

  if (dlg->ShowModal() == mrOk) {
    CurrentUser = dlg->GetUserName();
    LoggedIn = true;

    bool remember = dlg->GetRememberMe();
    gSettings.SetRememberUser(remember);
    gSettings.SetLastUser(CurrentUser);  // možeš ostaviti i kad remember=false
    gSettings.SaveRegistry();

    UpdateLoginLabel();
    UpdateAccess();

    UnicodeString msg =
        (currentLanguage == L"HR" ? L"Prijavljen: " : L"Logged in: ") +
        CurrentUser;
    Application->MessageBox(msg.c_str(),
                            (currentLanguage == L"HR" ? L"Info" : L"Info"),
                            MB_OK | MB_ICONINFORMATION);
  }
}

// ------ Postavke ------
void __fastcall TFReceptar::mnuPostavkeClick(TObject *Sender) {
  std::unique_ptr<TFPostavkeForm> dlg(new TFPostavkeForm(this));
  gSettings.ApplyLookAndFeel(dlg.get());

  dlg->SetCurrentLanguage(currentLanguage);
  dlg->SetCurrentFont(gSettings.GetFontName(), gSettings.GetFontSize());
  dlg->SetCurrentColor(gSettings.GetBackground());

  if (dlg->ShowModal() == mrOk) {
    currentLanguage = dlg->GetSelectedLanguage();
    gSettings.SetLanguage(currentLanguage);

    UnicodeString chosen = dlg->GetSelectedFont();
    int sz = dlg->GetSelectedFontSize();
    if (!chosen.IsEmpty()) {
      Font->Name = chosen;
      if (sz > 0) Font->Size = sz;
      gSettings.SetFontName(chosen);
      gSettings.SetFontSize(Font->Size);
    }

    gSettings.SetBackground(dlg->GetSelectedColor());
    gSettings.ApplyLookAndFeel(this);

    ApplyLanguage();
  }
}

bool TFReceptar::LoadRecipesXML(const UnicodeString &filePath) {
  try {
    _di_IXMLrecipesType recipes = Loadrecipes(filePath);
    Recepti.clear();

    for (int i = 0; i < recipes->Count; ++i) {
      _di_IXMLrecipeType rnode = recipes->recipe[i];

      TRecept r;
      r.Naziv = rnode->Get_name();
      r.Tezina = rnode->Get_difficulty();

      const int m = rnode->Count;

      for (int j = 0; j < m; ++j) {
        _di_IXMLingredientType inode = rnode->ingredient[j];

        TSastojak s;
        s.Naziv = inode->Text;
        UnicodeString qtyStr = inode->GetAttribute(L"qty");
        s.Kolicina = StrToFloatDef(qtyStr, 0.0);
        if (s.Validiraj()) r.Sastojci.push_back(s);
      }

      Recepti.push_back(r);
    }

    RefreshList();
    return true;
  } catch (const Exception &e) {
    Application->MessageBox((L"Ne mogu učitati XML:\n" + e.Message).c_str(),
                            L"Greška", MB_OK | MB_ICONERROR);
    return false;
  }
}

// ------ Učitavanje XML ------
void __fastcall TFReceptar::mnuLoadXMLClick(TObject *Sender) {
  std::unique_ptr<TOpenDialog> od(new TOpenDialog(this));
  od->Filter = L"XML files (*.xml)|*.xml|All files (*.*)|*.*";
  od->InitialDir = ExtractFilePath(Application->ExeName);
  od->FileName = L"PopisRecepata.xml";
  if (od->Execute()) {
    DataXmlPath = od->FileName;
    LoadRecipesXML(DataXmlPath);
  }
}

bool TFReceptar::SaveRecipesXML(const UnicodeString &filePath) {
  try {
    _di_IXMLrecipesType recipes = Newrecipes();

    for (const auto &r : Recepti) {
      _di_IXMLrecipeType rn = recipes->Add();
      rn->Set_name(r.Naziv);
      rn->Set_difficulty(r.Tezina);

      for (const auto &s : r.Sastojci) {
        _di_IXMLingredientType in = rn->Add();
        in->SetAttribute(L"qty", FloatToStr(s.Kolicina));
        in->Text = s.Naziv;
      }
    }

    recipes->OwnerDocument->SaveToFile(filePath);
    return true;
  } catch (const Exception &e) {
    Application->MessageBox((L"Ne mogu spremiti XML:\n" + e.Message).c_str(),
                            L"Greška", MB_OK | MB_ICONERROR);
    return false;
  }
}

void __fastcall TFReceptar::mnuRatingsClick(TObject *Sender) {
  std::unique_ptr<TFRatingForm> dlg(new TFRatingForm(this));
  gSettings.ApplyLookAndFeel(dlg.get());

  // Postavi jezik i dostupne recepte
  dlg->SetLanguage(currentLanguage);
  dlg->SetAvailableRecipes(GetRecipeNames());

  dlg->ShowModal();
}
std::vector<UnicodeString> TFReceptar::GetRecipeNames() const {
  std::vector<UnicodeString> names;
  for (const auto &recept : Recepti) {
    names.push_back(recept.Naziv);
  }
  return names;
}
void __fastcall TFReceptar::FormClose(TObject *Sender, TCloseAction &Action) {
  gSettings.CaptureFromForm(this);
  gSettings.SaveIni(gIniPath);
  gSettings.SaveRegistry();

  if (!DataXmlPath.IsEmpty()) SaveRecipesXML(DataXmlPath);
}

void __fastcall TFReceptar::mnuNutritionClick(TObject *Sender) {
  // Provjeri da li postoje recepti
  if (Recepti.empty()) {
    const UnicodeString msg =
        (currentLanguage == L"HR")
            ? L"Nema recepata za dodavanje nutritivnih informacija.\n\n"
              L"Prvo dodajte neki recept pomoću gumba 'Dodaj novi recept'."
            : L"No recipes available for nutrition information.\n\n"
              L"First add some recipes using 'Add new recipe' button.";
    Application->MessageBox(
        msg.c_str(),
        (currentLanguage == L"HR") ? L"Nema recepata" : L"No recipes",
        MB_OK | MB_ICONINFORMATION);
    return;
  }

  // Kreiraj i prikaži Nutrition formu
  std::unique_ptr<TFNutritionForm> dlg(new TFNutritionForm(this));
  gSettings.ApplyLookAndFeel(dlg.get());

  // Postavi jezik i dostupne recepte
  dlg->SetLanguage(currentLanguage);
  dlg->SetAvailableRecipes(GetRecipeNames());

  // Prikaži informacije o binarnim datotekama (za zadatak)
  if (currentLanguage == L"HR") {
    const UnicodeString info =
        L"BINARNE DATOTEKE (.nut format)\n\n"
        L"• Strukturirani podaci o nutritivnim vrijednostima\n"
        L"• Fixed-size struktura za efikasne I/O operacije\n"
        L"• CRUD operacije: Create, Read, Update, Delete\n"
        L"• Različito od XML/JSON struktura recepata\n\n"
        L"Datoteka: nutrition_data.nut";
    Application->MessageBox(info.c_str(),
                            L"Nutritivne informacije - Binarni format",
                            MB_OK | MB_ICONINFORMATION);
  }

  dlg->ShowModal();
}
void __fastcall TFReceptar::mnuMealPlansClick(TObject *Sender) {
  try {
    std::unique_ptr<TFMealPlans> dlg(new TFMealPlans(this));
    dlg->ShowModal();
  } catch (const Exception &e) {
    Application->MessageBox(
        (L"Ne mogu otvoriti formu Planovi prehrane:\n" + e.Message).c_str(),
        L"Greška", MB_OK | MB_ICONERROR);
  }
}

