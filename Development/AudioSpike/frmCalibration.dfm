object formCalibration: TformCalibration
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Calibration'
  ClientHeight = 416
  ClientWidth = 583
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  Position = poMainFormCenter
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    583
    416)
  TextHeight = 13
  object btnOk: TButton
    Left = 442
    Top = 375
    Width = 121
    Height = 33
    Anchors = [akRight, akBottom]
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = btnOkClick
  end
  object btnEnterLevel: TButton
    Left = 130
    Top = 375
    Width = 121
    Height = 33
    Anchors = [akRight, akBottom]
    Caption = 'Enter Target Level'
    TabOrder = 1
    OnClick = btnEnterLevelClick
  end
  object btnCancel: TButton
    Left = 306
    Top = 375
    Width = 121
    Height = 33
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object gb: TGroupBox
    Left = 0
    Top = 0
    Width = 583
    Height = 361
    Align = alTop
    TabOrder = 3
    object lblGain: TLabel
      Left = 253
      Top = 270
      Width = 29
      Height = 14
      AutoSize = False
      Caption = 'Gain:'
    end
    object lblTargetLevel: TLabel
      AlignWithMargins = True
      Left = 10
      Top = 144
      Width = 563
      Height = 28
      Margins.Left = 8
      Margins.Top = 20
      Margins.Right = 8
      Align = alTop
      Alignment = taCenter
      AutoSize = False
      Caption = 'Target Level: 80 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      WordWrap = True
      ExplicitTop = 141
      ExplicitWidth = 565
    end
    object lblDescription: TLabel
      AlignWithMargins = True
      Left = 10
      Top = 15
      Width = 563
      Height = 106
      Margins.Left = 8
      Margins.Top = 0
      Margins.Right = 8
      Align = alTop
      AutoSize = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      WordWrap = True
      ExplicitWidth = 565
    end
    object btnM10: TSpeedButton
      Tag = -100
      Left = 10
      Top = 262
      Width = 57
      Height = 49
      Caption = '-10 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object btnM2: TSpeedButton
      Tag = -20
      Left = 70
      Top = 262
      Width = 57
      Height = 49
      Caption = '-2 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object btnM1: TSpeedButton
      Tag = -10
      Left = 130
      Top = 262
      Width = 57
      Height = 49
      Caption = '-1 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object btn1: TSpeedButton
      Tag = 10
      Left = 400
      Top = 262
      Width = 57
      Height = 49
      Caption = '+1 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object btn2: TSpeedButton
      Tag = 20
      Left = 460
      Top = 262
      Width = 57
      Height = 49
      Caption = '+2 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object btn10: TSpeedButton
      Tag = 100
      Left = 520
      Top = 262
      Width = 57
      Height = 49
      Caption = '+10 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object btnM01: TSpeedButton
      Tag = -1
      Left = 190
      Top = 262
      Width = 57
      Height = 49
      Caption = '-0.1 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object btn01: TSpeedButton
      Tag = 1
      Left = 340
      Top = 262
      Width = 57
      Height = 49
      Caption = '+0.1 dB'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnLevelClick
    end
    object lblClip: TLabel
      Left = 253
      Top = 288
      Width = 35
      Height = 14
      AutoSize = False
      Caption = 'Clip:'
    end
    object lblGainVal: TLabel
      Left = 284
      Top = 270
      Width = 52
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
    end
    object shClip: TShape
      Left = 292
      Top = 290
      Width = 25
      Height = 11
      Brush.Color = clLime
    end
    object tb: TTrackBar
      Left = 2
      Top = 315
      Width = 579
      Height = 44
      Align = alBottom
      LineSize = 0
      Max = 200
      Min = -1000
      PageSize = 0
      TabOrder = 0
      TabStop = False
      ThumbLength = 30
      TickMarks = tmBoth
      TickStyle = tsNone
      OnChange = tbChange
    end
  end
  object ClipTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = ClipTimerTimer
    Left = 8
    Top = 368
  end
end
