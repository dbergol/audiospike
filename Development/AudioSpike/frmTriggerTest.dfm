object formTriggerTest: TformTriggerTest
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Trigger Test'
  ClientHeight = 284
  ClientWidth = 411
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  Position = poMainFormCenter
  DesignSize = (
    411
    284)
  TextHeight = 13
  object btnOk: TButton
    Left = 72
    Top = 245
    Width = 121
    Height = 33
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 0
    ExplicitTop = 219
  end
  object btnCancel: TButton
    Left = 217
    Top = 245
    Width = 121
    Height = 33
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
    ExplicitTop = 219
  end
  object gbTriggerTest: TGroupBox
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 405
    Height = 112
    Align = alTop
    Caption = 'Trigger Test'
    TabOrder = 2
    object lvBottom: TListView
      AlignWithMargins = True
      Left = 8
      Top = 18
      Width = 389
      Height = 89
      Margins.Left = 6
      Margins.Right = 6
      Align = alClient
      BevelInner = bvNone
      BevelOuter = bvNone
      BevelKind = bkSoft
      BorderStyle = bsNone
      Columns = <
        item
          Width = 150
        end
        item
          Width = 220
        end>
      Items.ItemData = {
        05180100000400000000000000FFFFFFFFFFFFFFFF01000000FFFFFFFF000000
        001054007200690067006700650072007300200050006C006100790065006400
        3A0000A07E2D1200000000FFFFFFFFFFFFFFFF01000000FFFFFFFF0000000012
        5400720069006700670065007200730020004400650074006500630074006500
        64003A0000709F2D1200000000FFFFFFFFFFFFFFFF01000000FFFFFFFF000000
        00144C0061007300740020004400650074006500630074006500640020005600
        61006C00750065003A0000C0762D1200000000FFFFFFFFFFFFFFFF01000000FF
        FFFFFF00000000144C0061007300740020005400720069006700670065007200
        20004A00690074007400650072003A000058742D12FFFFFFFFFFFFFFFF}
      ReadOnly = True
      ParentColor = True
      ShowColumnHeaders = False
      StateImages = il
      TabOrder = 0
      TabStop = False
      ViewStyle = vsReport
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 401
      ExplicitHeight = 68
    end
  end
  object gbTriggerValues: TGroupBox
    Left = 0
    Top = 118
    Width = 411
    Height = 117
    Align = alTop
    Caption = 'Trigger Values'
    TabOrder = 3
    object lvTop: TListView
      AlignWithMargins = True
      Left = 8
      Top = 21
      Width = 395
      Height = 89
      Margins.Left = 6
      Margins.Top = 6
      Margins.Right = 6
      Align = alTop
      BevelInner = bvNone
      BevelOuter = bvNone
      BevelKind = bkSoft
      BorderStyle = bsNone
      Columns = <
        item
          Width = 150
        end
        item
          Width = 220
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Items.ItemData = {
        055C0100000400000000000000FFFFFFFFFFFFFFFF01000000FFFFFFFF000000
        000F540072006900670067006500720020004F00750074007000750074003A00
        0161006800110200000000FFFFFFFFFFFFFFFF01000000FFFFFFFF000000000E
        5400720069006700670065007200200049006E007000750074003A0000000511
        0200000000FFFFFFFFFFFFFFFF01000000FFFFFFFF000000000E540072006900
        67006700650072002000560061006C00750065003A0016200020002000200020
        0020002000200020002000200020002000200020002000200020002000200064
        004200B0FC100200000000FFFFFFFFFFFFFFFF01000000FFFFFFFF0000000014
        44006500740065006300740069006F006E002000540068007200650073006800
        6F006C0064003A00162000200020002000200020002000200020002000200020
        002000200020002000200020002000200064004200E8D91002FFFFFFFFFFFFFF
        FF}
      ReadOnly = True
      ParentColor = True
      ParentFont = False
      ShowColumnHeaders = False
      StateImages = il
      TabOrder = 0
      TabStop = False
      ViewStyle = vsReport
    end
    object cobTriggerThreshold: TComboBox
      Left = 164
      Top = 87
      Width = 53
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 1
      Text = '-1'
      OnChange = cobTriggerThresholdChange
      Items.Strings = (
        '-1'
        '-2'
        '-3'
        '-4'
        '-5'
        '-6'
        '-7'
        '-8'
        '-9'
        '-10'
        '-11'
        '-12'
        '-13'
        '-14'
        '-15'
        '-16'
        '-17'
        '-18'
        '-19'
        '-20')
    end
    object cobTriggerValue: TComboBox
      Left = 164
      Top = 65
      Width = 53
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 2
      Text = '-1'
      OnChange = cobTriggerValueChange
      Items.Strings = (
        '-1'
        '-2'
        '-3'
        '-4'
        '-5'
        '-6'
        '-7'
        '-8'
        '-9'
        '-10')
    end
  end
  object Timer: TTimer
    Enabled = False
    Interval = 10
    OnTimer = TimerTimer
    Left = 352
    Top = 88
  end
  object il: TImageList
    Height = 21
    Width = 1
    Left = 16
    Top = 192
  end
end
