//------------------------------------------------------------------------------
/// \file frmVectorStrength.h
///
/// \author Berg
/// \brief Implementation of a form to display a vector stregth plot for one bubble
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
#ifndef frmVectorStrengthH
#define frmVectorStrengthH
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
#include "BubblePlotData.h"
#include "frmASUI.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form to display a vector stregth plot for one bubble
//------------------------------------------------------------------------------
class TformVectorStrength : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TChart *chrt;
      TLineSeries *csVS;
      TLineSeries *csP;
      TSeriesPointerStyle __fastcall csVSGetPointerStyle(TChartSeries *Sender, int ValueIndex);
      void __fastcall chrtGetLegendText(TCustomAxisPanel *Sender, TLegendStyle LegendStyle,
             int Index, UnicodeString &LegendText);
   private:	// Benutzer-Deklarationen
   public:		// Benutzer-Deklarationen
      __fastcall TformVectorStrength(TComponent* Owner);
      __fastcall ~TformVectorStrength();
      void     Plot(TBubblePlotData& rbpd, int nIndex);
      int      m_nPlotCounter;

};
//------------------------------------------------------------------------------
#endif
