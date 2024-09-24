object formWait: TformWait
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  ClientHeight = 76
  ClientWidth = 394
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  Position = poOwnerFormCenter
  OnCloseQuery = FormCloseQuery
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  TextHeight = 13
  object lb: TLabel
    Left = 24
    Top = 25
    Width = 69
    Height = 13
    Caption = 'Please wait ...'
  end
end
