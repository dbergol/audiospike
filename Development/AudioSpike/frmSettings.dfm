object formSettings: TformSettings
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Settings'
  ClientHeight = 549
  ClientWidth = 711
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poMainFormCenter
  OnShow = FormShow
  DesignSize = (
    711
    549)
  TextHeight = 13
  object btnOk: TButton
    Left = 586
    Top = 24
    Width = 121
    Height = 33
    Anchors = [akTop, akRight]
    Caption = 'Ok'
    Default = True
    TabOrder = 0
    OnClick = btnOkClick
  end
  object btnCancel: TButton
    Left = 586
    Top = 63
    Width = 121
    Height = 33
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object pc: TPageControl
    Left = 0
    Top = 0
    Width = 576
    Height = 549
    ActivePage = tsGeneral
    Align = alLeft
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    object tsGeneral: TTabSheet
      Caption = 'General Settings'
      object pnlGeneral: TPanel
        Left = 0
        Top = 0
        Width = 568
        Height = 521
        Align = alClient
        BevelOuter = bvNone
        Color = clWhite
        ParentBackground = False
        TabOrder = 0
        object gbPSTHWindow: TGroupBox
          Left = 0
          Top = 383
          Width = 568
          Height = 61
          Align = alTop
          Caption = 'PSTH-Window'
          TabOrder = 2
          ExplicitTop = 359
          object Label1: TLabel
            Left = 294
            Top = 27
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object Label2: TLabel
            Left = 8
            Top = 27
            Width = 74
            Height = 13
            Caption = 'Default binsize:'
          end
          object edPSTHBinSize: TEdit
            Left = 166
            Top = 24
            Width = 121
            Height = 21
            Alignment = taRightJustify
            TabOrder = 0
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
        end
        object gbProgramSettings: TGroupBox
          Left = 0
          Top = 0
          Width = 568
          Height = 289
          Align = alTop
          Caption = 'Program settings'
          TabOrder = 0
          DesignSize = (
            568
            289)
          object lbStyle: TLabel
            Left = 8
            Top = 55
            Width = 28
            Height = 13
            Caption = 'Style:'
          end
          object lblSettingsName: TLabel
            Left = 8
            Top = 23
            Width = 106
            Height = 13
            Caption = 'Settings subdirectory:'
          end
          object Bevel1: TBevel
            Left = 8
            Top = 86
            Width = 546
            Height = 3
            Shape = bsTopLine
          end
          object sbtnAddSettings: TSpeedButton
            Left = 407
            Top = 20
            Width = 23
            Height = 22
            Caption = '...'
            Enabled = False
            Visible = False
            OnClick = sbtnAddSettingsClick
          end
          object cbMultipleInstancesAllowed: TCheckBox
            Left = 9
            Top = 120
            Width = 190
            Height = 17
            Caption = 'Allow multiple AudioSpike instances'
            TabOrder = 4
          end
          object cbSaveMAT: TCheckBox
            Left = 9
            Top = 190
            Width = 179
            Height = 17
            Caption = 'Always save additional MAT file'
            TabOrder = 6
          end
          object cbFreeWindows: TCheckBox
            Left = 9
            Top = 262
            Width = 179
            Height = 17
            Caption = 'Free subwindow positions'
            TabOrder = 9
            OnClick = cbFreeWindowsClick
          end
          object cbSaveProbeMic: TCheckBox
            Left = 9
            Top = 214
            Width = 275
            Height = 17
            Caption = 'Save probe microphone audio data (in-situ)'
            TabOrder = 7
          end
          object cbStartupInSitu: TCheckBox
            Left = 9
            Top = 238
            Width = 179
            Height = 17
            Caption = 'Start in In-Situ-Mode'
            TabOrder = 8
          end
          object cbStyle: TComboBox
            Left = 166
            Top = 52
            Width = 235
            Height = 21
            Style = csDropDownList
            Sorted = True
            TabOrder = 0
            OnChange = cbStyleChange
          end
          object cbSettings: TComboBox
            Left = 166
            Top = 20
            Width = 235
            Height = 21
            Style = csDropDownList
            Sorted = True
            TabOrder = 2
          end
          object cbCheckUpdateOnStartup: TCheckBox
            Left = 9
            Top = 97
            Width = 384
            Height = 17
            Caption = 'Check for updates on startup (once per day)'
            TabOrder = 3
          end
          object cbAutoSave: TCheckBox
            Left = 9
            Top = 143
            Width = 208
            Height = 17
            Caption = 'Autosave complete measurements'
            TabOrder = 5
          end
          object btnResetDontShowAgain: TButton
            Left = 458
            Top = 20
            Width = 96
            Height = 22
            Anchors = [akTop, akRight]
            Caption = 'Reset Warnings'
            TabOrder = 1
            OnClick = btnResetDontShowAgainClick
          end
          object cbAlwaysLoadEpoches: TCheckBox
            Left = 9
            Top = 166
            Width = 208
            Height = 17
            Caption = 'Always reload epoches'
            TabOrder = 10
          end
        end
        object gbGeneral: TGroupBox
          Left = 0
          Top = 289
          Width = 568
          Height = 94
          Align = alTop
          Caption = 'Directories'
          TabOrder = 1
          ExplicitTop = 265
          object lblTemplatePath: TLabel
            Left = 8
            Top = 27
            Width = 70
            Height = 13
            Caption = 'Templatepath:'
          end
          object lblResultPath: TLabel
            Left = 8
            Top = 58
            Width = 56
            Height = 13
            Caption = 'Resultpath:'
          end
          object sbtnTemplatePath: TSpeedButton
            Left = 458
            Top = 24
            Width = 23
            Height = 22
            Caption = '...'
            OnClick = sbtnPathClick
          end
          object sbtnResultPath: TSpeedButton
            Left = 458
            Top = 55
            Width = 23
            Height = 22
            Caption = '...'
            OnClick = sbtnPathClick
          end
          object edTemplatePath: TEdit
            Left = 166
            Top = 24
            Width = 286
            Height = 21
            ReadOnly = True
            TabOrder = 0
            OnChange = edChange
          end
          object edResultPath: TEdit
            Left = 166
            Top = 55
            Width = 286
            Height = 21
            ReadOnly = True
            TabOrder = 1
            OnChange = edChange
          end
          object cbAutoTemplatePath: TCheckBox
            Left = 487
            Top = 26
            Width = 45
            Height = 17
            Caption = 'Auto'
            TabOrder = 2
            OnClick = cbAutoTemplatePathClick
          end
        end
      end
    end
    object tsFreeSearchSettings: TTabSheet
      Caption = 'Free Search Settings'
      ImageIndex = 2
      object pnlFreeSearch: TPanel
        Left = 0
        Top = 0
        Width = 568
        Height = 521
        Align = alClient
        BevelOuter = bvNone
        Color = clWhite
        ParentBackground = False
        TabOrder = 0
        ExplicitHeight = 413
        object gbSearch: TGroupBox
          Left = 0
          Top = 0
          Width = 568
          Height = 177
          Align = alTop
          Caption = 'Epoche Settings'
          TabOrder = 0
          object lbU3: TLabel
            Left = 326
            Top = 57
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lblSearchStimlLength: TLabel
            Left = 8
            Top = 57
            Width = 79
            Height = 13
            Caption = 'Stimulus Length:'
          end
          object lblRepetitionPeriod: TLabel
            Left = 8
            Top = 119
            Width = 82
            Height = 13
            Caption = 'Repetition Period'
          end
          object lbU4: TLabel
            Left = 326
            Top = 119
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lblSearchRampLength: TLabel
            Left = 8
            Top = 150
            Width = 67
            Height = 13
            Caption = 'Ramp Length:'
          end
          object lbU6: TLabel
            Left = 326
            Top = 150
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lblSearchPreStimLength: TLabel
            Left = 8
            Top = 88
            Width = 99
            Height = 13
            Caption = 'Pre-Stimulus Length:'
          end
          object lbU5: TLabel
            Left = 326
            Top = 88
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object edSearchStimLength: TEdit
            Left = 198
            Top = 54
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 1
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edSearchRepetitionPeriod: TEdit
            Left = 198
            Top = 116
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 3
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edSearchRampLength: TEdit
            Left = 198
            Top = 147
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 4
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edSearchPreStimLength: TEdit
            Left = 198
            Top = 85
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 2
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object cbFlipPolarity: TCheckBox
            Left = 9
            Top = 23
            Width = 384
            Height = 17
            Caption = 'Flip electrode polarity'
            TabOrder = 0
          end
        end
        inline frameSpikeSettings: TframeSpikeSettings
          Left = 0
          Top = 177
          Width = 568
          Height = 344
          Align = alTop
          TabOrder = 1
          ExplicitTop = 177
          ExplicitWidth = 568
          ExplicitHeight = 344
          inherited pnlSpikeSettings: TPanel
            Width = 568
            Height = 344
            StyleElements = [seFont, seClient, seBorder]
            ExplicitWidth = 568
            ExplicitHeight = 344
            inherited gbSpikeDetection: TGroupBox
              Width = 568
              ExplicitWidth = 568
              inherited lbSpikeDetectionMethod: TLabel
                Width = 40
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 40
                ExplicitHeight = 13
              end
              inherited cbSpikeDetectionMethod: TComboBox
                Height = 21
                StyleElements = [seFont, seClient, seBorder]
                ExplicitHeight = 21
              end
            end
            inherited gbVersion1: TGroupBox
              Width = 568
              ExplicitWidth = 568
              inherited lbU1: TLabel
                Width = 13
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 13
                ExplicitHeight = 13
              end
              inherited lbPreThreshold: TLabel
                Width = 71
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 71
                ExplicitHeight = 13
              end
              inherited lbSpikeLength: TLabel
                Width = 66
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 66
                ExplicitHeight = 13
              end
              inherited lbU2: TLabel
                Width = 13
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 13
                ExplicitHeight = 13
              end
              inherited lbPostThreshold: TLabel
                Width = 76
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 76
                ExplicitHeight = 13
              end
              inherited lbU11: TLabel
                Width = 13
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 13
                ExplicitHeight = 13
              end
              inherited edPreThreshold: TEdit
                Height = 21
                StyleElements = [seFont, seClient, seBorder]
                ExplicitHeight = 21
              end
              inherited edSpikeLength: TEdit
                Height = 21
                StyleElements = [seFont, seClient, seBorder]
                ExplicitHeight = 21
              end
              inherited edPostThreshold: TEdit
                Height = 21
                StyleElements = [seFont, seClient, seBorder]
                ExplicitHeight = 21
              end
            end
            inherited gbVersion2: TGroupBox
              Width = 568
              Height = 122
              ExplicitWidth = 568
              ExplicitHeight = 122
              inherited lbRefractoryTime: TLabel
                Width = 81
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 81
                ExplicitHeight = 13
              end
              inherited lbU4: TLabel
                Width = 13
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 13
                ExplicitHeight = 13
              end
              inherited lbRefractoryTimeTailFactor: TLabel
                Width = 134
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 134
                ExplicitHeight = 13
              end
              inherited lbSpikeTimeReference: TLabel
                Width = 102
                Height = 13
                StyleElements = [seFont, seClient, seBorder]
                ExplicitWidth = 102
                ExplicitHeight = 13
              end
              inherited edRefractoryTime: TEdit
                Height = 21
                StyleElements = [seFont, seClient, seBorder]
                ExplicitHeight = 21
              end
              inherited edRefractoryTimeTailFactor: TEdit
                Height = 21
                StyleElements = [seFont, seClient, seBorder]
                ExplicitHeight = 21
              end
            end
          end
        end
      end
    end
  end
end
