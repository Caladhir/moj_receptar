#ifndef MealPlansFormH
#define MealPlansFormH

#include <Objbase.h>

#include <Data.DB.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdUDPBase.hpp>
#include <IdUDPClient.hpp>
#include <System.Classes.hpp>
#include <System.SyncObjs.hpp>
#include <System.Threading.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.DBCtrls.hpp>
#include <Vcl.DBGrids.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.StdCtrls.hpp>

#include "frxClass.hpp"
#include "frxDBSet.hpp"
#include "frxExportPDF.hpp"

class TFMealPlans : public TForm {
 __published:
  TPanel *TopUsers;
  TLabel *LblUsersSection;
  TPanel *UsersFilterBar;
  TLabel *LblFilterUsers;
  TEdit *EdUserFilter;
  TButton *BtnFilterUser;
  TButton *BtnClearFilters;
  TPanel *UsersRight;
  TImage *ImgPhoto;
  TPanel *PhotoButtonsPanel;
  TButton *BtnLoadPhoto;
  TButton *BtnDeletePhoto;
  TDBGrid *GridUsers;
  TDBNavigator *NavUsers;

  TPanel *MidPlans;
  TLabel *LblPlansSection;
  TPanel *PlansTopBar;
  TLabel *LblReporting;
  TLabel *LblSorting;
  TDBGrid *GridPlans;
  TDBNavigator *NavPlans;

  TPanel *BottomItems;
  TLabel *LblItemsSection;
  TPanel *ItemsTopBar;
  TLabel *LblMasterDetail;
  TDBGrid *GridItems;
  TDBNavigator *NavItems;

  // FastReport
  TfrxReport *frxReport1;
  TfrxDBDataset *frxDBMealPlans;
  TfrxDBDataset *frxDBPlanItems;
  TfrxDBDataset *frxDBUsers;
  TOpenDialog *OpenDialog1;
  TSaveDialog *SaveDialog1;
  TButton *BtnParallelLoad;
  TButton *BtnReport;
  TIdTCPClient *IdTCPClient1;
  TButton *BtnGetRecipesTCP;
  TButton *BtnUploadRecipeTCP;
  TButton *BtnGetPhotoTCP;
  TIdUDPClient *IdUDPClient1;
  TMemo *MemoUDP;
  TButton *BtnNotifyUDP;
  TButton *BtnGetThumbUDP;

  // Dogaðaji
  void __fastcall FormShow(TObject *Sender);
  void __fastcall BtnFilterUserClick(TObject *Sender);
  void __fastcall BtnClearFiltersClick(TObject *Sender);
  void __fastcall BtnLoadPhotoClick(TObject *Sender);
  void __fastcall BtnDeletePhotoClick(TObject *Sender);
  void __fastcall GridPlansTitleClick(TColumn *Column);
  void __fastcall BtnReportClick(TObject *Sender);
  void __fastcall DSUsersDataChange(TObject *Sender, TField *Field);
  //  void __fastcall BtnAddItemClick(TObject *Sender);
  void __fastcall BtnParallelLoadClick(TObject *Sender);

  void __fastcall BtnGetRecipesTCPClick(TObject *Sender);
  void __fastcall BtnGetPhotoTCPClick(TObject *Sender);
  void __fastcall BtnUploadRecipeTCPClick(TObject *Sender);

  void __fastcall BtnNotifyUDPClick(TObject *Sender);
  void __fastcall BtnGetThumbUDPClick(TObject *Sender);

 private:
  void __fastcall ShowCurrentUserPhoto();
  //  void __fastcall AppExceptionFilter(TObject *Sender, Exception *E);

  void __fastcall ShowFinalSummary(int usersCnt, int plansCnt, int itemsCnt,
                                   const System::UnicodeString &whenSummary);

  // Guard protiv dvostrukog klika dok traje paralelno učitavanje
  int FParallelFlag{0};                    // 0=slobodno, 1=zauzeto
  bool __fastcall TryBeginParallelLoad();  // atomično zauzmi
  void __fastcall EndParallelLoad();       // oslobodi

  int __fastcall LaunchPDFViewer(const System::UnicodeString &pdfPath);

  // SERVER
  void __fastcall TcpConnect();
  void __fastcall TcpGetRecipes();
  void __fastcall TcpGetUserPhoto(int userId);
  void __fastcall TcpUploadRecipe(const UnicodeString &name,
                                  const UnicodeString &category);

 public:
  __fastcall TFMealPlans(TComponent *Owner) : TForm(Owner) {}
  void __fastcall SafeRefreshDatasets();
};

extern PACKAGE TFMealPlans *FMealPlans;

#endif

