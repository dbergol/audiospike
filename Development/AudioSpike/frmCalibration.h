//------------------------------------------------------------------------------
/// \file frmCalibration.h
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
#ifndef frmCalibrationH
#define frmCalibrationH
#include <System.Classes.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// form to adjust level during level calibration procedure
//------------------------------------------------------------------------------
class TformCalibration : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TButton *btnOk;
      TButton *btnEnterLevel;
      TButton *btnCancel;
      TTimer *ClipTimer;
      TGroupBox *gb;
      TTrackBar *tb;
      TLabel *lblGain;
      TLabel *lblTargetLevel;
      TLabel *lblDescription;
      TSpeedButton *btnM10;
      TSpeedButton *btnM2;
      TSpeedButton *btnM1;
      TSpeedButton *btn1;
      TSpeedButton *btn2;
      TSpeedButton *btn10;
      TSpeedButton *btnM01;
      TSpeedButton *btn01;
      TLabel *lblClip;
      TLabel *lblGainVal;
      TShape *shClip;
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall ClipTimerTimer(TObject *Sender);
      void __fastcall tbChange(TObject *Sender);
      void __fastcall btnOkClick(TObject *Sender);
      void __fastcall btnEnterLevelClick(TObject *Sender);
      void __fastcall btnLevelClick(TObject *Sender);
      void __fastcall FormShow(TObject *Sender);
   private:	// Benutzer-Deklarationen
      void     SetLevel(bool bForce = false);
      void     EnableControls(bool bEnable);
      TModalResult ShowModalCenter(TForm* pfrm);
   public:		// Benutzer-Deklarationen
      double   m_dLevel;
      double   m_dCalValue;
      double   m_dTargetLevel;
      __fastcall TformCalibration(TComponent* Owner);
      TModalResult Calibrate(TForm* pfrm = NULL, bool bExitOnClose = true);
};
//------------------------------------------------------------------------------
extern PACKAGE TformCalibration *formCalibration;
//------------------------------------------------------------------------------
#endif
