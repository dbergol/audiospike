//------------------------------------------------------------------------------
/// \file frmTriggerTest.cpp
///
/// \author Berg
/// \brief Implementation of a form to display the status of the trigger test
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
#pragma hdrstop

#include "frmTriggerTest.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TformTriggerTest *formTriggerTest;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructro, initializes members
//------------------------------------------------------------------------------
__fastcall TformTriggerTest::TformTriggerTest(TComponent* Owner)
   : TForm(Owner)
{
   m_bRunning = false;

   int n;
   cobTriggerValue->Items->Clear();
   cobTriggerThreshold->Items->Clear();
   for (n = 0; n < 10; n++)
      {
      cobTriggerValue->Items->Add(IntToStr(-n-1));
      }
   for (n = 0; n < 20; n++)
      {
      cobTriggerThreshold->Items->Add(IntToStr(-n-1));
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of OK button: closes form
//------------------------------------------------------------------------------
#pragma argsused

//------------------------------------------------------------------------------
/// runs trigger test
//------------------------------------------------------------------------------
void TformTriggerTest::TriggerTest(UnicodeString usOut, UnicodeString usIn)
{
   lvTop->Items->Item[0]->SubItems->Strings[0] = usOut;
   lvTop->Items->Item[1]->SubItems->Strings[0] = usIn;
   try
      {
      InitTriggerTest();
      TModalResult mr = ShowModal();
      // store trigger values on OK
      if (mr == mrOk)
         {
         // store only if changed at all (OnChange sets Tag f corresponding ComboBox)
         if (cobTriggerValue->Tag)
            formSpikeWare->m_pIni->WriteInteger("Settings", "TriggerAttenuation", formSpikeWare->m_smp.m_nTriggerValuedB);
         if (cobTriggerThreshold->Tag)
            formSpikeWare->m_pIni->WriteInteger("Settings", "TriggerThreshold", formSpikeWare->m_smp.m_nTriggerThresholddB);
         }
      }
   __finally
      {
      ExitTriggerTest();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// initializes trigger test
//------------------------------------------------------------------------------
void TformTriggerTest::InitTriggerTest(void)
{
   m_nTriggersPlayed    = -1;
   m_nTriggersDetected  = -1;
   lvBottom->Items->Item[0]->SubItems->Strings[0] = "0";
   lvBottom->Items->Item[1]->SubItems->Strings[0] = "0";
   m_gs = formSpikeWare->m_gs;
   cobTriggerValue->ItemIndex = -formSpikeWare->m_smp.m_nTriggerValuedB -1;
   cobTriggerThreshold->ItemIndex = -formSpikeWare->m_smp.m_nTriggerThresholddB -1;
   formSpikeWare->TriggerTest();
   m_bRunning = true;
   Timer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// deinitializes trigger test
//------------------------------------------------------------------------------
void TformTriggerTest::ExitTriggerTest(void)
{
   m_bRunning = false;
   Timer->Enabled = false;
   formSpikeWare->acStopExecute(NULL);
   formSpikeWare->m_smp.Exit();
   formSpikeWare->m_swsSpikes.Clear();
   formSpikeWare->m_sweEpoches.Clear();
   formSpikeWare->SetGUIStatus(m_gs);
   formSpikeWare->m_bFreeSearchRunning    = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// timer callback: updates triggers played and detected counts
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformTriggerTest::TimerTimer(TObject *Sender)
{
   Timer->Enabled = false;
   if (formSpikeWare->m_sweEpoches.m_nTriggerTestTriggersPlayed != m_nTriggersPlayed)
      {
      m_nTriggersPlayed = formSpikeWare->m_sweEpoches.m_nTriggerTestTriggersPlayed;
      lvBottom->Items->Item[0]->SubItems->Strings[0] = IntToStr(m_nTriggersPlayed+1);
      }
   if (formSpikeWare->m_sweEpoches.m_nTriggersDetected != m_nTriggersDetected)
      {
      m_nTriggersDetected = formSpikeWare->m_sweEpoches.m_nTriggersDetected;
      lvBottom->Items->Item[1]->SubItems->Strings[0] = IntToStr(m_nTriggersDetected);
      UnicodeString us;
      us.printf(  L"%.1lf dB (linear value: %.2lf)",
                  FactorTodB(formSpikeWare->m_sweEpoches.m_dTriggerTestLastTriggerValue),
                  formSpikeWare->m_sweEpoches.m_dTriggerTestLastTriggerValue
                  );
      lvBottom->Items->Item[2]->SubItems->Strings[0] = us;


      lvBottom->Items->Item[3]->SubItems->Strings[0] = IntToStr(
            (int)formSpikeWare->m_smp.m_fDefaultSampleRate
            - formSpikeWare->m_sweEpoches.m_nLastTriggerDistance
            );

      }
   Application->ProcessMessages();
   Timer->Enabled = m_bRunning;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of cobTriggerValue restarts trigger test with new trigger
/// value
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformTriggerTest::cobTriggerValueChange(TObject *Sender)
{
   ExitTriggerTest();
   cobTriggerValue->Tag = 1;
   formSpikeWare->m_smp.m_nTriggerValuedB = StrToInt(cobTriggerValue->Text);
   InitTriggerTest();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of cobTriggerValue sets new value for m_smp.m_nTriggerThresholddB
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformTriggerTest::cobTriggerThresholdChange(TObject *Sender)
{
   cobTriggerThreshold->Tag = 1;
   formSpikeWare->m_smp.m_nTriggerThresholddB = StrToInt(cobTriggerThreshold->Text);
}
//------------------------------------------------------------------------------


