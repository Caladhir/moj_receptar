// NutritionForm.h - Nutritivne informacije recepata (BIN .nut)
#ifndef NutritionFormH
#define NutritionFormH

#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <map>
#include <vector>

#pragma pack(push, 1)
class TNutritionInfo {
 public:
  char RecipeName[100];  // naziv recepta (ANSI/UTF-8 iz UnicodeStringa)
  double Calories;       // kcal
  double Proteins;       // g
  double Carbohydrates;  // g
  double Fats;           // g
  double Fiber;          // g
  double Sugar;          // g
  double Salts;          // g

  TNutritionInfo() { Clear(); }
  void Clear() { memset(this, 0, sizeof(TNutritionInfo)); }

  void SetRecipeName(const UnicodeString &name) {
    AnsiString a = AnsiString(name);
    strncpy(RecipeName, a.c_str(), sizeof(RecipeName) - 1);
    RecipeName[sizeof(RecipeName) - 1] = '\0';
  }

  UnicodeString GetRecipeName() const { return UnicodeString(RecipeName); }
  bool IsValid() const { return strlen(RecipeName) > 0; }
};
#pragma pack(pop)

class TFNutritionForm : public TForm {
 __published:
  // --- kontrole (kao u DFM-u) ---
  TPanel *PanelInput;
  TComboBox *CombRecipes;
  TLabel *LblRecipe;

  TLabel *LblCalories;
  TEdit *EditCalories;
  TLabel *LblProteins;
  TEdit *EditProteins;
  TLabel *LblCarbs;
  TEdit *EditCarbs;
  TLabel *LblFats;
  TEdit *EditFats;
  TLabel *LblFiber;
  TEdit *EditFiber;
  TLabel *LblSugar;
  TEdit *EditSugar;
  TLabel *LblSalts;
  TEdit *EditSalts;

  TListView *LVNutrition;

  TPanel *PanelButtons;
  TButton *BtnAdd;
  TButton *BtnEdit;
  TButton *BtnDelete;

  // --- eventi iz DFM-a ---
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall BtnAddClick(TObject *Sender);
  void __fastcall BtnEditClick(TObject *Sender);
  void __fastcall BtnDeleteClick(TObject *Sender);
  void __fastcall LVNutritionSelectItem(TObject *Sender, TListItem *Item,
                                        bool Selected);

 private:
  // Podaci i stanje
  std::vector<TNutritionInfo> NutritionData;
  std::vector<UnicodeString> AvailableRecipes;
  UnicodeString BinaryFilePath{L""};
  UnicodeString CurrentLanguage{L"HR"};

  std::map<UnicodeString, std::map<UnicodeString, UnicodeString>> translation;

  // Binarne operacije
  bool LoadNutritionBinary(const UnicodeString &filePath);
  bool SaveNutritionBinary(const UnicodeString &filePath);
  bool DeleteNutritionRecord(const UnicodeString &recipeName);

  // UI/Helper
  void RefreshNutritionList();
  void ClearForm();
  void LoadNutritionToForm(const TNutritionInfo &info);
  TNutritionInfo GetNutritionFromForm() const;
  void ApplyLanguage();
  void InitializeTranslations();
  int FindNutritionIndex(const UnicodeString &recipeName) const;

 public:
  __fastcall TFNutritionForm(TComponent *Owner);
  // API prema glavnoj formi
  void SetAvailableRecipes(const std::vector<UnicodeString> &recipes);
  void SetLanguage(const UnicodeString &lang) { CurrentLanguage = lang; }
  TNutritionInfo *GetNutritionForRecipe(const UnicodeString &recipeName);
  bool HasNutritionData(const UnicodeString &recipeName) const;
};

extern PACKAGE TFNutritionForm *FNutritionForm;

#endif

