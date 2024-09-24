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
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initializes possible FFT lengths, settings paths and styles
//------------------------------------------------------------------------------
__fastcall TformSettings::TformSettings(TComponent* Owner)
   : TForm(Owner)
{
   int n;
   for (n = 8; n < 12; n++)
      cobFFTLen->Items->Add(2<<n);


   DynamicArray<String> daDirectoryNames = TDirectory::GetDirectories(TformSpikeWare::GetSettingsRootPath());
   for (n = 0; n < daDirectoryNames.Length; ++n)
      cbSettings->Items->Add(ExtractFileName(daDirectoryNames[n]));

   cbSettings->ItemIndex = cbSettings->Items->IndexOf(TformSpikeWare::ms_usSettingsName);
   cbSettings->OnChange = cbSettingsChange;


   DynamicArray<String> daStyleNames = Vcl::Themes::TStyleManager::StyleNames;
   for (n = 0; n < daStyleNames.Length; ++n)
      cbStyle->Items->Add(daStyleNames[n]);

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
   gbEqualisation->Visible = !formSpikeWare->IsInSitu();
   gbOutput->Visible = gbEqualisation->Visible;
   gbInSitu->Visible = !gbEqualisation->Visible;

   gbEqualisation->Top = gbTrigger->Top + gbTrigger->Height;
   ReadSettings();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// checks, if all mandatory settings are fine
//------------------------------------------------------------------------------
int TformSettings::CheckSettings(bool bCanIgnore)
{
   int nReturn = ID_CANCEL;
   try
      {
      if (cbDrivers->ItemIndex < 0)
         throw Exception("Please select a driver.");
      if (cbTriggerIn->ItemIndex < 0)
         throw Exception("Please select a trigger input channel.");
      if (cbTriggerOut->ItemIndex < 0)
         throw Exception("Please select a trigger output channel.");
      if (formSpikeWare->IsInSitu() && cbMicIn->ItemIndex < 0)
         throw Exception("Please select a reference microphone channel");

      int n;
      bool bSelected = false;
      TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;
      for (n = 0; n < plv->Items->Count; n++)
         {
         if (plv->Items->Item[n]->Checked)
            {
            bSelected = true;
            break;
            }
         }
      if (!bSelected)
         throw Exception("Please select at least one output channel.");

      bSelected = false;
      for (n = 0; n < lvInput->Items->Count; n++)
         {
         if (lvInput->Items->Item[n]->Checked)
            {
            bSelected = true;
            break;
            }
         }
      if (!bSelected)
         throw Exception("Please select at least one input channel.");


      nReturn = ID_OK;
      }
   catch (Exception &e)
      {
      if (formWait->Visible)
         formWait->Hide();
      UnicodeString us = e.Message;
      UINT nFlags = MB_ICONERROR;
      if (bCanIgnore)
         {
         nFlags |= MB_OKCANCEL;
         us += " Press 'OK' to ignore the error or 'Cancel' to correct it.";
         }
      int n = MessageBoxW(Handle, us.w_str(), L"Error", nFlags);

      if (bCanIgnore && n == ID_OK)
         nReturn = ID_IGNORE;
      }
   return nReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads settings from INI
//------------------------------------------------------------------------------
void TformSettings::ReadSettings()
{
   unsigned int n;

   cbCheckUpdateOnStartup->Checked     = formSpikeWare->m_pIni->ReadBool("Settings", "CheckUpdateOnStartup", true);
   cbMultipleInstancesAllowed->Checked = formSpikeWare->m_pIni->ReadBool("Settings", "MulipleInstanceAllowed", false);
   cbSaveMAT->Checked                  = formSpikeWare->m_pIni->ReadBool("Settings", "SaveMATFile", false);
   cbSaveProbeMic->Checked             = formSpikeWare->m_pIni->ReadBool("Settings", "SaveProbeMic", false);
   cbStartupInSitu->Checked            = formSpikeWare->m_pIni->ReadBool("Settings", "StartupInSitu", false);
   cbFreeWindows->OnClick = NULL;
   cbFreeWindows->Checked              = formSpikeWare->m_pIni->ReadBool("Settings", "FreeWindows", false);
   cbFreeWindows->OnClick = cbFreeWindowsClick;
   cbUseCalibrator->Checked            = formSpikeWare->m_pIni->ReadBool("Settings", "UseCalibrator", false);

   edSampleRate->Text = IniReadDouble(formSpikeWare->m_pIni, "Settings", "SampleRateDefault", 44100.0);
   edSampleRate->Tag = 0;


   n = (unsigned int)formSpikeWare->m_pIni->ReadInteger("Settings", "SampleRateDeviderDefault", 1);
   int nIndex = cbSamplerateDevider->Items->IndexOf(IntToStr((int)n));

   if (nIndex > -1)
      cbSamplerateDevider->ItemIndex = nIndex;
   else
      cbSamplerateDevider->ItemIndex = 0;
   cbSamplerateDevider->Tag = 0;

   edSearchStimLength->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchStimLengthMs", 150);
   edSearchStimLength->Tag = 0;
   edSearchPreStimLength->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchPreStimLengthMs", 20);
   edSearchPreStimLength->Tag = 0;
   edSearchRepetitionPeriod->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchRepetitionPeriodMs", 350);
   edSearchRepetitionPeriod->Tag = 0;
   edSearchRampLength->Text = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchRampLengthMs", 5);
   edSearchRampLength->Tag = 0;

   edPreThreshold->Text = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "PreThreshold", 0.0005)* 1000.0);
   edPreThreshold->Tag = 0;
   edPostThreshold->Text = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "PostThreshold", 0.0)* 1000.0);
   edPostThreshold->Tag = 0;
   edSpikeLength->Text  = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "SpikeLength", 0.0025) * 1000.0);
   edSpikeLength->Tag = 0;

   edPSTHBinSize->Text  = formSpikeWare->m_pIni->ReadInteger("Settings", "PSTHBinSize", 1);
   edPSTHBinSize->Tag = 0;

   cbAutoTemplatePath->Checked       = formSpikeWare->m_pIni->ReadBool("Settings", "AutoTemplatePath", false);
   cbAutoTemplatePathClick(NULL);
   cbAutoTemplatePath->Tag = 0;
   edTemplatePath->Text = formSpikeWare->m_pIni->ReadString("Settings", "TemplatePath", ExpandFileName(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "..\\Templates\\"));
   edTemplatePath->Tag = 0;
   edResultPath->Text   = formSpikeWare->m_usResultPathRoot;
   edResultPath->Tag = 0;

   UnicodeString us = formSpikeWare->m_pIni->ReadString("Settings", "Style", "Windows");
   nIndex = cbStyle->Items->IndexOf(us);
   if (nIndex < 0)
      nIndex = cbStyle->Items->IndexOf("Windows");
   cbStyle->ItemIndex = nIndex;


   edLatency->Text = DoubleToStr(IniReadDouble(formSpikeWare->m_pIni, "Settings", "TriggerLatency", 0.0)*1000.0);
   edLatency->Tag = 0;

   //
   cobFFTLen->OnChange = NULL;
   rbImpulseResonse->OnClick = NULL;
   rbSpectralEq->OnClick = NULL;
   gbEqualisation->Tag = 0;
   try
      {
      int nEqMethod = formSpikeWare->m_pIni->ReadInteger("Settings", "EqualisationMethod", AW_SMP_EQ_FFT);
      if (nEqMethod == AW_SMP_EQ_IR)
         rbImpulseResonse->Checked = true;
      else
         rbSpectralEq->Checked = true;
      formSpikeWare->m_smp.m_nEqualisationMethod = nEqMethod;
      us = formSpikeWare->m_pIni->ReadInteger("Settings", "FFTLen", FFTLEN_DEFAULT);
      nIndex = cobFFTLen->Items->IndexOf(us);
      if (nIndex < 0)
         nIndex = cobFFTLen->Items->IndexOf(IntToStr(FFTLEN_DEFAULT));
      cobFFTLen->ItemIndex = nIndex;
      AdjustEQControls();
      gbEqualisation->Tag = 0;

      }
   __finally
      {
      cobFFTLen->OnChange = rbEqClick;
      rbImpulseResonse->OnClick = rbEqClick;
      rbSpectralEq->OnClick = rbEqClick;
      }


   cbDrivers->Items = formSpikeWare->m_smp.m_pslDrivers;
   us = formSpikeWare->m_smp.GetDriver();
   if (us == "")
      {
      formSpikeWare->SWErrorBox("Please select a valid sound driver", Handle);
      return;
      }
   nIndex = cbDrivers->Items->IndexOf(us);
   if (nIndex < 0)
      {
      formSpikeWare->SWErrorBox("Driver from settings not found: please select a valid sound driver", Handle);
      return;
      }

   cbDrivers->ItemIndex = nIndex;
   ReadChannels();

   TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;

   plv->OnItemChecked      = NULL;
   lvInput->OnItemChecked  = NULL;

   try
      {
      for (n = 0; n < formSpikeWare->m_smp.m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT); n++)
         {
         if (formSpikeWare->m_smp.m_swcHWChannels.IsOutput(n))
            plv->Items->Item[(int)n]->Checked = true;
         }
      for (n = 0; n < formSpikeWare->m_smp.m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_IN); n++)
         {

         if (formSpikeWare->m_smp.m_swcHWChannels.IsElectrode(n))
            lvInput->Items->Item[(int)n]->Checked = true;
         }
      }
   __finally
      {
      plv->OnItemChecked      = lvOutputItemChecked;
      lvInput->OnItemChecked  = lvInputItemChecked;
      }

   int nTriggerChannelIn   = formSpikeWare->m_smp.m_swcHWChannels.GetTrigger(SWSMPHWCDIR_IN);
   int nTriggerChannelOut  = formSpikeWare->m_smp.m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT);
   int nMonitorChannelOut  = formSpikeWare->m_smp.m_swcHWChannels.GetMonitor();
   int nRefMicChannelIn    = formSpikeWare->m_smp.m_swcHWChannels.GetRefMic();
   if (nTriggerChannelIn < cbTriggerIn->Items->Count)
      cbTriggerIn->ItemIndex = nTriggerChannelIn;
   if (nTriggerChannelOut < cbTriggerOut->Items->Count)
      cbTriggerOut->ItemIndex = nTriggerChannelOut;
   if (nMonitorChannelOut < cbMonitorOut->Items->Count)
      cbMonitorOut->ItemIndex = nMonitorChannelOut;
   if (nRefMicChannelIn < cbMicIn->Items->Count)
      cbMicIn->ItemIndex = nRefMicChannelIn;

   AdjustInternalNames(lvInput);
   AdjustInternalNames(plv);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Writes settings to INI
//------------------------------------------------------------------------------
void TformSettings::WriteSettings(bool bNoSoundSettings, bool bShowError)
{
   formSpikeWare->m_pIni->WriteBool("Settings", "CheckUpdateOnStartup", cbCheckUpdateOnStartup->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "MulipleInstanceAllowed", cbMultipleInstancesAllowed->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "SaveMATFile", cbSaveMAT->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "SaveProbeMic", cbSaveProbeMic->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "StartupInSitu", cbStartupInSitu->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "FreeWindows", cbFreeWindows->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "UseCalibrator", cbUseCalibrator->Checked);


   formSpikeWare->m_pIni->WriteString("Settings", "SampleRateDefault", edSampleRate->Text);
   formSpikeWare->m_pIni->WriteString("Settings", "SampleRateDeviderDefault", cbSamplerateDevider->Text);

   if (edSearchStimLength->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchStimLengthMs", edSearchStimLength->Text);
   if (edSearchPreStimLength->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchPreStimLengthMs", edSearchPreStimLength->Text);
   if (edSearchRepetitionPeriod->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchRepetitionPeriodMs", edSearchRepetitionPeriod->Text);
   if (edSearchRampLength->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "FreeSearchRampLengthMs", edSearchRampLength->Text);

   if (edPreThreshold->Tag || edPostThreshold->Tag || edSpikeLength->Tag)
      {
      double d1 = StrToDouble(edPreThreshold->Text) / 1000.0;
      double d2 = StrToDouble(edPostThreshold->Text) / 1000.0;
      double d3 = StrToDouble(edSpikeLength->Text) / 1000.0;
      formSpikeWare->m_swsSpikes.SetSpikeLength(d1, d2, d3);
      formSpikeWare->m_pIni->WriteString("Settings", "PreThreshold", DoubleToStr(d1));
      formSpikeWare->m_pIni->WriteString("Settings", "PostThreshold", DoubleToStr(d2));
      formSpikeWare->m_pIni->WriteString("Settings", "SpikeLength", DoubleToStr(d3));
      }

    if (edPSTHBinSize->Tag)
      formSpikeWare->m_pIni->WriteInteger("Settings", "PSTHBinSize", StrToInt(edPSTHBinSize->Text));

   if (edLatency->Tag)
      formSpikeWare->m_pIni->WriteString("Settings", "TriggerLatency", StrToDouble(edLatency->Text) / 1000.0);


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

   if (gbEqualisation->Tag)
      {
      // write equalisation
      formSpikeWare->m_pIni->WriteInteger("Settings", "EqualisationMethod", rbSpectralEq->Checked ? AW_SMP_EQ_FFT : AW_SMP_EQ_IR);
      formSpikeWare->m_pIni->WriteString("Settings", "FFTLen", cobFFTLen->Text);
      }

   if (cbStyle->Tag && cbStyle->ItemIndex >= 0)
      {
      formSpikeWare->m_pIni->WriteString("Settings", "Style", cbStyle->Items->Strings[cbStyle->ItemIndex]);
      ModalResult = mrIgnore;
      }


   #ifdef CHKCHNLS
   if (!bNoSoundSettings)
      {
      TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;

      formSpikeWare->m_smp.m_usDriver = cbDrivers->Text;
      formSpikeWare->m_smp.m_viChannelsOutSettings.clear();
      int n;
      bool bIsOutputRaw;

      for (n = 0; n < plv->Items->Count; n++)
         {
         if (formSpikeWare->IsInSitu())
            {
            bIsOutputRaw = OutputIsRaw(plv->Items->Item[n]);
            if (bIsOutputRaw)
               formSpikeWare->m_smp.SetInSituInput(cbTriggerOut->Items->Strings[n], "");
            else
               formSpikeWare->m_smp.SetInSituInput(cbTriggerOut->Items->Strings[n], plv->Items->Item[n]->SubItems->Strings[3]);
            }
         if (plv->Items->Item[n]->Checked)
            formSpikeWare->m_smp.m_viChannelsOutSettings.push_back(n);

         }

      formSpikeWare->m_smp.m_viChannelsInSettings.clear();
      for (n = 0; n < lvInput->Items->Count; n++)
         {
         if (lvInput->Items->Item[n]->Checked)
            formSpikeWare->m_smp.m_viChannelsInSettings.push_back(n);
         }

      formSpikeWare->m_smp.m_nTriggerChannelIn  = cbTriggerIn->ItemIndex;
      formSpikeWare->m_smp.m_nTriggerChannelOut = cbTriggerOut->ItemIndex;
      formSpikeWare->m_smp.m_nMonitorChannelOut = cbMonitorOut->ItemIndex;
      formSpikeWare->m_smp.m_nMicChannelIn      = cbMicIn->ItemIndex;
      }
   #endif

   if (!bNoSoundSettings)
      {
      TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;
      formSpikeWare->m_smp.SetDriver(cbDrivers->Items->Strings[cbDrivers->ItemIndex]);

      int n;
      bool bIsOutputRaw;
      for (n = 0; n < plv->Items->Count; n++)
         {
         if (formSpikeWare->IsInSitu())
            {
            bIsOutputRaw = OutputIsRaw(plv->Items->Item[n]);
            if (bIsOutputRaw)
               formSpikeWare->m_smp.SetInSituInput(cbTriggerOut->Items->Strings[n], "");
            else
               formSpikeWare->m_smp.SetInSituInput(cbTriggerOut->Items->Strings[n], plv->Items->Item[n]->SubItems->Strings[3]);
            }
         if (plv->Items->Item[n]->Checked)
            {
            formSpikeWare->m_smp.m_swcHWChannels.SetChannelType(n, AS_SMP_OUTPUT, SWSMPHWCDIR_OUT);
            formSpikeWare->m_smp.m_swcHWChannels.SetOutputRaw((unsigned int)n, OutputIsRaw(plv->Items->Item[n]));
            }
         }

      for (n = 0; n < lvInput->Items->Count; n++)
         {
         if (lvInput->Items->Item[n]->Checked)
            formSpikeWare->m_smp.m_swcHWChannels.SetChannelType(n, AS_SMP_ELECTRODE, SWSMPHWCDIR_IN);
         }
      formSpikeWare->m_smp.m_swcHWChannels.SetTrigger(cbTriggerIn->ItemIndex, SWSMPHWCDIR_IN);
      formSpikeWare->m_smp.m_swcHWChannels.SetTrigger(cbTriggerOut->ItemIndex, SWSMPHWCDIR_OUT);
      formSpikeWare->m_smp.m_swcHWChannels.SetMonitor(cbMonitorOut->ItemIndex);
      formSpikeWare->m_smp.m_swcHWChannels.SetRefMic(cbMicIn->ItemIndex);

      formSpikeWare->m_smp.WriteSettings();
      // force re-reading of settings
      formSpikeWare->m_smp.ReadSettings(bShowError, true);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnCange callback for cbDrivers: sets driver and calls ReadChannels();
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::cbDriversChange(TObject *Sender)
{
   formSpikeWare->m_smp.SetDriver(cbDrivers->Text);
   ReadChannels();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads channels of selected driver
//------------------------------------------------------------------------------
void TformSettings::ReadChannels()
{
   TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;
   plv->OnItemChecked = NULL;
   lvInput->OnItemChecked = NULL;
   try
      {
      plv->Clear();
      lvInput->Clear();
      cbTriggerOut->Clear();
      cbTriggerIn->Clear();
      cbMonitorOut->Clear();

      if (cbDrivers->ItemIndex < 0)
         return;

      formSpikeWare->m_smp.GetChannels(cbDrivers->Text, cbTriggerIn->Items, cbTriggerOut->Items);

      cbMicIn->Items = cbTriggerIn->Items;

      cbMonitorOut->Items = cbTriggerOut->Items;

      int n;
      float f1, f2;
      UnicodeString usHiPass;
      for (n = 0; n < cbTriggerOut->Items->Count; n++)
         {
         TListItem* pli = plv->Items->Add();
         pli->SubItems->Add(cbTriggerOut->Items->Strings[n]);
         // channel index, filled elsewhere
         pli->SubItems->Add("");

         // get hipass
         formSpikeWare->m_swfFilters->GetHiPass(cbTriggerOut->Items->Strings[n], f1);
         if (f1 > 0.0f)
            usHiPass = DoubleToStr((double)f1);
         else
            usHiPass = "";

         if (plv == lvOutput)
            {
            // calibration and equalisation
            if (formSpikeWare->m_smp.m_swcHWChannels.IsOutputRaw((unsigned int)n))
               pli->SubItems->Add("raw output");
            else
               pli->SubItems->Add(formSpikeWare->m_smp.GetCalibrationValue(cbTriggerOut->Items->Strings[n]));
            if (formSpikeWare->m_smp.m_nEqualisationMethod == AW_SMP_EQ_IR)
               pli->SubItems->Add(formSpikeWare->m_smp.GetEqualisation(cbTriggerOut->Items->Strings[n]));
            else
               pli->SubItems->Add("(internal)");

            pli->SubItems->Add(usHiPass);
            }
         else
            {
            // Hipass: same as above
            pli->SubItems->Add(usHiPass);
            // input
            if (formSpikeWare->m_smp.m_swcHWChannels.IsOutputRaw((unsigned int)n))
               {
               pli->SubItems->Add("raw output");
               pli->SubItems->Add("");
               }
            else
               {
               pli->SubItems->Add(formSpikeWare->m_smp.GetInSituInput(cbTriggerOut->Items->Strings[n]));
               // is calibrated ?
               pli->SubItems->Add("");
               SetProbeMicCalStatus(pli);
               }
            }
         }
      for (n = 0; n < cbTriggerIn->Items->Count; n++)
         {
         TListItem* pli = lvInput->Items->Add();
         pli->SubItems->Add(cbTriggerIn->Items->Strings[n]);
         pli->SubItems->Add("");

         formSpikeWare->m_swfFilters->GetBandPass(cbTriggerIn->Items->Strings[n], f1, f2);
         if (f1 > 0.0f && f2 > 0.0f)
            pli->SubItems->Add(DoubleToStr((double)f1) + " - " + DoubleToStr((double)f2));
         else
            pli->SubItems->Add("");
         }
      AdjustInternalNames(lvInput);
      AdjustInternalNames(plv);
      }
   __finally
      {
      plv->OnItemChecked = lvOutputItemChecked;
      lvInput->OnItemChecked = lvInputItemChecked;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// checks if channel in passed TListItem is in raw mode
//------------------------------------------------------------------------------
bool TformSettings::OutputIsRaw(TListItem* pli)
{
   bool b = false;
   if (pli)
      {
      if (formSpikeWare->IsInSitu())
         b = pli->SubItems->Strings[3] == "raw output";
      else
         b = !IsDouble(pli->SubItems->Strings[2]);
      }
   return b;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates equalisation status of all outputs
//------------------------------------------------------------------------------
void TformSettings::ReadCalEq()
{
   if (formSpikeWare->IsInSitu())
      return;

   int n;
   for (n = 0; n < lvOutput->Items->Count; n++)
      {
      if (formSpikeWare->m_smp.m_swcHWChannels.IsOutputRaw((unsigned int)n))
         lvOutput->Items->Item[n]->SubItems->Strings[2] = "raw output";
      else
         lvOutput->Items->Item[n]->SubItems->Strings[2] = formSpikeWare->m_smp.GetCalibrationValue(cbTriggerOut->Items->Strings[n]);
      if (formSpikeWare->m_smp.m_nEqualisationMethod == AW_SMP_EQ_IR)
         lvOutput->Items->Item[n]->SubItems->Strings[3] = formSpikeWare->m_smp.GetEqualisation(cbTriggerOut->Items->Strings[n]);
      else
         lvOutput->Items->Item[n]->SubItems->Strings[3] = "(internal)";
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adjusts channel indices to internal indices
//------------------------------------------------------------------------------
void TformSettings::AdjustInternalNames(TListView* plv)
{
   int nChannel = 1;
   int n;
   for (n = 0; n < plv->Items->Count; n++)
      {
      if (plv->Items->Item[n]->Checked)
         plv->Items->Item[n]->SubItems->Strings[1] = IntToStr(nChannel++);
      else
         plv->Items->Item[n]->SubItems->Strings[1] = "";
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnItemChecked callback for output channel listview: checks constraints and
/// calls AdjustInternalNames
//------------------------------------------------------------------------------
void __fastcall TformSettings::lvOutputItemChecked(TObject *Sender, TListItem *Item)
{
   if (Item->Index == cbTriggerOut->ItemIndex || Item->Index == cbMonitorOut->ItemIndex)
      Item->Checked = false;
   AdjustInternalNames((TListView*)Sender);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// AdvancedCustomDrawItem callback for output channel listview: draws unselected
/// items gray and items with constraints red
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::lvOutputAdvancedCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, TCustomDrawStage Stage,
          bool &DefaultDraw)
{
   Sender->Canvas->Font->Color = Item->Checked ? clBlack : clGrayText;
   if (  Item->Index == cbTriggerOut->ItemIndex 
      || Item->Index == cbMonitorOut->ItemIndex
      )
      Sender->Canvas->Font->Color = (TColor)0x008080FF;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnItemChecked callback for input channel listview: checks constraints and
/// calls AdjustInternalNames
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::lvInputItemChecked(TObject *Sender, TListItem *Item)
{
   if (Item->Index == cbTriggerIn->ItemIndex || Item->Index == cbMicIn->ItemIndex)
      Item->Checked = false;
   AdjustInternalNames(lvInput);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// AdvancedCustomDrawItem callback for input channel listview: draws unselected
/// items gray and items with constraints red
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::lvInputAdvancedCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, TCustomDrawStage Stage,
          bool &DefaultDraw)
{
   Sender->Canvas->Font->Color = Item->Checked ? clBlack : clGrayText;
   if (Item->Index == cbTriggerIn->ItemIndex || Item->Index == cbMicIn->ItemIndex)
      Sender->Canvas->Font->Color = (TColor)0x008080FF;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback for special channel comboboxes. Updates corresponding 
/// listview concerning changed constraints
//------------------------------------------------------------------------------
void __fastcall TformSettings::cbChannelChange(TObject *Sender)
{
   TComboBox *pcb = dynamic_cast<TComboBox *>(Sender);
   if (!pcb || pcb->ItemIndex < 0)
      return;

   TListView* plv;
   if (pcb == cbTriggerIn)
      plv = lvInput;
   else
      plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;

   plv->Items->Item[pcb->ItemIndex]->Checked = false;
   plv->Invalidate();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of cbTriggerOut: calls cbChannelChange and checks own 
/// constraints
//------------------------------------------------------------------------------
void __fastcall TformSettings::cbTriggerOutChange(TObject *Sender)
{
   cbChannelChange(Sender);
   if (  cbTriggerOut->ItemIndex >= 0
      && cbTriggerOut->ItemIndex == cbMonitorOut->ItemIndex
      )
      cbMonitorOut->ItemIndex = -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of cbMonitorOut: calls cbChannelChange and checks own 
/// constraints
//------------------------------------------------------------------------------
void __fastcall TformSettings::cbMonitorOutChange(TObject *Sender)
{
   cbChannelChange(Sender);
   if (  cbMonitorOut->ItemIndex >= 0
      && cbTriggerOut->ItemIndex == cbMonitorOut->ItemIndex
      )
      cbTriggerOut->ItemIndex = -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnColumnClick callback for listvews: toggles select/unselect all
//------------------------------------------------------------------------------
void __fastcall TformSettings::lvColumnClick(TObject *Sender, TListColumn *Column)
{
   if (Column->Index > 0)
      return;
   TListView *plv = dynamic_cast<TListView *>(Sender);
   if (!plv)
      return;
   plv->Tag = !plv->Tag;
   int n;
   for (n = 0; n < plv->Items->Count; n++)
      plv->Items->Item[n]->Checked = plv->Tag;
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
   int nReturn = CheckSettings(true);
   if (nReturn != ID_CANCEL)
      {
      WriteSettings(nReturn == ID_IGNORE);
      
      if (Sender == btnOk)
         ModalResult = mrOk;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of sbtnNoMonitor: clears monitor channel selection
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::sbtnNoMonitorClick(TObject *Sender)
{
   cbMonitorOut->ItemIndex = -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of sbtnNoMicIn: clears MicIn channel selection
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::sbtnNoMicInClick(TObject *Sender)
{
   cbMicIn->ItemIndex = -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of sbtnTriggerTest: runs the trigger test
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::sbtnTriggerTestClick(TObject *Sender)
{
   if (ID_OK != CheckSettings())
      return;
   WriteSettings();
   formTriggerTest->TriggerTest(cbTriggerOut->Text, cbTriggerIn->Text);
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
/// OnChange callback for mtliple edit fields: sets Tag to 1 as indicator, that
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
/// Adjusts control properties depending on spectral/impulse response equalisation
//------------------------------------------------------------------------------
void TformSettings::AdjustEQControls()
{
   cobFFTLen->Enabled = rbSpectralEq->Checked;
   lblFFTLen->Enabled = rbSpectralEq->Checked;
   lvOutput->Columns->Items[4]->Caption = rbSpectralEq->Checked ? "Equalisation" : "Impulse Response";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of rbEq: handles selectio of spectral/impulse response 
/// equalisation
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::rbEqClick(TObject *Sender)
{
   gbEqualisation->Tag = 1;
   AdjustEQControls();
   WriteSettings();
   ReadCalEq();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnPopup callback for context menu mnuOutput: handles contraints (enabled, 
/// visible) fr menu items
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::mnuOutputPopup(TObject *Sender)
{
   bool bSelected = lvOutput->SelCount == 1 && lvOutput->Selected->Checked;
   miRawOutput->Enabled             = bSelected;
   if (bSelected)
      miRawOutput->Checked = formSpikeWare->m_smp.m_swcHWChannels.IsOutputRaw((unsigned int)lvOutput->Selected->Index);

   miCalibrate->Enabled             = bSelected && !miRawOutput->Checked;
   miRemoveCalibration->Enabled     = miCalibrate->Enabled;
   miSelectEqualisation->Enabled    = bSelected;
   miRemoveEqualisation->Enabled    = miSelectEqualisation->Enabled;
   miEditEqualisation->Enabled      = miSelectEqualisation->Enabled;
   miSetOutputHighPass->Enabled     = miSelectEqualisation->Enabled;
   miRemoveOutputHighPass->Enabled  = miSelectEqualisation->Enabled;

   bool bIR = formSpikeWare->m_smp.m_nEqualisationMethod == AW_SMP_EQ_IR;
   miSelectEqualisation->Visible    = bIR;
   miRemoveEqualisation->Visible    = bIR;
   miEditEqualisation->Visible      = !bIR;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnPopup callback for context menu mnuInput: handles contraints (enabled, 
/// visible) fr menu items
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::mnuInputPopup(TObject *Sender)
{
   miSetInputBandPass->Enabled     = lvInput->SelCount == 1;
   miRemoveInputBandPass->Enabled  = miSetInputBandPass->Enabled;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'calibrate' context menu item: runs calibration
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miCalibrateClick(TObject *Sender)
{
   int n = lvOutput->ItemIndex;
   if (n > -1)
      {
      WriteSettings();
      if (formSpikeWare->m_smp.InitCalibration(n, cbUseCalibrator->Checked))
         {
         if (cbUseCalibrator->Checked)
            {
            if (formCalibrationCalibrator->Calibrate() == mrOk)
               {
               formSpikeWare->m_smp.SetCalibrationValue((unsigned int)n, formCalibrationCalibrator->m_dCalValue);
               ReadSettings();
               }
            }
         else
            {
            if (formCalibration->Calibrate() == mrOk)
               {
               formSpikeWare->m_smp.SetCalibrationValue((unsigned int)n, formCalibration->m_dCalValue);
               ReadSettings();
               }
            }
         }
      ScrollOutputs(n);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'remove calibration' context menu item: clears a 
/// calibration value
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miRemoveCalibrationClick(TObject *Sender)
{
   int n = lvOutput->ItemIndex;
   if (n > -1)
      {
      formSpikeWare->m_smp.SetCalibrationValue((unsigned int)n, 0.0);
      ReadSettings();
      ScrollOutputs(n);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'raw output' context menu item: toggles raw mode of a 
/// channel
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miRawOutputClick(TObject *Sender)
{
   int n = lvOutput->ItemIndex;
   if (n > -1)
      {
      bool b = !formSpikeWare->m_smp.m_swcHWChannels.IsOutputRaw((unsigned int)n);
      miRawOutput->Checked = b;
      formSpikeWare->m_smp.m_swcHWChannels.SetOutputRaw((unsigned int)n, b);
      ReadCalEq();
      // write settings without showing error
      WriteSettings(false, false);
      ReadSettings();
      ScrollOutputs(n);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'select equalisation' context menu item: shows dialog to 
/// select an equalisation
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miSelectEqualisationClick(TObject *Sender)
{
   int n = lvOutput->ItemIndex;
   if (n > -1)
      {
      formFileSel->flb->Directory = IncludeTrailingBackslash(ExtractFilePath(Application->ExeName) + "..\\equalisations");
      formFileSel->m_dSampleRate = StrToDouble(edSampleRate->Text);
      if (formFileSel->ShowModal() == mrOk)
         {
         if (formFileSel->flb->ItemIndex > -1)
            formSpikeWare->m_smp.SetEqualisation((unsigned int)n, formFileSel->flb->Items->Strings[formFileSel->flb->ItemIndex]);
         ReadSettings();
         }
      ScrollOutputs(n);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'remove equalistion' context menu item: removes a selected
/// equalisation
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miRemoveEqualisationClick(TObject *Sender)
{
   int n = lvOutput->ItemIndex;
   if (n > -1)
      {
      formSpikeWare->m_smp.SetEqualisation((unsigned int)n, "");
      ReadSettings();
      ScrollOutputs(n);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'edit equalisation' context menu item: shows editor to
/// edit an equalisation filter
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miEditEqualisationClick(TObject *Sender)
{
   int n = lvOutput->ItemIndex;
   WriteSettings();
   if (n > -1)
      {
      if (mrOk == formFFTEdit->Calibrate(n, CAL_MODE_SPEAKER))
         MessageBox(Handle, "Be sure to re-run the calibration if you have changed the equalisation!", "Warning", MB_ICONWARNING);
      ReadSettings();
      ScrollOutputs(n);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback to cbSamplerateDevider. Sets Tag to 1 to indicate that
/// it was changed
///-----------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::cbSamplerateDeviderChange(TObject *Sender)
{
   cbSamplerateDevider->Tag = 1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// scrolls passed item in output istview into view
//------------------------------------------------------------------------------
void TformSettings::ScrollOutputs(int n)
{
   TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;
   if (n > -1 && n < plv->Items->Count)
      {
      plv->ItemIndex = n;
      plv->Items->Item[n]->MakeVisible(false);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// set a hi-pass for an output
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miSetOutputHighPassClick(TObject *Sender)
{
   TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;
   if (plv->SelCount != 1)
      return;

   float f;
   formSpikeWare->m_swfFilters->GetHiPass(plv->Selected->SubItems->Strings[0], f);

   double dMaxFreq = (double)formSpikeWare->m_swsStimuli.m_dDeviceSampleRate;
   if (dMaxFreq == 0.0)
      dMaxFreq = (double)formSpikeWare->m_smp.m_fDefaultSampleRate;
   dMaxFreq /= 2.0;

   while (1)
      {
      // break if cancel pressed
      if (!formSpikeWare->m_pformSetParameters->SetParameter("Frequency", "Hz", f, this))
         return;
      if (f < 0.0f || f >= (float)dMaxFreq)
         {
         formSpikeWare->SWErrorBox("Please enter a frequency > 0 and below half of samplerate", Handle);
         continue;
         }
      break;
      }

   formSpikeWare->m_swfFilters->SaveHiPass(plv->Selected->SubItems->Strings[0], f);

   plv->Selected->SubItems->Strings[4] = (int)floor(f);

   MessageBox(Handle, "Be sure to re-run the equalisation and calibration if you have changed the Hi-Pass!", "Warning", MB_ICONWARNING);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// remove a hi-pass for an output
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miRemoveOutputHighPassClick(TObject *Sender)
{
   TListView* plv = formSpikeWare->IsInSitu() ? lvInSitu : lvOutput;
   if (plv->SelCount != 1)
      return;

   formSpikeWare->m_swfFilters->RemoveHiPass(plv->Selected->SubItems->Strings[0]);
   plv->Selected->SubItems->Strings[4] = "";

   // show message only if invoked by control (not, if called in code with argument NULL)
   if (Sender != NULL)
      MessageBox(Handle, "Be sure to re-run the equalisation and calibration if you have changed the Hi-Pass!", "Warning", MB_ICONWARNING);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// sets a band-pass for an input
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miSetInputBandPassClick(TObject *Sender)
{
   if (lvInput->SelCount != 1)
      return;


   float fLower, fUpper;
   formSpikeWare->m_swfFilters->GetBandPass(lvInput->Selected->SubItems->Strings[0], fLower, fUpper);

   double dMaxFreq = formSpikeWare->m_swsStimuli.m_dDeviceSampleRate;
   if (dMaxFreq == 0.0)
      dMaxFreq = (double)formSpikeWare->m_smp.m_fDefaultSampleRate;
   dMaxFreq /= 2.0;

   while (1)
      {
      // break if cancel pressed
      if (!formSpikeWare->m_pformSetParameters->SetParameters( "Lower frequency", "Hz", fLower,
                                                               "Upper frequency", "Hz", fUpper,
                                                               this
                                                             )
         )
         return;
      if (  fLower < 0.0f || fLower >= (float)dMaxFreq
         || fUpper < 0.0f || fUpper >= (float)dMaxFreq
         || fLower >= fUpper
         )
         {
         formSpikeWare->SWErrorBox("Please enter a frequencies > 0 and below half of samplerate (lower < upper)!", Handle);
         continue;
         }
      break;
      }

   formSpikeWare->m_swfFilters->SaveBandPass(lvInput->Selected->SubItems->Strings[0], fLower, fUpper, (float)dMaxFreq);

   lvInput->Selected->SubItems->Strings[2] = IntToStr((int)floor(fLower)) + " - " + IntToStr((int)floor(fUpper));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// remove a band-pass for an input
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miRemoveInputBandPassClick(TObject *Sender)
{
   if (lvInput->SelCount != 1)
      return;

   formSpikeWare->m_swfFilters->RemoveBandPass(lvInput->Selected->SubItems->Strings[0]);
   lvInput->Selected->SubItems->Strings[2] = "";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnPopup callback for context menu mnuInSitu: handles contraints (enabled, 
/// visible) fr menu items
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::mnuInSituPopup(TObject *Sender)
{
   bool bSelected = lvInSitu->SelCount == 1 && lvInSitu->Selected->Checked;

   miRawOutputInSitu->Enabled          = bSelected;
   miRawOutputInSitu->Checked          = OutputIsRaw(lvInSitu->Selected);


   miSelectInputInSitu->Enabled        = bSelected && !miRawOutputInSitu->Checked;
   miRemoveInputInSitu->Enabled        = miSelectInputInSitu->Enabled;
   miInSituCalibrate->Enabled          = miSelectInputInSitu->Enabled && lvInSitu->Selected->SubItems->Strings[3] != "";
   miInSituRemoveCalibration->Enabled  = miSelectInputInSitu->Enabled;
   miInSituSetHiPass->Enabled          = miSelectInputInSitu->Enabled;
   miInSituRemoveHiPass->Enabled       = miSelectInputInSitu->Enabled;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'select in-situ input' context menu item: shows dialog
/// to select a channel
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miSelectInputInSituClick(TObject *Sender)
{
   TListItem* pli = lvInSitu->Selected;
   if (!pli)
      return;
   int nIndex = pli->Index;
   TformSelectChannel* pfrm = NULL;
   try
      {
      pfrm = new TformSelectChannel(this);
      int n;
      for (n = 0; n < cbTriggerIn->Items->Count; n++)
         pfrm->lb->Items->Add(cbTriggerIn->Items->Strings[n]);
      if (mrOk == pfrm->ShowModal() && pfrm->lb->ItemIndex > -1)
         {
         pli->SubItems->Strings[3] = pfrm->lb->Items->Strings[pfrm->lb->ItemIndex];
         // write settings without showing error
         WriteSettings(false, false);
         ReadSettings();
         }

      ScrollOutputs(nIndex);
      }
   __finally
      {
      TRYDELETENULL(pfrm);
      Tag = 0;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'remove in-situ input' context menu item: clears input
/// channel selection for an in-situ channel
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miRemoveInputInSituClick(TObject *Sender)
{
   TListItem* pli = lvInSitu->Selected;
   if (!pli)
      return;
   int nIndex = pli->Index;
   pli->SubItems->Strings[3] = "";
   // write settings without showing error
   WriteSettings(false, false);
   ReadSettings();

   ScrollOutputs(nIndex);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'set in-situ output raw' context menu item: sets mode
/// of an in-situ output channel to 'raw'
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miRawOutputInSituClick(TObject *Sender)
{
   int n = lvInSitu->ItemIndex;
   if (n > -1)
      {
      miRawOutputInSitu->Checked = !miRawOutputInSitu->Checked;
      formSpikeWare->m_smp.m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][(unsigned int)n].m_bRawOutput = miRawOutputInSitu->Checked;

      if (miRawOutputInSitu->Checked)
         lvInSitu->Selected->SubItems->Strings[3] = "raw output";
      else
         {
         lvInSitu->Selected->SubItems->Strings[3] = "";
         }
      // write settings without showing error
      WriteSettings(false, false);
      ReadSettings();


      ScrollOutputs(n);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'Calibrate' context menu item: runs in-situ calibration
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miInSituCalibrateClick(TObject *Sender)
{
   TListItem *pli = lvInSitu->Selected;
   if (pli)
      {
      int nIndex = pli->Index;
      WriteSettings();
      if (mrOk == formFFTEdit->Calibrate(pli->Index, CAL_MODE_MIC_REF))
         ReadSettings();
      ScrollOutputs(nIndex);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for 'Calibrate' context menu item: removes in-situ calibration
/// value
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSettings::miInSituRemoveCalibrationClick(TObject *Sender)
{
   TListItem *pli = lvInSitu->Selected;
   if (pli)
      {
      int nIndex = pli->Index;
      UnicodeString us = formSpikeWare->m_smp.GetInSituInputFilterSection(pli->SubItems->Strings[0]);
      formSpikeWare->m_swfFilters->m_pFilterIni->EraseSection(us);
      SetProbeMicCalStatus(pli);
      ScrollOutputs(nIndex);
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// writes calibration status of probe microphone  for an item to listview
//------------------------------------------------------------------------------
void TformSettings::SetProbeMicCalStatus(TListItem *pli)
{
   UnicodeString us = formSpikeWare->m_smp.GetInSituInputFilterSection(pli->SubItems->Strings[0]);

   if (us.IsEmpty() || !formSpikeWare->m_swfFilters->m_pFilterIni->SectionExists(us))
      pli->SubItems->Strings[4] = "";
   else
      pli->SubItems->Strings[4] = "1";
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
//
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

