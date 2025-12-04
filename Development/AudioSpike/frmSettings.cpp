//------------------------------------------------------------------------------
/// \file frmSettings.cpp
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
#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "frmSettings.h"
#include "SpikeWareMain.h"
#include "frmTriggerTest.h"
#include "frmFileSel.h"
#include "SWTools.h"
#include "frmSelectChannel.h"
#include "frmWait.h"
#include "frmFFTEdit.h"
#include "frmCalibration.h"                            
#include "frmCalibrationCalibrator.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frame_SpikeSettings"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initializes possible FFT lengths, settings paths and styles
//------------------------------------------------------------------------------
__fastcall TformSettings::TformSettings(TComponent* Owner)
   : TForm(Owner)
{
   int n;
   DynamicArray<String> daDirectoryNames = TDirectory::GetDirectories(TformSpikeWare::GetSettingsRootPath());
   for (n = 0; n < daDirectoryNames.Length; ++n)
      cbSettings->Items->Add(ExtractFileName(daDirectoryNames[n]));

   cbSettings->ItemIndex = cbSettings->Items->IndexOf(TformSpikeWare::ms_usSettingsName);
   cbSettings->OnChange = cbSettingsChange;


   DynamicArray<String> daStyleNames = Vcl::Themes::TStyleManager::StyleNames;
   for (n = 0; n < daStyleNames.Length; ++n)
      cbStyle->Items->Add(daStyleNames[n]);

   frameSpikeSettings->ListSpikeDetectionMethods(formSpikeWare->m_swsSpikes.m_sdmDetectionMethods);
   #ifdef NOWRITEMAT
   cbSaveMAT->Enabled = false;
   #endif
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor - empty
//------------------------------------------------------------------------------
__fastcall TformSettings::~TformSettings()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback. Adjusts controsl for in-situ/non-insitu and calls ReadSettings
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::FormShow(TObject *Sender)
{
   pc->ActivePage = tsGeneral;
   ReadSettings();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// reads settings from INI
//------------------------------------------------------------------------------
void TformSettings::ReadSettings()
{
   unsigned int n;

   cbCheckUpdateOnStartup->Checked     = formSpikeWare->m_pIni->ReadBool("Settings", "CheckUpdateOnStartup", Ini_CheckUpdateOnStartup);
   cbMultipleInstancesAllowed->Checked = formSpikeWare->m_pIni->ReadBool("Settings", "MulipleInstanceAllowed", Ini_MulipleInstanceAllowed);
   cbAutoSave->Checked                 = formSpikeWare->m_pIni->ReadBool("Settings", "AutoSave", Ini_AutoSave);
   cbAlwaysLoadEpoches->Checked        = formSpikeWare->m_pIni->ReadBool("Settings", "AlwaysLoadEpoches", Ini_AlwaysLoadEpoches);
   cbSaveMAT->Checked                  = formSpikeWare->m_pIni->ReadBool("Settings", "SaveMATFile", Ini_SaveMATFile);
   cbSaveProbeMic->Checked             = formSpikeWare->m_pIni->ReadBool("Settings", "SaveProbeMic", Ini_SaveProbeMic);
   cbStartupInSitu->Checked            = formSpikeWare->m_pIni->ReadBool("Settings", "StartupInSitu", Ini_StartupInSitu);
   cbFreeWindows->OnClick = NULL;
   cbFreeWindows->Checked              = formSpikeWare->m_pIni->ReadBool("Settings", "FreeWindows", Ini_FreeWindows);
   cbFreeWindows->OnClick = cbFreeWindowsClick;


   edSearchStimLength->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchStimLengthMs", Ini_FreeSearchStimLengthMs);
   edSearchStimLength->Tag = 0;
   edSearchPreStimLength->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchPreStimLengthMs", Ini_FreeSearchPreStimLengthMs);
   edSearchPreStimLength->Tag = 0;
   edSearchRepetitionPeriod->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchRepetitionPeriodMs", Ini_FreeSearchRepetitionPeriodMs);
   edSearchRepetitionPeriod->Tag = 0;
   edSearchRampLength->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchRampLengthMs", Ini_FreeSearchRampLengthMs);
   edSearchRampLength->Tag = 0;

   UnicodeString us = formSpikeWare->m_pIni->ReadString("Settings", "SpikeDetectionMethod", formSpikeWare->m_swsSpikes.m_sdmDetectionMethods.GetMethodNameDefault());
   int nIndex = frameSpikeSettings->cbSpikeDetectionMethod->Items->IndexOf(us);
   if (nIndex < 0)
      nIndex = (int)formSpikeWare->m_swsSpikes.m_sdmDetectionMethods.GetMethodIndexDefault();
   frameSpikeSettings->SetSpikeDetectionMethod(nIndex);
   frameSpikeSettings->cbSpikeDetectionMethod->Tag = 0;

   frameSpikeSettings->cbRejectSingleSignSpikes->Checked = formSpikeWare->m_pIni->ReadBool("Settings", "RejectSingleSignSpikes", Ini_RejectSingleSignSpikes);

   frameSpikeSettings->edPreThreshold->Text = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "PreThreshold", Ini_PreThreshold)* 1000.0);
   frameSpikeSettings->edPreThreshold->Tag = 0;
   frameSpikeSettings->edPostThreshold->Text = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "PostThreshold", Ini_PostThreshold)* 1000.0);
   frameSpikeSettings->edPostThreshold->Tag = 0;
   frameSpikeSettings->edSpikeLength->Text  = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "SpikeLength", Ini_SpikeLength) * 1000.0);
   frameSpikeSettings->edSpikeLength->Tag = 0;
   frameSpikeSettings->edRefractoryTime->Text = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "RefractoryTime", Ini_RefractoryTime)* 1000.0);
   frameSpikeSettings->edRefractoryTime->Tag = 0;
   frameSpikeSettings->edRefractoryTimeTailFactor->Text  = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "RefractoryTimeTailFactor", Ini_RefractoryTimeTailFactor));
   frameSpikeSettings->edRefractoryTimeTailFactor->Tag = 0;

   cbFlipPolarity->Checked = formSpikeWare->m_pIni->ReadBool("Settings", "FlipPolarity", Ini_FlipPolarity);
   if (formSpikeWare->m_pIni->ReadBool("Settings", "SpikeTimeReferencePeak+", Ini_SpikeTimeReferencePeakPos))
      frameSpikeSettings->rbPeakPlus->Checked = true;
   else
      frameSpikeSettings->rbPeakMinus->Checked = true;


   edPSTHBinSize->Text  = formSpikeWare->m_pIni->ReadInteger("Settings", "PSTHBinSize", Ini_PSTHBinSize);
   edPSTHBinSize->Tag = 0;

   cbAutoTemplatePath->Checked       = formSpikeWare->m_pIni->ReadBool("Settings", "AutoTemplatePath", Ini_AutoTemplatePath);
   cbAutoTemplatePathClick(NULL);
   cbAutoTemplatePath->Tag = 0;
   edTemplatePath->Text = formSpikeWare->m_pIni->ReadString("Settings", "TemplatePath", ExpandFileName(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "..\\Templates\\"));
   edTemplatePath->Tag = 0;
   edResultPath->Text   = formSpikeWare->m_usResultPathRoot;
   edResultPath->Tag = 0;

   us = formSpikeWare->m_pIni->ReadString("Settings", "Style", Ini_Style);
   nIndex = cbStyle->Items->IndexOf(us);
   if (nIndex < 0)
      nIndex = cbStyle->Items->IndexOf("Windows");
   cbStyle->ItemIndex = nIndex;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Writes settings to INI
//------------------------------------------------------------------------------
void TformSettings::WriteSettings(void)
{
   formSpikeWare->m_pIni->WriteBool("Settings", "CheckUpdateOnStartup", cbCheckUpdateOnStartup->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "MulipleInstanceAllowed", cbMultipleInstancesAllowed->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "AutoSave", cbAutoSave->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "AlwaysLoadEpoches", cbAlwaysLoadEpoches->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "SaveMATFile", cbSaveMAT->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "SaveProbeMic", cbSaveProbeMic->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "StartupInSitu", cbStartupInSitu->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "FreeWindows", cbFreeWindows->Checked);



   if (edSearchStimLength->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchStimLengthMs", edSearchStimLength->Text);
   if (edSearchPreStimLength->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchPreStimLengthMs", edSearchPreStimLength->Text);
   if (edSearchRepetitionPeriod->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchRepetitionPeriodMs", edSearchRepetitionPeriod->Text);
   if (edSearchRampLength->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchRampLengthMs", edSearchRampLength->Text);


   if (frameSpikeSettings->cbSpikeDetectionMethod->Tag)
      {
      UnicodeString us = frameSpikeSettings->cbSpikeDetectionMethod->Text;
      formSpikeWare->m_pIni->WriteString("Settings", "SpikeDetectionMethod", us);
      }

   formSpikeWare->m_pIni->WriteBool("Settings", "RejectSingleSignSpikes", frameSpikeSettings->cbRejectSingleSignSpikes->Checked);

   if (frameSpikeSettings->edPreThreshold->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "PreThreshold", DoubleToStr(StrToDouble(frameSpikeSettings->edPreThreshold->Text) / 1000.0));
   if (frameSpikeSettings->edPostThreshold->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "PostThreshold", DoubleToStr(StrToDouble(frameSpikeSettings->edPostThreshold->Text) / 1000.0));
   if (frameSpikeSettings->edSpikeLength->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "SpikeLength", DoubleToStr(StrToDouble(frameSpikeSettings->edSpikeLength->Text) / 1000.0));
   if (frameSpikeSettings->edRefractoryTime->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "RefractoryTime", DoubleToStr(StrToDouble(frameSpikeSettings->edRefractoryTime->Text) / 1000.0));
   if (frameSpikeSettings->edRefractoryTimeTailFactor->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "RefractoryTimeTailFactor", DoubleToStr(StrToDouble(frameSpikeSettings->edRefractoryTimeTailFactor->Text)));


   formSpikeWare->m_pIni->WriteBool("Settings", "FreeWindows", cbFreeWindows->Checked);

   formSpikeWare->m_pIni->WriteBool("Settings", "FlipPolarity", cbFlipPolarity->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "SpikeTimeReferencePeak+", frameSpikeSettings->rbPeakPlus->Checked);


   if (edPSTHBinSize->Tag)
      formSpikeWare->m_pIni->WriteInteger("Settings", "PSTHBinSize", StrToInt(edPSTHBinSize->Text));



   if (cbAutoTemplatePath->Tag)
      formSpikeWare->m_pIni->WriteBool("Settings", "AutoTemplatePath", cbAutoTemplatePath->Checked);
   if (edTemplatePath->Tag)
      {
      formSpikeWare->m_usTemplatePath     = IncludeTrailingBackslash(edTemplatePath->Text);
      formSpikeWare->m_pIni->WriteString("Settings", "TemplatePath", formSpikeWare->m_usTemplatePath);
      }
   if (edResultPath->Tag)
      {
      formSpikeWare->m_usResultPathRoot   = IncludeTrailingBackslash(edResultPath->Text);
      formSpikeWare->m_pIni->WriteString("Settings", "ResultPathRoot", formSpikeWare->m_usResultPathRoot);
      }

   if (cbStyle->Tag && cbStyle->ItemIndex >= 0)
      {
      formSpikeWare->m_pIni->WriteString("Settings", "Style", cbStyle->Items->Strings[cbStyle->ItemIndex]);
      ModalResult = mrIgnore;
      }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnKeyPress callback for edit fields: checks input constraints
//------------------------------------------------------------------------------
void __fastcall TformSettings::edKeyPress(TObject *Sender, wchar_t &Key)
{
   TEdit* ped = dynamic_cast<TEdit* >(Sender);
   if (!ped)
      return;
   if (  (Key >= L'0' && Key <= L'9')
//      || (Key == L'-' && ped->SelStart == 0)
      || Key == VK_BACK
      )
      return;
   if (  ped->Hint == ""
      && (Key == FormatSettings.DecimalSeparator && ped->Text.Pos(FormatSettings.DecimalSeparator) == 0)
      )
      return;

   Key = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnCick callback of OK button. Validates settings and writes them on success
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::btnOkClick(TObject *Sender)
{
   // check tail factor constraint (if enabled at all)
   if (frameSpikeSettings->edRefractoryTimeTailFactor->Enabled)
      {
      double dRefractoryTimeTailFactor = StrToDouble(frameSpikeSettings->edRefractoryTimeTailFactor->Text);
      if (dRefractoryTimeTailFactor < 1.0)
         {
         if (ID_YES != MessageBox(  Handle,
                                 "The Refractory Time Tail Factor must be >= 1.0. Should it be set to 1.0?",
                                 "Question",
                                 MB_ICONQUESTION | MB_YESNO
                                 )
            )
            {
            frameSpikeSettings->edRefractoryTimeTailFactor->SetFocus();
            return;
            }

         frameSpikeSettings->edRefractoryTimeTailFactor->Text = 1.0;
         }
      }


   WriteSettings();

   if (Sender == btnOk)
      ModalResult = mrOk;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnClick callback of sbtnTemplatePath and sbtnResultPath: shows dialog to
/// select corresponding path
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::sbtnPathClick(TObject *Sender)
{
   TEdit *ped = Sender == sbtnTemplatePath ? edTemplatePath : edResultPath;
   // SelectDirectory encapsulates SHBrowseForFolder
   UnicodeString usDirectory = ped->Text;
   TSelectDirExtOpts sdeo;
   sdeo = sdeo << sdNewUI << sdNewFolder << sdShowEdit;
   if ( SelectDirectory("Select Path", L"", usDirectory, sdeo) )
      ped->Text = usDirectory;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback for multiple edit fields: sets Tag to 1 as indicator, that
/// this value was changed (used in WriteSettings)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::edChange(TObject *Sender)
{
   TEdit *ped = dynamic_cast<TEdit*>(Sender);
   if (!ped)
      return;
   ped->Tag = 1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of cbAutoTemplatePath: enables/disables automatic template 
/// path generation feature
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::cbAutoTemplatePathClick(TObject *Sender)
{
   edTemplatePath->Enabled    = !cbAutoTemplatePath->Checked;
   sbtnTemplatePath->Enabled  = edTemplatePath->Enabled;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of cbFreeWindows: toggles MDI / non-MDI forms
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::cbFreeWindowsClick(TObject *Sender)
{
   MessageBoxW(Handle, L"Change will be applied on next restart of AudioSpike.", L"Info", MB_ICONINFORMATION);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of cbSettings: handles selection of differnet settings (paths)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::cbSettingsChange(TObject *Sender)
{
   if (cbSettings->ItemIndex >= 0)
      {
      UnicodeString us = cbSettings->Items->Strings[cbSettings->ItemIndex];
      if (us != TformSpikeWare::ms_usSettingsName)
         {
         WriteSettingsName(us);
         MessageBoxW(Handle, L"Change will be applied on next restart of AudioSpike.", L"Info", MB_ICONINFORMATION);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of sbtnAddSettings: here adding of new settings in GUI
/// may happen: not implemented yet!
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::sbtnAddSettingsClick(TObject *Sender)
{
   // EMPTY UP TO NOW AND BUTTON IS SET INVISIBLE
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Writes name of currently selected settings to global INI
//------------------------------------------------------------------------------
void TformSettings::WriteSettingsName(UnicodeString us)
{
   TIniFile* pIni = new TIniFile(ChangeFileExt(Application->ExeName, ".Settings.ini"));
   try
      {
      pIni->WriteString("Global", "Settings", us);
      }
   __finally
      {
      TRYDELETENULL(pIni);
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback to cbStyle. Sets Tag to 1 to indicate that
/// it was changed
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::cbStyleChange(TObject *Sender)
{
   cbStyle->Tag = 1;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnClick callback of btnResetDontShowAgain: erases inifile section that stores
/// "Don't show again" - Dialog flags
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::btnResetDontShowAgainClick(TObject *Sender)
{
   formSpikeWare->m_pIni->EraseSection("OptionalDialogs");
}
//---------------------------------------------------------------------------

