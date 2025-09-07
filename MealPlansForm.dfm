object FMealPlans: TFMealPlans
  Left = 306
  Top = 187
  Caption = 'Planovi prehrane'
  ClientHeight = 663
  ClientWidth = 1202
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Position = poDesigned
  OnShow = FormShow
  TextHeight = 15
  object TopUsers: TPanel
    Left = 0
    Top = 0
    Width = 1202
    Height = 220
    Align = alTop
    TabOrder = 0
    ExplicitWidth = 1198
    object LblUsersSection: TLabel
      Left = 8
      Top = 42
      Width = 248
      Height = 17
      Caption = '1. KORISNICI (Users tablica - BLOB slike)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Segoe UI'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object UsersFilterBar: TPanel
      Left = 1
      Top = 1
      Width = 1200
      Height = 36
      Align = alTop
      TabOrder = 0
      ExplicitWidth = 1196
      object LblFilterUsers: TLabel
        Left = 8
        Top = 2
        Width = 118
        Height = 13
        Caption = 'FILTRIRANJE korisnika:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Segoe UI'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object EdUserFilter: TEdit
        Left = 8
        Top = 15
        Width = 220
        Height = 23
        TabOrder = 0
        TextHint = 'Unesite ime korisnika...'
      end
      object BtnFilterUser: TButton
        Left = 236
        Top = 13
        Width = 90
        Height = 26
        Caption = 'Filtriraj'
        TabOrder = 1
        OnClick = BtnFilterUserClick
      end
      object BtnClearFilters: TButton
        Left = 356
        Top = 13
        Width = 90
        Height = 26
        Caption = 'O'#269'isti sve'
        TabOrder = 2
        OnClick = BtnClearFiltersClick
      end
      object BtnGetRecipesTCP: TButton
        Left = 752
        Top = 14
        Width = 98
        Height = 24
        Caption = 'Preuzmi recepte'
        TabOrder = 3
        OnClick = BtnGetRecipesTCPClick
      end
      object BtnUploadRecipeTCP: TButton
        Left = 872
        Top = 14
        Width = 92
        Height = 24
        Caption = 'Podijeli recept'
        TabOrder = 4
        OnClick = BtnUploadRecipeTCPClick
      end
      object BtnGetPhotoTCP: TButton
        Left = 1027
        Top = 11
        Width = 126
        Height = 24
        Caption = 'U'#269'itaj sliku (TCP)'
        TabOrder = 5
        OnClick = BtnGetPhotoTCPClick
      end
    end
    object UsersRight: TPanel
      Left = 971
      Top = 37
      Width = 230
      Height = 157
      Align = alRight
      TabOrder = 1
      ExplicitLeft = 967
      object ImgPhoto: TImage
        Left = 1
        Top = 33
        Width = 228
        Height = 123
        Align = alClient
        Center = True
        Proportional = True
        Stretch = True
      end
      object PhotoButtonsPanel: TPanel
        Left = 1
        Top = 1
        Width = 228
        Height = 32
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object BtnLoadPhoto: TButton
          Left = 4
          Top = 4
          Width = 70
          Height = 24
          Caption = 'U'#269'itaj sliku'
          TabOrder = 0
          OnClick = BtnLoadPhotoClick
        end
        object BtnDeletePhoto: TButton
          Left = 128
          Top = 4
          Width = 70
          Height = 24
          Caption = 'Obri'#353'i sliku'
          TabOrder = 1
          OnClick = BtnDeletePhotoClick
        end
      end
    end
    object GridUsers: TDBGrid
      Left = 1
      Top = 37
      Width = 970
      Height = 157
      Align = alClient
      DataSource = AppDataModule1.DSUsers
      Options = [dgEditing, dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgAlwaysShowSelection, dgConfirmDelete, dgCancelOnExit]
      TabOrder = 2
      TitleFont.Charset = DEFAULT_CHARSET
      TitleFont.Color = clWindowText
      TitleFont.Height = -12
      TitleFont.Name = 'Segoe UI'
      TitleFont.Style = []
      Columns = <
        item
          Expanded = False
          FieldName = 'UserID'
          Title.Caption = 'ID'
          Width = 50
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'UserName'
          Title.Caption = 'Ime'
          Width = 150
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'Surname'
          Title.Caption = 'Prezime'
          Width = 150
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'Email'
          Title.Caption = 'E-mail adresa'
          Width = 200
          Visible = True
        end>
    end
    object NavUsers: TDBNavigator
      Left = 1
      Top = 194
      Width = 1200
      Height = 25
      DataSource = AppDataModule1.DSUsers
      Align = alBottom
      TabOrder = 3
      ExplicitWidth = 1196
    end
  end
  object MidPlans: TPanel
    Left = 0
    Top = 220
    Width = 1202
    Height = 210
    Align = alTop
    TabOrder = 1
    ExplicitWidth = 1198
    object LblPlansSection: TLabel
      Left = 8
      Top = 42
      Width = 400
      Height = 17
      Caption = '2. PLANOVI PREHRANE (MealPlans - Calculated polje, Sortiranje)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Segoe UI'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object PlansTopBar: TPanel
      Left = 1
      Top = 1
      Width = 1200
      Height = 36
      Align = alTop
      TabOrder = 0
      ExplicitWidth = 1196
      object LblReporting: TLabel
        Left = 184
        Top = 5
        Width = 780
        Height = 30
        Alignment = taCenter
        AutoSize = False
        Caption = 'STATUS'
        Color = clBackground
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clWhite
        Font.Height = -16
        Font.Name = 'Segoe UI Semibold'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object LblSorting: TLabel
        Left = 36
        Top = 17
        Width = 73
        Height = 13
        Caption = 'MEAL PLANS:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Segoe UI'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object BtnParallelLoad: TButton
        Left = 970
        Top = 5
        Width = 183
        Height = 33
        Caption = 'U'#269'itaj SVE (paralelno)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Segoe UI'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = BtnParallelLoadClick
      end
    end
    object GridPlans: TDBGrid
      Left = 1
      Top = 37
      Width = 1200
      Height = 147
      Align = alClient
      DataSource = AppDataModule1.DSMealPlans
      Options = [dgEditing, dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgAlwaysShowSelection, dgConfirmDelete, dgCancelOnExit, dgTitleClick]
      TabOrder = 1
      TitleFont.Charset = DEFAULT_CHARSET
      TitleFont.Color = clWindowText
      TitleFont.Height = -12
      TitleFont.Name = 'Segoe UI'
      TitleFont.Style = []
      OnTitleClick = GridPlansTitleClick
      Columns = <
        item
          Expanded = False
          FieldName = 'PlanID'
          Title.Caption = 'ID Plana'
          Width = 60
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'UserID'
          ReadOnly = True
          Title.Caption = 'ID Korisnika'
          Width = 80
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'PlanDate'
          Title.Caption = 'Datum (mm/dd/yyyy)'
          Width = 120
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'PlanName'
          Title.Caption = 'Naziv plana'
          Width = 250
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'ItemCount'
          Title.Caption = 'Broj stavki (CALCULATED)'
          Width = 150
          Visible = True
        end>
    end
    object NavPlans: TDBNavigator
      Left = 1
      Top = 184
      Width = 1200
      Height = 25
      DataSource = AppDataModule1.DSMealPlans
      Align = alBottom
      TabOrder = 2
      ExplicitWidth = 1196
    end
  end
  object BottomItems: TPanel
    Left = 0
    Top = 430
    Width = 1202
    Height = 233
    Align = alClient
    TabOrder = 2
    ExplicitWidth = 1198
    ExplicitHeight = 232
    object LblItemsSection: TLabel
      Left = 8
      Top = 42
      Width = 417
      Height = 17
      Caption = '3. STAVKE PLANOVA (PlanItems - Lookup polje, Master-Detail veza)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Segoe UI'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object ItemsTopBar: TPanel
      Left = 1
      Top = 1
      Width = 1200
      Height = 36
      Align = alTop
      TabOrder = 0
      ExplicitWidth = 1196
      object LblMasterDetail: TLabel
        Left = 36
        Top = 17
        Width = 68
        Height = 13
        Caption = 'PLAN ITEMS:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Segoe UI'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object BtnReport: TButton
        Left = 970
        Top = 4
        Width = 183
        Height = 36
        Caption = 'Generiraj PDF izvje'#353'taj'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Segoe UI'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = BtnReportClick
      end
      object BtnNotifyUDP: TButton
        Left = 680
        Top = 3
        Width = 90
        Height = 37
        Caption = 'UDP notify'
        TabOrder = 1
        OnClick = BtnNotifyUDPClick
      end
      object BtnGetThumbUDP: TButton
        Left = 816
        Top = 3
        Width = 105
        Height = 37
        Caption = 'UDP get thumb'
        TabOrder = 2
        OnClick = BtnGetThumbUDPClick
      end
    end
    object GridItems: TDBGrid
      Left = 1
      Top = 37
      Width = 1200
      Height = 170
      Align = alClient
      DataSource = AppDataModule1.DSPlanItems
      Options = [dgEditing, dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgAlwaysShowSelection, dgConfirmDelete, dgCancelOnExit]
      TabOrder = 1
      TitleFont.Charset = DEFAULT_CHARSET
      TitleFont.Color = clWindowText
      TitleFont.Height = -12
      TitleFont.Name = 'Segoe UI'
      TitleFont.Style = []
      Columns = <
        item
          Expanded = False
          FieldName = 'ItemID'
          Title.Caption = 'ID Stavke'
          Width = 60
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'PlanID'
          ReadOnly = True
          Title.Caption = 'ID Plana (FK)'
          Visible = False
        end
        item
          Expanded = False
          FieldName = 'RecipeName'
          Title.Caption = 'Naziv recepta'
          Width = 200
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'WhenInDay'
          Title.Caption = 'Obrok'
          Width = 100
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'Servings'
          Title.Caption = 'Porcije'
          Width = 60
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'PlanNameLkp'
          Title.Caption = 'Ime plana (LOOKUP)'
          Width = 180
          Visible = True
        end
        item
          Expanded = False
          FieldName = 'Comment'
          Title.Caption = 'Komentar'
          Width = 150
          Visible = True
        end>
    end
    object NavItems: TDBNavigator
      Left = 1
      Top = 207
      Width = 1200
      Height = 25
      DataSource = AppDataModule1.DSPlanItems
      Align = alBottom
      TabOrder = 2
      ExplicitTop = 206
      ExplicitWidth = 1196
    end
    object MemoUDP: TMemo
      Left = 920
      Top = 43
      Width = 281
      Height = 166
      ScrollBars = ssVertical
      TabOrder = 3
    end
  end
  object frxReport1: TfrxReport
    Version = '2022.2.10'
    DataSet = frxDBUsers
    DataSetName = 'Users'
    DotMatrixReport = False
    IniFile = '\Software\Fast Reports'
    PreviewOptions.Buttons = [pbPrint, pbLoad, pbSave, pbExport, pbZoom, pbFind, pbOutline, pbPageSetup, pbTools, pbEdit, pbNavigator, pbExportQuick, pbCopy, pbSelection]
    PreviewOptions.Zoom = 1.000000000000000000
    PrintOptions.Printer = 'Default'
    PrintOptions.PrintOnSheet = 0
    ReportOptions.CreateDate = 45903.192213229200000000
    ReportOptions.LastChange = 45905.617002384300000000
    ScriptLanguage = 'PascalScript'
    ScriptText.Strings = (
      'begin'
      ''
      'end.')
    Left = 400
    Top = 431
    Datasets = <
      item
        DataSet = frxDBMealPlans
        DataSetName = 'MealPlans'
      end
      item
        DataSet = frxDBPlanItems
        DataSetName = 'PlanItems'
      end
      item
        DataSet = frxDBUsers
        DataSetName = 'Users'
      end>
    Variables = <>
    Style = <>
    object Data: TfrxDataPage
      Height = 1000.000000000000000000
      Width = 1000.000000000000000000
    end
    object Page1: TfrxReportPage
      PaperWidth = 210.000000000000000000
      PaperHeight = 297.000000000000000000
      PaperSize = 9
      LeftMargin = 10.000000000000000000
      RightMargin = 10.000000000000000000
      TopMargin = 10.000000000000000000
      BottomMargin = 10.000000000000000000
      Frame.Style = fsDash
      Frame.Typ = []
      Frame.Width = 2.000000000000000000
      MirrorMode = []
      object MasterData1: TfrxMasterData
        FillType = ftBrush
        FillGap.Top = 0
        FillGap.Left = 0
        FillGap.Bottom = 0
        FillGap.Right = 0
        Frame.Typ = []
        Height = 151.181200000000000000
        Top = 343.937230000000000000
        Width = 718.110700000000000000
        DataSet = frxDBMealPlans
        DataSetName = 'MealPlans'
        RowCount = 0
        object MealPlansPlanDate: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 540.472790000000000000
          Top = 64.252010000000000000
          Width = 143.622140000000000000
          Height = 18.897650000000000000
          DataField = 'PlanDate'
          DataSet = frxDBMealPlans
          DataSetName = 'MealPlans'
          Frame.Typ = []
          Memo.UTF8W = (
            '[MealPlans."PlanDate"]')
        end
        object MealPlansPlanName: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 64.252010000000000000
          Width = 154.960730000000000000
          Height = 18.897650000000000000
          DataField = 'PlanName'
          DataSet = frxDBMealPlans
          DataSetName = 'MealPlans'
          Frame.Typ = []
          Memo.UTF8W = (
            '[MealPlans."PlanName"]')
        end
        object MealPlansItemCount: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 281.574985000000000000
          Top = 79.370130000000000000
          Width = 154.960730000000000000
          Height = 18.897650000000000000
          DataField = 'ItemCount'
          DataSet = frxDBMealPlans
          DataSetName = 'MealPlans'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[MealPlans."ItemCount"]')
          ParentFont = False
          VAlign = vaCenter
        end
        object MealPlansUserID: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 26.456710000000000000
          Width = 143.622140000000000000
          Height = 18.897650000000000000
          DataField = 'UserID'
          DataSet = frxDBMealPlans
          DataSetName = 'MealPlans'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[MealPlans."UserID"]')
          ParentFont = False
          VAlign = vaCenter
        end
        object MealPlansPlanID: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 540.472790000000000000
          Top = 26.456710000000000000
          Width = 147.401670000000000000
          Height = 18.897650000000000000
          DataField = 'PlanID'
          DataSet = frxDBMealPlans
          DataSetName = 'MealPlans'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[MealPlans."PlanID"]')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo5: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 3.779530000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'User ID:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo6: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 45.354360000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Ime plana::')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo7: TfrxMemoView
          AllowVectorExport = True
          Left = 311.811225000000000000
          Top = 49.133890000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Broj obroka:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo8: TfrxMemoView
          AllowVectorExport = True
          Left = 540.472790000000000000
          Top = 3.779530000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Plan ID:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo9: TfrxMemoView
          AllowVectorExport = True
          Left = 540.472790000000000000
          Top = 45.354360000000000000
          Width = 147.401670000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Datum po'#269'etka plana:')
          ParentFont = False
          VAlign = vaCenter
        end
      end
      object DetailData1: TfrxDetailData
        FillType = ftBrush
        FillGap.Top = 0
        FillGap.Left = 0
        FillGap.Bottom = 0
        FillGap.Right = 0
        Frame.Typ = []
        Height = 279.685220000000000000
        Top = 517.795610000000000000
        Width = 718.110700000000000000
        DataSet = frxDBPlanItems
        DataSetName = 'PlanItems'
        RowCount = 0
        object PlanItemsRecipeName: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 41.574830000000000000
          Width = 400.630180000000000000
          Height = 37.795300000000000000
          DataField = 'RecipeName'
          DataSet = frxDBPlanItems
          DataSetName = 'PlanItems'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[PlanItems."RecipeName"]')
          ParentFont = False
        end
        object PlanItemsWhenInDay: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 540.472790000000000000
          Top = 41.574830000000000000
          Width = 158.740260000000000000
          Height = 18.897650000000000000
          DataField = 'WhenInDay'
          DataSet = frxDBPlanItems
          DataSetName = 'PlanItems'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[PlanItems."WhenInDay"]')
          ParentFont = False
          VAlign = vaCenter
        end
        object PlanItemsServings: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 98.267780000000000000
          Width = 79.370130000000000000
          Height = 18.897650000000000000
          DataField = 'Servings'
          DataSet = frxDBPlanItems
          DataSetName = 'PlanItems'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[PlanItems."Servings"]')
          ParentFont = False
        end
        object PlanItemsComment: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 162.519790000000000000
          Width = 665.197280000000000000
          Height = 105.826840000000000000
          DataField = 'Comment'
          DataSet = frxDBPlanItems
          DataSetName = 'PlanItems'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[PlanItems."Comment"]')
          ParentFont = False
        end
        object Memo11: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 15.118120000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Recept:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo12: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 79.370130000000000000
          Width = 120.944960000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Broj obroka:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo13: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 132.283550000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Komentar:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo14: TfrxMemoView
          AllowVectorExport = True
          Left = 540.472790000000000000
          Top = 18.897650000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Obrok:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Line3: TfrxLineView
          AllowVectorExport = True
          Top = 7.559060000000000000
          Width = 718.110700000000000000
          Color = clBlack
          Frame.Style = fsDash
          Frame.Typ = [ftTop]
          Frame.Width = 2.000000000000000000
        end
      end
      object ReportTitle1: TfrxReportTitle
        FillType = ftBrush
        FillGap.Top = 0
        FillGap.Left = 0
        FillGap.Bottom = 0
        FillGap.Right = 0
        Frame.Typ = []
        Height = 37.795300000000000000
        Top = 18.897650000000000000
        Width = 718.110700000000000000
        object Memo1: TfrxMemoView
          AllowVectorExport = True
          Left = 253.228510000000000000
          Top = 9.448825000000000000
          Width = 211.653680000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -19
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          HAlign = haCenter
          Memo.UTF8W = (
            'MEAL PLAN REPORT')
          ParentFont = False
          VAlign = vaCenter
        end
      end
      object Header1: TfrxHeader
        FillType = ftBrush
        FillGap.Top = 0
        FillGap.Left = 0
        FillGap.Bottom = 0
        FillGap.Right = 0
        Frame.Typ = []
        Height = 204.094620000000000000
        Top = 117.165430000000000000
        Width = 718.110700000000000000
        object UsersUserName: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 15.118120000000000000
          Width = 204.094620000000000000
          Height = 18.897650000000000000
          DataField = 'UserName'
          DataSet = frxDBUsers
          DataSetName = 'Users'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[Users."UserName"]')
          ParentFont = False
          VAlign = vaCenter
        end
        object UsersSurname: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 75.590600000000000000
          Width = 400.630180000000000000
          Height = 18.897650000000000000
          DataField = 'Surname'
          DataSet = frxDBUsers
          DataSetName = 'Users'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[Users."Surname"]')
          ParentFont = False
          VAlign = vaCenter
        end
        object UsersEmail: TfrxMemoView
          IndexTag = 1
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 136.063080000000000000
          Width = 400.630180000000000000
          Height = 18.897650000000000000
          DataField = 'Email'
          DataSet = frxDBUsers
          DataSetName = 'Users'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = []
          Frame.Typ = []
          Memo.UTF8W = (
            '[Users."Email"]')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo2: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = -3.779530000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Ime:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo3: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 49.133890000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'Prezime:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Memo4: TfrxMemoView
          AllowVectorExport = True
          Left = 22.677180000000000000
          Top = 113.385900000000000000
          Width = 94.488250000000000000
          Height = 18.897650000000000000
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlack
          Font.Height = -13
          Font.Name = 'Arial'
          Font.Style = [fsBold]
          Frame.Typ = []
          Memo.UTF8W = (
            'E-mail:')
          ParentFont = False
          VAlign = vaCenter
        end
        object Picture1: TfrxPictureView
          AllowVectorExport = True
          Left = 529.134200000000000000
          Top = 15.118120000000000000
          Width = 158.740260000000000000
          Height = 139.842610000000000000
          DataField = 'Photo'
          DataSet = frxDBUsers
          DataSetName = 'Users'
          Frame.Typ = []
          HightQuality = False
          Transparent = False
          TransparentColor = clWhite
        end
        object Line1: TfrxLineView
          AllowVectorExport = True
          Top = 204.094620000000000000
          Width = 718.110700000000000000
          Color = clBlack
          Frame.Typ = []
          Diagonal = True
        end
      end
    end
  end
  object frxDBMealPlans: TfrxDBDataset
    UserName = 'MealPlans'
    CloseDataSource = False
    FieldAliases.Strings = (
      'PlanID=PlanID'
      'UserID=UserID'
      'PlanDate=PlanDate'
      'PlanName=PlanName'
      'ItemCount=ItemCount')
    DataSet = AppDataModule1.TMealPlans
    BCDToCurrency = False
    DataSetOptions = []
    Left = 536
    Top = 336
  end
  object frxDBPlanItems: TfrxDBDataset
    UserName = 'PlanItems'
    CloseDataSource = False
    FieldAliases.Strings = (
      'ItemID=ItemID'
      'PlanID=PlanID'
      'RecipeName=RecipeName'
      'Servings=Servings'
      'WhenInDay=WhenInDay'
      'Comment=Comment'
      'PlanNameLkp=PlanNameLkp')
    DataSet = AppDataModule1.TPlanItems
    BCDToCurrency = False
    DataSetOptions = []
    Left = 472
    Top = 544
  end
  object frxDBUsers: TfrxDBDataset
    RangeBegin = rbCurrent
    RangeEnd = reCurrent
    UserName = 'Users'
    CloseDataSource = False
    DataSet = AppDataModule1.TUsers
    BCDToCurrency = False
    DataSetOptions = []
    Left = 536
    Top = 128
  end
  object OpenDialog1: TOpenDialog
    Filter = 'Images|*.bmp;*.jpg;*.jpeg;*.png|All files|*.*'
    Left = 808
    Top = 120
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'pdf'
    Filter = 'PDF files (*.pdf)|*.pdf'
    Left = 896
    Top = 120
  end
  object IdTCPClient1: TIdTCPClient
    ConnectTimeout = 0
    Host = '127.0.0.1'
    Port = 8080
    ReadTimeout = -1
    Left = 324
    Top = 431
  end
  object IdUDPClient1: TIdUDPClient
    BufferSize = 16384
    Host = '127.0.0.1'
    Port = 9090
    ReceiveTimeout = 2000
    Left = 249
    Top = 431
  end
end
