// ReceptForm.cpp
#include <vcl.h>
#pragma hdrstop
#include "ReceptForm.h"

#include <Vcl.ComCtrls.hpp>
#include <Vcl.StdCtrls.hpp>

#pragma link "Vcl.StdCtrls"
#pragma link "Vcl.ComCtrls"

#include <System.SysUtils.hpp>
#include <TypInfo.hpp>
#include <map>
#pragma package(smart_init)
#pragma resource "*.dfm"

TFReceptForm* FReceptForm;

using std::map;
static map<String, map<String, String>> translation;

static void translateForm(TForm* Form, const String& Language,
                          const map<String, map<String, String>>& tr) {
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

__fastcall TFReceptForm::TFReceptForm(TComponent* Owner) : TForm(Owner) {
  translation[L"Caption"] = {{L"HR", L"Recept"}, {L"EN", L"Recipe"}};
  translation[L"LblNaziv"] = {{L"HR", L"Naziv recepta"},
                              {L"EN", L"Name of the recipe"}};
  translation[L"LblTezina"] = {{L"HR", L"Težina recepta \n (1-5)"},
                               {L"EN", L"Difficulty of the recipe \n (1-5)"}};
  translation[L"LblSastojak"] = {{L"HR", L"Sastojak"}, {L"EN", L"Ingredient"}};
  translation[L"LblKolicina"] = {{L"HR", L"Količina (g ili ml)"},
                                 {L"EN", L"Amount(g or ml)"}};
  translation[L"BtnAddS"] = {{L"HR", L"Dodaj sastojak"},
                             {L"EN", L"Add ingredient"}};
  translation[L"BtnRemoveS"] = {{L"HR", L"Ukloni sastojak"},
                                {L"EN", L"Remove ingredient"}};

  translation[L"BtnOK"] = {{L"HR", L"Spremi recept"}, {L"EN", L"Save recipe"}};
  translation[L"BtnCancel"] = {{L"HR", L"Odustani"}, {L"EN", L"Cancel"}};

  EditNaziv->TextHint = (Lang == L"HR") ? L"Unesite naziv" : L"Enter name";
  EditSastojak->TextHint =
      (Lang == L"HR") ? L"Unesite sastojak" : L"Enter ingredient";
  EditKolicina->TextHint =
      (Lang == L"HR") ? L"Unesite količinu" : L"Enter amount";
}

void __fastcall TFReceptForm::FormShow(TObject* Sender) {
  translateForm(this, Lang, translation);

  EditNaziv->TextHint = (Lang == L"HR") ? L"Unesite naziv" : L"Enter name";
  EditSastojak->TextHint =
      (Lang == L"HR") ? L"Unesite sastojak" : L"Enter ingredient";
  EditKolicina->TextHint =
      (Lang == L"HR") ? L"Unesite količinu" : L"Enter amount";

  BtnAddS->ShowHint = true;
  BtnRemoveS->ShowHint = true;
  LBItems->ShowHint = true;
  TrackTezina->ShowHint = true;

  BtnAddS->Hint = (Lang == L"HR") ? L"Dodaj sastojak u recept"
                                  : L"Add ingredient to the recipe";
  BtnRemoveS->Hint = (Lang == L"HR") ? L"Ukloni odabrani sastojak iz recepta"
                                     : L"Remove selected ingredient";
  LBItems->Hint = (Lang == L"HR") ? L"Popis sastojaka" : L"Ingredients list";
  TrackTezina->Hint =
      (Lang == L"HR") ? L"Odaberi težinu 1–5" : L"Choose difficulty 1–5";
}
void TFReceptForm::Reset() {
  Temp = TRecept();
  EditNaziv->Clear();
  TrackTezina->Position = 1;
  EditSastojak->Clear();
  EditKolicina->Clear();
  LBItems->Items->Clear();
}
void TFReceptForm::LoadFromRecept(const TRecept& r) {
  Temp = r;
  EditNaziv->Text = r.Naziv;
  TrackTezina->Position = r.Tezina;
  LBItems->Items->Clear();
  for (auto& s : r.Sastojci) LBItems->Items->Add(s.ToString());
}

void TFReceptForm::SaveToRecept(TRecept& out) const {
  out = Temp;  // kopiramo sastojke
  out.Naziv = EditNaziv->Text;
  out.Tezina = TrackTezina->Position;
}

void __fastcall TFReceptForm::BtnAddSClick(TObject* Sender) {
  TSastojak s{EditSastojak->Text,
              static_cast<double>(StrToFloatDef(EditKolicina->Text, 0))};
  if (s.Validiraj()) {
    Temp.AddSastojak(s);  // METODA ZA DODAVANJE
    LBItems->Items->Add(s.ToString());
    EditSastojak->Clear();
    EditKolicina->Clear();
  } else {
    const UnicodeString msg = (Lang == L"HR")
                                  ? L"Neispravan sastojak ili količina."
                                  : L"Invalid ingredient or amount.";
    const UnicodeString cap = (Lang == L"HR") ? L"Greška" : L"Error";
    Application->MessageBox(msg.c_str(), cap.c_str(), MB_OK | MB_ICONWARNING);
  }
}

void __fastcall TFReceptForm::BtnRemoveSClick(TObject* Sender) {
  int idx = LBItems->ItemIndex;
  if (idx < 0) {
    const UnicodeString msg =
        (Lang == L"HR") ? L"Odaberite sastojak." : L"Select an ingredient.";
    const UnicodeString cap = (Lang == L"HR") ? L"Info" : L"Info";
    Application->MessageBox(msg.c_str(), cap.c_str(),
                            MB_OK | MB_ICONINFORMATION);
    return;
  }
  if (Temp.RemoveSastojak(idx)) {  // METODA ZA BRISANJE
    LBItems->Items->Delete(idx);
  }
}

void __fastcall TFReceptForm::BtnOKClick(TObject* Sender) {
  if (EditNaziv->Text.Trim().IsEmpty()) {
    const UnicodeString msg =
        (Lang == L"HR") ? L"Unesite naziv recepta." : L"Enter the recipe name.";
    const UnicodeString cap = (Lang == L"HR") ? L"Greška" : L"Error";
    Application->MessageBox(msg.c_str(), cap.c_str(), MB_OK | MB_ICONWARNING);
    return;
  }
  if (Temp.Sastojci.empty()) {
    const UnicodeString q = (Lang == L"HR")
                                ? L"Nema sastojaka. Spremiti svejedno?"
                                : L"No ingredients. Save anyway?";
    const UnicodeString cap = (Lang == L"HR") ? L"Potvrda" : L"Confirm";
    if (Application->MessageBox(q.c_str(), cap.c_str(),
                                MB_YESNO | MB_ICONQUESTION) != IDYES)
      return;
  }

  ModalResult = mrOk;
}

