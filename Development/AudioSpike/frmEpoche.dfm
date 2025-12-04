object formEpoches: TformEpoches
  Left = 0
  Top = 10
  Align = alTop
  BorderIcons = [biSystemMenu, biMaximize]
  BorderStyle = bsNone
  Caption = 'Epoches'
  ClientHeight = 337
  ClientWidth = 651
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poDefault
  OnMouseActivate = FormMouseActivate
  OnShow = FormShow
  TextHeight = 13
  object bvl: TBevel
    Left = 0
    Top = 334
    Width = 651
    Height = 3
    Align = alBottom
    Shape = bsBottomLine
    ExplicitTop = 335
  end
  object pnl: TPanel
    Left = 0
    Top = 0
    Width = 651
    Height = 334
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object tb: TToolBar
      Left = 0
      Top = 0
      Width = 32
      Height = 334
      Align = alLeft
      ButtonHeight = 30
      ButtonWidth = 31
      Caption = 'tb'
      EdgeInner = esNone
      EdgeOuter = esNone
      TabOrder = 1
      object tbtnEvalActive: TToolButton
        Left = 0
        Top = 0
        Hint = 'Activate online evaluation for this electrode'
        AllowAllUp = True
        Caption = 'tbtnEvalActive'
        ImageIndex = 2
        ParentShowHint = False
        Wrap = True
        ShowHint = True
        Style = tbsCheck
        OnClick = tbtnEvalActiveClick
      end
      object tbtnClear: TToolButton
        Left = 0
        Top = 30
        Hint = 'Clear averaged epoche data'
        Caption = 'tbtnClear'
        ImageIndex = 0
        ParentShowHint = False
        Wrap = True
        ShowHint = True
        OnClick = tbtnClearClick
      end
      object tbtnAverage: TToolButton
        Left = 0
        Top = 60
        Hint = 'Toggle average mode on/off'
        AllowAllUp = True
        Caption = 'tbtnAverage'
        ImageIndex = 1
        ParentShowHint = False
        Wrap = True
        ShowHint = True
        Style = tbsCheck
      end
      object tbnListen: TToolButton
        Left = 0
        Top = 90
        Hint = 'Activate monitor for this electrode'
        Caption = 'tbnListen'
        ImageIndex = 3
        ParentShowHint = False
        Wrap = True
        ShowHint = True
        Style = tbsCheck
        OnClick = tbnListenClick
      end
      object tbtnFlipPolarity: TToolButton
        Left = 0
        Top = 120
        Hint = 'Flip electrode polarity'
        Caption = 'tbtnFlipPolarity'
        ImageIndex = 4
        ParentShowHint = False
        Wrap = True
        ShowHint = True
        Style = tbsCheck
        OnClick = tbtnFlipPolarityClick
      end
    end
    object chrt: TChart
      Left = 32
      Top = 0
      Width = 619
      Height = 334
      AllowPanning = pmNone
      Legend.Visible = False
      MarginBottom = 2
      MarginTop = 3
      Title.Font.Color = clBlack
      Title.Font.Height = -13
      Title.Font.Style = [fsBold]
      Title.Text.Strings = (
        'SpikeTrain')
      OnClickAxis = chrtClickAxis
      BottomAxis.Automatic = False
      BottomAxis.AutomaticMaximum = False
      BottomAxis.AutomaticMinimum = False
      BottomAxis.AxisValuesFormat = '###0.###'
      BottomAxis.Maximum = 500.000000000000000000
      BottomAxis.MinorTickCount = 4
      BottomAxis.Title.Caption = 'Time [ms]'
      BottomAxis.Title.Font.Height = -13
      DepthAxis.Automatic = False
      DepthAxis.AutomaticMaximum = False
      DepthAxis.AutomaticMinimum = False
      DepthAxis.Maximum = 0.169999999999999800
      DepthAxis.Minimum = -0.830000000000000300
      DepthTopAxis.Automatic = False
      DepthTopAxis.AutomaticMaximum = False
      DepthTopAxis.AutomaticMinimum = False
      DepthTopAxis.Maximum = 0.169999999999999800
      DepthTopAxis.Minimum = -0.830000000000000300
      CustomAxes = <
        item
          Automatic = False
          AutomaticMaximum = False
          AutomaticMinimum = False
          Horizontal = True
          OtherSide = False
          Maximum = 1.000000000000000000
          Visible = False
        end>
      Hover.Visible = False
      LeftAxis.Automatic = False
      LeftAxis.AutomaticMaximum = False
      LeftAxis.AutomaticMinimum = False
      LeftAxis.AxisValuesFormat = '###0.###'
      LeftAxis.Maximum = 1.000000000000000000
      LeftAxis.Minimum = -1.000000000000000000
      LeftAxis.Title.Caption = 'Amplitude'
      LeftAxis.Title.Font.Height = -13
      RightAxis.Automatic = False
      RightAxis.AutomaticMaximum = False
      RightAxis.AutomaticMinimum = False
      RightAxis.Maximum = 1.000000000000000000
      RightAxis.Visible = False
      TopAxis.Automatic = False
      TopAxis.AutomaticMaximum = False
      TopAxis.AutomaticMinimum = False
      TopAxis.Axis.Width = 1
      TopAxis.Labels = False
      TopAxis.LabelsFormat.Visible = False
      TopAxis.MinorTicks.Visible = False
      TopAxis.Ticks.Visible = False
      TopAxis.TicksInner.Visible = False
      View3D = False
      Zoom.Allow = False
      OnBeforeDrawAxes = chrtBeforeDrawAxes
      Align = alClient
      BevelOuter = bvNone
      Color = clWhite
      TabOrder = 0
      OnClick = chrtClick
      OnMouseDown = chrtMouseDown
      OnMouseLeave = chrtMouseLeave
      OnMouseMove = chrtMouseMove
      OnMouseUp = chrtMouseUp
      DesignSize = (
        619
        334)
      DefaultCanvas = 'TGDIPlusCanvas'
      ColorPaletteIndex = 0
      object shClip: TShape
        Left = 591
        Top = 3
        Width = 25
        Height = 11
        Anchors = [akTop, akRight]
        Brush.Color = clLime
      end
      object csEpoche: TFastLineSeries
        HorizAxis = aTopAxis
        SeriesColor = clBlue
        FastPen = True
        LinePen.Color = clBlue
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object csStimSeries: TChartShape
        HorizAxis = aTopAxis
        SeriesColor = 4227327
        VertAxis = aRightAxis
        Brush.Color = 4227327
        Style = chasRectangle
        Transparency = 68
        X0 = 3.000000000000000000
        X1 = 21.000000000000000000
        Y0 = 129.126718750000000000
        Y1 = 507.253437500000000000
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
        Data = {
          0102000000000000000000084048E17A140E246040000000000000354048E17A
          140EB47F40}
      end
      object csEpocheThreshold: TLineSeries
        Brush.BackColor = clDefault
        Pointer.InflateMargins = True
        Pointer.Style = psRectangle
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
      object csThreshold: TLineSeries
        Selected.Hover.Visible = False
        VertAxis = aCustomVertAxis
        Brush.BackColor = clDefault
        LinePen.Color = clRed
        Pointer.InflateMargins = True
        Pointer.Style = psRectangle
        XValues.Name = 'X'
        XValues.Order = loAscending
        YValues.Name = 'Y'
        YValues.Order = loNone
      end
    end
  end
  object ClipTimer: TTimer
    Enabled = False
    Interval = 500
    OnTimer = ClipTimerTimer
    Left = 120
  end
end
