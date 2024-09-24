inherited formStimuli: TformStimuli
  Left = 0
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Stimuli'
  ClientHeight = 520
  ClientWidth = 814
  Color = clBtnFace
  Visible = False
  StyleElements = [seFont, seClient, seBorder]
  ExplicitWidth = 830
  ExplicitHeight = 559
  TextHeight = 13
  object pnl: TPanel
    Left = 0
    Top = 0
    Width = 814
    Height = 520
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object lv: TListView
      Left = 0
      Top = 0
      Width = 814
      Height = 520
      Align = alClient
      Columns = <
        item
          Caption = 'N'
          Width = 30
        end
        item
          Caption = 'Name'
          Width = 80
        end>
      GridLines = True
      ReadOnly = True
      RowSelect = True
      TabOrder = 0
      ViewStyle = vsReport
      OnAdvancedCustomDrawItem = lvAdvancedCustomDrawItem
      OnSelectItem = lvSelectItem
    end
  end
end
