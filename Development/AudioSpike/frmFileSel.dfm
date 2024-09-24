object formFileSel: TformFileSel
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  ClientHeight = 415
  ClientWidth = 417
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poMainFormCenter
  DesignSize = (
    417
    415)
  TextHeight = 13
  object lblSelEq: TLabel
    Left = 14
    Top = 8
    Width = 141
    Height = 13
    Caption = 'Please select an equalisation:'
  end
  object flb: TFileListBox
    Left = 8
    Top = 24
    Width = 297
    Height = 385
    ItemHeight = 13
    Mask = '*.wav'
    TabOrder = 0
    OnChange = flbClick
    OnClick = flbClick
    OnDblClick = flbDblClick
  end
  object btnOk: TButton
    Left = 311
    Top = 24
    Width = 106
    Height = 30
    Anchors = [akTop, akRight]
    Caption = 'Ok'
    Default = True
    Enabled = False
    TabOrder = 1
    OnClick = btnOkClick
  end
  object btnCancel: TButton
    Left = 311
    Top = 58
    Width = 106
    Height = 30
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
end
