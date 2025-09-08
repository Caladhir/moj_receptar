//---------------------------------------------------------------------------

#include <vcl.h>

#include <Vcl.ComCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#pragma link "Vcl.StdCtrls"
#pragma link "Vcl.ComCtrls"
#include <Winapi.Windows.hpp>

#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------

USEFORM("PostavkeForm.cpp", Form2);
USEFORM("ReceptForm.cpp", Form1);
USEFORM("Receptar.cpp", FReceptar);
USEFORM("MealPlansForm.cpp", FMealPlans);
USEFORM("Login_Unit.cpp", Form3);
USEFORM("AppDataModule.cpp", AppDataModule1);
USEFORM("NutritionForm.cpp", Form4);
USEFORM("RatingForm.cpp", FRatingForm);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
  HANDLE hMutex =
      CreateMutexW(nullptr, TRUE, L"Local\\MojReceptar.SingleInstance");
  if (!hMutex) return 0;

  // 2) Ako već postoji vlasnik → druga instanca je pokrenuta
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    MessageBoxW(nullptr, L"Aplikacija je već pokrenuta!", L"Info",
                MB_OK | MB_ICONINFORMATION);
    CloseHandle(hMutex);
    return 0;
  }
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
  ReleaseMutex(hMutex);
  CloseHandle(hMutex);
  return 0;
}
//---------------------------------------------------------------------------

