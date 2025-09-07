#ifndef PostavkeFormH
#define PostavkeFormH

#include <IdAuthentication.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdIOHandler.hpp>
#include <IdIOHandlerSocket.hpp>
#include <IdIOHandlerStack.hpp>
#include <IdIntercept.hpp>
#include <IdInterceptThrottler.hpp>
#include <IdSSL.hpp>
#include <IdSSLOpenSSL.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>

class TFPostavkeForm : public TForm {
 __published:
  TLabel *LblJezik;
  TComboBox *CmbLang;

  TLabel *LblFont;
  TEdit *EdtFont;  // prikaz izabranog fonta (read-only)
  TButton *BtnFont;

  TButton *BtnOK;
  TButton *BtnCancel;
  TLabel *LblBoja;
  TColorBox *CBColor;

  TGroupBox *GBTerms;
  TButton *BtnDownloadTerms;
  TButton *BtnOpenTerms;
  TProgressBar *PBDownload;
  TLabel *LblPct;
  TLabel *LblLimit;
  TComboBox *CmbLimit;

  TIdHTTP *Http;
  TIdSSLIOHandlerSocketOpenSSL *SSL;
  TIdInterceptThrottler *Throttle;

  void __fastcall FormShow(TObject *Sender);
  void __fastcall BtnFontClick(TObject *Sender);

  void __fastcall BtnDownloadTermsClick(TObject *Sender);
  void __fastcall BtnOpenTermsClick(TObject *Sender);
  void __fastcall CmbLimitChange(TObject *Sender);
  void __fastcall HttpWorkBegin(TObject *ASender, TWorkMode AWorkMode,
                                __int64 AWorkCountMax);
  void __fastcall HttpWork(TObject *ASender, TWorkMode AWorkMode,
                           __int64 AWorkCount);
  void __fastcall HttpWorkEnd(TObject *ASender, TWorkMode AWorkMode);

 private:
  UnicodeString CurrentLang{L"HR"};
  UnicodeString CurrentFont{L""};
  int CurrentFontSize{10};
  TColor CurrentColor{clBtnFace};

  __int64 FTotal{0};
  UnicodeString TermsUrl{
      L"https://drive.google.com/"
      L"uc?export=download&id=1Hf8ShyndgkTTsNwz2cAq2acuARUMNPIv"};
  UnicodeString TermsPath;

  static __int64 KBsToBitsPerSec(int kb) {
    return (__int64)kb * 1024 * 8;  //  bps
  }

  void UpdateTosUI();

 public:
  __fastcall TFPostavkeForm(TComponent *Owner);

  void SetCurrentLanguage(const UnicodeString &code) { CurrentLang = code; }
  UnicodeString GetSelectedLanguage() const {
    return (CmbLang->ItemIndex == 1) ? L"EN" : L"HR";
  }

  // PostavkeForm.h
  void SetCurrentFont(const UnicodeString &name, int size = 10) {
    CurrentFont = name;
    CurrentFontSize = size;
  }

  UnicodeString GetSelectedFont() const { return CurrentFont; }
  int GetSelectedFontSize() const { return CurrentFontSize; }

  void SetCurrentColor(TColor c) { CurrentColor = c; }
  TColor GetSelectedColor() const {
    return CBColor ? CBColor->Selected : CurrentColor;
  }
};

extern PACKAGE TFPostavkeForm *FPostavkeForm;
#endif

