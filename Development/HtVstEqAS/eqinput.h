//------------------------------------------------------------------------------
/// \file eqinput.h
///
/// \author Berg
/// \brief Implementation form for displaying spectral filters + input and output
/// to/from filter
///
/// Project AudioSpike
/// Module  HtVSTEqAS.dll
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
#ifndef eqinputH
#define eqinputH
#include <System.Classes.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.TeeShape.hpp>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///
//------------------------------------------------------------------------------
class TfrmEqInput : public TForm
{
   __published:	// Von der IDE verwaltete Komponenten
      TChart *Chart;
      TFastLineSeries *PreSpecSeriesL;
      TFastLineSeries *PostSpecSeriesL;
      TLineSeries *DummySeries;
      TCheckBox *cbLog;
      TFastLineSeries *RawFilterSeries;
      void __fastcall ChartBeforeDrawAxes(TObject *Sender);
      void __fastcall cbLogClick(TObject *Sender);
      void __fastcall FormShow(TObject *Sender);

   private:	// Anwender-Deklarationen
      void ReadSettings();
   public:		// Anwender-Deklarationen
      __fastcall TfrmEqInput();
      __fastcall ~TfrmEqInput();
      void __fastcall Initialize(unsigned int nFFTLen, float fSampleRate);
      void AdjustLogAxis(TChartAxis* pca);
};
//------------------------------------------------------------------------------
#endif
