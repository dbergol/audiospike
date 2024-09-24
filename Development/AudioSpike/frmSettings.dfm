object formSettings: TformSettings
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Settings'
  ClientHeight = 730
  ClientWidth = 702
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poMainFormCenter
  OnShow = FormShow
  DesignSize = (
    702
    730)
  TextHeight = 13
  object btnOk: TButton
    Left = 556
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
    Left = 556
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
    Width = 545
    Height = 730
    ActivePage = tsGeneral
    Align = alLeft
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    object tsGeneral: TTabSheet
      Caption = 'General Settings'
      object pnlGeneral: TPanel
        Left = 0
        Top = 0
        Width = 537
        Height = 702
        Align = alClient
        BevelOuter = bvNone
        Color = clWhite
        ParentBackground = False
        TabOrder = 0
        object gbPSTHWindow: TGroupBox
          Left = 0
          Top = 323
          Width = 537
          Height = 61
          Align = alTop
          Caption = 'PSTH-Window'
          TabOrder = 0
          ExplicitTop = 300
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
          Width = 537
          Height = 227
          Align = alTop
          Caption = 'Program settings'
          TabOrder = 1
          object lbStyle: TLabel
            Left = 8
            Top = 202
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
            Top = 48
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
            Top = 80
            Width = 190
            Height = 17
            Caption = 'Allow Multiple Audiospike Instances'
            TabOrder = 0
          end
          object cbSaveMAT: TCheckBox
            Left = 9
            Top = 104
            Width = 179
            Height = 17
            Caption = 'Always save additional MAT file'
            TabOrder = 1
          end
          object cbFreeWindows: TCheckBox
            Left = 9
            Top = 176
            Width = 179
            Height = 17
            Caption = 'Free subwindow positions'
            TabOrder = 2
            OnClick = cbFreeWindowsClick
          end
          object cbSaveProbeMic: TCheckBox
            Left = 9
            Top = 128
            Width = 275
            Height = 17
            Caption = 'Save probe microphone audio data (in-situ)'
            TabOrder = 3
          end
          object cbStartupInSitu: TCheckBox
            Left = 9
            Top = 152
            Width = 179
            Height = 17
            Caption = 'Start in In-Situ-Mode'
            TabOrder = 4
          end
          object cbStyle: TComboBox
            Left = 166
            Top = 199
            Width = 235
            Height = 21
            Style = csDropDownList
            Sorted = True
            TabOrder = 5
            OnChange = cbStyleChange
          end
          object cbSettings: TComboBox
            Left = 166
            Top = 20
            Width = 235
            Height = 21
            Style = csDropDownList
            Sorted = True
            TabOrder = 6
          end
          object cbCheckUpdateOnStartup: TCheckBox
            Left = 9
            Top = 57
            Width = 384
            Height = 17
            Caption = 'Check for Updates on Startup (once per day)'
            TabOrder = 7
          end
        end
        object gbGeneral: TGroupBox
          Left = 0
          Top = 227
          Width = 537
          Height = 96
          Align = alTop
          Caption = 'Directories'
          TabOrder = 2
          ExplicitTop = 204
          object lblTemplatePath: TLabel
            Left = 8
            Top = 27
            Width = 70
            Height = 13
            Caption = 'Templatepath:'
          end
          object lblResultPath: TLabel
            Left = 8
            Top = 59
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
            Top = 56
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
            Top = 56
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
        object gbSpikes: TGroupBox
          Left = 0
          Top = 384
          Width = 537
          Height = 117
          Align = alTop
          Caption = 'Spikes'
          TabOrder = 3
          ExplicitTop = 361
          object lbU1: TLabel
            Left = 294
            Top = 27
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lbPreThreshold: TLabel
            Left = 8
            Top = 27
            Width = 116
            Height = 13
            Caption = 'Pre-Threshold (default):'
          end
          object lbSpikeLength: TLabel
            Left = 8
            Top = 89
            Width = 111
            Height = 13
            Caption = 'Spike-Length (default):'
          end
          object lbU2: TLabel
            Left = 293
            Top = 89
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lbPostThreshold: TLabel
            Left = 8
            Top = 58
            Width = 121
            Height = 13
            Caption = 'Post-Threshold (default):'
          end
          object lbU11: TLabel
            Left = 294
            Top = 58
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object edPreThreshold: TEdit
            Left = 166
            Top = 24
            Width = 121
            Height = 21
            Alignment = taRightJustify
            TabOrder = 0
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edSpikeLength: TEdit
            Left = 166
            Top = 86
            Width = 121
            Height = 21
            Alignment = taRightJustify
            TabOrder = 1
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edPostThreshold: TEdit
            Left = 166
            Top = 55
            Width = 121
            Height = 21
            Alignment = taRightJustify
            TabOrder = 2
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
        end
        object gbSeacrh: TGroupBox
          Left = 0
          Top = 501
          Width = 537
          Height = 169
          Align = alTop
          Caption = 'Free Search Mode'
          TabOrder = 4
          ExplicitTop = 478
          object lbU3: TLabel
            Left = 294
            Top = 27
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lblSearchStimlLength: TLabel
            Left = 8
            Top = 27
            Width = 79
            Height = 13
            Caption = 'Stimulus Length:'
          end
          object lblRepetitionPeriod: TLabel
            Left = 8
            Top = 91
            Width = 82
            Height = 13
            Caption = 'Repetition Period'
          end
          object lbU4: TLabel
            Left = 294
            Top = 91
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lblSearchRampLength: TLabel
            Left = 8
            Top = 123
            Width = 67
            Height = 13
            Caption = 'Ramp Length:'
          end
          object lbU6: TLabel
            Left = 294
            Top = 123
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lblSearchPreStimLength: TLabel
            Left = 8
            Top = 59
            Width = 99
            Height = 13
            Caption = 'Pre-Stimulus Length:'
          end
          object lbU5: TLabel
            Left = 294
            Top = 59
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object edSearchStimLength: TEdit
            Left = 166
            Top = 24
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 0
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edSearchRepetitionPeriod: TEdit
            Left = 166
            Top = 88
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 2
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edSearchRampLength: TEdit
            Left = 166
            Top = 120
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 3
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object edSearchPreStimLength: TEdit
            Left = 166
            Top = 56
            Width = 121
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 1
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
        end
      end
    end
    object tsSound: TTabSheet
      Caption = 'Sound Settings'
      ImageIndex = 1
      object pnlSound: TPanel
        Left = 0
        Top = 0
        Width = 537
        Height = 702
        Align = alClient
        BevelOuter = bvNone
        Color = clWhite
        ParentBackground = False
        TabOrder = 0
        object gbSystem: TGroupBox
          Left = 0
          Top = 0
          Width = 537
          Height = 81
          Align = alTop
          Caption = 'System'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          object lbDriver: TLabel
            Left = 8
            Top = 21
            Width = 33
            Height = 13
            Caption = 'Driver:'
          end
          object lblSamplerate: TLabel
            Left = 8
            Top = 53
            Width = 103
            Height = 13
            Caption = 'Samplerate (default):'
          end
          object lblHz: TLabel
            Left = 274
            Top = 53
            Width = 12
            Height = 13
            Caption = 'Hz'
          end
          object lbDownSamplingFactor: TLabel
            Left = 308
            Top = 53
            Width = 117
            Height = 13
            Caption = 'Downsampling (default):'
          end
          object cbDrivers: TComboBox
            Left = 166
            Top = 18
            Width = 322
            Height = 21
            Style = csDropDownList
            TabOrder = 0
            OnChange = cbDriversChange
          end
          object edSampleRate: TEdit
            Left = 166
            Top = 50
            Width = 100
            Height = 21
            Hint = '-'
            Alignment = taRightJustify
            TabOrder = 1
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object cbSamplerateDevider: TComboBox
            Left = 432
            Top = 50
            Width = 56
            Height = 21
            Style = csDropDownList
            ItemIndex = 0
            TabOrder = 2
            Text = '1'
            OnChange = cbSamplerateDeviderChange
            Items.Strings = (
              '1'
              '2'
              '4'
              '8'
              '16')
          end
        end
        object gbInput: TGroupBox
          AlignWithMargins = True
          Left = 0
          Top = 664
          Width = 537
          Height = 38
          Margins.Left = 0
          Margins.Top = 4
          Margins.Right = 0
          Margins.Bottom = 0
          Align = alClient
          Caption = 'Electrode Input'
          TabOrder = 1
          object lvInput: TListView
            Left = 2
            Top = 15
            Width = 533
            Height = 21
            Align = alClient
            BorderStyle = bsNone
            Checkboxes = True
            Columns = <
              item
                Width = 25
              end
              item
                Caption = 'Channel'
                Width = 150
              end
              item
                Caption = 'Index'
                Width = 40
              end
              item
                Caption = 'Band-Pass'
                Width = 200
              end>
            ReadOnly = True
            RowSelect = True
            PopupMenu = mnuInput
            TabOrder = 0
            ViewStyle = vsReport
            OnAdvancedCustomDrawItem = lvInputAdvancedCustomDrawItem
            OnColumnClick = lvColumnClick
            OnItemChecked = lvInputItemChecked
          end
        end
        object gbOutput: TGroupBox
          AlignWithMargins = True
          Left = 0
          Top = 516
          Width = 537
          Height = 144
          Margins.Left = 0
          Margins.Top = 4
          Margins.Right = 0
          Margins.Bottom = 0
          Align = alTop
          Caption = 'Sound Output'
          TabOrder = 2
          object lvOutput: TListView
            Left = 2
            Top = 15
            Width = 533
            Height = 127
            Align = alClient
            BorderStyle = bsNone
            Checkboxes = True
            Columns = <
              item
                Width = 25
              end
              item
                Caption = 'Channel'
                Width = 150
              end
              item
                Caption = 'Index'
                Width = 40
              end
              item
                Alignment = taRightJustify
                Caption = 'Calibration'
                Width = 70
              end
              item
                Caption = 'Equalisation'
                Width = 130
              end
              item
                Caption = 'Hi-Pass'
                Width = 100
              end>
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clBlack
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ReadOnly = True
            RowSelect = True
            ParentFont = False
            PopupMenu = mnuOutput
            TabOrder = 0
            ViewStyle = vsReport
            OnAdvancedCustomDrawItem = lvOutputAdvancedCustomDrawItem
            OnColumnClick = lvColumnClick
            OnItemChecked = lvOutputItemChecked
          end
        end
        object gbTrigger: TGroupBox
          Left = 0
          Top = 81
          Width = 537
          Height = 198
          Align = alTop
          Caption = 'Special Audio Channels'
          TabOrder = 3
          object lbTriggerOut: TLabel
            Left = 8
            Top = 23
            Width = 75
            Height = 13
            Caption = 'Trigger Output:'
          end
          object lbTriggerIn: TLabel
            Left = 8
            Top = 55
            Width = 67
            Height = 13
            Caption = 'Trigger Input:'
          end
          object sbtnTriggerTest: TSpeedButton
            Left = 494
            Top = 20
            Width = 22
            Height = 21
            Glyph.Data = {
              36060000424D3606000000000000360000002800000020000000100000000100
              18000000000000060000120B0000120B00000000000000000000FF00FF4A667C
              BE9596FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFFF00FF777777BDBDBDFF00FFFF00FFFF00FFFF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF6B9CC31E89E8
              4B7AA3C89693FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFABABAB9797978B8B8BC1C1C1FF00FFFF00FFFF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF4BB4FE51B5FF
              2089E94B7AA2C69592FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFB8B8B8BCBCBC9898988A8A8AC0C0C0FF00FFFF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF51B7FE
              51B3FF1D87E64E7AA0CA9792FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFFF00FFBBBBBBBCBCBC9595958B8B8BC2C2C2FF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
              51B7FE4EB2FF1F89E64E7BA2B99497FF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFFF00FFFF00FFBBBBBBBABABA9696968C8C8CBABABAFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
              FF00FF52B8FE4BB1FF2787D95F6A76FF00FFB0857FC09F94C09F96BC988EFF00
              FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFBCBCBCB9B9B99494947E7E7EFF
              00FF979797AAAAAAABABABA5A5A5FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FF55BDFFB5D6EDBF9D92BB9B8CE7DAC2FFFFE3FFFFE5FDFADAD8C3
              B3B58D85FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFBEBEBED1D1D1A8A8A8A3
              A3A3D4D4D4F1F1F1F2F2F2EBEBEBC5C5C59D9D9DFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFCEA795FDEEBEFFFFD8FFFFDAFFFFDBFFFFE6FFFF
              FBEADDDCAE837FFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFB1B1B1DD
              DDDDEBEBEBECECECEDEDEDF2F2F2FDFDFDE3E3E3969696FF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFC1A091FBDCA8FEF7D0FFFFDBFFFFE3FFFFF8FFFF
              FDFFFFFDC6A99CFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFA9A9A9D1
              D1D1E7E7E7EDEDEDF1F1F1FBFBFBFEFEFEFEFEFEB1B1B1FF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFC1A091FEE3ACF1C491FCF2CAFFFFDDFFFFE4FFFFF7FFFF
              F7FFFFE9EEE5CBB9948CFF00FFFF00FFFF00FFFF00FFFF00FFA9A9A9D5D5D5C1
              C1C1E3E3E3EEEEEEF1F1F1FBFBFBFBFBFBF4F4F4DCDCDCA2A2A2FF00FFFF00FF
              FF00FFFF00FFFF00FFC2A191FFE6AEEEB581F7DCAEFEFDD8FFFFDFFFFFE3FFFF
              E4FFFFE0F3ECD2BB968EFF00FFFF00FFFF00FFFF00FFFF00FFA9A9A9D6D6D6B7
              B7B7D2D2D2EBEBEBEFEFEFF1F1F1F1F1F1EFEFEFE2E2E2A4A4A4FF00FFFF00FF
              FF00FFFF00FFFF00FFBC978CFBE7B7F4C791F2C994F8E5B9FEFCD8FFFFDDFFFF
              DCFFFFE0E2D2BAB68E86FF00FFFF00FFFF00FFFF00FFFF00FFA4A4A4D9D9D9C2
              C2C2C3C3C3D8D8D8EBEBEBEEEEEEEDEDEDEFEFEFCECECE9E9E9EFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFD9C3A9FFFEE5F7DCB8F2C994F5D4A5FAE8BDFDF4
              C9FDFBD6B69089FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFC1C1C1F2
              F2F2D7D7D7C3C3C3CDCDCDDBDBDBE3E3E3E9E9E99F9F9FFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFB58D85E8DEDDFFFEF2F9D8A3F4C48CF9D49FFDEA
              B8D0B49FB89086FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF9D9D9DE2
              E2E2F8F8F8CECECEC0C0C0CCCCCCDADADAB7B7B79F9F9FFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFAD827FC9AA9EEFE0B7EFDFB2E7CEACB890
              86B89086FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF96
              9696B3B3B3D3D3D3D0D0D0C9C9C99F9F9F9F9F9FFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFBA968ABB988CB79188FF00
              FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FFFF00FFA2A2A2A3A3A39F9F9FFF00FFFF00FFFF00FFFF00FF}
            NumGlyphs = 2
            OnClick = sbtnTriggerTestClick
          end
          object lbLatency: TLabel
            Left = 8
            Top = 87
            Width = 72
            Height = 13
            Caption = 'Trigger Offset:'
          end
          object lbms: TLabel
            Left = 274
            Top = 87
            Width = 13
            Height = 13
            Caption = 'ms'
          end
          object lbMonitorOut: TLabel
            Left = 8
            Top = 119
            Width = 77
            Height = 13
            Caption = 'Monitor Output:'
          end
          object sbtnNoMonitor: TSpeedButton
            Left = 494
            Top = 116
            Width = 22
            Height = 21
            Glyph.Data = {
              36030000424D3603000000000000360000002800000010000000100000000100
              18000000000000030000C40E0000C40E00000000000000000000FFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFF2
              FD5572EA0732E0FFFFFFFFFFFF3659E4173FE0D0D8F9FFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFEFF2FD3559E80732E2647FECFFFFFFFFFFFF3659E4
              0732DE173FE0A2B2F3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD0D9FA3559EA0633
              E4647FEDFFFFFFFFFFFFFFFFFFEFF2FD4565E60732E00632E2A1B2F5FFFFFFFF
              FFFFFFFFFFD0D9FB1640E90633E66480EEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFF738CEF0633E40633E6A1B2F6FFFFFFB1BFF91540EA0633E8A1B2F6FFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA1B2F61640E90533EA63
              80F40533EC0533EAA1B2F6FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFD0D9FB0533ED0534EE0534EEA1B2F7FFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6280F70534F005
              34F00434F26280F8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFEFF2FE6280F80434F20534F0A1B3F9728DF90434F66281FBFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFF2FE335AF80434F41441F3A1B3FAFF
              FFFFFFFFFFB0C0FD1342FA4268FCEFF2FFFFFFFFFFFFFFFFFFFFFFFFFFDFE6FE
              325BF90435F61441F5D0D9FDFFFFFFFFFFFFFFFFFFFFFFFFD0D9FE325BFC325B
              FCEFF2FFFFFFFFFFFFFFFFFFFF1342FA0335F81442F7D0D9FDFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFEFF2FF5274FC819AFDFFFFFFFFFFFFFFFFFF1342FA
              1342F8D0D9FDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
            OnClick = sbtnNoMonitorClick
          end
          object lblMicIn: TLabel
            Left = 8
            Top = 151
            Width = 112
            Height = 13
            Caption = 'Reference Microphone:'
          end
          object sbtnNoMicIn: TSpeedButton
            Left = 494
            Top = 148
            Width = 22
            Height = 21
            Glyph.Data = {
              36030000424D3603000000000000360000002800000010000000100000000100
              18000000000000030000C40E0000C40E00000000000000000000FFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFF2
              FD5572EA0732E0FFFFFFFFFFFF3659E4173FE0D0D8F9FFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFEFF2FD3559E80732E2647FECFFFFFFFFFFFF3659E4
              0732DE173FE0A2B2F3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD0D9FA3559EA0633
              E4647FEDFFFFFFFFFFFFFFFFFFEFF2FD4565E60732E00632E2A1B2F5FFFFFFFF
              FFFFFFFFFFD0D9FB1640E90633E66480EEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFF738CEF0633E40633E6A1B2F6FFFFFFB1BFF91540EA0633E8A1B2F6FFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA1B2F61640E90533EA63
              80F40533EC0533EAA1B2F6FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFD0D9FB0533ED0534EE0534EEA1B2F7FFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6280F70534F005
              34F00434F26280F8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFEFF2FE6280F80434F20534F0A1B3F9728DF90434F66281FBFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFF2FE335AF80434F41441F3A1B3FAFF
              FFFFFFFFFFB0C0FD1342FA4268FCEFF2FFFFFFFFFFFFFFFFFFFFFFFFFFDFE6FE
              325BF90435F61441F5D0D9FDFFFFFFFFFFFFFFFFFFFFFFFFD0D9FE325BFC325B
              FCEFF2FFFFFFFFFFFFFFFFFFFF1342FA0335F81442F7D0D9FDFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFEFF2FF5274FC819AFDFFFFFFFFFFFFFFFFFF1342FA
              1342F8D0D9FDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
              FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}
            OnClick = sbtnNoMicInClick
          end
          object cbTriggerOut: TComboBox
            Left = 166
            Top = 20
            Width = 322
            Height = 21
            Style = csDropDownList
            TabOrder = 0
            OnChange = cbTriggerOutChange
          end
          object cbTriggerIn: TComboBox
            Left = 166
            Top = 52
            Width = 322
            Height = 21
            Style = csDropDownList
            TabOrder = 1
            OnChange = cbChannelChange
          end
          object edLatency: TEdit
            Left = 166
            Top = 84
            Width = 100
            Height = 21
            Alignment = taRightJustify
            TabOrder = 2
            OnChange = edChange
            OnKeyPress = edKeyPress
          end
          object cbMonitorOut: TComboBox
            Left = 166
            Top = 116
            Width = 322
            Height = 21
            Style = csDropDownList
            TabOrder = 3
            OnChange = cbMonitorOutChange
          end
          object cbMicIn: TComboBox
            Left = 166
            Top = 148
            Width = 322
            Height = 21
            Style = csDropDownList
            TabOrder = 4
          end
          object cbUseCalibrator: TCheckBox
            Left = 167
            Top = 174
            Width = 360
            Height = 17
            Caption = 
              'Use acoustic calibrator and reference microphone for level calib' +
              'ration'
            TabOrder = 5
          end
        end
        object gbEqualisation: TGroupBox
          Left = 0
          Top = 279
          Width = 537
          Height = 85
          Align = alTop
          Caption = 'Equalisation Method'
          TabOrder = 4
          object lblFFTLen: TLabel
            Left = 304
            Top = 22
            Width = 59
            Height = 13
            Caption = 'FFT-Length:'
          end
          object lblSamples: TLabel
            Left = 488
            Top = 22
            Width = 39
            Height = 13
            Caption = 'Samples'
          end
          object rbImpulseResonse: TRadioButton
            Left = 166
            Top = 52
            Width = 113
            Height = 17
            Caption = 'Impulse Resonse'
            TabOrder = 0
            OnClick = rbEqClick
          end
          object rbSpectralEq: TRadioButton
            Left = 166
            Top = 20
            Width = 123
            Height = 17
            Caption = 'Spectral Equalisation'
            TabOrder = 1
            OnClick = rbEqClick
          end
          object cobFFTLen: TComboBox
            Left = 369
            Top = 19
            Width = 108
            Height = 21
            Style = csDropDownList
            TabOrder = 2
            OnChange = rbEqClick
          end
        end
        object gbInSitu: TGroupBox
          AlignWithMargins = True
          Left = 0
          Top = 368
          Width = 537
          Height = 144
          Margins.Left = 0
          Margins.Top = 4
          Margins.Right = 0
          Margins.Bottom = 0
          Align = alTop
          Caption = 'In-Situ Output/Input'
          TabOrder = 5
          object lvInSitu: TListView
            Left = 2
            Top = 15
            Width = 533
            Height = 127
            Align = alClient
            BorderStyle = bsNone
            Checkboxes = True
            Columns = <
              item
                Width = 25
              end
              item
                Caption = 'Channel'
                Width = 150
              end
              item
                Caption = 'Index'
                Width = 40
              end
              item
                Caption = 'Hi-Pass'
                Width = 84
              end
              item
                Caption = 'Probe-Microphone'
                Width = 150
              end
              item
                Caption = 'Calibrated'
                Width = 64
              end>
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clBlack
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ReadOnly = True
            RowSelect = True
            ParentFont = False
            PopupMenu = mnuInSitu
            TabOrder = 0
            ViewStyle = vsReport
            OnAdvancedCustomDrawItem = lvOutputAdvancedCustomDrawItem
            OnColumnClick = lvColumnClick
            OnItemChecked = lvOutputItemChecked
          end
        end
      end
    end
  end
  object mnuOutput: TPopupMenu
    OnPopup = mnuOutputPopup
    Left = 592
    Top = 352
    object miCalibrate: TMenuItem
      Caption = 'Calibrate'
      OnClick = miCalibrateClick
    end
    object miRemoveCalibration: TMenuItem
      Caption = 'Remove Calibration Value'
      OnClick = miRemoveCalibrationClick
    end
    object miRawOutput: TMenuItem
      Caption = 'Raw Output'
      OnClick = miRawOutputClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object miSelectEqualisation: TMenuItem
      Caption = 'Select Impulse Response'
      OnClick = miSelectEqualisationClick
    end
    object miRemoveEqualisation: TMenuItem
      Caption = 'Remove Impulse Response'
      OnClick = miRemoveEqualisationClick
    end
    object miEditEqualisation: TMenuItem
      Caption = 'Edit Spectral Equalisation'
      OnClick = miEditEqualisationClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object miSetOutputHighPass: TMenuItem
      Caption = 'Set Hi-Pass'
      OnClick = miSetOutputHighPassClick
    end
    object miRemoveOutputHighPass: TMenuItem
      Caption = 'Remove Hi-Pass'
      OnClick = miRemoveOutputHighPassClick
    end
  end
  object mnuInput: TPopupMenu
    OnPopup = mnuInputPopup
    Left = 592
    Top = 664
    object miSetInputBandPass: TMenuItem
      Caption = 'Set Band-Pass'
      OnClick = miSetInputBandPassClick
    end
    object miRemoveInputBandPass: TMenuItem
      Caption = 'Remove Band-Pass'
      OnClick = miRemoveInputBandPassClick
    end
  end
  object mnuInSitu: TPopupMenu
    OnPopup = mnuInSituPopup
    Left = 584
    Top = 536
    object miSelectInputInSitu: TMenuItem
      Caption = 'Select Input Channel'
      OnClick = miSelectInputInSituClick
    end
    object miRemoveInputInSitu: TMenuItem
      Caption = 'Remove Input Channel'
      OnClick = miRemoveInputInSituClick
    end
    object miRawOutputInSitu: TMenuItem
      Caption = 'Raw Output'
      OnClick = miRawOutputInSituClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object miInSituCalibrate: TMenuItem
      Caption = 'Calibrate Microphone'
      OnClick = miInSituCalibrateClick
    end
    object miInSituRemoveCalibration: TMenuItem
      Caption = 'Remove Calibration '
      OnClick = miInSituRemoveCalibrationClick
    end
    object MenuItem3: TMenuItem
      Caption = '-'
    end
    object miInSituSetHiPass: TMenuItem
      Caption = 'Set Hi-Pass'
      OnClick = miSetOutputHighPassClick
    end
    object miInSituRemoveHiPass: TMenuItem
      Caption = 'Remove Hi-Pass'
      OnClick = miRemoveOutputHighPassClick
    end
  end
end
