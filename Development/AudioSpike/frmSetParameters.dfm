object formSetParameters: TformSetParameters
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = 'Set Parameters'
  ClientHeight = 114
  ClientWidth = 294
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  Position = poMainFormCenter
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  TextHeight = 13
  object btnOk: TButton
    Left = 96
    Top = 81
    Width = 105
    Height = 26
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object pnl1: TPanel
    Left = 0
    Top = 0
    Width = 294
    Height = 45
    Align = alTop
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object lbU1: TLabel
      Left = 207
      Top = 16
      Width = 21
      Height = 13
      Caption = 'lbU1'
    end
    object btnReset1: TSpeedButton
      Left = 233
      Top = 12
      Width = 21
      Height = 20
      Margins.Left = 2
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Glyph.Data = {
        E6000000424DE60000000000000076000000280000000C0000000E0000000100
        04000000000070000000C40E0000C40E00001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        0000FF2FF82228FF0000FF22828F828F0000FF2228FFF82F0000FF2222FFFF2F
        0000FF22222FFFFF0000FFFFFFFFFFFF0000FFFFFF22222F0000FF2FFFF2222F
        0000FF28FFF8222F0000FF828F82822F0000FFF82228FF2F0000FFFFFFFFFFF8
        0000FFFFFFFFFF880000}
      Margin = 1
      OnClick = btnReset1Click
    end
    object led1: TLabeledEdit
      Left = 96
      Top = 12
      Width = 105
      Height = 21
      Alignment = taRightJustify
      EditLabel.Width = 40
      EditLabel.Height = 21
      EditLabel.Caption = 'Minimum'
      LabelPosition = lpLeft
      LabelSpacing = 6
      TabOrder = 0
      Text = ''
      OnChange = ledChange
      OnKeyPress = ledKeyPress
    end
  end
  object pnl2: TPanel
    Left = 0
    Top = 45
    Width = 294
    Height = 30
    Align = alTop
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 1
    object lbU2: TLabel
      Left = 207
      Top = 6
      Width = 21
      Height = 13
      Caption = 'lbU2'
    end
    object btnReset2: TSpeedButton
      Left = 235
      Top = 2
      Width = 21
      Height = 20
      Margins.Left = 2
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Glyph.Data = {
        E6000000424DE60000000000000076000000280000000C0000000E0000000100
        04000000000070000000C40E0000C40E00001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
        0000FF2FF82228FF0000FF22828F828F0000FF2228FFF82F0000FF2222FFFF2F
        0000FF22222FFFFF0000FFFFFFFFFFFF0000FFFFFF22222F0000FF2FFFF2222F
        0000FF28FFF8222F0000FF828F82822F0000FFF82228FF2F0000FFFFFFFFFFF8
        0000FFFFFFFFFF880000}
      Margin = 1
      OnClick = btnReset2Click
    end
    object led2: TLabeledEdit
      Left = 96
      Top = 2
      Width = 105
      Height = 21
      Alignment = taRightJustify
      EditLabel.Width = 44
      EditLabel.Height = 21
      EditLabel.Caption = 'Maximum'
      LabelPosition = lpLeft
      LabelSpacing = 6
      TabOrder = 0
      Text = ''
      OnChange = ledChange
      OnKeyPress = ledKeyPress
    end
  end
end
