object FReceptar: TFReceptar
  Left = 449
  Top = 228
  Anchors = [akLeft, akTop, akRight, akBottom]
  Caption = 'Moj Receptar'
  ClientHeight = 593
  ClientWidth = 1084
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Menu = MainMenu1
  Position = poDesigned
  OnClose = FormClose
  OnCreate = FormCreate
  OnKeyDown = LVRecipesKeyDown
  OnShow = FormShow
  DesignSize = (
    1084
    593)
  TextHeight = 15
  object LblLogged: TLabel
    Left = 792
    Top = 8
    Width = 120
    Height = 15
    Caption = 'PRIJAVLJENI KORISNIK:'
  end
  object LVRecipes: TListView
    Left = 24
    Top = 55
    Width = 1001
    Height = 442
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        AutoSize = True
      end
      item
        AutoSize = True
      end
      item
        AutoSize = True
      end>
    Constraints.MinWidth = 100
    HideSelection = False
    ReadOnly = True
    RowSelect = True
    ParentShowHint = False
    ShowHint = False
    TabOrder = 0
    ViewStyle = vsReport
    OnKeyDown = LVRecipesKeyDown
  end
  object BtnAdd: TButton
    Left = 885
    Top = 517
    Width = 157
    Height = 38
    Anchors = [akRight, akBottom]
    Caption = 'Dodaj novi recept'
    TabOrder = 1
    OnClick = BtnAddClick
    ExplicitLeft = 881
    ExplicitTop = 516
  end
  object BtnEdit: TButton
    Left = 481
    Top = 517
    Width = 105
    Height = 38
    Anchors = [akRight, akBottom]
    Caption = 'Uredi recept'
    TabOrder = 2
    OnClick = BtnEditClick
    ExplicitLeft = 477
    ExplicitTop = 516
  end
  object BtnDelete: TButton
    Left = 41
    Top = 517
    Width = 105
    Height = 38
    Anchors = [akLeft, akBottom]
    Caption = 'Ukloni recept'
    TabOrder = 3
    OnClick = BtnDeleteClick
    ExplicitTop = 516
  end
  object MainMenu1: TMainMenu
    Left = 16
    Top = 65528
    object mnuPrijava: TMenuItem
      Caption = 'Prijava'
      ShortCut = 16460
      OnClick = mnuPrijavaClick
    end
    object mnuPostavke: TMenuItem
      Caption = 'Postavke'
      ShortCut = 16496
      OnClick = mnuPostavkeClick
    end
    object mnuLoadXML: TMenuItem
      Caption = 'U'#269'itaj recepte'#8230
      ShortCut = 16460
      Visible = False
      OnClick = mnuLoadXMLClick
    end
    object mnuRatings: TMenuItem
      Caption = 'Ocjeni recept'
      ShortCut = 49234
      OnClick = mnuRatingsClick
    end
    object mnuNutrition: TMenuItem
      Caption = 'Nutritivne informacije'
      ShortCut = 49230
      OnClick = mnuNutritionClick
    end
    object mnuMealPlans: TMenuItem
      Caption = 'Planovi prehrane'#8230
      OnClick = mnuMealPlansClick
    end
  end
  object XMLDocument1: TXMLDocument
    Active = True
    FileName = 
      'C:\Users\MobiS PC\Desktop\SCHOOL\4.SEMESTAR\NAPREDNE TEHNIKE PRO' +
      'GRAMIRANJA\&PROJEKT\MojReceptar\PopisRecepata.xml'
    Left = 64
    Top = 80
  end
end
