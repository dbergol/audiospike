//------------------------------------------------------------------------------
/// \file frmSignalPSTH.h
///
/// \author Berg
/// \brief Implementation of a form to display a PSTH (histogram) for one signal
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
#ifndef frmSignalPSTHH
#define frmSignalPSTHH
//------------------------------------------------------------------------------
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
#include "frmASUI.h"
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form for showing PSTH for one signal/bubble
//------------------------------------------------------------------------------
class TformSignalPSTH : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TChart *chrt;
      TImageList *il1;
      TToolBar *tb;
      TToolButton *tbtnZoomOut;
      TToolButton *tbtnZoomIn;
      TPanel *pnlBinSize;
      TToolButton *tbtnBinSize;
      TToolButton *ToolButton2;
      TBevel *bvl;
      void __fastcall tbtnZoomOutClick(TObject *Sender);
      void __fastcall tbtnZoomInClick(TObject *Sender);
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall tbtnBinSizeClick(TObject *Sender);
      void __fastcall chrtBeforeDrawSeries(TObject *Sender);
      void __fastcall FormShow(TObject *Sender);

   private:	// Benutzer-Deklarationen
      std::vector<THistogramFunction* > m_vpthf;
      void CleanupChart(void);
   public:		// Benutzer-Deklarationen
      __fastcall TformSignalPSTH(TComponent* Owner, TMenuItem* pmi);
      __fastcall ~TformSignalPSTH();
      void  Initialize();
      void  InitAxes();
      void  Plot(unsigned int nChannelIndex);
      void  Clear();
      void  ShowBinSize();
      void  SetBinSize(int nBinSize);
      void  AdjustYMax(double dMax = -1.0);
      int   m_nPlotCounter;
};
//------------------------------------------------------------------------------
#endif
