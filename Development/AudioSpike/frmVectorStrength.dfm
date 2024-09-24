inherited formVectorStrength: TformVectorStrength
  Caption = 'Vector Strength Plot'
  Visible = False
  StyleElements = [seFont, seClient, seBorder]
  TextHeight = 13
  object chrt: TChart
    Left = 0
    Top = 0
    Width = 410
    Height = 365
    AllowPanning = pmNone
    Foot.Alignment = taLeftJustify
    Foot.Font.Color = clBlack
    Foot.Font.Style = []
    Legend.LegendStyle = lsSeries
    Legend.ResizeChart = False
    Legend.TopPos = -1
    MarginBottom = 6
    MarginLeft = 6
    MarginRight = 15
    MarginTop = 10
    MarginUnits = muPixels
    Title.Font.Color = clBlack
    Title.Font.Height = -13
    Title.Font.Style = [fsBold]
    Title.Text.Strings = (
      ' ')
    Title.TextAlignment = taLeftJustify
    Title.VertMargin = 6
    BottomAxis.AxisValuesFormat = '###0.###'
    BottomAxis.Logarithmic = True
    BottomAxis.MaximumOffset = 20
    BottomAxis.MinimumOffset = 20
    BottomAxis.MinorGrid.Color = clSilver
    BottomAxis.MinorTickLength = 1
    BottomAxis.MinorTicks.Visible = False
    BottomAxis.Title.Caption = 'Frequency'
    BottomAxis.Title.Font.Height = -13
    Hover.Visible = False
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMaximum = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.AxisValuesFormat = '###0.###'
    LeftAxis.LabelsSeparation = 100
    LeftAxis.Maximum = 1.000000000000000000
    LeftAxis.MinorTicks.Visible = False
    LeftAxis.Title.Caption = 'Vector strength'
    LeftAxis.Title.Font.Height = -13
    RightAxis.Automatic = False
    RightAxis.AutomaticMaximum = False
    RightAxis.AutomaticMinimum = False
    View3D = False
    Zoom.Allow = False
    OnGetLegendText = chrtGetLegendText
    Align = alClient
    BevelOuter = bvNone
    Color = clWhite
    TabOrder = 0
    ExplicitWidth = 402
    ExplicitHeight = 363
    DefaultCanvas = 'TGDIPlusCanvas'
    PrintMargins = (
      15
      5
      15
      5)
    ColorPaletteIndex = 0
    object csVS: TLineSeries
      SeriesColor = clBlue
      Brush.BackColor = clDefault
      Pointer.HorizSize = 5
      Pointer.InflateMargins = True
      Pointer.Pen.Color = clBlue
      Pointer.Style = psCircle
      Pointer.VertSize = 5
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      OnGetPointerStyle = csVSGetPointerStyle
    end
    object csP: TLineSeries
      Active = False
      SeriesColor = clBlue
      Brush.BackColor = clDefault
      Pointer.InflateMargins = True
      Pointer.Pen.Color = clBlue
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
end
