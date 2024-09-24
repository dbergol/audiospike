//------------------------------------------------------------------------------
/// \file frmSignalPSTH.cpp
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
#include <vcl.h>
#pragma hdrstop

#include "frmSignalPSTH.h"
#include "SpikeWareMain.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VCLTee.StatChar"
#pragma link "frmASUI"
#pragma resource "*.dfm"
#pragma warn -aus
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor, initializes members
//------------------------------------------------------------------------------
__fastcall TformSignalPSTH::TformSignalPSTH(TComponent* Owner, TMenuItem* pmi)
   : TformASUI(Owner, pmi), m_nPlotCounter(0)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, saves settings and does cleanup
//------------------------------------------------------------------------------
__fastcall TformSignalPSTH::~TformSignalPSTH()
{
   if (m_vpthf.size())
      formSpikeWare->m_pIni->WriteInteger(Name, "NumBins", m_vpthf[0]->NumBins);
   CleanupChart();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback. Calls base class ad Plot()
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSignalPSTH::FormShow(TObject *Sender)
{
   TformASUI::FormShow(Sender);
   Plot((unsigned int)Tag);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears all histogram functions and chart series
//------------------------------------------------------------------------------
void TformSignalPSTH::CleanupChart(void)
{
   // remove all histogram functions
   unsigned int n;
   for (n = 0; n < m_vpthf.size(); n++)
      {
      TRYDELETENULL(m_vpthf[n]);
      }
   m_vpthf.clear();

   // remove all series
   TChartSeries *pcs;
   while (chrt->SeriesList->Count)
      {
      pcs = chrt->Series[0];
      chrt->RemoveSeries(pcs);
      TRYDELETENULL(pcs);
      }

   // remove all custom axis
   while (chrt->CustomAxes->Count)
      chrt->CustomAxes->Delete(0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initailization: creates histogram functions and chart series for all stimuli
//------------------------------------------------------------------------------
void TformSignalPSTH::Initialize()
{
   CleanupChart();
   InitAxes();

   int n;
   int nNumStim = (int)formSpikeWare->m_swsStimuli.m_swstStimuli.size();
   int nNumBins = formSpikeWare->m_pIni->ReadInteger(Name, "NumBins", 100);

   // create chart series and histogram functions
   THistogramSeries*    phs;
   TPointSeries*        pps;
   THistogramFunction*  phf;
   for (n = 0; n < nNumStim; n++)
      {
      phf = new THistogramFunction(NULL);
      phf->DataStyle = hdsTruncate;
      phf->Cumulative = false;
      phf->NumBins = nNumBins;
      m_vpthf.push_back(phf);

      phs = new THistogramSeries(NULL);
      phs->Selected->Hover->Visible = false;
      phs->ParentChart            = chrt;
      phs->Name                   = "Hist_" + IntToStr(n);;
      phs->SeriesColor = clGreen;
      phs->LinesPen->Color = clBlack;
      phs->LinesPen->Visible = true;
      phs->Marks->Visible = false;
      phs->XValues->Order = loAscending;
      phs->YValues->Order = loNone;
      phs->FunctionType = phf;
      phs->SetFunction(phf);
      phs->XValues->Order = loAscending;
      phs->YValues->Order = loNone;

      pps = new TPointSeries(NULL);
      pps->ParentChart            = chrt;
      pps->Name                   = "Point_" + IntToStr(n);;
      pps->Visible = false;
      pps->XValues->Order = loAscending;
      pps->YValues->Order = loNone;
      }

   chrt->BottomAxis->SetMinMax(0, formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0);
   ShowBinSize();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Create custom axis, one per stimulus
//------------------------------------------------------------------------------
void  TformSignalPSTH::InitAxes()
{
   chrt->LeftAxis->Visible = false;

   // create custom axes
   int n;
   int nNumAxes = (int)formSpikeWare->m_swsStimuli.m_swstStimuli.size();
   double dInc = 100.0/(double)nNumAxes;
   double dPos = 0.0;
   TChartAxis* pca;
   for (n = 0; n < nNumAxes; n++)
      {
      pca = chrt->CustomAxes->Add();
      pca->Minimum = 0;
      pca->Automatic = false;
      pca->AutomaticMinimum = false;
      pca->AutomaticMaximum = false;
      pca->AxisValuesFormat = "###0.###";
      pca->MaximumOffset = 10;
      pca->MinorTicks->Visible = false;
      pca->StartPosition = dPos;
      pca->Labels = false;
      dPos += dInc;
      pca->EndPosition = dPos;

      //pca->Title->Caption = IntToStr(n+1);
      pca->Title->Angle = 0;
      pca->Title->Font->Size = 8;
      pca->LabelsSize = 0;
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots PSTH
//------------------------------------------------------------------------------
void TformSignalPSTH::Plot(unsigned int nChannelIndex)
{
   Tag = (NativeInt)nChannelIndex;
   if (!Visible)
      return;
   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;
   try
      {
      m_nPlotCounter++;
      double dMax = 0;
      Caption = "Stimulus-PSTH - Channel " + IntToStr((int)nChannelIndex+1);
      int nStimIndex;
      int nNumStim = (int)m_vpthf.size();
      TChartAxis* pca;
      for (nStimIndex = 0; nStimIndex < nNumStim; nStimIndex ++)
         {
         TChartSeries* csData    = chrt->Series[2*nStimIndex];
         TChartSeries* csPoints  = chrt->Series[2*nStimIndex+1];
         if (!csData->Visible)
            continue;
         csData->Clear();
         csPoints->Clear();

         pca = chrt->CustomAxes->Items[nStimIndex];
         csData->CustomVertAxis     = pca;
         csPoints->CustomVertAxis   = pca;
         // SAME MAX FOR ALL!!
//         pca->AutomaticMaximum = true;

         unsigned int nNum = formSpikeWare->m_swsSpikes.GetNumSpikes(nChannelIndex);
         if (!nNum)
            return;
         unsigned int n;
         for (n = 0; n < nNum; n++)
            {
            if (formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, n) < 0)
               continue;
            if (nStimIndex != (int)formSpikeWare->m_swsSpikes.GetStimIndex(nChannelIndex, n))
               continue;
            csPoints->AddY(formSpikeWare->m_swsSpikes.GetSpikeTime(nChannelIndex, n)*1000.0);
            }

         csData->DataSources->Clear();
         csData->DataSources->Add(csPoints);
         OutputDebugStringW(FloatToStr((Extended)csData->MaxYValue()).w_str());
         if (csData->MaxYValue() > dMax)
            dMax = csData->MaxYValue();
         }

      AdjustYMax(dMax);

      }
   __finally
      {
      m_nPlotCounter--;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears all series
//------------------------------------------------------------------------------
void TformSignalPSTH::Clear()
{
   int n;
   for  (n = 0; n < chrt->SeriesList->Count; n++)
      chrt->Series[n]->Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnZoomOut: raises binsize
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSignalPSTH::tbtnZoomOutClick(TObject *Sender)
{
   if (!m_vpthf.size())
      return;
   if (m_vpthf[0]->NumBins >= 1000)
      return;

   SetBinSize(m_vpthf[0]->NumBins*2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnZoomIn: lowers binsize
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSignalPSTH::tbtnZoomInClick(TObject *Sender)
{
   if (!m_vpthf.size())
      return;
   if (m_vpthf[0]->NumBins <= 2)
      return;
   SetBinSize(m_vpthf[0]->NumBins/2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Shows dialog to set binsize by value
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSignalPSTH::tbtnBinSizeClick(TObject *Sender)
{
   if (!m_vpthf.size())
      return;
   //
   double dValue = formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / (double)m_vpthf[0]->NumBins;
   dValue = (double)StrToFloat(FormatFloat("0.00", (Extended)dValue));

   if (!formSpikeWare->m_pformSetParameters->SetParameter("Binsize", "ms", dValue, this))
      return;

   int nNumBins = (int)floor(formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / dValue);
   SetBinSize(nNumBins);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets binsize in all histogram functions
//------------------------------------------------------------------------------
void TformSignalPSTH::SetBinSize(int nBinSize)
{
   if (!m_vpthf.size())
      return;
   unsigned int n;
   for (n = 0; n < m_vpthf.size(); n++)
      m_vpthf[n]->NumBins = nBinSize;
   formSpikeWare->m_pIni->WriteInteger(Name, "NumBins", (int)n);

   AdjustYMax();
   ShowBinSize();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows binsize on label
//------------------------------------------------------------------------------
void  TformSignalPSTH::ShowBinSize()
{
   int nNumBins = m_vpthf.size() ? m_vpthf[0]->NumBins : 0;
   pnlBinSize->Caption = "binsize: " + FormatFloat("0.00", (Extended)(formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / nNumBins)) + " ms";;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adjusts YMax for all custom axis
//------------------------------------------------------------------------------
void TformSignalPSTH::AdjustYMax(double dMax)
{
   int n;
   if (dMax < 0.0)
      {
      double d;
      dMax = 1.0;
      for (n = 0; n < (int)m_vpthf.size(); n++)
         {
         d = chrt->Series[2*n]->MaxYValue();
         if (d > dMax)
            dMax = d;
         }
      }

   for (n = 0; n < chrt->CustomAxes->Count; n++)
      chrt->CustomAxes->Items[n]->Maximum = dMax;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSignalPSTH::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
   if (Axis != chrt->BottomAxis)
      return;
   formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis,
                                                      0,
                                                      formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0,
                                                      this);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnBeforeDrawSeries calback for chart: draws 'baseline' for each custom Y-Axis 
/// and writes stimulus index
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSignalPSTH::chrtBeforeDrawSeries(TObject *Sender)
{
   int n, nY;
   UnicodeString us;
   for (n = 0; n < chrt->CustomAxes->Count; n++)
      {
      chrt->Canvas->Pen->Color = clGray;
      nY = chrt->CustomAxes->Items[n]->CalcYPosValue(0);
      chrt->Canvas->MoveTo(chrt->ChartRect.Left, nY);
      chrt->Canvas->LineTo(chrt->ChartRect.Right, nY);

      us = IntToStr(n+1);
      chrt->Canvas->Pen->Color = clBlack;
      chrt->Canvas->Brush->Color = clWhite;
      nY =  chrt->CustomAxes->Items[n]->CalcYPosValue(chrt->CustomAxes->Items[n]->Maximum / 2)
         - chrt->Canvas->TextHeight(us);
      chrt->Canvas->TextOut(chrt->ChartRect.Left-14, nY, us);
      }
}
//------------------------------------------------------------------------------




