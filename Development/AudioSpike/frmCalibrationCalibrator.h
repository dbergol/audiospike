//------------------------------------------------------------------------------
/// \file frmCalibration.h
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
#ifndef frmCalibrationCalibratorH
#define frmCalibrationCalibratorH
#include <System.Classes.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#include "HtFFT3.h"
#include "SWSMP.h"

class TformWait;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form used for running the calibration of the calibrator
//------------------------------------------------------------------------------
class TformCalibrationCalibrator : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TButton *btnOk;
      TButton *btnEnterLevel;
      TButton *btnCancel;
      TTimer *ClipTimer;
      TGroupBox *gb;
      TLabel *lblCalLevel;
      TLabel *lblDescription;
      TLabel *lblClip;
      TShape *shClip;
      TLabel *lbDebug;
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall ClipTimerTimer(TObject *Sender);
      void __fastcall btnOkClick(TObject *Sender);
      void __fastcall btnEnterLevelClick(TObject *Sender);
      void __fastcall FormShow(TObject *Sender);
   private:	// Benutzer-Deklarationen
      TModalResult ShowModalCenter(TForm* pfrm);
      TformWait *m_pfrmWait;
      vaf      m_vafRecBufferLong;
      vaf      m_vafRecBuffer;
      vaf      m_vafCalcBuffer;
      int      m_nRecBufPosLong;
      bool     m_bRecDone;
      bool     m_bGo;
      bool     m_bAutoReturn;
      double   m_dCalibratorRMS;
      double   m_dCalibratorLevel;
      double   m_dCalibrationLevel;
      void     ShowWait(UnicodeString usMsg, bool bHide = true);
      bool     GetLevel(UnicodeString usMsg, UnicodeString usDefault, double &rdValue);
   public:		// Benutzer-Deklarationen
      double   m_dLevel;
      double   m_dCalValue;
      __fastcall TformCalibrationCalibrator(TComponent* Owner);
      __fastcall ~TformCalibrationCalibrator();
      TModalResult Calibrate(TForm* pfrm = NULL, bool bExitOnClose = true);
      void ProcessRecordBuffer(vvaf& rvvaf);
};
//------------------------------------------------------------------------------
extern PACKAGE TformCalibrationCalibrator *formCalibrationCalibrator;
//------------------------------------------------------------------------------
#endif
