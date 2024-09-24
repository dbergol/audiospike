//------------------------------------------------------------------------------
/// \file frmCalibration.cpp
///
/// \author Berg
/// \brief Implementation of a form for performing th calibration of a calibrator
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

#include "frmCalibrationCalibrator.h"
#include "SpikeWareMain.h"
#include "frmWait.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TformCalibrationCalibrator *formCalibrationCalibrator;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor, initializes members
//------------------------------------------------------------------------------
__fastcall TformCalibrationCalibrator::TformCalibrationCalibrator(TComponent* Owner)
   : TForm(Owner), m_pfrmWait(NULL)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, does cleanup
//------------------------------------------------------------------------------
__fastcall TformCalibrationCalibrator::~TformCalibrationCalibrator()
{
   TRYDELETENULL(m_pfrmWait);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// runs calibration procedure for calibrator
//------------------------------------------------------------------------------
TModalResult TformCalibrationCalibrator::Calibrate(TForm* pfrm, bool bExitOnClose)
{
   // mute output, only record now
   m_dLevel = -100.0;
   formSpikeWare->m_smp.Command("volume", "value=0");

   TRYDELETENULL(m_pfrmWait);
   m_pfrmWait = new TformWait(this);


   lblDescription->Caption = "If needed change the calibration level used for sound output "
      "using the speaker to be calibrated. Then "
      "attach your calibrator to the reference microphone and check that no clipping occurs. "
      "Afterwards click 'Start'";

   btnEnterLevel->Enabled = true;
   lbDebug->Visible = false;
   btnOk->Tag = 0;

   int nCalibratorAvgSeconds = formSpikeWare->m_pIni->ReadInteger("Settings", "CalibratorAvgSeconds", 3);
   m_vafRecBufferLong.resize((unsigned int)(nCalibratorAvgSeconds*formSpikeWare->m_swsStimuli.m_dDeviceSampleRate));


   m_nRecBufPosLong = 0;
   m_bGo = false;
   m_bRecDone = false;
   m_bAutoReturn = !bExitOnClose;
   m_dCalibratorLevel = IniReadDouble(formSpikeWare->m_pIni, "Settings", "CalibratorLevel", 94);
   m_dCalibratorRMS = 0.0;

   nCalibratorAvgSeconds = formSpikeWare->m_pIni->ReadInteger("Settings", "CalibratorAvgSeconds2", 1);
   m_vafRecBuffer.resize((unsigned int)(nCalibratorAvgSeconds*formSpikeWare->m_swsStimuli.m_dDeviceSampleRate));
   m_vafCalcBuffer.resize(m_vafRecBuffer.size());
   m_vafRecBuffer = 0.0f;
   m_vafCalcBuffer = 0.0f;


   TModalResult mr = mrCancel;

   try
      {
      m_dCalibrationLevel = 80.0;
      mr = ShowModalCenter(pfrm);
      }
   __finally
      {
      if (bExitOnClose || mr != mrOk)
         formSpikeWare->m_smp.Exit();
      }
   m_vafRecBufferLong.resize(0);
   return mr;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows form centered and modal
//------------------------------------------------------------------------------
TModalResult TformCalibrationCalibrator::ShowModalCenter(TForm* pfrm)
{
   if (!pfrm)
      Position = poMainFormCenter;
   else
      {
      Position = poDesigned;
      Top = pfrm->Top + (pfrm->Height - Height)/2;
      Left = pfrm->Left + (pfrm->Width - Width)/2;
      }
   return ShowModal();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback: enables clip timer
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibrationCalibrator::FormShow(TObject *Sender)
{
   ClipTimer->Tag = 0;
   ClipTimer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClose callback. disables clip timer
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibrationCalibrator::FormClose(TObject *Sender, TCloseAction &Action)

{
   ClipTimer->Enabled = false;
   m_bGo = false;
   m_bRecDone = false;
   TRYDELETENULL(m_pfrmWait);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clip timer: indicates if clipping occurred or not. SMP has written clip status
/// to tag of timer!
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibrationCalibrator::ClipTimerTimer(TObject *Sender)
{
   shClip->Brush->Color = ClipTimer->Tag ? clRed : clLime;
   ClipTimer->Tag = 0;

   #ifdef NEWCAL
   static int x = 0;
   if ((x++%10) == 0 && m_vafCalcBuffer.size())
      {
      // create a copy of the current buffer
      m_vafCalcBuffer = m_vafRecBuffer;
      double dRMS = RMS(m_vafCalcBuffer);
      double dLevel;
      // if CalibratorRMS not set yet show pure dB RMS fullscale
      if (m_dCalibratorRMS == 0.0)
         dLevel = dRMS;
      // otherwise the level
      else
         dLevel = m_dCalibratorLevel - (m_dCalibratorRMS - dRMS);
      OutputDebugStringW(FloatToStr(dLevel).w_str());
      }
   #endif
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of OK button. Performs procedure step by step, current step
/// is stored in Tag of the button.
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibrationCalibrator::btnOkClick(TObject *Sender)
{
   // NOTE: Tag of button holds the 'step'
   if (btnOk->Tag == 0)
      {
      btnEnterLevel->Enabled = false;
      if (!GetLevel("Please enter the output level of your calibrator", m_dCalibratorLevel, m_dCalibratorLevel))
         return;
      formSpikeWare->m_pIni->WriteString("Settings", "CalibratorLevel", DoubleToStr(m_dCalibratorLevel));
      ShowWait("Evaluating calibrator level, please wait ...");
      // calculate RMS of recorded buffer
      m_dCalibratorRMS = (double)RMS(m_vafRecBufferLong);
      if (formSpikeWare->m_pIni->ReadBool("Debug", "WriteCalibratorRMS", false))
         {
         if (ID_YES == MessageBox(0, "Write RMS?", "Question", MB_YESNO | MB_ICONQUESTION))
            formSpikeWare->m_pIni->WriteString("Debug", "CalibratorRMS", DoubleToStr(m_dCalibratorRMS));
         }
      // for debugging/testing without calibrator: read this RMS from Ini
      double dDebugRMS = IniReadDouble(formSpikeWare->m_pIni, "Debug", "CalibratorRMS", 1);
      if (dDebugRMS < 0.0)
         {
         m_dCalibratorRMS = dDebugRMS;
         lbDebug->Caption = "DEBUG-RMS";
         lbDebug->Visible = true;
         }
      btnOk->Tag = 1;
      btnOk->Caption = "Proceed";
      lblDescription->Caption = "Now mount the reference microphone to the speaker and click 'Proceed'";
      }
   else if (btnOk->Tag == 1)
      {

      // calculate volume 'expected' to result in m_dCalibrationLevel
      double dTargetRMS = m_dCalibratorRMS - (m_dCalibratorLevel - m_dCalibrationLevel);

      // start with 60 dB attenuation
      m_dLevel = -60.0;

      // do a loop to try to 'hit' m_dCalibrationLevel
      double dRMS, dDelta;
      int n;
      bool bSuccess = false;
      try
         {
         try
            {
            for (n = 0; n < 5; n++)
               {
               formSpikeWare->m_smp.Command("volume", "value=" + DoubleToStr(dBToFactor(m_dLevel)));
               m_nRecBufPosLong = 0;
               m_bRecDone = false;
               m_bGo = true;
               ShowWait("Calibrating, please wait ...", false);
               dRMS = (double)RMS(m_vafRecBufferLong);
               dDelta = dTargetRMS - dRMS;
               if (fabs(dDelta) < 1.0)
                  {
                  bSuccess = true;
                  }

               m_dLevel += dDelta;
               if (m_dLevel > 1)
                  throw Exception("Requested calibration level above available level range");

               }
            if (!bSuccess)
               throw Exception("Unexpected error: requested calibration level not reached");
            }
         __finally
            {
            Enabled = true;
            m_pfrmWait->Hide();
            }
         }
      catch (Exception &e)
         {
         formSpikeWare->m_smp.Exit();
         ShowMessage(e.Message);
         ModalResult = mrCancel;
         }

      lblDescription->Caption = "Calibration successfully finished. Click 'OK' to store the calibration";
      // round to 1/100 dB
      m_dCalValue = RoundToTWoDecimalPlaces(m_dCalibrationLevel - m_dLevel);
      btnOk->Tag = 2;
      btnOk->Caption = "Ok";
      if (m_bAutoReturn)
         ModalResult = mrOk;
      }
   else
      ModalResult = mrOk;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows an 'Enter level' dialog to let the user enter a custom calibrator level
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibrationCalibrator::btnEnterLevelClick(TObject *Sender)
{
   if (GetLevel("Please enter calibration level", "80", m_dCalibrationLevel))
      lblCalLevel->Caption = "Calibration  Level: " + FloatToStr((Extended)m_dCalibrationLevel) + " dB";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows an 'Enter level' dialog. Used, if calibration level is not reached 
/// at maximum possible total level (minimum attenuation of signal)
//------------------------------------------------------------------------------
bool TformCalibrationCalibrator::GetLevel( UnicodeString usMsg,
                                             UnicodeString usDefault,
                                             double &rdValue)
{
   UnicodeString us = usDefault;
   while (1)
      {
      double d;
      if (InputQuery("Enter Level", usMsg, us))
         {
         if (TryStrToDouble(us, d))
            {
            rdValue = d;
            return true;
            }
         }
      else
         return false;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// proc called by static external callback from TformSpikeWare::SMPRecPreVSTProc
/// NOTE: currently we access first channel HARD: always only one recording channel
/// currently during spectral calibration!!
//------------------------------------------------------------------------------
#pragma argsused
void TformCalibrationCalibrator::ProcessRecordBuffer(vvaf& rvvaf)
{
   // indicate clipping...
   float f = rvvaf[0].max();
   if (rvvaf[0].max() > 0.999999f)
      ClipTimer->Tag = 1;
   else if (rvvaf[0].min() < -0.999999f)
      ClipTimer->Tag = 1;

   #ifdef NEWCAL
   unsigned int nSamples = rvvaf[0].size();
   if (m_vafRecBuffer.size() >= nSamples)
      {
      try
         {
         // move all values by one buffersize to the left
         MoveMemory(&m_vafRecBuffer[0], &m_vafRecBuffer[nSamples], sizeof(float)*(m_vafRecBuffer.size()- nSamples));
         // write new buffer to the end
         CopyMemory(&m_vafRecBuffer[m_vafRecBuffer.size()-nSamples], &rvvaf[0][0], sizeof(float)*nSamples);
         }
      catch (Exception &e)
         {
         UnicodeString us = "error in " + UnicodeString(__FUNC__) + ": " + e.Message;
         OutputDebugStringW(us.w_str());
         }
      catch (...)
         {
         UnicodeString us = "unknown error in " + UnicodeString(__FUNC__);
         OutputDebugStringW(us.w_str());
         }
      }
   #endif

   if (!m_bGo || m_bRecDone || !m_vafRecBufferLong.size())
      return;
   try
      {
      bool bDone = false;
      int nSamplesToCopy = (int)rvvaf[0].size();
      int nSpaceLeft = (int)m_vafRecBufferLong.size() - m_nRecBufPosLong;
      if ((int)nSamplesToCopy > nSpaceLeft)
         {
         nSamplesToCopy = nSpaceLeft;
         bDone = true;
         }

      CopyMemory(&m_vafRecBufferLong[(unsigned int)m_nRecBufPosLong], &rvvaf[0][0], sizeof(float)*(unsigned int)nSamplesToCopy);
      m_nRecBufPosLong += nSamplesToCopy;
      if (bDone)
         m_bRecDone = true;
       }
   catch (Exception &e)
      {
      UnicodeString us = "error in " + UnicodeString(__FUNC__) + ": " + e.Message;
      OutputDebugStringW(us.w_str());
      }
   catch (...)
      {
      UnicodeString us = "unknown error in " + UnicodeString(__FUNC__);
      OutputDebugStringW(us.w_str());
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows wait dialog and waits undtil it is closed keeping message loop alive
//------------------------------------------------------------------------------
void TformCalibrationCalibrator::ShowWait(UnicodeString usMsg, bool bHide)
{
   Enabled = false;
   m_bGo = true;

   m_pfrmWait->ShowWait(usMsg);
   try
      {
      while (1)
         {
         Sleep(10);
         Application->ProcessMessages();
         if (m_bRecDone)
            break;
         }
      }
   __finally
      {
      if (bHide)
         {
         Enabled = true;
         m_pfrmWait->Hide();
         }
      m_bGo = false;
      }
}
//------------------------------------------------------------------------------

