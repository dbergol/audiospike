//------------------------------------------------------------------------------
/// \file frmTriggerTest.h
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
#ifndef frmTriggerTestH
#define frmTriggerTestH
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows a form for the trigger test
//------------------------------------------------------------------------------
class TformTriggerTest : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TListView *lv;
      TButton *btnOk;
      TTimer *Timer;
      void __fastcall btnOkClick(TObject *Sender);
      void __fastcall TimerTimer(TObject *Sender);
   private:	// Benutzer-Deklarationen
      int   m_nTriggersPlayed;
      int   m_nTriggersDetected;
      bool  m_bRunning;
   public:		// Benutzer-Deklarationen
      __fastcall TformTriggerTest(TComponent* Owner);
      void TriggerTest(UnicodeString usOut, UnicodeString usIn);
};
//------------------------------------------------------------------------------
extern PACKAGE TformTriggerTest *formTriggerTest;
//------------------------------------------------------------------------------
#endif
