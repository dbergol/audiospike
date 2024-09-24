//------------------------------------------------------------------------------
/// \file frmEpoche.h
///
/// \author Berg
/// \brief Implementation of a form to show the spike train of one epoche
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
#ifndef frmEpocheH
#define frmEpocheH
//------------------------------------------------------------------------------
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.TeeShape.hpp>
#include <System.Classes.hpp>
#include <valarray>
#include "SWEpoches.h"


//------------------------------------------------------------------------------
/// form to show the spike train of one epoche
//------------------------------------------------------------------------------
class TformEpoches : public TForm
{
   friend class TformEpocheWindow;
   __published:	// IDE-verwaltete Komponenten
      TImageList *il2;
      TImageList *il1;
      TPanel *pnl;
      TBevel *bvl;
      TToolBar *tb;
      TToolButton *tbtnEvalActive;
      TToolButton *tbtnClear;
      TToolButton *tbtnAverage;
      TToolButton *tbnListen;
      TChart *chrt;
      TLineSeries *csThreshold;
      TFastLineSeries *csEpoche;
      TChartShape *csStimSeries;
      TLineSeries *csEpocheThreshold;
      TShape *shClip;
      void __fastcall chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall chrtMouseLeave(TObject *Sender);
      void __fastcall chrtMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
      void __fastcall chrtMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall FormMouseActivate(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y, int HitTest, TMouseActivate &MouseActivate);
      void __fastcall FormShow(TObject *Sender);
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall chrtClick(TObject *Sender);
      void __fastcall tbtnClearClick(TObject *Sender);
      void __fastcall chrtBeforeDrawAxes(TObject *Sender);
      void __fastcall tbtnEvalActiveClick(TObject *Sender);
      void __fastcall tbnListenClick(TObject *Sender);

   private:	// Benutzer-Deklarationen
      CRITICAL_SECTION        m_cs;
      std::valarray<double >  m_vad;
      double                  m_dAverageCounter;
      bool                    m_bMouseDown;
      void                    ClearAverage();
      void                    Plot(TSWEpoche *pswe);
   public:		// Benutzer-Deklarationen
      __fastcall TformEpoches(TComponent* Owner, int nChannelIndex);
      __fastcall ~TformEpoches();
      void Initialize();
      void Plot(vvd &rvvdData);
      void SetData(vvd &rvvdData);
      void PlotData();
      void UpdateThreshold(double dThreshold);
      void Clear();
      int  m_nPlotCounter;
};
//------------------------------------------------------------------------------
#endif
