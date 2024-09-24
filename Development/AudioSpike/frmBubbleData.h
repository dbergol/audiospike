/// \file frmBubbleData.h
///
/// \author Berg
/// \brief Implementation of a TForm using TFrame TframeBubbleData to show PSTH in
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
#ifndef frmBubbleDataH
#define frmBubbleDataH
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
#include "VCLTee.StatChar.hpp"
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include "frame_BubbleData.h"

//------------------------------------------------------------------------------
#include <valarray>

//------------------------------------------------------------------------------
/// form for showing a bubble-PSTH and period histogramm for bubble data
//------------------------------------------------------------------------------
class TformBubbleData : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
   void __fastcall FormResize(TObject *Sender);

   private:	// Benutzer-Deklarationen

   public:		// Benutzer-Deklarationen
      __fastcall TformBubbleData(TComponent* Owner);
      TframeBubbleData *framePSTH;
      TframeBubbleData *framePeriodHist;
      void  Plot(TBubbleData &rbd, bool bPeriodHistogram, UnicodeString usInfo = "");
      void  UpdatePeriodHistogram(TBubbleData &rbd);
      void  Clear();
      __fastcall ~TformBubbleData();
};
//------------------------------------------------------------------------------
#endif
