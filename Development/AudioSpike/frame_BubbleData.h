//------------------------------------------------------------------------------
/// \file frame_BubbleData.cpp
///
/// \author Berg
/// \brief Implementation of a TFrame used by TformBubbleData to show PSTH in
/// a histogram using bubble-plot data
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
#ifndef frame_BubbleDataH
#define frame_BubbleDataH
//---------------------------------------------------------------------------
#include <System.ImageList.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.StatChar.hpp>
#include "VCLTee.TeeHistogram.hpp"
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.StdCtrls.hpp>
#include <VCLTee.TeeFunci.hpp>
#include <vector>

#define BPSTH_STANDARD  0
#define BPSTH_PERIOD    1

class TBubbleData;
class TformBubblePlot;

//------------------------------------------------------------------------------
/// TFrame for showing a historgram based on bubble plot
//------------------------------------------------------------------------------
class TframeBubbleData : public TFrame
{
   __published:	// Von der IDE verwaltete Komponenten
      TPanel *pnlPSTH;
      TChart *chrt;
      THistogramSeries *csData;
      TPointSeries *csPoints;
      THistogramSeries *csNormalizedData;
      TToolBar *tb;
      TToolButton *tbtnZoomOut;
      TToolButton *tbtnZoomIn;
      TToolButton *tbtnBinSize;
      TToolButton *ToolButton1;
      TPanel *pnlBinSize;
      TImageList *il1;
      TBevel *bvl;
      void __fastcall tbtnZoomOutClick(TObject *Sender);
      void __fastcall tbtnZoomInClick(TObject *Sender);
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall tbtnBinSizeClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
      bool                 m_bInitialized;
      UnicodeString        m_usName;
      THistogramFunction*  m_pthf;
      int                  m_nChartType;
      void                 SetNumBins(int n);
      void                 NormalizeHistogram();
      void                 Initialize(int nType);
   public:		// Benutzer-Deklarationen
      __fastcall TframeBubbleData(TComponent* Owner);
      __fastcall TframeBubbleData(TComponent* Owner, int nType);
      __fastcall ~TframeBubbleData();
      void  UpdateEpochelength();
      void  Plot(TBubbleData &rbd, UnicodeString usInfo = "");
      void  Clear();
      void  ShowBinSize();
      int   m_nPlotCounter;
};
//------------------------------------------------------------------------------
#endif
