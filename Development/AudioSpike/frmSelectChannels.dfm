object formSelectChannels: TformSelectChannels
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Select Channels ...'
  ClientHeight = 417
  ClientWidth = 306
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poMainFormCenter
  TextHeight = 13
  object btnOk: TButton
    Left = 32
    Top = 377
    Width = 121
    Height = 33
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object btnCancel: TButton
    Left = 159
    Top = 377
    Width = 121
    Height = 33
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object clb: TCheckListBox
    Left = 8
    Top = 8
    Width = 297
    Height = 361
    ItemHeight = 17
    TabOrder = 2
  end
end
