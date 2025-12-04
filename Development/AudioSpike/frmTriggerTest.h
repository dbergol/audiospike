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
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "SpikeWareMain.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows a form for the trigger test
//------------------------------------------------------------------------------
class TformTriggerTest : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TListView *lvTop;
      TButton *btnOk;
      TTimer *Timer;
      TListView *lvBottom;
      TImageList *il;
      TButton *btnCancel;
      TGroupBox *gbTriggerTest;
      TGroupBox *gbTriggerValues;
      TComboBox *cobTriggerThreshold;
      TComboBox *cobTriggerValue;
      void __fastcall TimerTimer(TObject *Sender);
      void __fastcall cobTriggerValueChange(TObject *Sender);
      void __fastcall cobTriggerThresholdChange(TObject *Sender);
   private:	// Benutzer-Deklarationen
      int   m_nTriggersPlayed;
      int   m_nTriggersDetected;
      bool  m_bRunning;
      TSWGuiStatus m_gs;
   public:		// Benutzer-Deklarationen
      __fastcall TformTriggerTest(TComponent* Owner);
      void TriggerTest(UnicodeString usOut, UnicodeString usIn);
      void InitTriggerTest(void);
      void ExitTriggerTest(void);
};
//------------------------------------------------------------------------------
extern PACKAGE TformTriggerTest *formTriggerTest;
//------------------------------------------------------------------------------
#endif
