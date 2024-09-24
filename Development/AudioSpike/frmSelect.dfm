object formSelect: TformSelect
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Select Parameters ...'
  ClientHeight = 423
  ClientWidth = 436
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poMainFormCenter
  TextHeight = 13
  object lblX: TLabel
    Left = 12
    Top = 10
    Width = 36
    Height = 13
    Caption = 'X-Axis'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblY: TLabel
    Left = 227
    Top = 10
    Width = 36
    Height = 13
    Caption = 'Y-Axis'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbX: TListBox
    Left = 8
    Top = 28
    Width = 209
    Height = 348
    ItemHeight = 13
    TabOrder = 0
    OnClick = lbClick
  end
  object lbY: TListBox
    Left = 223
    Top = 28
    Width = 209
    Height = 348
    ItemHeight = 13
    TabOrder = 1
    OnClick = lbClick
  end
  object btnOk: TButton
    Left = 96
    Top = 386
    Width = 121
    Height = 33
    Caption = 'Ok'
    Default = True
    Enabled = False
    ModalResult = 1
    TabOrder = 2
  end
  object btnCancel: TButton
    Left = 223
    Top = 386
    Width = 121
    Height = 33
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
end
