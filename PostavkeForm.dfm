object FPostavkeForm: TFPostavkeForm
  Left = 347
  Top = 332
  Caption = 'Postavke'
  ClientHeight = 268
  ClientWidth = 1109
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Position = poDesigned
  OnShow = FormShow
  TextHeight = 15
  object LblJezik: TLabel
    Left = 144
    Top = 34
    Width = 27
    Height = 17
    Caption = 'Jezik'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object LblFont: TLabel
    Left = 145
    Top = 87
    Width = 25
    Height = 17
    Caption = 'Font'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Segoe UI'
    Font.Style = []
    ParentFont = False
  end
  object LblBoja: TLabel
    Left = 144
    Top = 141
    Width = 26
    Height = 15
    Caption = 'Boja '
  end
  object CmbLang: TComboBox
    Left = 260
    Top = 31
    Width = 145
    Height = 23
    Style = csDropDownList
    TabOrder = 0
  end
  object BtnOK: TButton
    Left = 464
    Top = 199
    Width = 75
    Height = 25
    Caption = 'U redu'
    ModalResult = 1
    TabOrder = 1
  end
  object BtnCancel: TButton
    Left = 32
    Top = 199
    Width = 75
    Height = 25
    Caption = 'Odustani'
    ModalResult = 2
    TabOrder = 2
  end
  object BtnFont: TButton
    Left = 443
    Top = 83
    Width = 81
    Height = 25
    Caption = 'Odaberi font'
    TabOrder = 3
    OnClick = BtnFontClick
  end
  object EdtFont: TEdit
    Left = 248
    Top = 84
    Width = 169
    Height = 23
    ReadOnly = True
    TabOrder = 4
  end
  object CBColor: TColorBox
    Left = 260
    Top = 138
    Width = 145
    Height = 22
    TabOrder = 5
  end
  object GBTerms: TGroupBox
    Left = 632
    Top = 8
    Width = 457
    Height = 252
    Caption = 'Terms of Services'
    TabOrder = 6
    object LblPct: TLabel
      Left = 312
      Top = 192
      Width = 10
      Height = 15
      Caption = '%'
    end
    object LblLimit: TLabel
      Left = 312
      Top = 166
      Width = 99
      Height = 15
      Caption = 'Ograni'#269'enje brzine'
    end
    object BtnDownloadTerms: TButton
      Left = 157
      Top = 50
      Width = 143
      Height = 39
      Caption = 'Preuzmi PDF'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -21
      Font.Name = 'Segoe UI'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = BtnDownloadTermsClick
    end
    object BtnOpenTerms: TButton
      Left = 191
      Top = 214
      Width = 75
      Height = 25
      Caption = 'Otvori PDF'
      TabOrder = 1
      OnClick = BtnOpenTermsClick
    end
    object PBDownload: TProgressBar
      Left = 156
      Top = 191
      Width = 145
      Height = 17
      TabOrder = 2
    end
    object CmbLimit: TComboBox
      Left = 156
      Top = 162
      Width = 145
      Height = 23
      Style = csDropDownList
      TabOrder = 3
      OnChange = CmbLimitChange
      Items.Strings = (
        '50 KB/s'
        '100 KB/s'
        '500 KB/s'
        '1000 KB/s')
    end
  end
  object Http: TIdHTTP
    Intercept = Throttle
    IOHandler = SSL
    OnWork = HttpWork
    OnWorkBegin = HttpWorkBegin
    OnWorkEnd = HttpWorkEnd
    HandleRedirects = True
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.ContentRangeEnd = -1
    Request.ContentRangeStart = -1
    Request.ContentRangeInstanceLength = -1
    Request.Accept = 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8'
    Request.BasicAuthentication = False
    Request.UserAgent = 'Mozilla/3.0 (compatible; Indy Library)'
    Request.Ranges.Units = 'bytes'
    Request.Ranges = <>
    HTTPOptions = [hoForceEncodeParams]
    Left = 640
    Top = 128
  end
  object SSL: TIdSSLIOHandlerSocketOpenSSL
    Intercept = Throttle
    MaxLineAction = maException
    Port = 0
    DefaultPort = 0
    SSLOptions.Mode = sslmUnassigned
    SSLOptions.VerifyMode = []
    SSLOptions.VerifyDepth = 0
    Left = 640
    Top = 208
  end
  object Throttle: TIdInterceptThrottler
    BitsPerSec = 0
    RecvBitsPerSec = 0
    SendBitsPerSec = 0
    Left = 712
    Top = 168
  end
end
