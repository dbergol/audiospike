//------------------------------------------------------------------------------
/// \file frmSpikes.h
///
/// \author Berg
/// \brief Implementation of a form to display spikes
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
#ifndef frmSpikesH
#define frmSpikesH
//------------------------------------------------------------------------------
#include <System.Classes.hpp>
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
#include <Vcl.StdCtrls.hpp>
#include "frmASUI.h"
#include <vector>
#include <valarray>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// form for displaying spikes
//------------------------------------------------------------------------------
class TformSpikes : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TChart *chrt;
      TLineSeries *csThreshold;
      TPointSeries *Series2;
      TToolBar *tb;
      TToolButton *btnPlot;
      TToolButton *tbtnSpikesBack;
      TToolButton *tbtnSpikesForward;
      TImageList *il;
      TImageList *ild;
      TCheckBox *cbPlotEpocheSpikesOnly;
      TBevel *bvl;
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall btnPlotClick(TObject *Sender);
      void __fastcall tbtnSpikesClick(TObject *Sender);
      void __fastcall cbPlotEpocheSpikesOnlyClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
      double   m_dThreshold;
      int      m_nMaxNumSpikes;
   public:		// Benutzer-Deklarationen
      __fastcall TformSpikes(TComponent* Owner, TMenuItem* pmi);
      __fastcall ~TformSpikes();
      void Initialize();
      void UpdateThreshold();
      void Clear();
      void Plot(unsigned int nChannelIndex);
      int   m_nPlotCounter;
};
//------------------------------------------------------------------------------
#endif
