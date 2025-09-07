object FLoginForm: TFLoginForm
  Left = 756
  Top = 332
  Caption = 'Login'
  ClientHeight = 207
  ClientWidth = 456
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Position = poDesigned
  OnShow = FormShow
  TextHeight = 15
  object LblUser: TLabel
    Left = 56
    Top = 48
    Width = 81
    Height = 15
    Caption = 'Korisni'#269'ko ime:'
  end
  object LblPass: TLabel
    Left = 56
    Top = 112
    Width = 40
    Height = 15
    Caption = 'Lozinka'
  end
  object EditUser: TEdit
    Left = 168
    Top = 44
    Width = 137
    Height = 23
    TabOrder = 0
    TextHint = 'Unesite korisnicko ime'
    OnChange = EditUserChange
  end
  object EditPass: TEdit
    Left = 168
    Top = 108
    Width = 137
    Height = 23
    TabOrder = 1
    TextHint = 'Unesite lozinku'
    OnChange = EditPassChange
  end
  object BtnOK: TButton
    Left = 360
    Top = 175
    Width = 75
    Height = 25
    Caption = 'Login'
    ModalResult = 1
    TabOrder = 2
    OnClick = BtnOKClick
  end
  object BtnCancel: TButton
    Left = 20
    Top = 175
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
  object CheckBoxRemember: TCheckBox
    Left = 168
    Top = 144
    Width = 137
    Height = 17
    Caption = 'Remember me'
    TabOrder = 4
  end
end
