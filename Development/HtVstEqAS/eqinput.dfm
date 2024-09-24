object frmEqInput: TfrmEqInput
  Left = 385
  Top = 171
  Caption = 'Filter-Editor'
  ClientHeight = 725
  ClientWidth = 788
  Color = clWhite
  Constraints.MinHeight = 400
  Constraints.MinWidth = 530
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000000020000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF007777
    7777777777777777777777777777777777777777777777777777777777777700
    0000000000000000000000000077770000000000000200000000000000777700
    00000000000A00000000000000777700A0000000000A00000000000000777700
    A0000000000A20000000000000777700A0000000000A200A0000000000777700
    A0000000000A202A000A000000777700A2000A00002AA02A000A000000777702
    A2000A00002AA0AA000A000000777702AA002A2000AAA0AA002A20000077770A
    AA002A2000AAA0AA002A200A0077770A2A002A2002A2A2AA20AAA02A2077770A
    2A20A2A002A2A2AA20A2A0AAA07777FFFFFFFFFFFFFFFFFFFFFFFFFFFF777700
    0A2A20AA22A2AAA2AAA0AAA0007777000A2A202A22A02AA2AAA02A2000777700
    0AAA002A2AA02AA0AA200A000077770002A2000A2A200A20AA200A0000777700
    02A20002AA200A20AA0000000077770002A20002AA200A00A200000000777700
    00A00002AA200000A20000000077770000A00002AA000000A000000000777700
    00A00000A2000000000000000077770000000000A20000000000000000777700
    00000000A00000000000000000777787784444444444444444F888788877778F
    F84444444444444444F8087808777788884444444444444444F8887888777777
    7777777777777777777777777777777777777777777777777777777777770000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000}
  KeyPreview = True
  Position = poScreenCenter
  Scaled = False
  OnShow = FormShow
  TextHeight = 13
  object Chart: TChart
    Left = 0
    Top = 0
    Width = 788
    Height = 725
    AllowPanning = pmNone
    BackWall.Color = clBlack
    BackWall.Transparent = False
    BackWall.Picture.Data = {
      07544269746D617066000000424D660000000000000036000000280000000400
      000004000000010018000000000030000000120B0000120B0000000000000000
      0000280000280000280000280000280000280000280000280000280000280000
      280000280000280000280000280000280000}
    Border.Visible = True
    Foot.Alignment = taLeftJustify
    Foot.Color = clBlack
    Foot.Font.Color = clBlack
    Foot.Font.Style = []
    Legend.Alignment = laTop
    Legend.Color = clBlack
    Legend.Font.Color = clWhite
    Legend.LegendStyle = lsSeries
    Legend.MaxNumRows = 10
    Legend.Selected.Hover.Visible = False
    Legend.Shadow.Color = clGray
    Legend.TextStyle = ltsPlain
    Legend.TopPos = 9
    Legend.VertMargin = 3
    MarginLeft = 6
    MarginRight = 5
    MarginTop = 0
    SubTitle.Alignment = taLeftJustify
    SubTitle.Font.Color = clBlack
    Title.Alignment = taLeftJustify
    Title.Font.Color = clBlack
    Title.Font.Height = -16
    Title.Font.Style = [fsBold]
    Title.Text.Strings = (
      '')
    BottomAxis.Automatic = False
    BottomAxis.AutomaticMaximum = False
    BottomAxis.AutomaticMinimum = False
    BottomAxis.Grid.Color = clGreen
    BottomAxis.Maximum = 22050.000000000000000000
    BottomAxis.MaximumOffset = 10
    BottomAxis.MinimumOffset = 10
    BottomAxis.Title.Caption = 'Frequency [Hz]'
    BottomAxis.Title.Font.Height = -16
    Hover.Visible = False
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMaximum = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.AxisValuesFormat = '#,##0.#############'
    LeftAxis.ExactDateTime = False
    LeftAxis.Grid.Color = clGreen
    LeftAxis.LabelsOnAxis = False
    LeftAxis.LabelsSize = 20
    LeftAxis.Maximum = 40.000000000000000000
    LeftAxis.Minimum = -140.000000000000000000
    LeftAxis.MinorTickCount = 1
    LeftAxis.MinorTicks.Visible = False
    LeftAxis.Ticks.Style = psDash
    LeftAxis.Ticks.Visible = False
    LeftAxis.TicksInner.Visible = False
    LeftAxis.Title.Caption = 'Level [dB]'
    LeftAxis.Title.Font.Height = -16
    LeftAxis.TitleSize = 12
    RightAxis.Automatic = False
    RightAxis.AutomaticMaximum = False
    RightAxis.AutomaticMinimum = False
    RightAxis.ExactDateTime = False
    RightAxis.Logarithmic = True
    RightAxis.Maximum = 100.000000000000000000
    RightAxis.Minimum = 0.000000100000000000
    RightAxis.Title.Angle = 90
    RightAxis.Title.Caption = 'Level dB'
    RightAxis.Visible = False
    TopAxis.Automatic = False
    TopAxis.AutomaticMaximum = False
    TopAxis.AutomaticMinimum = False
    TopAxis.ExactDateTime = False
    TopAxis.Grid.Color = clGreen
    TopAxis.Increment = 1.000000000000000000
    TopAxis.LabelsOnAxis = False
    TopAxis.LabelsSeparation = 1
    TopAxis.LabelStyle = talText
    TopAxis.Maximum = 10000.000000000000000000
    TopAxis.Minimum = 125.000000000000000000
    TopAxis.MinorTickCount = 0
    TopAxis.MinorTickLength = 1
    TopAxis.RoundFirstLabel = False
    TopAxis.Ticks.Style = psDash
    TopAxis.Title.Caption = 'Frequency [Hz]'
    TopAxis.Title.Font.Height = -13
    TopAxis.Visible = False
    View3D = False
    View3DWalls = False
    Zoom.Allow = False
    Zoom.Animated = True
    OnBeforeDrawAxes = ChartBeforeDrawAxes
    Align = alClient
    Color = clWhite
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    ExplicitWidth = 784
    ExplicitHeight = 724
    DesignSize = (
      788
      725)
    DefaultCanvas = 'TTeeCanvas3D'
    PrintMargins = (
      15
      17
      15
      17)
    ColorPaletteIndex = 0
    object cbLog: TCheckBox
      Left = 588
      Top = 684
      Width = 161
      Height = 17
      Anchors = [akRight, akBottom]
      Caption = 'Logarithmic Frequency Axis'
      Checked = True
      State = cbChecked
      TabOrder = 0
      OnClick = cbLogClick
      ExplicitLeft = 584
      ExplicitTop = 683
    end
    object RawFilterSeries: TFastLineSeries
      Title = 'Raw Filter'
      VertAxis = aRightAxis
      LinePen.Color = clRed
      LinePen.Width = 2
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object PreSpecSeriesL: TFastLineSeries
      SeriesColor = clLime
      Title = 'Prefilter'
      VertAxis = aRightAxis
      LinePen.Color = clLime
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object DummySeries: TLineSeries
      Legend.Visible = False
      ShowInLegend = False
      Brush.BackColor = clDefault
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
    object PostSpecSeriesL: TFastLineSeries
      SeriesColor = clAqua
      Title = 'Postfilter'
      VertAxis = aRightAxis
      LinePen.Color = clAqua
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
end
