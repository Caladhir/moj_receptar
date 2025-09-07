#include "Settings.h"

#include <Registry.hpp>
#include <System.IniFiles.hpp>
#include <memory>

static const UnicodeString REG_KEY = L"Software\\MojReceptar";

// ---------------- INI ----------------
void AppSettings::LoadIni(const UnicodeString& p) {
  std::unique_ptr<TIniFile> ini(new TIniFile(p));
  Language = ini->ReadString(L"UI", L"Language", Language);
  FontName = ini->ReadString(L"UI", L"FontName", FontName);
  FontSize = ini->ReadInteger(L"UI", L"FontSize", FontSize);
  Background = (TColor)ini->ReadInteger(L"UI", L"Background", (int)Background);

  WinLeft = ini->ReadInteger(L"MainForm", L"Left", WinLeft);
  WinTop = ini->ReadInteger(L"MainForm", L"Top", WinTop);
  WinWidth = ini->ReadInteger(L"MainForm", L"Width", WinWidth);
  WinHeight = ini->ReadInteger(L"MainForm", L"Height", WinHeight);
}

void AppSettings::SaveIni(const UnicodeString& p) const {
  std::unique_ptr<TIniFile> ini(new TIniFile(p));
  ini->WriteString(L"UI", L"Language", Language);
  ini->WriteString(L"UI", L"FontName", FontName);
  ini->WriteInteger(L"UI", L"FontSize", FontSize);
  ini->WriteInteger(L"UI", L"Background", (int)Background);

  ini->WriteInteger(L"MainForm", L"Left", WinLeft);
  ini->WriteInteger(L"MainForm", L"Top", WinTop);
  ini->WriteInteger(L"MainForm", L"Width", WinWidth);
  ini->WriteInteger(L"MainForm", L"Height", WinHeight);
}

// -------------- Registry --------------
void AppSettings::LoadRegistry() {
  std::unique_ptr<TRegistry> r(new TRegistry(KEY_READ));
  r->RootKey = HKEY_CURRENT_USER;
  if (r->OpenKeyReadOnly(REG_KEY)) {
    RememberUser = r->ReadBool(L"RememberUser");
    LastUser = r->ReadString(L"LastUser");
  }
}

void AppSettings::SaveRegistry() const {
  std::unique_ptr<TRegistry> r(new TRegistry(KEY_WRITE));
  r->RootKey = HKEY_CURRENT_USER;
  if (r->OpenKey(REG_KEY, true)) {
    r->WriteBool(L"RememberUser", RememberUser);
    r->WriteString(L"LastUser", LastUser);
  }
}

// --------- Primjena na formu ----------
void AppSettings::ApplyToForm(TForm* f) const {
  if (!f) return;
  f->Font->Name = FontName;
  f->Font->Size = FontSize;
  f->Color = Background;

  f->WindowState = wsNormal;  // da naša geo vrijedi
  f->Left = WinLeft;
  f->Top = WinTop;
  f->Width = WinWidth;
  f->Height = WinHeight;
}

void AppSettings::CaptureFromForm(TForm* f) {
  if (!f) return;
  // Ako je maximiziran, ne želimo spremiti ogromne dimenzije
  if (f->WindowState == wsNormal) {
    WinLeft = f->Left;
    WinTop = f->Top;
    WinWidth = f->Width;
    WinHeight = f->Height;
  }
  FontName = f->Font->Name;
  FontSize = f->Font->Size;
  Background = f->Color;
}

void AppSettings::ApplyLookAndFeel(TForm* f) const {
  if (!f) return;
  f->Font->Name = FontName;
  f->Font->Size = FontSize;
  f->Color = Background;
}

