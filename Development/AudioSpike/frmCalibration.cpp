//------------------------------------------------------------------------------
/// \file frmCalibration.cpp
///
/// \author Berg
/// \brief Implementation of a form for performing a calibration task
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

#include "frmCalibration.h"
#include "SpikeWareMain.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TformCalibration *formCalibration;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor initializes members and captions
//------------------------------------------------------------------------------
__fastcall TformCalibration::TformCalibration(TComponent* Owner)
   : TForm(Owner)
{
   m_dLevel = -100.0;
   UnicodeString us;
   us.printf(L"Use the slider and/or buttons to adjust the level until the levelmeter shows the target level "
               "displayed below and press 'Ok'. Alternatively you can hit 'Enter Target Level' and enter "
               "the currently measured level.\n\nNOTE: when using the slider the level is set AFTER "
               "releasing the slider, so use it with care!!");
   lblDescription->Caption = us;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// main calibration procedure. Sets staring values and calls ShowModalCenter
//------------------------------------------------------------------------------
TModalResult TformCalibration::Calibrate(TForm* pfrm, bool bExitOnClose)
{
   TModalResult mr = mrCancel;
   try
      {
      m_dTargetLevel = 80.0;
      tb->Position = -1000;
      SetLevel(true);
      mr = ShowModalCenter(pfrm);
      }
   __finally
      {
      if (bExitOnClose || mr != mrOk)
         formSpikeWare->m_smp.Exit();
      }
   return mr;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows form centered and modal
//------------------------------------------------------------------------------
TModalResult TformCalibration::ShowModalCenter(TForm* pfrm)
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
void __fastcall TformCalibration::FormShow(TObject *Sender)
{
   ClipTimer->Tag = 0;
   ClipTimer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClose callback. disables clip timer
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibration::FormClose(TObject *Sender, TCloseAction &Action)

{
   ClipTimer->Enabled = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clip timer: indicates if clipping occurred or not. SMP has written clip status
/// to tag of timer!
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibration::ClipTimerTimer(TObject *Sender)
{
   shClip->Brush->Color = ClipTimer->Tag ? clRed : clLime;
   ClipTimer->Tag = 0;

   // if trackbar was changed, apply change only, if left mouse button is up again!
   if (tb->Tag && GetAsyncKeyState(VK_LBUTTON) == 0)
      {
      ClipTimer->Enabled = false;
      try
         {
      
         tb->Tag = 0;
         SetLevel();
         }
      __finally
         {
         ClipTimer->Enabled = true;
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of trackbar: calls SetLevel 
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibration::tbChange(TObject *Sender)
{
   // store, that trackbar was changed (with mouse): change is applied in
   // ClipTimerTimer
   tb->Tag = 1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets level by calling SoundMexPro
//------------------------------------------------------------------------------
void TformCalibration::SetLevel(bool bForce)
{
   EnableControls(false);
   try
      {
      double dLevel = (double)tb->Position / 10.0;
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wfloat-equal"
      if (bForce || (GetAsyncKeyState(VK_LBUTTON) == 0 && dLevel != m_dLevel))
         {
         m_dLevel = dLevel;
         formSpikeWare->m_smp.Command("volume", "value=" + DoubleToStr(dBToFactor(m_dLevel)));
         }
      #pragma clang diagnostic pop
      UnicodeString us;
      us.printf(L"%.2f dB", m_dLevel);
      lblGainVal->Caption = us;
      }
   __finally
      {
      EnableControls(true);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enables/disables controls
//------------------------------------------------------------------------------
void TformCalibration::EnableControls(bool bEnable)
{
   // have to set tb->Enabled twice: no idea why....
   tb->Enabled       = !bEnable;
   tb->Enabled       = bEnable;
   btnM10->Enabled   = bEnable;
   btnM2->Enabled    = bEnable;
   btnM1->Enabled    = bEnable;
   btnM01->Enabled   = bEnable;
   btn01->Enabled    = bEnable;
   btn1->Enabled     = bEnable;
   btn2->Enabled     = bEnable;
   btn10->Enabled    = bEnable;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calculates calvalue
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibration::btnOkClick(TObject *Sender)
{
   if (tb->Position == -1000)
      m_dCalValue = 0.0;
   else
      {
      double dRMS = (double)formSpikeWare->m_smp.m_fSchroederRMSdB;
      // round to 1/100 dB
      m_dCalValue = RoundToTWoDecimalPlaces(m_dTargetLevel - (double)tb->Position / 10.0 - dRMS);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows an 'Enter level' dialog. Used, if calibration level is not reached 
/// at maximum possible total level (minimum attenuation of signal)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCalibration::btnEnterLevelClick(TObject *Sender)
{
   UnicodeString us;
   while (1)
      {
      if (InputQuery("Target Level", "Please enter the current level", us))
         {
         double d;
         if (TryStrToDouble(us, d))
            {
            m_dTargetLevel = d;
            btnOkClick(NULL);
            ModalResult = mrOk;
            break;
            }
         }
      else
         break;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// callback for al btnLevel-Buttons: increases/decreases current level by
/// value of buttons tag
//------------------------------------------------------------------------------
void __fastcall TformCalibration::btnLevelClick(TObject *Sender)
{
   TSpeedButton* pbtn = dynamic_cast<TSpeedButton*>(Sender);
   if (pbtn)
      {
      tb->Position = tb->Position + (int)pbtn->Tag;
      SetLevel(true);
      }

}
//------------------------------------------------------------------------------



