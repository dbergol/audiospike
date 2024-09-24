//------------------------------------------------------------------------------
/// \file frmBubblePlot.h
///
/// \author Berg
/// \brief Implementation of a form to show a bubble plot
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
#ifndef frmBubblePlotH
#define frmBubblePlotH
//------------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <System.ImageList.hpp>
#include <VCLTee.BubbleCh.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include "SWSpike.h"
#include "BubblePlotData.h"
#include "frmASUI.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form for showing a bubble plot
//------------------------------------------------------------------------------
class TformBubblePlot : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TChart *chrt;
      TBubbleSeries *csDisplayData;
      TImageList *il1;
      TCheckBox *cbOnlyActiveBubble;
      TLineSeries *csResponseData;
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall chrtAfterDraw(TObject *Sender);
      void __fastcall chrtBeforeDrawSeries(TObject *Sender);
      void __fastcall chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall chrtGetAxisLabel(TChartAxis *Sender, TChartSeries *Series, int ValueIndex,
             UnicodeString &LabelText);
      void __fastcall cbOnlyActiveBubbleClick(TObject *Sender);
      void __fastcall chrtMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
   private:	// Benutzer-Deklarationen
      TColor      m_clBubbleColor;
      double      m_dSelLen;
      double      m_dXMinDefault;
      double      m_dXMaxDefault;
      double      m_dYMinDefault;
      double      m_dYMaxDefault;
      bool        m_bSearch;
      int         m_nSearchStim;
      int         m_nLastBubbleIndex;

      void        AdjustLogAxis(TChartAxis* pca);
      double      GetAbsMax();
   public:		// Benutzer-Deklarationen
      TBubblePlotData   m_bpd;
      bool        m_bResponseYAxis;
      std::valarray<double > m_vadData;
      unsigned int   m_nParamX;
      unsigned int   m_nParamY;
      int         m_nPlotCounter;
      __fastcall TformBubblePlot(TComponent* Owner, unsigned int nIndexX, unsigned int nIndexY, bool bSearch);
      __fastcall ~TformBubblePlot();
      void     Initialize();
      void     Clear();
      void     Plot(unsigned int nChannelIndex);
      void     PlotResponse(unsigned int nChannelIndex);
      void     UpdateBubble(int nIndex);
};
//------------------------------------------------------------------------------
#endif
