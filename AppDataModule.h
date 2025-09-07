#ifndef AppDataModuleH
#define AppDataModuleH

#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <System.Classes.hpp>
#include <System.SyncObjs.hpp>

class TAppDataModule1 : public TDataModule {
 __published:
  // ADO konekcija i tablice (komponente iz .dfm-a)
  TADOConnection *Conn;
  TADOTable *TUsers;
  TADOTable *TMealPlans;
  TADOTable *TPlanItems;

  // DataSource-ovi
  TDataSource *DSUsers;
  TDataSource *DSMealPlans;
  TDataSource *DSPlanItems;

  // --- Strongly-typed polja (Users)
  TAutoIncField *TUsersUserID;
  TWideStringField *TUsersUserName;
  TWideStringField *TUsersSurname;
  TWideStringField *TUsersEmail;
  TBlobField *TUsersPhoto;

  // --- Strongly-typed polja (MealPlans)
  TAutoIncField *TMealPlansPlanID2;  // PlanID
  TIntegerField *TMealPlansUserID;
  TDateTimeField *TMealPlansPlanDate;
  TWideStringField *TMealPlansPlanName;
  TIntegerField *TMealPlansItemCount;  // Calculated

  // --- Strongly-typed polja (PlanItems)
  TAutoIncField *TPlanItemsItemID;
  TIntegerField *TPlanItemsPlanID;
  TWideStringField *TPlanItemsRecipeName;
  TIntegerField *TPlanItemsServings;
  TWideStringField *TPlanItemsWhenInDay;
  TWideStringField *TPlanItemsComment;
  TWideStringField *TPlanItemsPlanNameLkp;

  // Dogaðaji (vezani u .dfm-u)
  void __fastcall DataModuleCreate(TObject *Sender);
  void __fastcall TMealPlansNewRecord(TDataSet *DataSet);
  void __fastcall TMealPlansCalcFields(TDataSet *DataSet);
  void __fastcall TPlanItemsNewRecord(TDataSet *DataSet);
  void __fastcall TPlanItemsBeforeInsert(TDataSet *DataSet);
  void __fastcall TPlanItemsAfterPost(TDataSet *DataSet);
  void __fastcall TPlanItemsAfterDelete(TDataSet *DataSet);

 public:
  __fastcall TAppDataModule1(TComponent *Owner);
  __fastcall ~TAppDataModule1();
  // Sort
  void __fastcall SortUsers(const UnicodeString &orderBy);
  void __fastcall SortMealPlans(const UnicodeString &orderBy);
  void __fastcall SortPlanItems(const UnicodeString &orderBy);

  // Filter
  void __fastcall FilterUsersByName(const UnicodeString &likeText);
  void __fastcall FilterMealPlansByUserID(int userId);
  void __fastcall ClearAllFilters();

  // BLOB helperi
  bool __fastcall LoadUserPhotoFromFile(int userId, const UnicodeString &path);
  bool __fastcall SaveUserPhotoToFile(int userId, const UnicodeString &path);

  // SQL dodavanje stavke (ako poželiš direktno bez Append-a)
  bool __fastcall AddPlanItemSQL(int planId, const UnicodeString &recipeName,
                                 int servings = 1,
                                 const UnicodeString &whenInDay = L"Breakfast",
                                 const UnicodeString &comment = L"");

  TEvent *ReportEvent{nullptr};
  bool ReportInProgress{false};
};

extern PACKAGE TAppDataModule1 *AppDataModule1;

#endif

