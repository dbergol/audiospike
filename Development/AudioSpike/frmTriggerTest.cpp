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
#include "SpikeWareMain.h"
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
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of OK button: closes form
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformTriggerTest::btnOkClick(TObject *Sender)
{
   Close();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// rns trigger test
//------------------------------------------------------------------------------
void TformTriggerTest::TriggerTest(UnicodeString usOut, UnicodeString usIn)
{
   m_nTriggersPlayed    = -1;
   m_nTriggersDetected  = -1;
   lv->Items->Item[0]->SubItems->Strings[0] = usOut;
   lv->Items->Item[1]->SubItems->Strings[0] = usIn;
   lv->Items->Item[2]->SubItems->Strings[0] = "0";
   lv->Items->Item[3]->SubItems->Strings[0] = "0";
   TSWGuiStatus gs = formSpikeWare->m_gs;
   formSpikeWare->TriggerTest();
   try
      {
      m_bRunning = true;
      Timer->Enabled = true;

      ShowModal();
      }
   __finally
      {
      m_bRunning = false;
      Timer->Enabled = false;
      formSpikeWare->btnStopClick(NULL);
      formSpikeWare->m_smp.Exit();
      formSpikeWare->m_swsSpikes.Clear();
      formSpikeWare->m_sweEpoches.Clear();
      formSpikeWare->SetGUIStatus(gs);
      formSpikeWare->m_bFreeSearchRunning    = false;
      }
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
      lv->Items->Item[2]->SubItems->Strings[0] = IntToStr(m_nTriggersPlayed+1);
      }
   if (formSpikeWare->m_sweEpoches.m_nTriggersDetected != m_nTriggersDetected)
      {
      m_nTriggersDetected = formSpikeWare->m_sweEpoches.m_nTriggersDetected;
      lv->Items->Item[3]->SubItems->Strings[0] = IntToStr(m_nTriggersDetected);
      UnicodeString us;
      us.printf(L"%.2f", formSpikeWare->m_sweEpoches.m_dTriggerTestLastTriggerValue);
      lv->Items->Item[4]->SubItems->Strings[0] = us;
      
      }
   Application->ProcessMessages();
   Timer->Enabled = m_bRunning;
}
//------------------------------------------------------------------------------

