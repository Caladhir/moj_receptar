object FNutritionForm: TFNutritionForm
  Left = 551
  Top = 208
  Caption = 'Nutritivne informacije recepata'
  ClientHeight = 664
  ClientWidth = 900
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Position = poDesigned
  OnCreate = FormCreate
  OnShow = FormShow
  DesignSize = (
    900
    664)
  TextHeight = 15
  object PanelInput: TPanel
    Left = 16
    Top = 16
    Width = 856
    Height = 200
    Anchors = [akLeft, akTop, akRight]
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 0
    ExplicitWidth = 852
    object LblRecipe: TLabel
      Left = 221
      Top = 35
      Width = 39
      Height = 15
      Caption = 'Recept:'
    end
    object LblCalories: TLabel
      Left = 176
      Top = 96
      Width = 42
      Height = 15
      Caption = 'Kalorije:'
    end
    object LblProteins: TLabel
      Left = 296
      Top = 96
      Width = 62
      Height = 15
      Caption = 'Proteini (g):'
    end
    object LblCarbs: TLabel
      Left = 416
      Top = 96
      Width = 92
      Height = 15
      Caption = 'Ugljikohidrati (g):'
    end
    object LblFats: TLabel
      Left = 556
      Top = 96
      Width = 50
      Height = 15
      Caption = 'Masti (g):'
    end
    object LblFiber: TLabel
      Left = 176
      Top = 136
      Width = 56
      Height = 15
      Caption = 'Vlakna (g):'
    end
    object LblSugar: TLabel
      Left = 296
      Top = 136
      Width = 49
      Height = 15
      Caption = #352'e'#263'er (g):'
    end
    object LblSalts: TLabel
      Left = 416
      Top = 136
      Width = 22
      Height = 15
      Caption = 'Soli:'
    end
    object CombRecipes: TComboBox
      Left = 291
      Top = 31
      Width = 275
      Height = 23
      Style = csDropDownList
      TabOrder = 0
    end
    object EditCalories: TEdit
      Left = 176
      Top = 112
      Width = 60
      Height = 23
      TabOrder = 1
    end
    object EditProteins: TEdit
      Left = 296
      Top = 112
      Width = 60
      Height = 23
      TabOrder = 2
    end
    object EditCarbs: TEdit
      Left = 416
      Top = 112
      Width = 60
      Height = 23
      TabOrder = 3
    end
    object EditFats: TEdit
      Left = 556
      Top = 112
      Width = 60
      Height = 23
      TabOrder = 4
    end
    object EditFiber: TEdit
      Left = 176
      Top = 152
      Width = 60
      Height = 23
      TabOrder = 5
    end
    object EditSugar: TEdit
      Left = 296
      Top = 152
      Width = 60
      Height = 23
      TabOrder = 6
    end
    object EditSalts: TEdit
      Left = 416
      Top = 157
      Width = 60
      Height = 23
      TabOrder = 7
    end
  end
  object LVNutrition: TListView
    Left = 16
    Top = 232
    Width = 856
    Height = 284
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <>
    GridLines = True
    HideSelection = False
    ReadOnly = True
    RowSelect = True
    TabOrder = 1
    ViewStyle = vsReport
    OnSelectItem = LVNutritionSelectItem
    ExplicitWidth = 852
    ExplicitHeight = 283
  end
  object PanelButtons: TPanel
    Left = 16
    Top = 532
    Width = 856
    Height = 56
    Anchors = [akLeft, akRight, akBottom]
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 2
    ExplicitTop = 531
    ExplicitWidth = 852
    object BtnAdd: TButton
      Left = 16
      Top = 12
      Width = 169
      Height = 32
      Caption = 'Dodaj nutritivne podatke'
      TabOrder = 0
      OnClick = BtnAddClick
    end
    object BtnEdit: TButton
      Left = 245
      Top = 12
      Width = 100
      Height = 32
      Caption = 'Uredi podatke'
      TabOrder = 1
      OnClick = BtnEditClick
    end
    object BtnDelete: TButton
      Left = 722
      Top = 12
      Width = 100
      Height = 32
      Caption = 'Ukloni podatke'
      TabOrder = 2
      OnClick = BtnDeleteClick
    end
  end
end
