//------------------------------------------------------------------------------
/// \file frmPSTH.h
///
/// \author Berg
/// \brief Implementation of a form to display a PSTH (histogram) for a cluster
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
#ifndef frmPSTHH
#define frmPSTHH
//------------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.TeeShape.hpp>
#include "VCLTee.StatChar.hpp"
#include "VCLTee.TeeHistogram.hpp"
#include "frmASUI.h"
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for storing selection properties
//------------------------------------------------------------------------------
class TSWPSTHSelection
{
   public:
      TSWPSTHSelection();
      void     Clear();
      double   dX0;
      double   dX1;
      bool     bActive;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form for dsiplaying PSTH (histogram)
//------------------------------------------------------------------------------
class TformPSTH : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TChart *chrt;
      THistogramSeries *csData;
      TPointSeries *csPoints;
      TChartShape *csSelection;
      TChartShape *csNoiseSelection;
      TImageList *il1;
      TToolBar *tb;
      TToolButton *tbtnSelClear;
      TToolButton *tbtnZoomOut;
      TToolButton *tbtnZoomIn;
      TToolButton *tbtnSel;
      TToolButton *tbtnNoiseSel;
      TToolButton *tbtnNoiseSelClear;
      TToolButton *tbtnBinSize;
      TLabel *lblBinSize;
      TBevel *bvl;
      void __fastcall tbtnZoomOutClick(TObject *Sender);
      void __fastcall tbtnZoomInClick(TObject *Sender);
      void __fastcall tbtnSelClearClick(TObject *Sender);
      void __fastcall chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall chrtMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall tbtnSelClick(TObject *Sender);
      void __fastcall tbtnNoiseSelClearClick(TObject *Sender);
      void __fastcall tbtnNoiseSelClick(TObject *Sender);
      void __fastcall chrtMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall tbtnBinSizeClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
      double   m_dLastX0;
      double   m_dLastX1;
      THistogramFunction* m_pthf;
      void StoreSelection(TChartShape* pcs);
   public:		// Benutzer-Deklarationen
      std::vector<TSWPSTHSelection > m_vSWSelections;
      std::vector<TSWPSTHSelection > m_vSWNoiseSelections;
      int m_nPlotCounter;
      __fastcall TformPSTH(TComponent* Owner, TMenuItem* pmi);
      __fastcall ~TformPSTH();
      void  Initialize(unsigned int nChannels);
      void  Plot(unsigned int nChannelIndex, bool bForce = false);
      void  Clear();
      bool  Selected();
      bool  NoiseSelected();
      void  GetSelections(double& dMin, double& dMax, double& dNoiseMin, double& dNoiseMax);
      void  ShowBinSize();
};
//------------------------------------------------------------------------------
#endif
