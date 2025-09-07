object FRatingForm: TFRatingForm
  Left = 735
  Top = 270
  Caption = 'Ocjene i komentari recepata'
  ClientHeight = 520
  ClientWidth = 680
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
    680
    520)
  TextHeight = 15
  object PanelInput: TPanel
    Left = 16
    Top = 16
    Width = 636
    Height = 120
    Anchors = [akLeft, akTop, akRight]
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 0
    ExplicitWidth = 632
    DesignSize = (
      636
      120)
    object LblRecipe: TLabel
      Left = 16
      Top = 16
      Width = 39
      Height = 15
      Caption = 'Recept:'
    end
    object LblRating: TLabel
      Left = 350
      Top = 16
      Width = 40
      Height = 15
      Caption = 'Ocjena:'
    end
    object LblRatingValue: TLabel
      Left = 546
      Top = 16
      Width = 19
      Height = 15
      Caption = '5/5'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Segoe UI'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object LblComment: TLabel
      Left = 16
      Top = 56
      Width = 55
      Height = 15
      Caption = 'Komentar:'
    end
    object CombRecipes: TComboBox
      Left = 80
      Top = 12
      Width = 250
      Height = 23
      Style = csDropDownList
      TabOrder = 0
    end
    object TrackRating: TTrackBar
      Left = 404
      Top = 11
      Width = 120
      Height = 25
      Max = 5
      Min = 1
      Position = 5
      TabOrder = 1
      OnChange = TrackRatingChange
    end
    object MemoComment: TMemo
      Left = 80
      Top = 52
      Width = 540
      Height = 54
      Anchors = [akLeft, akTop, akRight]
      MaxLength = 500
      ScrollBars = ssVertical
      TabOrder = 2
      ExplicitWidth = 536
    end
  end
  object LVRatings: TListView
    Left = 16
    Top = 152
    Width = 636
    Height = 280
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <>
    GridLines = True
    HideSelection = False
    ReadOnly = True
    RowSelect = True
    TabOrder = 1
    ViewStyle = vsReport
    OnSelectItem = LVRatingsSelectItem
    ExplicitWidth = 632
    ExplicitHeight = 279
  end
  object PanelButtons: TPanel
    Left = 16
    Top = 448
    Width = 636
    Height = 56
    Anchors = [akLeft, akRight, akBottom]
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 2
    ExplicitTop = 447
    ExplicitWidth = 632
    object BtnAdd: TButton
      Left = 16
      Top = 12
      Width = 100
      Height = 32
      Caption = 'Spremi ocjenu'
      TabOrder = 0
      OnClick = BtnAddClick
    end
    object BtnEdit: TButton
      Left = 130
      Top = 12
      Width = 100
      Height = 32
      Caption = 'Uredi ocjenu'
      TabOrder = 1
      OnClick = BtnEditClick
    end
    object BtnDelete: TButton
      Left = 244
      Top = 12
      Width = 100
      Height = 32
      Caption = 'Ukloni ocjenu'
      TabOrder = 2
      OnClick = BtnDeleteClick
    end
  end
end
