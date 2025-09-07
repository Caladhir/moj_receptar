// ReceptForm.h
#ifndef ReceptFormH
#define ReceptFormH

#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <vector>

class TSastojak {
 public:
  UnicodeString Naziv;
  double Kolicina{0.0};

  //
  bool Validiraj() const { return !Naziv.IsEmpty() && Kolicina > 0; }

  UnicodeString ToString() const {
    return Naziv + " (" + FloatToStr(Kolicina) + ")";
  }
};
class TRecept {
 public:
  UnicodeString Naziv;
  int Tezina{1};
  std::vector<TSastojak> Sastojci;
  //
  void AddSastojak(const TSastojak &s) {
    if (s.Validiraj()) Sastojci.push_back(s);
  }
  bool RemoveSastojak(int index) {
    if (index >= 0 && index < (int)Sastojci.size()) {
      Sastojci.erase(Sastojci.begin() + index);
      return true;
    }
    return false;
  }
  int BrojSastojaka() const { return (int)Sastojci.size(); }
  double UkupnaKolicina() const {
    double sum = 0;
    for (const auto &x : Sastojci) sum += x.Kolicina;
    return sum;
  }
  bool Validiraj() const {
    return !Naziv.Trim().IsEmpty() && Tezina >= 1 && Tezina <= 5;
  }
};

class TFReceptForm : public TForm {
 __published:
  TLabel *LblNaziv;
  TLabel *LblTezina;
  TLabel *LblSastojak;
  TLabel *LblKolicina;

  TTrackBar *TrackTezina;
  TEdit *EditNaziv;
  TEdit *EditSastojak;
  TEdit *EditKolicina;
  TButton *BtnOK;
  TButton *BtnCancel;
  TListBox *LBItems;
  TButton *BtnAddS;
  TButton *BtnRemoveS;

  void __fastcall FormShow(TObject *Sender);
  void __fastcall BtnAddSClick(TObject *Sender);
  void __fastcall BtnRemoveSClick(TObject *Sender);
  void __fastcall BtnOKClick(TObject *Sender);

 private:
  UnicodeString Lang{L"HR"};
  TRecept Temp;

 public:
  __fastcall TFReceptForm(TComponent *Owner);
  void SetLanguage(const UnicodeString &l) { Lang = l; }
  void Reset();

  // komunikacija s glavnom formom
  void LoadFromRecept(const TRecept &r);
  void SaveToRecept(TRecept &out) const;
};

extern PACKAGE TFReceptForm *FReceptForm;
#endif

