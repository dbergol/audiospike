//------------------------------------------------------------------------------
/// \file frmSettings.h
///
/// \author Berg
/// \brief Implementation of the AudioSpike main settings dialog
///
/// Project AudioSpike
/// Module  AudioSpike.exe
///
/// ****************************************************************************
/// Copyright 2023 Daniel Berg, Oldenburg, Germany
/// ****************************************************************************
///
/// This file is part of AudioSpike.
///
///    AudioSpike is free software: you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation, either version 3 of the License, or
///    (at your option) any later version.
///
///    AudioSpike is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License
///    along with AudioSpike.  If not, see <http:///www.gnu.org/licenses/>.
///
//------------------------------------------------------------------------------
#ifndef frmSettingsH
#define frmSettingsH
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <System.Classes.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <System.IOUtils.hpp>
//------------------------------------------------------------------------------
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///  form implementing AudioSpike main settings dialog
//------------------------------------------------------------------------------
class TformSettings : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TButton *btnOk;
      TButton *btnCancel;
      TPageControl *pc;
      TTabSheet *tsGeneral;
      TTabSheet *tsSound;
      TGroupBox *gbSeacrh;
      TLabel *lbU3;
      TLabel *lblSearchStimlLength;
      TLabel *lblRepetitionPeriod;
      TLabel *lbU4;
      TLabel *lblSearchRampLength;
      TLabel *lbU6;
      TEdit *edSearchStimLength;
      TEdit *edSearchRepetitionPeriod;
      TEdit *edSearchRampLength;
      TGroupBox *gbSpikes;
      TLabel *lbU1;
      TLabel *lbPreThreshold;
      TLabel *lbSpikeLength;
      TLabel *lbU2;
      TEdit *edPreThreshold;
      TEdit *edSpikeLength;
      TGroupBox *gbGeneral;
      TLabel *lblTemplatePath;
      TLabel *lblResultPath;
      TSpeedButton *sbtnTemplatePath;
      TSpeedButton *sbtnResultPath;
      TEdit *edTemplatePath;
      TEdit *edResultPath;
      TGroupBox *gbSystem;
      TLabel *lbDriver;
      TComboBox *cbDrivers;
      TLabel *lblSamplerate;
      TEdit *edSampleRate;
      TLabel *lblHz;
      TGroupBox *gbInput;
      TListView *lvInput;
      TGroupBox *gbOutput;
      TListView *lvOutput;
      TGroupBox *gbTrigger;
      TGroupBox *gbEqualisation;
      TLabel *lbTriggerOut;
      TLabel *lbTriggerIn;
      TComboBox *cbTriggerOut;
      TComboBox *cbTriggerIn;
      TSpeedButton *sbtnTriggerTest;
      TLabel *lbLatency;
      TEdit *edLatency;
      TLabel *lbms;
      TLabel *lblSearchPreStimLength;
      TEdit *edSearchPreStimLength;
      TLabel *lbU5;
      TPopupMenu *mnuOutput;
      TMenuItem *miCalibrate;
      TMenuItem *miRemoveCalibration;
      TMenuItem *N1;
      TMenuItem *miSelectEqualisation;
      TMenuItem *miRemoveEqualisation;
      TLabel *lbMonitorOut;
      TComboBox *cbMonitorOut;
      TSpeedButton *sbtnNoMonitor;
      TRadioButton *rbImpulseResonse;
      TRadioButton *rbSpectralEq;
      TComboBox *cobFFTLen;
      TLabel *lblFFTLen;
      TMenuItem *miEditEqualisation;
      TLabel *lblMicIn;
      TComboBox *cbMicIn;
      TSpeedButton *sbtnNoMicIn;
      TLabel *lblSamples;
      TComboBox *cbSamplerateDevider;
      TLabel *lbDownSamplingFactor;
      TGroupBox *gbProgramSettings;
      TCheckBox *cbMultipleInstancesAllowed;
      TCheckBox *cbSaveMAT;
      TMenuItem *N2;
      TMenuItem *miSetOutputHighPass;
      TMenuItem *miRemoveOutputHighPass;
      TPopupMenu *mnuInput;
      TMenuItem *miSetInputBandPass;
      TMenuItem *miRemoveInputBandPass;
      TGroupBox *gbInSitu;
      TListView *lvInSitu;
      TPopupMenu *mnuInSitu;
      TMenuItem *miInSituCalibrate;
      TMenuItem *miInSituRemoveCalibration;
      TMenuItem *MenuItem3;
      TMenuItem *miInSituSetHiPass;
      TMenuItem *miInSituRemoveHiPass;
      TMenuItem *miSelectInputInSitu;
      TMenuItem *N3;
      TMenuItem *miRemoveInputInSitu;
      TCheckBox *cbAutoTemplatePath;
      TCheckBox *cbFreeWindows;
      TCheckBox *cbSaveProbeMic;
      TCheckBox *cbStartupInSitu;
      TComboBox *cbStyle;
      TLabel *lbStyle;
      TLabel *lblSettingsName;
      TComboBox *cbSettings;
      TBevel *Bevel1;
      TSpeedButton *sbtnAddSettings;
      TCheckBox *cbUseCalibrator;
      TMenuItem *miRawOutputInSitu;
      TMenuItem *miRawOutput;
      TGroupBox *gbPSTHWindow;
      TLabel *Label1;
      TLabel *Label2;
      TEdit *edPSTHBinSize;
      TLabel *lbPostThreshold;
      TEdit *edPostThreshold;
      TLabel *lbU11;
      TPanel *pnlGeneral;
      TPanel *pnlSound;
   TCheckBox *cbCheckUpdateOnStartup;
      void __fastcall FormShow(TObject *Sender);
      void __fastcall cbDriversChange(TObject *Sender);
      void __fastcall lvOutputAdvancedCustomDrawItem(TCustomListView *Sender, TListItem *Item,
             TCustomDrawState State, TCustomDrawStage Stage, bool &DefaultDraw);
      void __fastcall lvOutputItemChecked(TObject *Sender, TListItem *Item);
      void __fastcall lvInputAdvancedCustomDrawItem(TCustomListView *Sender, TListItem *Item,
             TCustomDrawState State, TCustomDrawStage Stage, bool &DefaultDraw);
      void __fastcall lvInputItemChecked(TObject *Sender, TListItem *Item);
      void __fastcall cbChannelChange(TObject *Sender);
      void __fastcall lvColumnClick(TObject *Sender, TListColumn *Column);
      void __fastcall edKeyPress(TObject *Sender, wchar_t &Key);
      void __fastcall btnOkClick(TObject *Sender);
      void __fastcall sbtnNoMonitorClick(TObject *Sender);
      void __fastcall sbtnTriggerTestClick(TObject *Sender);
      void __fastcall sbtnPathClick(TObject *Sender);
      void __fastcall edChange(TObject *Sender);
      void __fastcall mnuOutputPopup(TObject *Sender);
      void __fastcall miCalibrateClick(TObject *Sender);
      void __fastcall miRemoveCalibrationClick(TObject *Sender);
      void __fastcall miSelectEqualisationClick(TObject *Sender);
      void __fastcall miRemoveEqualisationClick(TObject *Sender);
      void __fastcall rbEqClick(TObject *Sender);
      void __fastcall miEditEqualisationClick(TObject *Sender);
      void __fastcall sbtnNoMicInClick(TObject *Sender);
      void __fastcall cbSamplerateDeviderChange(TObject *Sender);
      void __fastcall miSetOutputHighPassClick(TObject *Sender);
      void __fastcall miRemoveOutputHighPassClick(TObject *Sender);
      void __fastcall mnuInputPopup(TObject *Sender);
      void __fastcall miSetInputBandPassClick(TObject *Sender);
      void __fastcall miRemoveInputBandPassClick(TObject *Sender);
      void __fastcall miSelectInputInSituClick(TObject *Sender);
      void __fastcall mnuInSituPopup(TObject *Sender);
      void __fastcall miRemoveInputInSituClick(TObject *Sender);
      void __fastcall miInSituCalibrateClick(TObject *Sender);
      void __fastcall miInSituRemoveCalibrationClick(TObject *Sender);
      void __fastcall cbAutoTemplatePathClick(TObject *Sender);
      void __fastcall cbFreeWindowsClick(TObject *Sender);
      void __fastcall cbTriggerOutChange(TObject *Sender);
      void __fastcall cbMonitorOutChange(TObject *Sender);
      void __fastcall cbSettingsChange(TObject *Sender);
      void __fastcall sbtnAddSettingsClick(TObject *Sender);
      void __fastcall cbStyleChange(TObject *Sender);
      void __fastcall miRawOutputClick(TObject *Sender);
      void __fastcall miRawOutputInSituClick(TObject *Sender);

   private:	// Benutzer-Deklarationen
      void     WriteSettingsName(UnicodeString us);
      void     ReadSettings();
      void     ReadChannels();
      void     ReadCalEq();
      void     SetSubItemCheckStatus(TListItem* pli, int nSubItem, int nStatus = -1);
      void     AdjustInternalNames(TListView* plv);
      void     WriteSettings(bool bNoSoundSettings = false, bool bShowError = true);
      void     ScrollOutputs(int n);
      void     SetProbeMicCalStatus(TListItem *pli);
      bool     OutputIsRaw(TListItem* pli);
   public:		// Benutzer-Deklarationen
      __fastcall TformSettings(TComponent* Owner);
      __fastcall ~TformSettings();
      void     ReadSoundSettings();
      int      CheckSettings(bool bCanIgnore = false);
      void     AdjustEQControls();
};
//------------------------------------------------------------------------------
#endif
