#ifndef ReceptarH
#define ReceptarH

#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Xml.XMLDoc.hpp>
#include <Xml.XMLIntf.hpp>
#include <Xml.xmldom.hpp>
#include <vector>

#include "MealPlansForm.h"
#include "ReceptForm.h"
#include "Settings.h"

// globalne postavke (definirane u Receptar.cpp)
extern AppSettings gSettings;
extern UnicodeString gIniPath;

class TFReceptar : public TForm {
 __published:  // vizualne komponente + EVENTI
  TListView *LVRecipes;
  TButton *BtnAdd;
  TButton *BtnEdit;
  TButton *BtnDelete;
  TMainMenu *MainMenu1;
  TMenuItem *mnuPrijava;
  TMenuItem *mnuPostavke;
  TMenuItem *mnuRatings;
  TMenuItem *mnuNutrition;
  TMenuItem *mnuMealPlans;

  TLabel *LblLogged;
  TXMLDocument *XMLDocument1;
  TMenuItem *mnuLoadXML;

  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall BtnAddClick(TObject *Sender);
  void __fastcall BtnEditClick(TObject *Sender);
  void __fastcall BtnDeleteClick(TObject *Sender);
  void __fastcall mnuPrijavaClick(TObject *Sender);
  void __fastcall mnuPostavkeClick(TObject *Sender);
  void __fastcall LVRecipesKeyDown(TObject *Sender, WORD &Key,
                                   TShiftState Shift);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall mnuLoadXMLClick(TObject *Sender);
  void __fastcall mnuRatingsClick(TObject *Sender);
  void __fastcall mnuNutritionClick(TObject *Sender);
  void __fastcall mnuMealPlansClick(TObject *Sender);

 private:
  std::vector<TRecept> Recepti;
  UnicodeString currentLanguage{L"HR"};
  bool LoggedIn{false};
  UnicodeString CurrentUser;

  UnicodeString DataXmlPath;
  bool LoadRecipesXML(const UnicodeString &filePath);
  bool SaveRecipesXML(const UnicodeString &filePath);

  void RefreshList();
  bool EditRecept(TRecept &r);
  void ApplyLanguage();
  void UpdateLoginLabel();
  void UpdateAccess();

 public:
  __fastcall TFReceptar(TComponent *Owner);
  std::vector<UnicodeString> GetRecipeNames() const;
};

extern PACKAGE TFReceptar *FReceptar;
#endif

