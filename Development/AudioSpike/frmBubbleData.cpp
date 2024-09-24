/// \file frmBubbleData.cpp
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
#include <vcl.h>
#pragma hdrstop

#include "frmBubbleData.h"
#include "SpikeWareMain.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma link "VCLTee.StatChar"
#pragma link "frame_BubbleData"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// constructor. creates frame with histogram and initializes members
//------------------------------------------------------------------------------
__fastcall TformBubbleData::TformBubbleData(TComponent* Owner)
   : TformASUI(Owner), framePSTH(NULL), framePeriodHist(NULL)

{
   framePSTH = new TframeBubbleData(this, BPSTH_STANDARD);
   framePSTH->Align = alClient;

   framePeriodHist = new TframeBubbleData(this, BPSTH_PERIOD);
   framePeriodHist->Align = alBottom;
   // hide binsize for period histogram
   framePeriodHist->tb->Visible = false;
   framePeriodHist->pnlBinSize->Visible = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor. empty...
//------------------------------------------------------------------------------
__fastcall TformBubbleData::~TformBubbleData()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots historgram for a bubble
//------------------------------------------------------------------------------
void TformBubbleData::Plot(TBubbleData &rbd, bool bPeriodHistogram, UnicodeString usInfo)
{
   if (!framePSTH || !framePeriodHist)
      return;

   framePSTH->Plot(rbd, usInfo);

   framePeriodHist->Visible = bPeriodHistogram;
   if (bPeriodHistogram)
      {
      usInfo.printf(L"VS=%1.2lf, p=%1.3lf", rbd.m_dVectorStrength, rbd.m_dPUniform);
      framePeriodHist->Plot(rbd, usInfo);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///  updates the period histogram
//------------------------------------------------------------------------------
void  TformBubbleData::UpdatePeriodHistogram(TBubbleData &rbd)
{
   if (Visible && framePeriodHist && framePeriodHist->Visible)
      framePeriodHist->Plot(rbd);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears PSTH and period histogram data
//------------------------------------------------------------------------------
void  TformBubbleData::Clear()
{
   if (!framePSTH || !framePeriodHist)
      return;
   framePSTH->Clear();
   framePeriodHist->Clear();
}
//------------------------------------------------------------------------------
/// OnResize callback adjusting chart heights
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubbleData::FormResize(TObject *Sender)
{
   if (framePeriodHist)
      framePeriodHist->Height = (ClientHeight - framePeriodHist->tb->Height)/2;
}
//------------------------------------------------------------------------------

