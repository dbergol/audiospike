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

#define NUM_DUMMY_SERIES 2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for averaging spikes
/// NOTE: no 'security' within this class at all:
///  - length checks
///  - no check, if external caller has already averaged by deviding by m_nNumSpikes
/// User must take complete care of meaningful usage!!
/// constructor initializes members
//------------------------------------------------------------------------------
TSpikeAvg::TSpikeAvg()
  : m_nNumSpikes(0)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears members
//------------------------------------------------------------------------------
void TSpikeAvg::Reset()
{
   m_nNumSpikes = 0;
   m_vad = 0.0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets size and calls reset
//------------------------------------------------------------------------------
void TSpikeAvg::SetSize(unsigned int nSize)
{
   m_vad.resize(nSize);
   Reset();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds array to own buffer
//------------------------------------------------------------------------------
void TSpikeAvg::Add(std::valarray<double>& rvad)
{
   m_vad += rvad;
   m_nNumSpikes++;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// constructor. Initializes members
//------------------------------------------------------------------------------
__fastcall TformSpikes::TformSpikes(TComponent* Owner, TMenuItem* pmi)
   :  TformASUI(Owner, pmi),
      m_nNumSpikesSelected(0),
      m_mng(MNG_MEAUSUREMENT),
      m_nPlotCounter(0)
{

   m_vaSpikesSelected.resize(formSpikeWare->m_vclSpikeColors.size());
   m_vaSpikesFound.resize(m_vaSpikesSelected.size());


   // invert depth axis to keep threshold series on top!
   chrt->DepthAxis->Inverted = true;

   cbPlotEpocheSpikesOnly->Visible = true;


   m_vsaAverage.resize(formSpikeWare->m_vclSpikeColors.size());
   // create averaging series
   unsigned int i;
   for (i = 0; i < formSpikeWare->m_vclSpikeColors.size(); i++)
      {
      TFastLineSeries* pls = (TFastLineSeries*)chrt->AddSeries(new TFastLineSeries(NULL));
      pls->Selected->Hover->Visible = false;
      pls->DrawAllPoints = false;
      pls->XValues->Order = loNone;
      pls->FastPen = true;
      pls->AutoRepaint = false;
      pls->HorizAxis = aTopAxis;
      pls->SeriesColor = clLime;
      pls->LinePen->Width = 3;
      pls->Active = false;
      pls->SeriesColor = formSpikeWare->SpikeGroupToColor((int)i);
      }

   SetMaxSpikesMode(MNG_MEAUSUREMENT);
   SetMaxNumSpikes();
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
/// Intializes maximum number of chart seres for spikes
//------------------------------------------------------------------------------
void TformSpikes::SetMaxNumSpikes()
{
   int nNumNonSpikeSeries = NumNonSpikeSeries();
   while (chrt->SeriesCount() > nNumNonSpikeSeries)
      {
      delete chrt->Series[nNumNonSpikeSeries];
      }

   // this number is 'fix' to allow a max of 500 Spikes per spike group. maybe changed
   // in INI file, but it's NOT recommended to do so.......
   m_nMaxNumSpikes = formSpikeWare->m_pIni->ReadInteger("Settings", "MaxNumSpikesTotal", Ini_MaxNumSpikesTotal);

   int i;
   for (i = 0; i < m_nMaxNumSpikes; i++)
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
   tbtnSpikesBack->Tag = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Intializes chart axis properties
//------------------------------------------------------------------------------
void TformSpikes::Initialize()
{
   UpdateXAxes();
   formSpikeWare->RestoreChartAxis(this, chrt);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates properties of XAxes
//------------------------------------------------------------------------------
void TformSpikes::UpdateXAxes()
{
   // NOTE: -1 because first value is at 0 !!
   chrt->TopAxis->SetMinMax(0,      formSpikeWare->m_swsSpikes.GetSpikeLengthSamples() - 1);
   chrt->BottomAxis->SetMinMax(0,   formSpikeWare->m_swsSpikes.GetSpikeLength()*1000.0);
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

//------------------------------------------------------------------------------
/// returns number of series in the beginning if chrt->Series array that are NOT
/// series for a particular spike
//------------------------------------------------------------------------------
int TformSpikes::NumNonSpikeSeries()
{
   return(int)formSpikeWare->m_vclSpikeColors.size() + NUM_DUMMY_SERIES;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets all series except dummy series to inactive
//------------------------------------------------------------------------------
void TformSpikes::Clear()
{
   SetMaxSpikesMode(m_mng);
   int nStart = NUM_DUMMY_SERIES;
   for (int i = nStart; i < chrt->SeriesCount(); i++)
      chrt->Series[i]->Active = false;
   chrt->Series[NUM_DUMMY_SERIES]->SeriesColor = formSpikeWare->m_bFreeSearchRunning ?
      clBlack : formSpikeWare->SpikeGroupToColor(0);
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
      cbNumSpikesChange(NULL);
      m_nPlotCounter++;

      // adjust buffer length for averaging buffers if necessary
      unsigned int nSpikeLen = (unsigned int)formSpikeWare->m_swsSpikes.GetSpikeLengthSamples();
      unsigned int i;
      for (i = 0; i < m_vsaAverage.size(); i++)
         {
         if (m_vsaAverage[i].m_vad.size() != nSpikeLen)
            m_vsaAverage[i].SetSize(nSpikeLen);
         else
            m_vsaAverage[i].Reset();
         }



      btnPlot->Enabled = !formSpikeWare->m_smp.Playing();

      if ((int)nChannelIndex != Tag)
         m_dThreshold = formSpikeWare->GetThreshold(nChannelIndex);

      Clear();

      Tag = (NativeInt)nChannelIndex;

      // determine total number of selected (!) spikes per group
      unsigned int nTotalNum = formSpikeWare->m_swsSpikes.GetNumSpikes(nChannelIndex);
      if (!nTotalNum)
         return;

      m_vaSpikesSelected   = 0;
      m_vaSpikesFound      = 0;

      int nGroup;
      for (i = 0; i < nTotalNum; i++)
         {
         nGroup = formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, i);
         if (nGroup >= 0)
            m_vaSpikesSelected[(unsigned int)nGroup] += 1;
         }

      int nTotalNumSelected = m_vaSpikesSelected.sum();


      // check if we have to reset. NOTE: total num is a good counter, because
      // adding or removing a selection on cluster window will ALWAYS change the count
      // except if we add/remove a selection which is empty: then it's fine for plotting
      // here as well
      if (nTotalNumSelected != m_nNumSpikesSelected)
         {
         m_nNumSpikesSelected = nTotalNumSelected;
         tbtnSpikesBack->Tag = 0;
         }


      int nNumNonSpikeSeries  = NumNonSpikeSeries();
      int nSpikes             = 0;
      unsigned int nAvgIndex;

      TFastLineSeries* pls;
      int n = (int)nTotalNum-1;

      // always go through ALL spikes
      while (n--)
         {
         // only current epoche??
         int nEpocheIndex = (int)formSpikeWare->m_swsSpikes.GetEpocheIndex(nChannelIndex, (unsigned int)n);
         if (cbPlotEpocheSpikesOnly->Checked && !!formSpikeWare->m_pformEpoches->tbEpoches->Tag)
            {
            if (nEpocheIndex != formSpikeWare->m_pformEpoches->tbEpoches->Position)
               continue;
            }

         // get group: it is >= 0 only, if in any selection on cluster plot
         nGroup = formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, (unsigned int)n);

         // spike must be either selected OR we are in free search mode (use all spikes)
         if (!formSpikeWare->m_bFreeSearchRunning && nGroup < 0)
            continue;

         // in freesearch mode we use m_vsaAverage[0] for averaging all spikes
         nAvgIndex = formSpikeWare->m_bFreeSearchRunning ? 0 : (unsigned int)nGroup;

         // add up counter for spikes found for this group
         m_vaSpikesFound[nAvgIndex] += 1;

         // check spike limit per group: do we already have plotted 'enough'?
         if (m_vsaAverage[nAvgIndex].m_nNumSpikes >= m_nMaxNumGroupSpikes)
            continue;

         // now check if this spike to be skipped due to position of 'back/forward'
         // scrollers (stored in tbtnSpikesBack->Tag)
         // only skip if not LAST bunch of spikes used in order to always plot last spikes
         // if other selection has more to scroll through....
         if (m_vaSpikesFound[nAvgIndex] + m_nMaxNumGroupSpikes < m_vaSpikesSelected[nAvgIndex])
            {
            if (m_vaSpikesFound[nAvgIndex] <= tbtnSpikesBack->Tag*m_nMaxNumGroupSpikes)
               continue;
            }

         // add up currently plotted spikes
         nSpikes++;

         // break here if m_nMaxNumSpikes reached
         if (nSpikes >= m_nMaxNumSpikes)
            {
            tbtnSpikesForward->Tag = n;
            break;
            }

         // NOTE: in the front we have the NumNonSpikeSeries:
         // chrt->Series[0] and chrt->Series[1] are dummies, + the averaging series!!
         pls = (TFastLineSeries*)chrt->Series[nSpikes+nNumNonSpikeSeries];
         pls->Clear();
         pls->SeriesColor = formSpikeWare->SpikeGroupToColor(nGroup, true);

         std::valarray<double >& rvad = formSpikeWare->m_swsSpikes.GetSpike(nChannelIndex, (unsigned int)n);

         m_vsaAverage[nAvgIndex].Add(rvad);


         // NOTE: the second parameter must be the index of the last item rather than the size
         // of the array (despite it's name). For this purpose we can use the SLICE macro
         pls->AddArray(SLICE(&rvad[0], (int)rvad.size()));
         pls->Active = true;
         }


      // calculazte averages and plot them
      for (i = 0; i < m_vsaAverage.size(); i++)
         {
         // any values contained at all?
         if (m_vsaAverage[i].m_nNumSpikes)
            {
            pls = (TFastLineSeries*)chrt->Series[NUM_DUMMY_SERIES+(int)i];
            pls->Clear();
            // calculate average
            m_vsaAverage[i].m_vad /= (double)m_vsaAverage[i].m_nNumSpikes;
            // add it to series
            pls->AddArray(SLICE(&m_vsaAverage[i].m_vad[0], (int)m_vsaAverage[i].m_vad.size()));
            // clear avaraging class: we have devided in place, so data would be invalid afterwards!
            m_vsaAverage[i].Reset();
            pls->Active = true;
            }
         }
      SetBackForwardEnabled();

      Caption = "Spikes - Channel " + IntToStr(Tag+1) + " (" + IntToStr(nSpikes) + ")";
      }
   __finally
      {
      m_nPlotCounter--;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikes::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if (!formSpikeWare->FormsCreated())
      return;
   if (Axis == chrt->LeftAxis)
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
   tbtnSpikesBack->Tag = 0;
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
   bool bReplot = false;
   int n = (int)tbtnSpikesBack->Tag;
   if (Sender == tbtnSpikesForward)
      {
      if (n > 0)
         {
         n--;
         bReplot = true;
         }
      }
   else if (BackIndexAllowed())
      {
      n++;
      bReplot = true;
      }
   if (bReplot)
      {
      tbtnSpikesBack->Tag = n;
      SetBackForwardEnabled();
      Plot((unsigned int)Tag);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// returns true if moving back one slice of plotted spikes is currently allowed
//------------------------------------------------------------------------------
bool TformSpikes::BackIndexAllowed(void)
{
   int nMax = m_vaSpikesSelected.max();
   return nMax > ((tbtnSpikesBack->Tag+1) * m_nMaxNumGroupSpikes);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Sets enabled-status of back/forward buttons
//------------------------------------------------------------------------------
void TformSpikes::SetBackForwardEnabled(void)
{
   bool bGeneral = !formSpikeWare->m_smp.Playing() && !cbPlotEpocheSpikesOnly->Checked;

   tbtnSpikesForward->Enabled = bGeneral && tbtnSpikesBack->Tag > 0;
   // we must NOT go back (any more) if the last index is -1 AND we are at the last
   // but one position
   tbtnSpikesBack->Enabled    = bGeneral && BackIndexAllowed();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns current name for spike limit ini entry
//------------------------------------------------------------------------------
UnicodeString TformSpikes::GetMaxSpikesIniEntry(void)
{
   if (m_mng == MNG_FREESEARCH)
      return "NumSpikesSearchMode";
   else if (m_mng == MNG_RESULT)
      return "MaxNumGroupSpikesResult";
   else
      return "MaxNumGroupSpikes";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Read and set spike display limit
//------------------------------------------------------------------------------
void TformSpikes::SetMaxSpikesMode(TMaxNumGroupSpikesMode mng)
{
   m_mng = mng;

   UnicodeString us = formSpikeWare->m_pIni->ReadString("Settings", GetMaxSpikesIniEntry(), cbNumSpikes->Items->Strings[1]);
   if (cbNumSpikes->Items->IndexOf(us) < 0)
      us = cbNumSpikes->Items->Strings[1];

   m_nMaxNumGroupSpikes = StrToInt(us);

   cbNumSpikes->ItemIndex = cbNumSpikes->Items->IndexOf(us);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Adjust spike display limit and store it
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikes::cbNumSpikesChange(TObject *Sender)
{
   int nNum;
   if (TryStrToInt(cbNumSpikes->Text, nNum))
      {
      m_nMaxNumGroupSpikes = nNum;
      if (!!Sender)
         {
         formSpikeWare->m_pIni->WriteString("Settings", GetMaxSpikesIniEntry(), cbNumSpikes->Text);
         m_nNumSpikesSelected = 0;
         Plot((unsigned int)Tag);
         }
      }
}
//------------------------------------------------------------------------------


