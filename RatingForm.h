// RatingForm.h - Pojednostavljena verzija za ocjene recepata (JSON)
#ifndef RatingFormH
#define RatingFormH

#include <System.Classes.hpp>
#include <System.DateUtils.hpp>
#include <System.JSON.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <vector>

class TRecipeRating {
 public:
  int Id{0};
  UnicodeString RecipeName;
  UnicodeString UserName{L"Korisnik"};
  int Rating{1};
  UnicodeString Comment;
  TDateTime DateCreated;

  TRecipeRating() { DateCreated = Now(); }

  TRecipeRating(int id, const UnicodeString &recipe, int rating,
                const UnicodeString &comment)
      : Id(id), RecipeName(recipe), Rating(rating), Comment(comment) {
    DateCreated = Now();
    UserName = L"Korisnik";
  }

  // Validacija ocjene
  bool Validate() const {
    return Id > 0 && !RecipeName.Trim().IsEmpty() && Rating >= 1 && Rating <= 5;
  }

  // Formatiranje za prikaz
  UnicodeString ToString() const {
    return RecipeName + L" - " + IntToStr(Rating) + L"/5";
  }

  UnicodeString GetDateString() const {
    return FormatDateTime(L"dd.mm.yyyy hh:nn", DateCreated);
  }

  // JSON SERIALIZACIJA:

  // Pretvori u JSON objekt
  TJSONObject *ToJSON() const {
    TJSONObject *obj = new TJSONObject();
    obj->AddPair(L"id", new TJSONNumber(Id));
    obj->AddPair(L"recipeName", new TJSONString(RecipeName));
    obj->AddPair(L"userName", new TJSONString(UserName));
    obj->AddPair(L"rating", new TJSONNumber(Rating));
    obj->AddPair(L"comment", new TJSONString(Comment));
    obj->AddPair(L"date", new TJSONString(FormatDateTime(
                              L"yyyy-mm-dd\"T\"hh:nn:ss", DateCreated)));
    return obj;
  }

  // Uèitaj iz JSON objekta
  static TRecipeRating FromJSON(TJSONObject *obj) {
    TRecipeRating rating;

    if (auto idVal = dynamic_cast<TJSONNumber *>(obj->GetValue(L"id"))) {
      rating.Id = idVal->AsInt;
    }
    if (auto nameVal =
            dynamic_cast<TJSONString *>(obj->GetValue(L"recipeName"))) {
      rating.RecipeName = nameVal->Value();
    }
    if (auto userVal =
            dynamic_cast<TJSONString *>(obj->GetValue(L"userName"))) {
      rating.UserName = userVal->Value();
    }
    if (auto ratingVal =
            dynamic_cast<TJSONNumber *>(obj->GetValue(L"rating"))) {
      rating.Rating = ratingVal->AsInt;
    }
    if (auto commentVal =
            dynamic_cast<TJSONString *>(obj->GetValue(L"comment"))) {
      rating.Comment = commentVal->Value();
    }
    if (auto dateVal = dynamic_cast<TJSONString *>(obj->GetValue(L"date"))) {
      try {
        rating.DateCreated = ISO8601ToDate(dateVal->Value());
      } catch (...) {
        rating.DateCreated = Now();
      }
    }

    return rating;
  }
};

class TFRatingForm : public TForm {
 __published:
  // Paneli
  TPanel *PanelInput;
  TPanel *PanelButtons;

  // ListView za prikaz ocjena
  TListView *LVRatings;

  // Kontrole za unos ocjene
  TLabel *LblRecipe;
  TComboBox *CombRecipes;
  TLabel *LblRating;
  TTrackBar *TrackRating;
  TLabel *LblRatingValue;
  TLabel *LblComment;
  TMemo *MemoComment;

  // Gumbovi - SVA TRI GUMBA
  TButton *BtnAdd;     // Spremi novu ocjenu
  TButton *BtnEdit;    // Uredi postojeæu ocjenu
  TButton *BtnDelete;  // Zatvori dijalog

  // Event handleri
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall BtnAddClick(TObject *Sender);
  void __fastcall BtnEditClick(TObject *Sender);  // DODANO!
  void __fastcall BtnDeleteClick(TObject *Sender);
  void __fastcall TrackRatingChange(TObject *Sender);
  void __fastcall LVRatingsSelectItem(TObject *Sender, TListItem *Item,
                                      bool Selected);

 private:
  std::vector<TRecipeRating> Ratings;    // Sve ocjene
  UnicodeString JsonFilePath;            // Putanja do JSON datoteke
  UnicodeString CurrentLanguage{L"HR"};  // Jezik
  int NextId{1};                         // Sljedeæi ID za novu ocjenu

  // JSON CRUD operacije:
  bool LoadRatingsJSON(const UnicodeString &filePath);  // READ
  bool SaveRatingsJSON(const UnicodeString &filePath);  // CREATE/UPDATE
  // DELETE je implementiran kroz uklanjanje iz vektora i ponovni save

  // UI operacije
  void RefreshRatingsList();                // Osvježi prikaz
  void ClearForm();                         // Oèisti formu
  void LoadRatingToForm(int ratingId);      // DODANO - uèitaj ocjenu u formu
  TRecipeRating GetRatingFromForm() const;  // Uzmi podatke iz forme
  void ApplyLanguage();                     // Primijeni jezik
  int GetNextId() const { return NextId; }  // Vrati sljedeæi ID

 public:
  __fastcall TFRatingForm(TComponent *Owner);

  // Postavi dostupne recepte iz glavne aplikacije
  void SetAvailableRecipes(const std::vector<UnicodeString> &recipes);

  // Postavi jezik
  void SetLanguage(const UnicodeString &lang) { CurrentLanguage = lang; }
};

extern PACKAGE TFRatingForm *FRatingForm;
#endif

