object formSelectChannel: TformSelectChannel
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Select Channels'
  ClientHeight = 335
  ClientWidth = 264
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poOwnerFormCenter
  TextHeight = 13
  object lb: TListBox
    Left = 0
    Top = 0
    Width = 264
    Height = 335
    Align = alClient
    ItemHeight = 13
    TabOrder = 0
    OnClick = lbClick
  end
end
