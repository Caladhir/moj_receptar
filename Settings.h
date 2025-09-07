// Settings.h
#ifndef SettingsH
#define SettingsH

#include <System.hpp>
#include <Vcl.Forms.hpp>  // zbog TForm u metodama
#include <Vcl.Graphics.hpp>

class AppSettings {
 private:
  UnicodeString Language{L"HR"};
  UnicodeString FontName{L"Segoe UI"};  // ← DODANO
  int FontSize{10};
  TColor Background{clBtnFace};
  bool RememberUser{false};
  UnicodeString LastUser{L""};
  int WinLeft{100};
  int WinTop{100};
  int WinWidth{800};
  int WinHeight{600};

 public:
  // --- GET / SET ---
  void SetLanguage(const UnicodeString& lang) { Language = lang; }
  UnicodeString GetLanguage() const { return Language; }

  void SetFontName(const UnicodeString& name) { FontName = name; }  // ← DODANO
  UnicodeString GetFontName() const { return FontName; }  // ← DODANO

  void SetFontSize(int size) {
    if (size > 0) FontSize = size;
  }
  int GetFontSize() const { return FontSize; }

  void SetBackground(TColor c) { Background = c; }
  TColor GetBackground() const { return Background; }

  void SetRememberUser(bool r) { RememberUser = r; }
  bool GetRememberUser() const { return RememberUser; }

  void SetLastUser(const UnicodeString& u) { LastUser = u; }
  UnicodeString GetLastUser() const { return LastUser; }

  void SetWindow(int l, int t, int w, int h) {
    WinLeft = l;
    WinTop = t;
    WinWidth = w;
    WinHeight = h;
  }
  void GetWindow(int& l, int& t, int& w, int& h) const {
    l = WinLeft;
    t = WinTop;
    w = WinWidth;
    h = WinHeight;
  }

  // --- IO ---
  void LoadIni(const UnicodeString& iniPath);
  void SaveIni(const UnicodeString& iniPath) const;
  void LoadRegistry();
  void SaveRegistry() const;

  // --- Primjena na formu ---
  void ApplyToForm(TForm* form) const;
  void CaptureFromForm(TForm* form);
  void ApplyLookAndFeel(TForm* form) const;  // ← DODANO (postoji u .cpp)
};

#endif

