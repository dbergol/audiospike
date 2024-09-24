object frmOL: TfrmOL
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'License'
  ClientHeight = 135
  ClientWidth = 622
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  DesignSize = (
    622
    135)
  PixelsPerInch = 96
  TextHeight = 13
  object gb: TGroupBox
    AlignWithMargins = True
    Left = 3
    Top = 8
    Width = 616
    Height = 89
    Margins.Top = 8
    Align = alTop
    TabOrder = 0
    DesignSize = (
      616
      89)
    object edtProductKey: TLabeledEdit
      Left = 80
      Top = 20
      Width = 523
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 65
      EditLabel.Height = 13
      EditLabel.Caption = 'Product Key: '
      LabelPosition = lpLeft
      TabOrder = 0
      OnChange = edtProductKeyChange
    end
    object edtHostId: TLabeledEdit
      Left = 80
      Top = 52
      Width = 523
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 43
      EditLabel.Height = 13
      EditLabel.Caption = 'Host ID: '
      LabelPosition = lpLeft
      ReadOnly = True
      TabOrder = 1
      OnChange = edtProductKeyChange
    end
  end
  object btnOK: TButton
    Left = 502
    Top = 103
    Width = 105
    Height = 28
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Close'
    ModalResult = 1
    TabOrder = 1
  end
  object btnActivate: TButton
    Left = 391
    Top = 103
    Width = 105
    Height = 28
    Anchors = [akRight, akBottom]
    Caption = 'Activate'
    TabOrder = 2
    OnClick = btnActivateClick
  end
  object btnClipboard: TButton
    Left = 8
    Top = 103
    Width = 73
    Height = 28
    Anchors = [akRight, akBottom]
    Caption = 'Clipboard'
    TabOrder = 3
    OnClick = btnClipboardClick
  end
end
