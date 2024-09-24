//------------------------------------------------------------------------------
/// \file frmSpikes.cpp
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
#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "frmSpikes.h"
#include "SpikeWareMain.h"
#include "frmEpocheWindow.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initializes members
//------------------------------------------------------------------------------
__fastcall TformSpikes::TformSpikes(TComponent* Owner, TMenuItem* pmi)
   : TformASUI(Owner, pmi), m_nPlotCounter(0)
{
   // invert depth axis to keep threshold series on top!
   chrt->DepthAxis->Inverted = true;

   m_nMaxNumSpikes = formSpikeWare->m_pIni->ReadInteger("Settings", "MaxNumSpikes", 2000);
   cbPlotEpocheSpikesOnly->Visible = true;

   // NOTE: ALLSPIKES is currently NOT defined - and should not be  !!!!
   #ifndef ALLSPIKES
   for (int i = 0; i < m_nMaxNumSpikes; i++)
      {
      TFastLineSeries* pls = (TFastLineSeries*)chrt->AddSeries(new TFastLineSeries(NULL));
      pls->Selected->Hover->Visible = false;
      pls->DrawAllPoints = false;
      pls->XValues->Order = loNone;
      pls->FastPen = true;
      pls->AutoRepaint = false;
      pls->HorizAxis = aTopAxis;
      pls->Active = false;
      }
   #endif
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, sotores settings and calls Clear
//------------------------------------------------------------------------------
__fastcall TformSpikes::~TformSpikes()
{
   formSpikeWare->StoreChartAxis(this, chrt);
   Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Intializes chart axis properties
//------------------------------------------------------------------------------
void TformSpikes::Initialize()
{
   // NOTE: -1 because first value is at 0 !!
   chrt->TopAxis->SetMinMax(0,      formSpikeWare->m_swsSpikes.m_dSpikeLength*formSpikeWare->m_swsSpikes.GetSampleRate() - 1);
   chrt->BottomAxis->SetMinMax(0,   formSpikeWare->m_swsSpikes.m_dSpikeLength*1000.0);
   formSpikeWare->RestoreChartAxis(this, chrt);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates threshold by invalidating chart
//------------------------------------------------------------------------------
void TformSpikes::UpdateThreshold()
{
   m_dThreshold = formSpikeWare->GetThreshold((unsigned int)Tag);
   chrt->Invalidate();
}
//------------------------------------------------------------------------------

#ifndef ALLSPIKES
//------------------------------------------------------------------------------
/// sets all series except dummy series  to inactive
//------------------------------------------------------------------------------
void TformSpikes::Clear()
{
   for (int i = 2; i < chrt->SeriesCount(); i++)
      chrt->Series[i]->Active = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots spikes
//------------------------------------------------------------------------------
void TformSpikes::Plot(unsigned int nChannelIndex)
{
   if (!Visible || WindowState == wsMinimized || !formSpikeWare->FormsCreated())
      return;

   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;

   try
      {
      m_nPlotCounter++;

      btnPlot->Enabled              = !formSpikeWare->m_smp.Playing();
      tbtnSpikesBack->Enabled       = btnPlot->Enabled;
      tbtnSpikesForward->Enabled    = btnPlot->Enabled;

      if ((int)nChannelIndex != Tag)
         m_dThreshold = formSpikeWare->GetThreshold(nChannelIndex);

      Clear();
      Tag = (NativeInt)nChannelIndex;

      int n, nGroup;
      int nNum    = (int)formSpikeWare->m_swsSpikes.GetNumSpikes(nChannelIndex);
      int nOffset = m_nMaxNumSpikes*(int)tbtnSpikesBack->Tag;

      if ((nNum - nOffset) > m_nMaxNumSpikes)
         nNum -= nOffset;

      int nSpikes = 0;
      unsigned int m, mNum;
      TFastLineSeries* pls;
      for (n = nNum-1; n >= 0; n--)
         {
         nGroup = formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, (unsigned int)n);
         if (nGroup >= 0 || formSpikeWare->m_bFreeSearchRunning)
            nSpikes++;
         else
            continue;

         if (nSpikes >= (chrt->SeriesCount()-2))
            break;

         int nEpocheIndex = (int)formSpikeWare->m_swsSpikes.GetEpocheIndex(nChannelIndex, (unsigned int)n);
         if (cbPlotEpocheSpikesOnly->Checked && !!formSpikeWare->m_pformEpoches->tbEpoches->Tag)
            {
            if (nEpocheIndex != formSpikeWare->m_pformEpoches->tbEpoches->Position)
               continue;
            }

         // NOTE: chrt->Series[0] and chrt->Series[1] are NO spike series!!
         pls = (TFastLineSeries*)chrt->Series[nSpikes+2];
         pls->Clear();
         pls->SeriesColor = formSpikeWare->SpikeGroupToColor(nGroup);
         std::valarray<double >& rvad = formSpikeWare->m_swsSpikes.GetSpike(nChannelIndex, (unsigned int)n);
         // NOTE: the second parameter must be the index of the last item rather than the size
         // of the array (despite it's name). For this purpose we can use the SLICE macro
         pls->AddArray(SLICE(&rvad[0], (int)rvad.size()));
         pls->Active = true;
         }
      if (formSpikeWare->m_bFreeSearchRunning)
         Caption = "Spikes - Channel " + IntToStr(Tag+1) + " (" + IntToStr(nNum) + ")";
      else
         Caption = "Spikes - Channel " + IntToStr(Tag+1) + " (" + IntToStr(nSpikes) + "/" + IntToStr(nNum) + ")";
      }
   __finally
      {
      m_nPlotCounter--;
      }

}
//------------------------------------------------------------------------------

#else // ALLSPIKES _is_ defined
//------------------------------------------------------------------------------
/// removes all series
//------------------------------------------------------------------------------
void TformSpikes::Clear()
{
   while (chrt->SeriesCount() > 2)
      {
      TChartSeries *pcs = chrt->Series[2];
      chrt->RemoveSeries(2);
      TRYDELETENULL(pcs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// pots spikes
//------------------------------------------------------------------------------
void TformSpikes::Plot(unsigned int nChannelIndex)
{
   if (!Visible || WindowState == wsMinimized)
      return;

   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;
   try
      {
      m_nPlotCounter++;

      if ((int)nChannelIndex != Tag)
         {
         m_dThreshold = formSpikeWare->m_swsSpikes.GetThreshold(nChannelIndex);
         Clear();
         }
      Tag = nChannelIndex;

      int n, nGroup;
      int nNum = formSpikeWare->m_swsSpikes.GetNumSpikes(nChannelIndex);


      // NOTE: chrt->Series[0] and chrt->Series[1] are NO spike series!!
      int nStart = chrt->SeriesCount()-2;
      int nSpikes = 0;
      unsigned int m, mNum;

      for (n = 0; n < nNum; n++)
         {
         nGroup = formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, n);
         if (nGroup >= 0)
            nSpikes++;
         if (n < nStart)
            {
            // NOTE: chrt->Series[0] and chrt->Series[1] are NO spike series!!
            chrt->Series[n+2]->Active        = nGroup >= 0 || formSpikeWare->m_bFreeSearchRunning;
            chrt->Series[n+2]->SeriesColor   = formSpikeWare->SpikeGroupToColor(nGroup);
            continue;
            }

         TFastLineSeries* pls = (TFastLineSeries*)chrt->AddSeries(new TFastLineSeries(NULL));
         pls->Selected->Hover->Visible = false;
         pls->DrawAllPoints = false;
         pls->XValues->Order = loNone;
         pls->FastPen = true;
         pls->AutoRepaint = false;
         pls->SeriesColor = formSpikeWare->SpikeGroupToColor(nGroup);
         pls->HorizAxis = aTopAxis;
         pls->Active    = nGroup >= 0 || formSpikeWare->m_bFreeSearchRunning;
         std::valarray<double >& rvad = formSpikeWare->m_swsSpikes.GetSpike(nChannelIndex, n);
         // NOTE: the second parameter must be the index of the last item rather than the size
         // of the array (despite it's name). For this purpose we can use the SLICE macro
         pls->AddArray(SLICE(&rvad[0], rvad.size()));
         }


      if (formSpikeWare->m_bFreeSearchRunning)
         Caption = "Spikes - Channel " + IntToStr(Tag+1) + " (" + IntToStr(nNum) + ")";
      else
         Caption = "Spikes - Channel " + IntToStr(Tag+1) + " (" + IntToStr(nSpikes) + "/" + IntToStr(nNum) + ")";
      }
   __finally
      {
      m_nPlotCounter--;
      }

}
//------------------------------------------------------------------------------
#endif // #ifndef ALLSPIKES 

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikes::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if (!formSpikeWare->FormsCreated())
      return;
   if (Axis == chrt->BottomAxis)
      {
      // NOTE:THIS IS DISABLED BY PURPOSE: bottom axis NOT to be set arbitrary!!

      // adjust top axis as well if bottom changed!     
      // if (formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis, 0, formSpikeWare->m_swsSpikes.m_dSpikeLength*1000.0, this))
      // chrt->TopAxis->SetMinMax( MsToSamples(Axis->Minimum, formSpikeWare->m_swsSpikes.GetSampleRate()), MsToSamples(Axis->Maximum, formSpikeWare->m_swsSpikes.GetSampleRate()));
      }
   else if (Axis == chrt->LeftAxis)
      {
      // if changed, then set identical on epoche form
      if (  formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis, -1, 1, this)
         && (int)formSpikeWare->m_pformEpoches->m_vpformEpoches.size() > Tag
         )
         formSpikeWare->m_pformEpoches->m_vpformEpoches[(unsigned int)Tag]->chrt->LeftAxis->SetMinMax(chrt->LeftAxis->Minimum, chrt->LeftAxis->Maximum);
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnPlot: calls Plot()
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikes::btnPlotClick(TObject *Sender)
{
   Plot((unsigned int)Tag);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of cbPlotEpocheSpikesOnly: calls Plot()
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikes::cbPlotEpocheSpikesOnlyClick(TObject *Sender)
{
   Plot((unsigned int)Tag);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for tbtnSpikesBack and tbtnSpikesForward: advances or goes
/// back in list of plotted spikes
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikes::tbtnSpikesClick(TObject *Sender)
{
   if (formSpikeWare->m_smp.Playing())
      return;
   // adjust Tag of tbtnSpikesBack which is a n Offset for plotting
   // a maximum of m_nMaxNumSpikes spikes
   int n = (int)tbtnSpikesBack->Tag;
   if (Sender == tbtnSpikesForward)
      {
      if (n > 0)
         n--;
      }
   else
      {
      int nNum    = (int)formSpikeWare->m_swsSpikes.GetNumSpikes((unsigned int)Tag);
      int nOffset = m_nMaxNumSpikes*(int)tbtnSpikesBack->Tag;
      if (nNum - nOffset >= m_nMaxNumSpikes)
         n++;
      }
   tbtnSpikesBack->Tag = n;
   //
   Plot((unsigned int)Tag);
}
//------------------------------------------------------------------------------


