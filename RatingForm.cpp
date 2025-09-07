#include <vcl.h>
#pragma hdrstop
#include "RatingForm.h"

#include <System.DateUtils.hpp>
#include <System.IOUtils.hpp>
#include <System.JSON.hpp>
#include <System.SysUtils.hpp>
#include <TypInfo.hpp>
#include <map>
#include <memory>

#pragma package(smart_init)
#pragma resource "*.dfm"

TFRatingForm* FRatingForm;

using std::map;
static map<UnicodeString, map<UnicodeString, UnicodeString>> translation;

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

static void translateListViewHR(TListView* lv) {
  if (!lv || lv->Columns->Count < 4) return;
  lv->Columns->Items[0]->Caption = L"Recept";
  lv->Columns->Items[1]->Caption = L"Ocjena";
  lv->Columns->Items[2]->Caption = L"Komentar";
  lv->Columns->Items[3]->Caption = L"Datum";
}

static void translateListViewEN(TListView* lv) {
  if (!lv || lv->Columns->Count < 4) return;
  lv->Columns->Items[0]->Caption = L"Recipe";
  lv->Columns->Items[1]->Caption = L"Rating";
  lv->Columns->Items[2]->Caption = L"Comment";
  lv->Columns->Items[3]->Caption = L"Date";
}

__fastcall TFRatingForm::TFRatingForm(TComponent* Owner) : TForm(Owner) {
  JsonFilePath =
      ExtractFilePath(Application->ExeName) + L"ocjene_recepata.json";

  // Inicijalizacija translation mape
  translation[L"Caption"] = {{L"HR", L"Ocjene i komentari recepata"},
                             {L"EN", L"Recipe Ratings & Comments"}};
  translation[L"LblRecipe"] = {{L"HR", L"Recept:"}, {L"EN", L"Recipe:"}};
  translation[L"LblRating"] = {{L"HR", L"Ocjena:"}, {L"EN", L"Rating:"}};
  translation[L"LblComment"] = {{L"HR", L"Komentar:"}, {L"EN", L"Comment:"}};
  translation[L"BtnAdd"] = {{L"HR", L"Spremi ocjenu"}, {L"EN", L"Save Rating"}};
  translation[L"BtnEdit"] = {{L"HR", L"Uredi ocjenu"}, {L"EN", L"Edit Rating"}};
  translation[L"BtnDelete"] = {{L"HR", L"Ukloni ocjenu"},
                               {L"EN", L"Delete Rating"}};
  translation[L"BtnClose"] = {{L"HR", L"Zatvori"}, {L"EN", L"Close"}};
}

void __fastcall TFRatingForm::FormCreate(TObject* Sender) {
  // Postavke ListView-a za prikaz ocjena
  LVRatings->ViewStyle = vsReport;
  LVRatings->RowSelect = true;
  LVRatings->GridLines = true;
  LVRatings->HideSelection = false;

  // Kreiraj kolone za ListView
  LVRatings->Columns->Clear();

  auto col1 = LVRatings->Columns->Add();
  col1->Caption = L"Recept";
  col1->Width = 150;

  auto col2 = LVRatings->Columns->Add();
  col2->Caption = L"Ocjena";
  col2->Width = 60;

  auto col3 = LVRatings->Columns->Add();
  col3->Caption = L"Komentar";
  col3->Width = 250;

  auto col4 = LVRatings->Columns->Add();
  col4->Caption = L"Datum";
  col4->Width = 120;

  // Postavke TrackBar-a
  TrackRating->Min = 1;
  TrackRating->Max = 5;
  TrackRating->Position = 5;
  LblRatingValue->Caption = L"5/5";

  // Uèitaj postojeæe ocjene iz JSON datoteke
  if (FileExists(JsonFilePath)) {
    LoadRatingsJSON(JsonFilePath);
  }
}

void __fastcall TFRatingForm::FormShow(TObject* Sender) {
  ApplyLanguage();
  RefreshRatingsList();
}

void TFRatingForm::ApplyLanguage() {
  // Koristi automatsku translation mapu (kao u Receptar.cpp)
  translateForm(this, CurrentLanguage, translation);

  // Posebno za ListView kolone
  if (CurrentLanguage == L"HR")
    translateListViewHR(LVRatings);
  else
    translateListViewEN(LVRatings);
}

// ===== JSON CRUD OPERACIJE =====

// CREATE - Spremi novu ocjenu
void __fastcall TFRatingForm::BtnAddClick(TObject* Sender) {
  // Provjeri da li je odabran recept
  if (CombRecipes->ItemIndex == -1) {
    const UnicodeString msg = (CurrentLanguage == L"HR")
                                  ? L"Odaberite recept za ocjenjivanje."
                                  : L"Select a recipe to rate.";
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Greška" : L"Error",
                            MB_OK | MB_ICONWARNING);
    return;
  }

  // Kreiraj novu ocjenu iz forme
  TRecipeRating newRating = GetRatingFromForm();
  newRating.Id = GetNextId();

  // Validacija
  if (!newRating.Validate()) {
    const UnicodeString msg = (CurrentLanguage == L"HR")
                                  ? L"Podaci ocjene nisu ispravni."
                                  : L"Rating data is invalid.";
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Greška" : L"Error",
                            MB_OK | MB_ICONWARNING);
    return;
  }

  // Dodaj u memoriju
  Ratings.push_back(newRating);
  NextId++;

  // Spremi u JSON
  bool success = SaveRatingsJSON(JsonFilePath);

  // Osvježi prikaz
  RefreshRatingsList();
  ClearForm();

  // Prikaži rezultat
  const UnicodeString msg =
      success ? ((CurrentLanguage == L"HR") ? L"Ocjena je uspješno spremljena!"
                                            : L"Rating saved successfully!")
              : ((CurrentLanguage == L"HR") ? L"Greška pri spremanju ocjene!"
                                            : L"Error saving rating!");

  Application->MessageBox(
      msg.c_str(),
      success ? ((CurrentLanguage == L"HR") ? L"Uspjeh" : L"Success")
              : ((CurrentLanguage == L"HR") ? L"Greška" : L"Error"),
      success ? (MB_OK | MB_ICONINFORMATION) : (MB_OK | MB_ICONERROR));
}

// EDIT/UPDATE - Uredi postojeæu ocjenu
void __fastcall TFRatingForm::BtnEditClick(TObject* Sender) {
  if (!LVRatings->Selected) {
    const UnicodeString msg = (CurrentLanguage == L"HR")
                                  ? L"Odaberite ocjenu za ureðivanje."
                                  : L"Select a rating to edit.";
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Info" : L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }

  // Provjeri da li je odabran recept u formi
  if (CombRecipes->ItemIndex == -1) {
    const UnicodeString msg = (CurrentLanguage == L"HR")
                                  ? L"Odaberite recept za ocjenjivanje."
                                  : L"Select a recipe to rate.";
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Greška" : L"Error",
                            MB_OK | MB_ICONWARNING);
    return;
  }

  int ratingId = reinterpret_cast<int>(LVRatings->Selected->Data);

  // Pronaði ocjenu u vektoru
  auto it = std::find_if(
      Ratings.begin(), Ratings.end(),
      [ratingId](const TRecipeRating& r) { return r.Id == ratingId; });

  if (it == Ratings.end()) {
    return;
  }

  // Kreiraj ažuriranu ocjenu iz forme
  TRecipeRating updatedRating = GetRatingFromForm();
  updatedRating.Id = ratingId;
  updatedRating.DateCreated = it->DateCreated;  // Zadrži originalni datum

  // Validacija
  if (!updatedRating.Validate()) {
    const UnicodeString msg = (CurrentLanguage == L"HR")
                                  ? L"Podaci ocjene nisu ispravni."
                                  : L"Rating data is invalid.";
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Greška" : L"Error",
                            MB_OK | MB_ICONWARNING);
    return;
  }

  // Ažuriraj u memoriji
  *it = updatedRating;

  // Spremi u JSON
  bool success = SaveRatingsJSON(JsonFilePath);

  // Osvježi prikaz
  RefreshRatingsList();
  ClearForm();

  // Prikaži rezultat
  const UnicodeString msg =
      success ? ((CurrentLanguage == L"HR") ? L"Ocjena je uspješno ažurirana!"
                                            : L"Rating updated successfully!")
              : ((CurrentLanguage == L"HR") ? L"Greška pri ažuriranju ocjene!"
                                            : L"Error updating rating!");

  Application->MessageBox(
      msg.c_str(),
      success ? ((CurrentLanguage == L"HR") ? L"Uspjeh" : L"Success")
              : ((CurrentLanguage == L"HR") ? L"Greška" : L"Error"),
      success ? (MB_OK | MB_ICONINFORMATION) : (MB_OK | MB_ICONERROR));
}

// DELETE - Ukloni odabranu ocjenu
void __fastcall TFRatingForm::BtnDeleteClick(TObject* Sender) {
  if (!LVRatings->Selected) {
    const UnicodeString msg = (CurrentLanguage == L"HR")
                                  ? L"Odaberite ocjenu za brisanje."
                                  : L"Select a rating to delete.";
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Info" : L"Info",
                            MB_OK | MB_ICONINFORMATION);
    return;
  }

  int ratingId = reinterpret_cast<int>(LVRatings->Selected->Data);

  // Pronaði ocjenu u vektoru
  auto it = std::find_if(
      Ratings.begin(), Ratings.end(),
      [ratingId](const TRecipeRating& r) { return r.Id == ratingId; });

  if (it == Ratings.end()) {
    return;
  }

  UnicodeString ratingInfo = it->ToString();

  const UnicodeString confirmMsg =
      (CurrentLanguage == L"HR")
          ? L"Jeste li sigurni da želite obrisati ocjenu:\n\n" + ratingInfo +
                L"?"
          : L"Are you sure you want to delete rating:\n\n" + ratingInfo + L"?";

  if (Application->MessageBox(confirmMsg.c_str(),
                              (CurrentLanguage == L"HR") ? L"Potvrda brisanja"
                                                         : L"Confirm deletion",
                              MB_YESNO | MB_ICONQUESTION) == IDYES) {
    // Ukloni iz vektora
    Ratings.erase(it);

    // Spremi u JSON
    bool success = SaveRatingsJSON(JsonFilePath);

    // Osvježi prikaz
    RefreshRatingsList();
    ClearForm();

    // Prikaži rezultat
    const UnicodeString msg =
        success ? ((CurrentLanguage == L"HR") ? L"Ocjena je uspješno obrisana!"
                                              : L"Rating deleted successfully!")
                : ((CurrentLanguage == L"HR") ? L"Greška pri brisanju ocjene!"
                                              : L"Error deleting rating!");

    Application->MessageBox(
        msg.c_str(),
        success ? ((CurrentLanguage == L"HR") ? L"Uspjeh" : L"Success")
                : ((CurrentLanguage == L"HR") ? L"Greška" : L"Error"),
        success ? (MB_OK | MB_ICONINFORMATION) : (MB_OK | MB_ICONERROR));
  }
}

// READ - Uèitaj ocjene iz JSON datoteke
bool TFRatingForm::LoadRatingsJSON(const UnicodeString& filePath) {
  try {
    if (!FileExists(filePath)) {
      return false;
    }

    std::unique_ptr<TStringList> jsonText(new TStringList);
    jsonText->LoadFromFile(filePath, TEncoding::UTF8);

    TJSONObject* jsonRoot =
        dynamic_cast<TJSONObject*>(TJSONObject::ParseJSONValue(jsonText->Text));

    if (!jsonRoot) {
      throw Exception(L"Invalid JSON format");
    }

    std::unique_ptr<TJSONObject> rootGuard(jsonRoot);

    TJSONArray* ratingsArray =
        dynamic_cast<TJSONArray*>(jsonRoot->GetValue(L"ratings"));

    if (!ratingsArray) {
      throw Exception(L"Missing 'ratings' array in JSON");
    }

    Ratings.clear();
    int maxId = 0;

    for (int i = 0; i < ratingsArray->Count; ++i) {
      TJSONObject* ratingObj =
          dynamic_cast<TJSONObject*>(ratingsArray->Items[i]);

      if (ratingObj) {
        TRecipeRating rating = TRecipeRating::FromJSON(ratingObj);
        if (rating.Validate()) {
          Ratings.push_back(rating);
          if (rating.Id > maxId) maxId = rating.Id;
        }
      }
    }

    NextId = maxId + 1;
    return true;

  } catch (const Exception& e) {
    const UnicodeString msg =
        (CurrentLanguage == L"HR")
            ? L"Greška pri uèitavanju JSON datoteke:\n" + e.Message
            : L"Error loading JSON file:\n" + e.Message;
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Greška" : L"Error",
                            MB_OK | MB_ICONERROR);
    return false;
  }
}

// UPDATE/CREATE - Spremi sve ocjene u JSON datoteku
bool TFRatingForm::SaveRatingsJSON(const UnicodeString& filePath) {
  try {
    TJSONObject* jsonRoot = new TJSONObject();
    TJSONArray* ratingsArray = new TJSONArray();

    // Dodaj sve ocjene u JSON array
    for (const auto& rating : Ratings) {
      if (rating.Validate()) {
        ratingsArray->AddElement(rating.ToJSON());
      }
    }

    jsonRoot->AddPair(L"ratings", ratingsArray);

    // Spremi u datoteku
    std::unique_ptr<TStringList> jsonText(new TStringList);
    jsonText->Text = jsonRoot->ToString();
    jsonText->SaveToFile(filePath, TEncoding::UTF8);

    delete jsonRoot;
    return true;

  } catch (const Exception& e) {
    const UnicodeString msg =
        (CurrentLanguage == L"HR")
            ? L"Greška pri spremanju JSON datoteke:\n" + e.Message
            : L"Error saving JSON file:\n" + e.Message;
    Application->MessageBox(msg.c_str(),
                            (CurrentLanguage == L"HR") ? L"Greška" : L"Error",
                            MB_OK | MB_ICONERROR);
    return false;
  }
}

// ===== UI OPERACIJE =====

void TFRatingForm::RefreshRatingsList() {
  LVRatings->Items->BeginUpdate();
  LVRatings->Items->Clear();

  for (const auto& rating : Ratings) {
    auto item = LVRatings->Items->Add();
    item->Caption = rating.RecipeName;
    item->SubItems->Add(IntToStr(rating.Rating) + L"/5");

    // Skrati komentar ako je predugaèak
    UnicodeString comment = rating.Comment.Length() > 50
                                ? rating.Comment.SubString(1, 47) + L"..."
                                : rating.Comment;
    item->SubItems->Add(comment);

    item->SubItems->Add(rating.GetDateString());
    item->Data = reinterpret_cast<void*>(rating.Id);
  }

  LVRatings->Items->EndUpdate();
}

void TFRatingForm::ClearForm() {
  CombRecipes->ItemIndex = -1;
  TrackRating->Position = 5;
  LblRatingValue->Caption = L"5/5";
  MemoComment->Clear();
}

TRecipeRating TFRatingForm::GetRatingFromForm() const {
  TRecipeRating rating;

  if (CombRecipes->ItemIndex >= 0) {
    rating.RecipeName = CombRecipes->Text;
  }

  rating.Rating = TrackRating->Position;
  rating.Comment = MemoComment->Text;
  rating.UserName = L"Korisnik";  // Zadana vrijednost

  return rating;
}

void TFRatingForm::LoadRatingToForm(int ratingId) {
  // Pronaði ocjenu s danim ID-jem
  auto it = std::find_if(
      Ratings.begin(), Ratings.end(),
      [ratingId](const TRecipeRating& r) { return r.Id == ratingId; });

  if (it != Ratings.end()) {
    // Postavi ComboBox na odgovarajuæi recept
    for (int i = 0; i < CombRecipes->Items->Count; ++i) {
      if (CombRecipes->Items->Strings[i] == it->RecipeName) {
        CombRecipes->ItemIndex = i;
        break;
      }
    }

    // Postavi ocjenu na TrackBar
    TrackRating->Position = it->Rating;
    LblRatingValue->Caption = IntToStr(it->Rating) + L"/5";

    // Postavi komentar
    MemoComment->Text = it->Comment;
  }
}

// ===== EVENT HANDLERI =====

void __fastcall TFRatingForm::TrackRatingChange(TObject* Sender) {
  LblRatingValue->Caption = IntToStr(TrackRating->Position) + L"/5";
}

void __fastcall TFRatingForm::LVRatingsSelectItem(TObject* Sender,
                                                  TListItem* Item,
                                                  bool Selected) {
  if (Selected && Item) {
    // Automatski uèitaj odabranu ocjenu u formu za lakše ureðivanje
    int ratingId = reinterpret_cast<int>(Item->Data);
    LoadRatingToForm(ratingId);
  }
}

// ===== JAVNE METODE =====

void TFRatingForm::SetAvailableRecipes(
    const std::vector<UnicodeString>& recipes) {
  CombRecipes->Items->Clear();
  for (const auto& recipe : recipes) {
    CombRecipes->Items->Add(recipe);
  }
}

