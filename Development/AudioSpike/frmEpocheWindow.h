//------------------------------------------------------------------------------
/// \file frmEpocheWindow.h
///
/// \author Berg
/// \brief Implementation of a container form to show multiple TformEpoches forms
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
#ifndef frmEpocheWindowH
#define frmEpocheWindowH
#include <System.Classes.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include "frmASUI.h"
#include <Vcl.Buttons.hpp>
//------------------------------------------------------------------------------
#include <valarray>
#include "frmASUI.h"
#include "frmEpoche.h"


//------------------------------------------------------------------------------
/// container form to show multiple TformEpoches forms
//------------------------------------------------------------------------------
class TformEpocheWindow : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TScrollBox *scb;
      TToolBar *tb;
      TToolButton *tbtnZoomOut;
      TToolButton *tbtnZoomIn;
      TImageList *il1;
      TToolButton *btnChannels;
      TTimer *Timer;
      TTimer *ClipTimer;
      TToolButton *ToolButton1;
      TCheckBox *cbEpocheThreshold;
      TCheckBox *cbAllAxis;
      TPanel *pnltbEpoches;
      TTimer *EpocheScrollTimer;
      TTrackBar *tbEpoches;
      TSpeedButton *sbtnScrollLeft;
      TSpeedButton *sbtnScrollRight;
      TPanel *pnlTop;
      TBevel *bvl;
      TPanel *pnlTopRight;
      void __fastcall btnChannelsClick(TObject *Sender);
      void __fastcall tbtnZoomClick(TObject *Sender);
      void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta,
             TPoint &MousePos, bool &Handled);
      void __fastcall TimerTimer(TObject *Sender);
      void __fastcall ClipTimerTimer(TObject *Sender);
      void __fastcall tbEpochesChange(TObject *Sender);
      void __fastcall tbEpochesEnter(TObject *Sender);
      void __fastcall EpocheScrollTimerTimer(TObject *Sender);
      void __fastcall sbtnScrollClick(TObject *Sender);
      void __fastcall FormShow(TObject *Sender);
   private:	// Benutzer-Deklarationen
      int   m_nHeight;
   public:		// Benutzer-Deklarationen
      unsigned int m_nPlotIndex;
      std::vector<TformEpoches*>       m_vpformEpoches;
      std::vector<bool >               m_vbClip;
      __fastcall TformEpocheWindow(TComponent* Owner, TMenuItem* pmi);
      __fastcall ~TformEpocheWindow();
      void Clear();
      void Initialize(int nChannels = -1);
      void Plot(TSWEpoche *pswe);
      void PlotSearchData(vvd &rvvdData);
      void UpdateListenButtons(TformEpoches* pfrm);
      void UpdateThreshold(unsigned int nChannel, double dThreshold);
      void SetEvalWindow(TformEpoches* pfrm);
      void SetAllAxis(TformEpoches* pfrm, bool bLeft, bool bForce = false);
      void ShowClipping(std::vector<bool > &rvb);
      void ResetClipping();
      void EnableEpocheScrolling(bool bEnable);
};
//------------------------------------------------------------------------------
#endif
