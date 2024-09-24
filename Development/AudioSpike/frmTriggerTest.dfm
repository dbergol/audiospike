object formTriggerTest: TformTriggerTest
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Trigger Test'
  ClientHeight = 147
  ClientWidth = 411
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  Position = poMainFormCenter
  TextHeight = 13
  object lv: TListView
    AlignWithMargins = True
    Left = 6
    Top = 6
    Width = 399
    Height = 90
    Margins.Left = 6
    Margins.Top = 6
    Margins.Right = 6
    Align = alTop
    BevelKind = bkSoft
    BorderStyle = bsNone
    Columns = <
      item
        Width = 120
      end
      item
        Width = 200
      end>
    Items.ItemData = {
      05490100000500000000000000FFFFFFFFFFFFFFFF01000000FFFFFFFF000000
      000F540072006900670067006500720020004F00750074007000750074003A00
      00C089711500000000FFFFFFFFFFFFFFFF01000000FFFFFFFF000000000E5400
      720069006700670065007200200049006E007000750074003A0000584A291900
      000000FFFFFFFFFFFFFFFF01000000FFFFFFFF00000000105400720069006700
      6700650072007300200050006C0061007900650064003A0000C02C0019000000
      00FFFFFFFFFFFFFFFF01000000FFFFFFFF000000001254007200690067006700
      6500720073002000440065007400650063007400650064003A0000F82C001900
      000000FFFFFFFFFFFFFFFF01000000FFFFFFFF00000000134C00610073007400
      200054007200690067006700650072002000560061006C00750065003A000048
      61E628FFFFFFFFFFFFFFFFFFFF}
    ReadOnly = True
    ParentColor = True
    ShowColumnHeaders = False
    TabOrder = 0
    ViewStyle = vsReport
    ExplicitLeft = 0
    ExplicitTop = 0
    ExplicitWidth = 411
  end
  object btnOk: TButton
    Left = 144
    Top = 106
    Width = 121
    Height = 33
    Cancel = True
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 1
    OnClick = btnOkClick
  end
  object Timer: TTimer
    Enabled = False
    Interval = 10
    OnTimer = TimerTimer
    Left = 352
    Top = 88
  end
end
