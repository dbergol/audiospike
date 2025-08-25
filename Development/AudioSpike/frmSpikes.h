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
/// class for averaging spikes
//------------------------------------------------------------------------------
class TSpikeAvg
{
   public:
      TSpikeAvg();
      std::valarray<double> m_vad;
      int  m_nNumSpikes;
      void Reset();
      void SetSize(unsigned int nSize);
      void Add(std::valarray<double>& rvad);
};

//------------------------------------------------------------------------------
/// form for displaying spikes
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
      TComboBox *cbNumSpikes;
      TStaticText *stPlotLimit;
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall btnPlotClick(TObject *Sender);
      void __fastcall tbtnSpikesClick(TObject *Sender);
      void __fastcall cbPlotEpocheSpikesOnlyClick(TObject *Sender);
      void __fastcall cbNumSpikesChange(TObject *Sender);
   private:	// Benutzer-Deklarationen
      double                  m_dThreshold;
      int                     m_nNumSpikesSelected;
      std::vector<TSpikeAvg > m_vsaAverage;
      std::valarray<int >     m_vaSpikesSelected;
      std::valarray<int >     m_vaSpikesFound;

   public:		// Benutzer-Deklarationen
      __fastcall TformSpikes(TComponent* Owner, TMenuItem* pmi);
      __fastcall ~TformSpikes();
      void SetMaxNumSpikes();
      void Initialize();
      void UpdateThreshold();
      int  NumNonSpikeSeries();
      void Clear();
      void Plot(unsigned int nChannelIndex);
      bool BackIndexAllowed(void);
      void SetBackForwardEnabled(void);
      UnicodeString GetMaxSpikesIniEntry(void);
      void SetMaxSpikesMode(TMaxNumGroupSpikesMode mng);
      TMaxNumGroupSpikesMode  m_mng;
      int                     m_nMaxNumSpikes;
      int                     m_nMaxNumGroupSpikes;
      int                     m_nPlotCounter;
};
//------------------------------------------------------------------------------
#endif
