object FReceptForm: TFReceptForm
  Left = 654
  Top = 270
  Caption = 'Recept'
  ClientHeight = 442
  ClientWidth = 628
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Position = poDesigned
  OnShow = FormShow
  TextHeight = 15
  object LblNaziv: TLabel
    Left = 32
    Top = 335
    Width = 77
    Height = 15
    Alignment = taCenter
    Caption = 'Naziv recepta'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Segoe UI'
    Font.Style = [fsBold]
    ParentFont = False
    WordWrap = True
  end
  object LblTezina: TLabel
    Left = 48
    Top = 58
    Width = 87
    Height = 39
    Caption = 'Te'#382'ina recepta (1-5)'
    WordWrap = True
  end
  object LblSastojak: TLabel
    Left = 48
    Top = 115
    Width = 43
    Height = 15
    Caption = 'Sastojak'
  end
  object LblKolicina: TLabel
    Left = 48
    Top = 172
    Width = 89
    Height = 15
    Caption = 'Koli'#269'ina (g ili ml)'
    WordWrap = True
  end
  object EditNaziv: TEdit
    Left = 160
    Top = 335
    Width = 137
    Height = 23
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    TextHint = 'Unesite naziv'
  end
  object TrackTezina: TTrackBar
    Left = 147
    Top = 54
    Width = 150
    Height = 25
    Max = 5
    Min = 1
    ParentShowHint = False
    Position = 3
    ShowHint = False
    TabOrder = 1
  end
  object EditSastojak: TEdit
    Left = 160
    Top = 111
    Width = 121
    Height = 23
    TabOrder = 2
    TextHint = 'Unesite sastojak'
  end
  object EditKolicina: TEdit
    Left = 160
    Top = 169
    Width = 121
    Height = 23
    TabOrder = 3
    TextHint = 'Unesite kolicinu'
  end
  object BtnOK: TButton
    Left = 330
    Top = 400
    Width = 95
    Height = 25
    Caption = 'Spremi recept'
    ModalResult = 1
    TabOrder = 4
    OnClick = BtnOKClick
  end
  object BtnCancel: TButton
    Left = 512
    Top = 400
    Width = 75
    Height = 25
    Caption = 'Odustani'
    ModalResult = 2
    TabOrder = 5
  end
  object LBItems: TListBox
    Left = 330
    Top = 8
    Width = 257
    Height = 361
    ItemHeight = 15
    TabOrder = 6
  end
  object BtnAddS: TButton
    Left = 184
    Top = 224
    Width = 97
    Height = 33
    Caption = 'Dodaj sastojak'
    TabOrder = 7
    WordWrap = True
    OnClick = BtnAddSClick
  end
  object BtnRemoveS: TButton
    Left = 48
    Top = 224
    Width = 97
    Height = 33
    Caption = 'Ukloni sastojak'
    TabOrder = 8
    WordWrap = True
    OnClick = BtnRemoveSClick
  end
end
