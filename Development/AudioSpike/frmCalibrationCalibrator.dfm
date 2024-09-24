object formCalibrationCalibrator: TformCalibrationCalibrator
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Calibration'
  ClientHeight = 193
  ClientWidth = 587
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
    587
    193)
  TextHeight = 13
  object lbDebug: TLabel
    Left = 8
    Top = 161
    Width = 74
    Height = 16
    Caption = 'DEBUG-RMS'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    StyleElements = [seClient, seBorder]
  end
  object btnOk: TButton
    Left = 454
    Top = 152
    Width = 121
    Height = 33
    Anchors = [akRight, akBottom]
    Caption = 'Start'
    Default = True
    TabOrder = 0
    OnClick = btnOkClick
  end
  object btnEnterLevel: TButton
    Left = 92
    Top = 152
    Width = 171
    Height = 33
    Anchors = [akRight, akBottom]
    Caption = 'Set Calbration Level'
    TabOrder = 1
    OnClick = btnEnterLevelClick
  end
  object btnCancel: TButton
    Left = 318
    Top = 152
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
    Width = 587
    Height = 145
    Align = alTop
    TabOrder = 3
    object lblCalLevel: TLabel
      AlignWithMargins = True
      Left = 10
      Top = 104
      Width = 567
      Height = 28
      Margins.Left = 8
      Margins.Top = 20
      Margins.Right = 8
      Align = alTop
      Alignment = taCenter
      AutoSize = False
      Caption = 'Calibration  Level: 80 dB'
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
      Width = 567
      Height = 66
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
    end
    object lblClip: TLabel
      Left = 469
      Top = 107
      Width = 28
      Height = 14
      AutoSize = False
      Caption = 'Clip:'
    end
    object shClip: TShape
      Left = 499
      Top = 109
      Width = 25
      Height = 11
      Brush.Color = clLime
    end
  end
  object ClipTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = ClipTimerTimer
    Left = 8
    Top = 16
  end
end
