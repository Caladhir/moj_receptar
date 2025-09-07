//---------------------------------------------------------------------------

#include <vcl.h>

#include <Vcl.ComCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#pragma link "Vcl.StdCtrls"
#pragma link "Vcl.ComCtrls"

#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("RatingForm.cpp", RatigForm);
USEFORM("PostavkeForm.cpp", Form2);
USEFORM("ReceptForm.cpp", Form1);
USEFORM("Receptar.cpp", FReceptar);
USEFORM("MealPlansForm.cpp", Form5);
USEFORM("Login_Unit.cpp", Form3);
USEFORM("AppDataModule.cpp", AppDataModule1);
USEFORM("NutritionForm.cpp", Form4);
//---------------------------------------------------------------------------
USEFORM("RatingForm.cpp", FRatingForm)
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
  try {
    Application->Initialize();
    Application->MainFormOnTaskBar = true;
    Application->CreateForm(__classid(TFReceptar), &FReceptar);
    Application->CreateForm(__classid(TAppDataModule1), &AppDataModule1);
    Application->Run();
  } catch (Exception &exception) {
    Application->ShowException(&exception);
  } catch (...) {
    try {
      throw Exception("");
    } catch (Exception &exception) {
      Application->ShowException(&exception);
    }
  }
  return 0;
}
//---------------------------------------------------------------------------

