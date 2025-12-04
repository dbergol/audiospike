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
#include "frame_SpikeSettings.h"
#include <Vcl.Forms.hpp>
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
      TGroupBox *gbSearch;
      TLabel *lbU3;
      TLabel *lblSearchStimlLength;
      TLabel *lblRepetitionPeriod;
      TLabel *lbU4;
      TLabel *lblSearchRampLength;
      TLabel *lbU6;
      TEdit *edSearchStimLength;
      TEdit *edSearchRepetitionPeriod;
      TEdit *edSearchRampLength;
      TGroupBox *gbGeneral;
      TLabel *lblTemplatePath;
      TLabel *lblResultPath;
      TSpeedButton *sbtnTemplatePath;
      TSpeedButton *sbtnResultPath;
      TEdit *edTemplatePath;
      TEdit *edResultPath;
      TLabel *lblSearchPreStimLength;
      TEdit *edSearchPreStimLength;
      TLabel *lbU5;
      TGroupBox *gbProgramSettings;
      TCheckBox *cbMultipleInstancesAllowed;
      TCheckBox *cbSaveMAT;
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
      TGroupBox *gbPSTHWindow;
      TLabel *Label1;
      TLabel *Label2;
      TEdit *edPSTHBinSize;
      TPanel *pnlGeneral;
      TCheckBox *cbCheckUpdateOnStartup;
      TCheckBox *cbAutoSave;
      TTabSheet *tsFreeSearchSettings;
      TButton *btnResetDontShowAgain;
      TCheckBox *cbFlipPolarity;
      TframeSpikeSettings *frameSpikeSettings;
      TPanel *pnlFreeSearch;
      TCheckBox *cbAlwaysLoadEpoches;
      void __fastcall FormShow(TObject *Sender);
      void __fastcall edKeyPress(TObject *Sender, wchar_t &Key);
      void __fastcall btnOkClick(TObject *Sender);
      void __fastcall sbtnPathClick(TObject *Sender);
      void __fastcall edChange(TObject *Sender);
      void __fastcall cbAutoTemplatePathClick(TObject *Sender);
      void __fastcall cbFreeWindowsClick(TObject *Sender);
      void __fastcall cbSettingsChange(TObject *Sender);
      void __fastcall sbtnAddSettingsClick(TObject *Sender);
      void __fastcall cbStyleChange(TObject *Sender);
      void __fastcall btnResetDontShowAgainClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
      void     WriteSettingsName(UnicodeString us);
      void     ReadSettings();
      void     WriteSettings(void);
   public:		// Benutzer-Deklarationen
      __fastcall TformSettings(TComponent* Owner);
      __fastcall ~TformSettings();
};
//------------------------------------------------------------------------------
#endif
