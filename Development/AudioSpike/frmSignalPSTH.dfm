inherited formSignalPSTH: TformSignalPSTH
  Left = 0
  Top = 400
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Signal-PSTH'
  ClientHeight = 298
  ClientWidth = 631
  KeyPreview = True
  Visible = False
  StyleElements = [seFont, seClient, seBorder]
  ExplicitWidth = 647
  ExplicitHeight = 337
  TextHeight = 13
  object bvl: TBevel
    Left = 0
    Top = 31
    Width = 631
    Height = 1
    Align = alTop
    Shape = bsTopLine
    ExplicitLeft = 8
    ExplicitTop = 39
    ExplicitWidth = 410
  end
  object chrt: TChart
    Left = 0
    Top = 32
    Width = 631
    Height = 266
    AllowPanning = pmNone
    Legend.Visible = False
    Title.Text.Strings = (
      'AllSpikes')
    Title.Visible = False
    OnClickAxis = chrtClickAxis
    BottomAxis.Automatic = False
    BottomAxis.AutomaticMinimum = False
    BottomAxis.AxisValuesFormat = '###0.###'
    BottomAxis.MinimumOffset = 10
    BottomAxis.Title.Caption = 'Spiketime [ms]'
    BottomAxis.Title.Font.Height = -13
    DepthAxis.Automatic = False
    DepthAxis.AutomaticMaximum = False
    DepthAxis.AutomaticMinimum = False
    DepthAxis.Maximum = 0.500000000000000000
    DepthAxis.Minimum = -0.500000000000000000
    DepthTopAxis.Automatic = False
    DepthTopAxis.AutomaticMaximum = False
    DepthTopAxis.AutomaticMinimum = False
    DepthTopAxis.Maximum = 0.500000000000000000
    DepthTopAxis.Minimum = -0.500000000000000000
    Hover.Visible = False
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.AxisValuesFormat = '###0.###'
    LeftAxis.Grid.DrawAlways = True
    LeftAxis.Labels = False
    LeftAxis.LabelsFormat.Visible = False
    LeftAxis.MaximumOffset = 10
    LeftAxis.MinorTicks.Visible = False
    LeftAxis.StartPosition = 67.000000000000000000
    LeftAxis.Title.Caption = 'Frequency'
    LeftAxis.Title.Font.Height = -13
    RightAxis.Automatic = False
    RightAxis.AutomaticMaximum = False
    RightAxis.AutomaticMinimum = False
    RightAxis.Maximum = 1.000000000000000000
    RightAxis.Visible = False
    TopAxis.Automatic = False
    TopAxis.AutomaticMaximum = False
    TopAxis.AutomaticMinimum = False
    TopAxis.Visible = False
    View3D = False
    Zoom.Allow = False
    OnBeforeDrawSeries = chrtBeforeDrawSeries
    Align = alClient
    BevelOuter = bvNone
    Color = clWhite
    TabOrder = 0
    DefaultCanvas = 'TGDIPlusCanvas'
    ColorPaletteIndex = 0
  end
  object tb: TToolBar
    Left = 0
    Top = 0
    Width = 631
    Height = 31
    ButtonHeight = 30
    ButtonWidth = 31
    Caption = 'tb'
    EdgeInner = esLowered
    EdgeOuter = esNone
    Images = il1
    TabOrder = 1
    object tbtnZoomOut: TToolButton
      Left = 0
      Top = 0
      Hint = 'Zoom out'
      Caption = 'tbtnZoomOut'
      ImageIndex = 0
      ParentShowHint = False
      ShowHint = True
      OnClick = tbtnZoomOutClick
    end
    object tbtnZoomIn: TToolButton
      Left = 31
      Top = 0
      Hint = 'Zoom in'
      Caption = 'tbtnZoomIn'
      ImageIndex = 1
      ParentShowHint = False
      ShowHint = True
      OnClick = tbtnZoomInClick
    end
    object ToolButton2: TToolButton
      Left = 62
      Top = 0
      Width = 8
      Caption = 'ToolButton2'
      ImageIndex = 3
      Style = tbsSeparator
    end
    object tbtnBinSize: TToolButton
      Left = 70
      Top = 0
      Hint = 'Update binsize display'
      ImageIndex = 2
      ParentShowHint = False
      ShowHint = True
      OnClick = tbtnBinSizeClick
    end
  end
  object pnlBinSize: TPanel
    Left = 104
    Top = 5
    Width = 164
    Height = 24
    Alignment = taLeftJustify
    BevelOuter = bvNone
    Caption = 'pnlBinSize'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
  end
  object il1: TImageList
    Height = 24
    Masked = False
    Width = 24
    Left = 208
    Bitmap = {
      494C010103002800040018001800FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000600000001800000001002000000000000024
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000064707A00BF9E9600000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000064707A00BCA2A300000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000063A8F1003879F40060758800C59D95000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00006D8EC9001D55F30060758800C6A49F000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000559FF500559FF5003879F40060758800C59D950000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00006FB6F3006FB6F3001D55F30060758800C6A49F0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000559FF500559FF5003879F40060758800C59D9500000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000006FB6F3006FB6F3001D55F30060758800CBA69D00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000559FF500559FF5003879F40060758800BA9E96000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000006FB6F3006FB6F3001D55F30060758800BCA2A3000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000559FF500559FF5003879F400656F77000000
      00009F928D00C59F9700D3B5A900CFAA9F000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000006FB6F3006FB6F3006A93D500656F77000000
      000089878900C6A49F00D3B4A800CDA69D000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF000000FF008800FF008800FF0088
      00FF008800FF008800FF008800FF000000FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000559FF500D0D0D1009F928D00AD9A
      9200FAEFC800FDF9DA00FDF9DA00FDF9DA00F4D6B200D1ADA100000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000006FB6F3009EBBE10089878900BCA2
      A300EDE7D300FAF4D400FAF4D400FAF4D400E2DACC00D2AEA200000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF000000FF000000FF008800FF008800FF0088
      00FF008800FF008800FF008800FF000000FF000000FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000CFBDB700D6B9AB00FBED
      C400FCF8D700FDF9DA00FDF9DA00FDFADF00FDFAE300F4F1DF00B49C95000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D5B9AD00D5B9AD00FCEF
      C500FAF4D4009EBBE10066A3E700EDE7D300F9F5DB00F6F2D900898789000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF000000FF000000FF000000FF000000FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C59D9500F7E0B200F8E2
      B200FCF6D400FDF9DA00FDF9E100FDFAE300FDFAE300FDFAE300F2D0B4000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C6A49F00FCE6C200FCE6
      C200FCF4CB0088B9EC000734FB00DDD5CC00F9F5DB00F9F5DB00D8CCC5000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF000000FF000000FF008800FF008800FF0088
      00FF008800FF008800FF008800FF000000FF000000FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D6B9AB00F8E2B200AD9A
      920063A8F1004584F3004584F3004584F3005DA2F200DFE0D700FBF4D100C79F
      9600000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D5B9AD00FCECC100B6A9
      B3006998D8003849F5000734FB0066A3E7007FB9F000E8DFCE00F3EED600C6A4
      9F00000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF000000FF008800FF008800FF0088
      00FF008800FF008800FF008800FF000000FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000F2C9B400F8E2B200356D
      F7002749FC002749FC002749FC002749FC002749FC00CBC8CC00FCF8D700CDA5
      9A00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D7BFB200FCECC1006C91
      D0000734FB000734FB000734FB000734FB000734FB009EBBE100FAF4D400C6A4
      9F00000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D6B9AB00F9E5B700CEA7
      9C00D1B0A50063A8F10063A8F10063A8F100D0D0D100F0EFDE00FBF4D100CBA2
      9700000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D7BFB200FCECC100CEA7
      9D00B6A9B30064A1E2000734FB0096BBE600DAD1CB00F6F2D900F6F2D900C6A4
      9F00000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C79F9600FBEDC400FBED
      C400F4D6B200F2D0B400F9E5B700FCF6D400FCF6D400FDF9DA00F4D9B1000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000CBA69D00FCEFC500FCEC
      C100FCE6C2006F88C1000734FB00DAD1CB00FAF4D400F9F5DB00E2DACC000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000D6B9AB00FDFA
      E300FDFAE300F2C9B400F3BFBB00F2D0B400F9E5B700FBF3CD00BF9E96000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000D7BFB200F9F5
      DB00F9F5DB00D3B4A800BCA2A300D7BFB200FCE6C200FCF4CB00BCA2A3000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CDBF
      BD00F4F1DF00FBEDC400F8E2B200F9E5B700F5DDB100D1ADA100000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000D7BF
      B200F6F2D900FCECC100FCECC100FCECC100FCDBC700D0ABA000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CBA29700CFAA9F00D6B9AB00D6B9AB000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CBA69D00D0ABA000D7BFB200D5B9AD000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF008800FF008800FF008800FF008800FF008800FF008800FF008800FF0088
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000060000000180000000100010000000000200100000000000000000000
      000000000000000000000000FFFFFF00FFFFFFFFFFFFFFFFFF000000FFFFFFFF
      FFFFFFFFFF000000FFFFFFFFFFFFE00007000000FFFFFFFFFFFFE00007000000
      F9FFFFF9FFFFE00007000000F0FFFFF0FFFFE00007000000F07FFFF07FFFE000
      07000000F83FFFF83FFFE00007000000FC1FFFFC1FFFE00007000000FE10FFFE
      10FFE00007000000FF003FFF003FE00007000000FF801FFF801FE00007000000
      FF801FFF801FE00007000000FF800FFF800FE00007000000FF800FFF800FE000
      07000000FF800FFF800FE00007000000FF801FFF801FE00007000000FFC01FFF
      C01FE00007000000FFE03FFFE03FE00007000000FFF0FFFFF0FFE00007000000
      FFFFFFFFFFFFE00007000000FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFF
      FF000000FFFFFFFFFFFFFFFFFF00000000000000000000000000000000000000
      000000000000}
  end
end
