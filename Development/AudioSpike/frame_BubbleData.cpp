//------------------------------------------------------------------------------
/// \file frame_BubbleData.cpp
///
/// \author Berg
/// \brief Implementation of a TFrame used by TformBubbleData to show PSTH in
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

#include "frame_BubbleData.h"
#include "SpikeWareMain.h"
#include "frmPSTH.h"
#include "BubblePlotData.h"
#include "frmBubblePlot.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VCLTee.StatChar"
#pragma resource "*.dfm"

//------------------------------------------------------------------------------
/// default VCL contructor (not used)
//------------------------------------------------------------------------------
__fastcall TframeBubbleData::TframeBubbleData(TComponent* Owner)
   : TFrame(Owner), m_pthf(NULL)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// custom contructor , initializes memers and calls Initialize()
//------------------------------------------------------------------------------
__fastcall TframeBubbleData::TframeBubbleData(TComponent* Owner, int nType)
   : TFrame(Owner), m_bInitialized(false), m_pthf(NULL), m_nPlotCounter(0)
{
   Parent = dynamic_cast<TWinControl*>(Owner);
   Initialize(nType);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destrutor. Writes current NumBins to INI and does cleanup
//------------------------------------------------------------------------------
__fastcall TframeBubbleData::~TframeBubbleData()
{
   if (m_pthf)
      formSpikeWare->m_pIni->WriteInteger(Name, "NumBins", m_pthf->NumBins);
   TRYDELETENULL(m_pthf);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets up chart and THistogramFunction touse
//------------------------------------------------------------------------------
void TframeBubbleData::Initialize(int nType)
{
   if (m_bInitialized)
      return;

   m_nChartType      = nType;

   TRYDELETENULL(m_pthf);
   m_pthf = new THistogramFunction(this);
   m_pthf->DataStyle = hdsTruncate;
   m_pthf->Cumulative = false;

   csData->FunctionType = m_pthf;
   csData->SetFunction(m_pthf);
   csData->Marks->Visible = false;

   chrt->LeftAxis->Minimum = 0;

   if (m_nChartType == BPSTH_PERIOD)
      {
      Name = "BubblePeriodHistogram";
      m_usName = "Period-Histogram";
      chrt->BottomAxis->Title->Caption = "Period";
      chrt->LeftAxis->Title->Caption = "rel. Frequency";
      csNormalizedData->Visible = true;
      csData->Visible = false;
      chrt->BottomAxis->SetMinMax(0, 1);
      m_pthf->NumBins = formSpikeWare->m_pIni->ReadInteger(Name, "NumBins", 100);
      // HIER
      SetNumBins(100);
      chrt->LeftAxis->AutomaticMaximum = false;
      }
   else
      {
      Name = "BubblePSTH";
      m_usName = "PSTH";
      csData->SeriesColor = m_nChartType ? clAqua : clLime;
      chrt->BottomAxis->SetMinMax(0, formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0);
      m_pthf->NumBins = formSpikeWare->m_pIni->ReadInteger(Name, "NumBins", 100);
      // HIER
      SetNumBins(1000);
      chrt->LeftAxis->AutomaticMaximum = true;
      }

   chrt->Title->Text->Text = m_usName;

   if (m_nChartType != BPSTH_PERIOD)
      chrt->BottomAxis->SetMinMax(0, formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0);
   ShowBinSize();
   m_bInitialized = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets bin size and adjusts display
//------------------------------------------------------------------------------
void TframeBubbleData::SetNumBins(int n)
{
   m_pthf->NumBins = n;

   if (m_nChartType == BPSTH_PERIOD)
      NormalizeHistogram();

   ShowBinSize();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots passed TBubbleData
//------------------------------------------------------------------------------
void TframeBubbleData::Plot(TBubbleData& rbd, UnicodeString usInfo)
{
   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;

   try
      {
      m_nPlotCounter++;
      if (m_nChartType != BPSTH_PERIOD)
         chrt->BottomAxis->SetMinMax(0, formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0);
      ShowBinSize();

      // access spike times or cycles respectively
      std::vector<double >& rvd = (m_nChartType == BPSTH_STANDARD) ? rbd.m_vdSpikeTimes : rbd.m_vdSpikeCycles;

      csData->Clear();
      csNormalizedData->Clear();
      csPoints->Clear();

      if (!usInfo.IsEmpty())
         chrt->Title->Text->Text = m_usName + " - " + usInfo;

      unsigned int nNum = (unsigned int)rvd.size();
      if (!nNum)
         return;

      // scale seconds to milliseconds for spiketime
      double dMultiplier = 1.0;
      if (m_nChartType == BPSTH_STANDARD)
         dMultiplier = 1000.0;

      unsigned int n;
      for (n = 0; n < nNum; n++)
         {
         csPoints->AddY(rvd[n] * dMultiplier);
         }

      csData->DataSources->Clear();
      csData->DataSources->Add(csPoints);

      if (m_nChartType == BPSTH_PERIOD)
         NormalizeHistogram();
      }
   __finally
      {
      m_nPlotCounter--;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// normalizes histogram data
//------------------------------------------------------------------------------
void TframeBubbleData::NormalizeHistogram()
{
   double dx = csData->YValues->Total;
   csNormalizedData->AssignValues(csData);
   int n;
   for (n = 0; n < csNormalizedData->YValues->Count; n++)
      csNormalizedData->YValues->Value[n] /= dx;
   double dMaxY = floor(csNormalizedData->YValues->MaxValue * 10.0) / 10.0;
   if (dMaxY < 0.1)
      dMaxY = 0.1;
   chrt->LeftAxis->Maximum = dMaxY;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears data
//------------------------------------------------------------------------------
void TframeBubbleData::Clear()
{
   csPoints->Clear();
   csData->Clear();
   csNormalizedData->Clear();
   chrt->LeftAxis->Minimum = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// raises bin size
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TframeBubbleData::tbtnZoomOutClick(TObject *Sender)
{
   if (m_pthf->NumBins < 1000)
      SetNumBins(m_pthf->NumBins * 2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// lowers bin size
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TframeBubbleData::tbtnZoomInClick(TObject *Sender)
{
   if (m_pthf->NumBins > 2)
      SetNumBins(m_pthf->NumBins / 2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows dialog to set bin size 
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TframeBubbleData::tbtnBinSizeClick(TObject *Sender)
{
   double dValue = formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / (double)m_pthf->NumBins;
   dValue = (double)StrToFloat(FormatFloat("0.00", (Extended)dValue));

   if (!formSpikeWare->m_pformSetParameters->SetParameter("Binsize", "ms", dValue, NULL))
      return;

   SetNumBins((int)floor(formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / dValue));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TframeBubbleData::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
   if (Axis != chrt->BottomAxis)
      return;

   formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis,
                                                      0,
                                                      formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0,
                                                      NULL);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows current bin size and adjusts histogram display properties
//------------------------------------------------------------------------------
void  TframeBubbleData::ShowBinSize()
{
  pnlBinSize->Caption = "binsize: " + FormatFloat("0.00", (Extended)(formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / (double)m_pthf->NumBins)) + " ms";;
  csData->LinesPen->Visible = m_pthf->NumBins <= 100;
  csData->LinePen->Visible = csData->LinesPen->Visible;
}
//------------------------------------------------------------------------------



