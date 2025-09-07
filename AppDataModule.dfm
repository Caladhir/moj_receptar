object AppDataModule1: TAppDataModule1
  OnCreate = DataModuleCreate
  Height = 391
  Width = 582
  object Conn: TADOConnection
    ConnectionString = 
      'Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=C:\Us' +
      'ers\MobiS PC\Desktop\SCHOOL\4.SEMESTAR\NAPREDNE TEHNIKE PROGRAMI' +
      'RANJA\&PROJEKT\MojReceptar\Receptar.mdb;Mode=Share Deny None;Per' +
      'sist Security Info=False;Jet OLEDB:System database="";Jet OLEDB:' +
      'Registry Path="";Jet OLEDB:Database Password="";Jet OLEDB:Engine' +
      ' Type=5;Jet OLEDB:Database Locking Mode=1;Jet OLEDB:Global Parti' +
      'al Bulk Ops=2;Jet OLEDB:Global Bulk Transactions=1;Jet OLEDB:New' +
      ' Database Password="";Jet OLEDB:Create System Database=False;Jet' +
      ' OLEDB:Encrypt Database=False;Jet OLEDB:Don'#39't Copy Locale on Com' +
      'pact=False;Jet OLEDB:Compact Without Replica Repair=False;Jet OL' +
      'EDB:SFP=False'
    LoginPrompt = False
    Provider = 'Microsoft.Jet.OLEDB.4.0'
    Left = 56
    Top = 8
  end
  object TUsers: TADOTable
    Connection = Conn
    CursorType = ctStatic
    TableName = 'Users'
    Left = 80
    Top = 128
    object TUsersUserID: TAutoIncField
      FieldName = 'UserID'
      ReadOnly = True
    end
    object TUsersUserName: TWideStringField
      FieldName = 'UserName'
      Size = 50
    end
    object TUsersSurname: TWideStringField
      FieldName = 'Surname'
      Size = 50
    end
    object TUsersEmail: TWideStringField
      FieldName = 'Email'
      Size = 80
    end
    object TUsersPhoto: TBlobField
      FieldName = 'Photo'
    end
  end
  object TMealPlans: TADOTable
    Connection = Conn
    CursorType = ctStatic
    OnCalcFields = TMealPlansCalcFields
    OnNewRecord = TMealPlansNewRecord
    TableName = 'MealPlans'
    Left = 176
    Top = 128
    object TMealPlansPlanID2: TAutoIncField
      FieldName = 'PlanID'
      ReadOnly = True
    end
    object TMealPlansUserID: TIntegerField
      FieldName = 'UserID'
    end
    object TMealPlansPlanDate: TDateTimeField
      FieldName = 'PlanDate'
    end
    object TMealPlansPlanName: TWideStringField
      FieldName = 'PlanName'
      Size = 100
    end
    object TMealPlansItemCount: TIntegerField
      FieldKind = fkCalculated
      FieldName = 'ItemCount'
      ReadOnly = True
      Calculated = True
    end
  end
  object TPlanItems: TADOTable
    Connection = Conn
    CursorType = ctStatic
    BeforeInsert = TPlanItemsBeforeInsert
    AfterPost = TPlanItemsAfterPost
    AfterDelete = TPlanItemsAfterDelete
    OnNewRecord = TPlanItemsNewRecord
    IndexFieldNames = 'PlanID'
    MasterFields = 'PlanID'
    MasterSource = DSMealPlans
    TableName = 'PlanItems'
    Left = 440
    Top = 120
    object TPlanItemsItemID: TAutoIncField
      FieldName = 'ItemID'
      ReadOnly = True
    end
    object TPlanItemsPlanID: TIntegerField
      FieldName = 'PlanID'
    end
    object TPlanItemsRecipeName: TWideStringField
      FieldName = 'RecipeName'
      Size = 120
    end
    object TPlanItemsServings: TIntegerField
      FieldName = 'Servings'
    end
    object TPlanItemsWhenInDay: TWideStringField
      FieldName = 'WhenInDay'
    end
    object TPlanItemsComment: TWideStringField
      FieldName = 'Comment'
      Size = 255
    end
    object TPlanItemsPlanNameLkp: TWideStringField
      FieldKind = fkLookup
      FieldName = 'PlanNameLkp'
      LookupDataSet = TMealPlans
      LookupKeyFields = 'PlanID'
      LookupResultField = 'PlanName'
      KeyFields = 'PlanID'
      ReadOnly = True
      Size = 100
      Lookup = True
    end
  end
  object DSUsers: TDataSource
    DataSet = TUsers
    Left = 80
    Top = 248
  end
  object DSMealPlans: TDataSource
    DataSet = TMealPlans
    Left = 176
    Top = 248
  end
  object DSPlanItems: TDataSource
    DataSet = TPlanItems
    Left = 440
    Top = 248
  end
end
