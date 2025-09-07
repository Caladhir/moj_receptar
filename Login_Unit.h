// Login_Unit.h
#ifndef Login_UnitH
#define Login_UnitH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>

class TFLoginForm : public TForm {
 __published:
  TLabel *LblUser;
  TLabel *LblPass;
  TEdit *EditUser;
  TEdit *EditPass;
  TButton *BtnOK;
  TButton *BtnCancel;
  TCheckBox *CheckBoxRemember;

  void __fastcall FormShow(TObject *Sender);
  void __fastcall EditUserChange(TObject *Sender);
  void __fastcall EditPassChange(TObject *Sender);
  void __fastcall BtnOKClick(TObject *Sender);

 private:
  UnicodeString Lang{L"HR"};
  void UpdateOkEnabled();

 public:
  __fastcall TFLoginForm(TComponent *Owner);
  void SetLanguage(const UnicodeString &l) { Lang = l; }
  UnicodeString GetUserName() const { return EditUser->Text; }
  bool GetRememberMe() const { return CheckBoxRemember->Checked; }
};

extern PACKAGE TFLoginForm *FLoginForm;

#endif

