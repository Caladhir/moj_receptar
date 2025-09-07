#include <vcl.h>
#pragma hdrstop
#include "PostavkeForm.h"

#include <IdSSLOpenSSLHeaders.hpp>
#include <IniFiles.hpp>
#include <System.IOUtils.hpp>
#include <System.SysUtils.hpp>
#include <TypInfo.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Winapi.ShellAPI.hpp>
#include <map>
#include <memory>
#pragma package(smart_init)
#pragma resource "*.dfm"

TFPostavkeForm* FPostavkeForm;

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

__fastcall TFPostavkeForm::TFPostavkeForm(TComponent* Owner) : TForm(Owner) {
  TermsPath =
      IncludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName)) +
      L"Terms_of_service.pdf";

  translation[L"Caption"] = {{L"HR", L"Postavke"}, {L"EN", L"Settings"}};
  translation[L"LblJezik"] = {{L"HR", L"Jezik"}, {L"EN", L"Language"}};
  translation[L"LblFont"] = {{L"HR", L"Font"}, {L"EN", L"Font"}};
  translation[L"BtnFont"] = {{L"HR", L"Odaberi…"}, {L"EN", L"Choose…"}};
  translation[L"BtnOK"] = {{L"HR", L"U redu"}, {L"EN", L"OK"}};
  translation[L"BtnCancel"] = {{L"HR", L"Odustani"}, {L"EN", L"Cancel"}};
  translation[L"LblBoja"] = {{L"HR", L"Boja pozadine"}, {L"EN", L"Background"}};
}

void __fastcall TFPostavkeForm::FormShow(TObject* Sender) {
  IdOpenSSLSetLibPath(ExtractFilePath(Application->ExeName));

  SSL->SSLOptions->Method = sslvTLSv1_2;
  SSL->SSLOptions->Mode = sslmClient;

  // jezici
  CmbLang->Items->BeginUpdate();
  CmbLang->Items->Clear();
  CmbLang->Items->Add(L"HR");
  CmbLang->Items->Add(L"EN");
  CmbLang->Items->EndUpdate();
  CmbLang->Style = csDropDownList;
  CmbLang->ItemIndex = (CurrentLang.UpperCase() == L"EN") ? 1 : 0;

  // prikaz trenutnog fonta (ako nije postavljen, preuzmi s forme)
  if (CurrentFont.Trim().IsEmpty()) CurrentFont = Font->Name;
  if (EdtFont) {
    EdtFont->ReadOnly = true;
    EdtFont->Text = CurrentFont + L", " + IntToStr(CurrentFontSize) + L" pt";
  }

  //  trenutnu boju
  if (CBColor) CBColor->Selected = CurrentColor;

  translateForm(this, CurrentLang, translation);

  if (CmbLimit->Items->Count == 0) {
    CmbLimit->Items->Add(L"50 KB/s");
    CmbLimit->Items->Add(L"100 KB/s");
    CmbLimit->Items->Add(L"500 KB/s");
    CmbLimit->Items->Add(L"1000 KB/s");
  }
  if (CmbLimit->ItemIndex < 0) CmbLimit->ItemIndex = 2;  // default 500 KB/s

  Http->Request->UserAgent = "Mozilla/5.0 (compatible; Indy Library)";
  Http->ReadTimeout = 15000;
  Http->ConnectTimeout = 10000;

  SSL->SSLOptions->Method = sslvTLSv1_2;
  SSL->SSLOptions->Mode = sslmClient;
  Http->Request->UserAgent = "Mozilla/5.0 (compatible; Indy Library)";

  // Init UI
  PBDownload->Position = 0;
  LblPct->Caption = L"0%";
}

void __fastcall TFPostavkeForm::BtnFontClick(TObject* Sender) {
  std::unique_ptr<TFontDialog> fd(new TFontDialog(this));
  // inicijalni font u dijalogu:
  fd->Font->Assign(Font);  // trenutni font forme
  if (!CurrentFont.Trim().IsEmpty()) fd->Font->Name = CurrentFont;

  if (fd->Execute()) {
    CurrentFont = fd->Font->Name;
    CurrentFontSize = fd->Font->Size;
    if (EdtFont)
      EdtFont->Text = CurrentFont + L", " + IntToStr(CurrentFontSize) + L" pt";

    this->Font->Assign(fd->Font);  // mali preview
  }
}

//////TERMS AND SERVICES

void __fastcall TFPostavkeForm::BtnDownloadTermsClick(TObject* Sender) {
  // Primijeni odabrani limit na throttler
  switch (CmbLimit->ItemIndex) {
    case 0:
      Throttle->BitsPerSec = KBsToBitsPerSec(50);
      break;
    case 1:
      Throttle->BitsPerSec = KBsToBitsPerSec(100);
      break;
    case 2:
      Throttle->BitsPerSec = KBsToBitsPerSec(500);
      break;
    case 3:
      Throttle->BitsPerSec = KBsToBitsPerSec(1000);
      break;
    default:
      Throttle->BitsPerSec = KBsToBitsPerSec(500);
      break;  // fallback
  }

  PBDownload->Position = 0;
  LblPct->Caption = L"0%";
  FTotal = 0;

  try {
    std::unique_ptr<TFileStream> fs(
        new TFileStream(TermsPath, fmCreate | fmShareDenyNone));
    Http->Get(TermsUrl, fs.get());

    Application->MessageBox((CurrentLang == L"HR" ? L"Preuzimanje završeno."
                                                  : L"Download complete."),
                            L"Info", MB_OK | MB_ICONINFORMATION);

    ShellExecute(0, L"open", TermsPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
  } catch (const Exception& e) {
    UnicodeString msg = (CurrentLang == L"HR")
                            ? L"Greška pri preuzimanju:\n" + e.Message
                            : L"Download error:\n" + e.Message;
    Application->MessageBox(msg.c_str(), L"Error", MB_OK | MB_ICONERROR);
  }
}

void __fastcall TFPostavkeForm::BtnOpenTermsClick(TObject* Sender) {
  if (!FileExists(TermsPath)) {
    Application->MessageBox(
        (CurrentLang == L"HR" ? L"PDF nije pronađen. Preuzmite ga prvo."
                              : L"PDF not found. Please download it first."),
        L"Info", MB_OK | MB_ICONINFORMATION);
    return;
  }
  ShellExecute(0, L"open", TermsPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void __fastcall TFPostavkeForm::CmbLimitChange(TObject* Sender) {
  // Pripremi limit odmah (nije nužno, ali održava konzistentnost)
  switch (CmbLimit->ItemIndex) {
    case 0:
      Throttle->BitsPerSec = KBsToBitsPerSec(50);
      break;
    case 1:
      Throttle->BitsPerSec = KBsToBitsPerSec(100);
      break;
    case 2:
      Throttle->BitsPerSec = KBsToBitsPerSec(500);
      break;
    case 3:
      Throttle->BitsPerSec = KBsToBitsPerSec(1000);
      break;
    default:
      Throttle->BitsPerSec = KBsToBitsPerSec(500);
      break;
  }
}

// ---------- Progress eventi ----------
void __fastcall TFPostavkeForm::HttpWorkBegin(TObject* ASender,
                                              TWorkMode AWorkMode,
                                              __int64 AWorkCountMax) {
  FTotal = AWorkCountMax;
  if (FTotal > 0) {
    PBDownload->Max = 100;
    PBDownload->Position = 0;
    LblPct->Caption = L"0%";
  } else {
    LblPct->Caption = L"...";
  }
  Application->ProcessMessages();
}

void __fastcall TFPostavkeForm::HttpWork(TObject* ASender, TWorkMode AWorkMode,
                                         __int64 AWorkCount) {
  if (FTotal > 0) {
    int pct = (int)((AWorkCount * 100) / FTotal);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    PBDownload->Position = pct;
    LblPct->Caption = IntToStr(pct) + L"%";
  } else {
    LblPct->Caption = L"...";
  }
  Application->ProcessMessages();
}

void __fastcall TFPostavkeForm::HttpWorkEnd(TObject* ASender,
                                            TWorkMode AWorkMode) {
  if (FTotal > 0) {
    PBDownload->Position = 100;
    LblPct->Caption = L"100%";
  }
}

