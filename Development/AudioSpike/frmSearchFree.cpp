//------------------------------------------------------------------------------
/// \file frmFreeSearch.cpp
///
/// \author Berg
/// \brief Implementation of a form for free search, where stimulus parameters
/// can be selected in a 2d-plot
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
#include <algorithm>
#pragma hdrstop

#include "frmSearchFree.h"
#include "SpikeWareMain.h"
#include "frmSelectChannels.h"
#include "frmEpocheWindow.h"
#include "frmSpikes.h"
#include "frmWait.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor, initializes members
//------------------------------------------------------------------------------
__fastcall TformSearchFree::TformSearchFree(TComponent* Owner)
   : TformASUI(Owner), m_pfrmSelectChannels(NULL)
{
   m_fGain = 50;
   m_fFrequency = 1000;
   cs->AddXY((double)m_fFrequency, (double)m_fGain, "", clRed);
   m_bSchroederCached = false;
   m_nClearCounter = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, saves settings
//------------------------------------------------------------------------------
__fastcall TformSearchFree::~TformSearchFree()
{
   formSpikeWare->StoreChartAxis(this, chrt, true, true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates channel selection dialog, sets some default values and shows form
//------------------------------------------------------------------------------
void __fastcall TformSearchFree::ShowFreeSearch()
{
   TRYDELETENULL(m_pfrmSelectChannels);
   m_pfrmSelectChannels = new TformSelectChannels(NULL);
   try
      {
      int n;
      int nNumOutputs = (int)formSpikeWare->m_smp.m_swcUsedChannels.GetOutputs().size();
      #ifdef CHKCHNLS
      if (nNumOutputs != (int)formSpikeWare->m_smp.m_viChannelsOutSettings.size())
         ShowMessage("error B " + UnicodeString(__FUNC__));
      #endif
      for (n = 0; n < nNumOutputs; n++)
         {
         m_pfrmSelectChannels->clb->Items->Add("Channel " + IntToStr((int)n+1));
         m_pfrmSelectChannels->clb->Checked[n] = true;
         }
      }
   catch(...)
      {
      TRYDELETENULL(m_pfrmSelectChannels);
      throw;
      }

   formSpikeWare->RestoreChartAxis(this, chrt, true, true);
   chrt->BottomAxis->SetMinMax(100, formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/2.0);
   formSpikeWare->SetGUIStatus(SWGS_FREESEARCHSTOP);
   btnStop->Enabled     =  false;
   btnStart->Enabled    =  true;
   ClipTimer->Tag       = 0;
   ClipTimer->Enabled   = true;
   formSpikeWare->SetWindowVisible(this, true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnCloseQuery callback: tells user to stop free search beforeclosing window
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::FormCloseQuery(TObject *Sender, bool &CanClose)
{
   if (formSpikeWare->m_smp.Playing())
      {
      CanClose = false;
      formSpikeWare->SWErrorBox("Please stop the free search before closing the window", Handle);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClose callback: does cleanup
//------------------------------------------------------------------------------
void __fastcall TformSearchFree::FormClose(TObject *Sender, TCloseAction &Action)
{
   ClipTimer->Enabled = false;
   formSpikeWare->m_smp.Stop();
   formSpikeWare->m_smp.Exit();
   formSpikeWare->m_bFreeSearchRunning = false;
   formSpikeWare->Cleanup();
   formSpikeWare->StoreChartAxis(this, chrt, true, true);
   TformASUI::FormClose(Sender, Action);
   TRYDELETENULL(m_pfrmSelectChannels);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseDown callback of chart: writes values to members and calls SetValues
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
   if (  X <= chrt->ChartRect.Left || X >= chrt->ChartRect.Right
      || Y <= chrt->ChartRect.Top || Y >= chrt->ChartRect.Bottom
      )
      return;
   chrt->Enabled = false;
   try
      {
      if (tbtnSine->Down)
         {
         double dVal = cs->XScreenToValue(X);
         if (dVal < 1.0)
            dVal = 1.0;
         if (dVal > formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/2.0)
            dVal = formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/2.0;
         cs->XValues->Value[0] = dVal;
         }
      else
         cs->XValues->Value[0] = cs->XScreenToValue(chrt->ChartRect.Left + chrt->ChartRect.Width()/2);
      cs->YValues->Value[0] = cs->YScreenToValue(Y);
      m_fFrequency   = (float)cs->XValues->Value[0];
      m_fGain        = (float)cs->YValues->Value[0];
      SetValues();
      }
   __finally
      {
      chrt->Enabled = TRUE;
      cs->Repaint();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets properties of current stimulus
//------------------------------------------------------------------------------
bool TformSearchFree::SetValues()
{
   formSpikeWare->SetGUIBusy(true, this);
   try
      {
      ClearData();

      // for the free search we assume that the signal has the RMS of the pure, unmodulated
      // signal, i.e. 1/sqrt(2) for the sine and the RMS of the schroeder tone comples else
      // build arguments for 'volume'
      double dRMS = tbtnSine->Down ? FactorTodB(1.0/sqrt(2.0)) : (double)formSpikeWare->m_smp.m_fSchroederRMSdB;
      UnicodeString usOutput  = "channel=";
      UnicodeString usValue   = "value=";
      unsigned int n;
      double dGain, dCal;


      #ifdef CHKCHNLS
      // determine total number of outputs: all channels + trigger channel
      unsigned int nNum = (unsigned int)formSpikeWare->m_smp.m_swcUsedChannels.GetOutputs().size() + 1;
      if (formSpikeWare->m_smp.m_nMonitorChannelOutIndex >= 0)
         nNum++;

      unsigned int nNum2 = (unsigned int)formSpikeWare->m_smp.m_viChannelsOutSettings.size() + 1;
      if (formSpikeWare->m_smp.m_nMonitorChannelOut >= 0)
         nNum2++;
      if (formSpikeWare->m_smp.m_swcUsedChannels.GetOutputs().size() != formSpikeWare->m_smp.m_viChannelsOutSettings.size())
         ShowMessage("error A " + UnicodeString(__FUNC__));
      if (nNum != nNum2)
         ShowMessage("error A " + UnicodeString(__FUNC__));
      #endif

      // loop through all current outputs
      for (n = 0; n < formSpikeWare->m_smp.m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_OUT].size(); n++)
         {
         if (!formSpikeWare->m_smp.m_swcUsedChannels.IsOutput(n))
            continue;
         // for raw outputs the gain is ALWAYS 1.0
         if (formSpikeWare->m_smp.m_swcUsedChannels.IsOutputRaw(n))
            dGain = 1.0;
         else
            {
            dCal = formSpikeWare->m_smp.GetCalibrationValue(formSpikeWare->m_smp.m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][n].m_usName);
            if (dCal == 0.0)
               {
               formSpikeWare->m_smp.Stop();
               formSpikeWare->SWErrorBox("One or more calibration values missing (see settings)", Handle);
               return false;
               }
            dGain    = dBToFactor((double)m_fGain - dCal - dRMS);
            }

         usValue += DoubleToStr(dGain) + ",";
         usOutput+= IntToStr((int)n) + ",";
         }
      RemoveTrailingDelimiter(usOutput);
      RemoveTrailingDelimiter(usValue);


      if (!formSpikeWare->m_smp.Command("volume", usOutput + ";" + usValue))
         {
         formSpikeWare->m_smp.Stop();
         formSpikeWare->SWErrorBox("error setting gain", Handle);
         return false;
         }

      if (tbtnSine->Down)
         {
         formSpikeWare->m_smp.m_fSineFreqPending = m_fFrequency;
         chrt->Title->Text->Text = FormatFloat("0.0", (Extended)m_fFrequency) + " Hz, " + FormatFloat("0.0", (Extended)m_fGain) + " dB";
         }
      else
         {
         formSpikeWare->m_smp.m_fSineFreqPending = 0.0f;
         chrt->Title->Text->Text = "Noise, " + FormatFloat("0.0", (Extended)m_fGain) + " dB";
         }
      }
   __finally
      {
      formSpikeWare->SetGUIBusy(false, this);
      btnStart->Enabled    =  formSpikeWare->m_gs == SWGS_FREESEARCHSTOP;
      btnStop->Enabled    =  formSpikeWare->m_gs == SWGS_FREESEARCHRUN;
      }
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears spike and epoche data
//------------------------------------------------------------------------------
void TformSearchFree::ClearData()
{
   m_nClearCounter = 0;
   formSpikeWare->m_swsSpikes.Clear();
   formSpikeWare->m_sweEpoches.Clear();
   if (formSpikeWare->FormsCreated())
      formSpikeWare->m_pformSpikes->Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnChannels. Shows channel selection dialog and applies
/// selectionby calling ApplyMute
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::tbtnChannelsClick(TObject *Sender)
{
   try
      {
      if (m_pfrmSelectChannels->ShowModal() == mrOk)
         ApplyMute();
      }
   catch (Exception &e)
      {
      formSpikeWare->m_smp.Stop();
      formSpikeWare->SWErrorBox(e.Message, Handle);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// mutes/unmutes channels depending on channel selection
//------------------------------------------------------------------------------
void TformSearchFree::ApplyMute()
{
   UnicodeString us;
   unsigned int n;
   int nOut = 0;
   for (n = 0; n < formSpikeWare->m_smp.m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_OUT].size(); n++)
      {
      if (formSpikeWare->m_smp.m_swcUsedChannels.IsOutput(n))
         us += IntToStr((int)!m_pfrmSelectChannels->clb->Checked[nOut++]) + ",";
      else
         us += "0,";
      }


   RemoveTrailingDelimiter(us);
   if (!formSpikeWare->m_smp.Command("channelmute", "value=" + us))
      throw Exception("error setting mute status");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnSignal. Toggles signal type (sine or schroeder tone 
/// complex)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::tbtnSignalClick(TObject *Sender)
{
   if (tbtnSine->Down)
      {
      chrt->BottomAxis->Title->Text = "Frequency [Hz]";
      cs->XValues->Value[0] = (double)m_fFrequency;
      }
   else
      {
      CreateFreeSearchSchroederPhaseToneComplex();
      chrt->BottomAxis->Title->Text = "Noise";
      cs->XValues->Value[0] = cs->XScreenToValue(chrt->ChartRect.Left + chrt->ChartRect.Width()/2);
      }
   m_fFrequency   = (float)cs->XValues->Value[0];
   cs->Repaint();
   SetValues();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnLog. Toggles bottom axis lin/log
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::tbtnLogClick(TObject *Sender)
{
   chrt->BottomAxis->Logarithmic = tbtnLog->Down;
   Application->ProcessMessages();
   if (!tbtnSine->Down)
      {
      cs->XValues->Value[0] = cs->XScreenToValue(chrt->ChartRect.Left + chrt->ChartRect.Width()/2);
      cs->Repaint();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if (Axis == chrt->BottomAxis)
      formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis,
                                                         1,
                                                         formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/2,
                                                         this
                                                         );
   else if (Axis == chrt->LeftAxis)
      formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis, 0, 110, this);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnGetAxisLabel callback for chart: clears bottom labels, if noise is used
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::chrtGetAxisLabel(TChartAxis *Sender, TChartSeries *Series,
          int ValueIndex, UnicodeString &LabelText)
{
   if (Sender == chrt->BottomAxis && !tbtnSine->Down)
      LabelText = "";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// timer callback for ClipTimer: updates clip indicator
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::ClipTimerTimer(TObject *Sender)
{
   if (formSpikeWare->m_smp.Playing())
      m_nClearCounter++;

   // clear every 10 seconds
   if (m_nClearCounter == 100)
      ClearData();

   shClip->Brush->Color = ClipTimer->Tag ? clRed : clLime;
   ClipTimer->Tag = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnStart. Starts the free search
//------------------------------------------------------------------------------
#pragma argsused
#pragma argsused
void __fastcall TformSearchFree::btnStartClick(TObject *Sender)
{
      if (!SetValues())
         return;
      formSpikeWare->m_sweEpoches.Clear();
      formSpikeWare->m_SearchModeStimIndex = 0;
      if (!formSpikeWare->m_smp.Prepare(-2)) // -2 is magic argument for 'free search'
         return;
      ApplyMute();
      btnStart->Enabled   =  false;
      formSpikeWare->m_smp.Start();
      formSpikeWare->EnableEpocheTimer(true);
      formSpikeWare->SetGUIStatus(SWGS_FREESEARCHRUN);
      btnStop->Enabled    =  true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnStop. Stops the free search
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSearchFree::btnStopClick(TObject *Sender)
{
   btnStop->Enabled   =  false;
   formSpikeWare->m_smp.Stop();
   formSpikeWare->SetGUIStatus(SWGS_FREESEARCHSTOP);
   btnStart->Enabled  =  true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Sets current schroeder phase tone complex properties and resets m_bSchroederCached
/// member, if anything has changed
//------------------------------------------------------------------------------
#pragma argsused
void TformSearchFree::SetSchroederPhaseToneComplex(float fLowFreq,
                                                   float fHiFreq,
                                                   bool bRandom,
                                                   int nSamples)
{
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wfloat-equal"
   if (  (int)m_vfSchroeder.size() == nSamples
      && fLowFreq == m_fSchroederLowFreq
      && fHiFreq == m_fSchroederHiFreq
      )
      return;
   #pragma clang diagnostic push
   m_vfSchroeder.resize((unsigned int)nSamples);
   m_fSchroederLowFreq  = fLowFreq;
   m_fSchroederHiFreq   = fHiFreq;
   m_bSchroederCached   = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Creates schroeder phase tone complex including chaching to file
//------------------------------------------------------------------------------
void  TformSearchFree::CreateFreeSearchSchroederPhaseToneComplex()
{
   // use memory cache ?
   if (m_bSchroederCached)
      {
      formSpikeWare->m_smp.m_vfSchroeder = m_vfSchroeder;
      formSpikeWare->m_smp.m_fSchroederRMSdB = m_fSchroederRMSdB;
      }
   else
      {
      // try to read it from disk-cache
      UnicodeString usFileName;
      usFileName.printf(L"%ls\\..\\cache\\%f_%f_%f_%d.pcm",
                        ExcludeTrailingBackslash(ExtractFilePath(Application->ExeName)).w_str(),
                        (double)m_fSchroederLowFreq, (double)m_fSchroederHiFreq, 
                        formSpikeWare->m_swsStimuli.m_dDeviceSampleRate, (int)m_vfSchroeder.size());

      if (FileExists(usFileName))
         {
         TMemoryStream* pms = new TMemoryStream();
         try
            {
            pms->LoadFromFile(usFileName);
            m_vfSchroeder.resize((unsigned int)pms->Size / sizeof(float));
            CopyMemory(&m_vfSchroeder[0], pms->Memory, pms->Size);
            m_fSchroederRMSdB = RMS(m_vfSchroeder);
            formSpikeWare->m_smp.m_vfSchroeder = m_vfSchroeder;
            formSpikeWare->m_smp.m_fSchroederRMSdB = m_fSchroederRMSdB;
            m_bSchroederCached   = true;
            TRYDELETENULL(pms);
            return;
            }
         catch (...)
            {
            TRYDELETENULL(pms);
            formSpikeWare->SWErrorBox("Invalid Schroeder phase cache file detetcted. Removed...", Handle);
            DeleteFile(usFileName);
            /// go on by purpose with creating it newly!
            }
         }


      formWait->ShowWait("Creating Schroeder phase tone complex, this may take a while ...");
      try
         {
         formSpikeWare->m_smp.CreateSchroederPhaseToneComplex(m_fSchroederLowFreq, m_fSchroederHiFreq, true, (int)m_vfSchroeder.size());
         }
      __finally
         {
         formWait->Hide();
         }
      m_vfSchroeder        = formSpikeWare->m_smp.m_vfSchroeder;
      m_fSchroederRMSdB    = formSpikeWare->m_smp.m_fSchroederRMSdB;
      m_bSchroederCached   = true;

      // store it in cache
      ForceDirectories(ExtractFilePath(usFileName));
      TMemoryStream* pms = new TMemoryStream();
      try
         {
         pms->Size = (NativeInt)(sizeof(float) * m_vfSchroeder.size());
         CopyMemory(pms->Memory, &m_vfSchroeder[0], pms->Size);
         pms->SaveToFile(usFileName);
         }
      __finally
         {
         TRYDELETENULL(pms);
         }
      }

}
//------------------------------------------------------------------------------



