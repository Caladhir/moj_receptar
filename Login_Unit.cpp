// Login_Unit.cpp
#include <vcl.h>
#pragma hdrstop

#include "Login_Unit.h"

#include <TypInfo.hpp>
#include <map>

#include "Settings.h"

#pragma package(smart_init)
#pragma resource "*.dfm"
extern AppSettings gSettings;

TFLoginForm* FLoginForm;

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

__fastcall TFLoginForm::TFLoginForm(TComponent* Owner) : TForm(Owner) {
  translation[L"Caption"] = {{L"HR", L"Prijava"}, {L"EN", L"Login"}};
  translation[L"LblUser"] = {{L"HR", L"Korisnik"}, {L"EN", L"Username"}};
  translation[L"LblPass"] = {{L"HR", L"Lozinka"}, {L"EN", L"Password"}};
  translation[L"BtnOK"] = {{L"HR", L"Prijavi se"}, {L"EN", L"Sign in"}};
  translation[L"BtnCancel"] = {{L"HR", L"Odustani"}, {L"EN", L"Cancel"}};
  translation[L"CheckBoxRemember"] = {{L"HR", L"Zapamti me"},
                                      {L"EN", L"Remember me"}};

  EditUser->TextHint =
      (Lang == L"HR") ? L"Unesite korisnièko ime" : L"Enter username";
  EditPass->TextHint = (Lang == L"HR") ? L"Unesite lozinku" : L"Enter password";
}

void TFLoginForm::UpdateOkEnabled() {
  BtnOK->Enabled =
      !(EditUser->Text.Trim().IsEmpty() || EditPass->Text.Trim().IsEmpty());
}

void __fastcall TFLoginForm::FormShow(TObject* Sender) {
  EditPass->PasswordChar = L'*';
  UpdateOkEnabled();
  translateForm(this, Lang, translation);

  // --- Remember me (iz registryja) ---
  bool remembered = gSettings.GetRememberUser();
  CheckBoxRemember->Checked = remembered;
  if (remembered) {
    EditUser->Text = gSettings.GetLastUser();  // prefill user
  }
}

void __fastcall TFLoginForm::EditUserChange(TObject* Sender) {
  UpdateOkEnabled();
}
void __fastcall TFLoginForm::EditPassChange(TObject* Sender) {
  UpdateOkEnabled();
}

void __fastcall TFLoginForm::BtnOKClick(TObject* Sender) {
  const bool remember = CheckBoxRemember->Checked;
  gSettings.SetRememberUser(remember);
  if (remember)
    gSettings.SetLastUser(EditUser->Text);  // ili EditUsername->Text
  else
    gSettings.SetLastUser(L"");  // oèisti ako ne želi pamtiti

  gSettings.SaveRegistry();
}
//---------------------------------------------------------------------------

