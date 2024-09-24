//------------------------------------------------------------------------------
/// \file frmEpoche.cpp
///
/// \author Berg
/// \brief Implementation of a form to show the spike train of one epoche
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

#include "frmEpoche.h"
#include "SpikeWareMain.h"
#include "frmEpocheWindow.h"
#include "frmSpikes.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initializes members, theshold and stimulus (length) series
//------------------------------------------------------------------------------
__fastcall TformEpoches::TformEpoches(TComponent* Owner, int nChannelIndex)
   : TForm(Owner)
{
   InitializeCriticalSection(&m_cs);
   Name        = "Epoches_" + IntToStr(nChannelIndex);
   Tag         = nChannelIndex;
   chrt->Title->Text->Text     = "Channel " + IntToStr(Tag+1);
   Parent = formSpikeWare->m_pformEpoches->scb;
   m_bMouseDown = false;
   csThreshold->SeriesColor = clRed;
   csThreshold->AddXY(0, 0, "", clTeeColor);
   csThreshold->AddXY(1, 0, "", clTeeColor);
   csThreshold->CustomHorizAxis = chrt->CustomAxes->Items[0];
   /*
   csEpocheThreshold->AddXY(0, 0, "", clTeeColor);
   csEpocheThreshold->AddXY(1, 0, "", clTeeColor);
   csEpocheThreshold->CustomHorizAxis = chrt->CustomAxes->Items[0];
   */

   m_dAverageCounter = 0.0;

   csStimSeries->Y0 = 0;
   csStimSeries->Y1 = 1;
   csStimSeries->X0 = 0;
   csStimSeries->X1 = 0;

   m_nPlotCounter = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor: cleanup
//------------------------------------------------------------------------------
__fastcall TformEpoches::~TformEpoches()
{
   formSpikeWare->StoreChartAxis(this, chrt);
   DeleteCriticalSection(&m_cs);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes chart properties and threshold values
//------------------------------------------------------------------------------
void TformEpoches::Initialize()
{
   chrt->BottomAxis->SetMinMax(0, formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0);
   chrt->TopAxis->SetMinMax(  MsToSamples(chrt->BottomAxis->Minimum, formSpikeWare->m_swsSpikes.GetSampleRate()),
                              MsToSamples(chrt->BottomAxis->Maximum, formSpikeWare->m_swsSpikes.GetSampleRate())
                           );
   formSpikeWare->RestoreChartAxis(this, chrt);
   UpdateThreshold(formSpikeWare->GetThreshold((unsigned int)Tag));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnShow callback. Re-initializes axes and sets top within 'owning' TformEpocheWindow
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::FormShow(TObject *Sender)
{
   chrt->TopAxis->SetMinMax(  MsToSamples(chrt->BottomAxis->Minimum, formSpikeWare->m_swsSpikes.GetSampleRate()),
                              MsToSamples(chrt->BottomAxis->Maximum, formSpikeWare->m_swsSpikes.GetSampleRate())
                           );
   Top = (int)Tag*Height +1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// brings window to front on mouse activation
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::FormMouseActivate(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y, int HitTest, TMouseActivate &MouseActivate)
{
   BringToFront();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots passed epoche
//------------------------------------------------------------------------------
void TformEpoches::Plot(TSWEpoche *pswe)
{
   if (!Visible)
      return;

   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;

   if (!TryEnterCriticalSection(&m_cs))
      return;

   try
      {
      m_nPlotCounter++;

      if (formSpikeWare->m_smp.Playing())
         {
         csStimSeries->X0 = MsToSamples(formSpikeWare->m_sweEpoches.m_dPreStimulus*1000.0, formSpikeWare->m_swsSpikes.GetSampleRate());
         // for free search subtract trigger offset!
         if (formSpikeWare->m_bFreeSearchRunning)
            csStimSeries->X0 -= MsToSamples(1000.0*formSpikeWare->m_smp.m_dTriggerLatency, formSpikeWare->m_swsStimuli.m_dDeviceSampleRate);
         csStimSeries->X1 = formSpikeWare->m_swsStimuli.m_swstStimuli[pswe->m_nStimIndex].m_nLength/formSpikeWare->m_swsSpikes.m_dSampleRateDevider + csStimSeries->X0;
         csStimSeries->Active = true;
         }
      else
         {
         csStimSeries->Active = false;
         }
      formSpikeWare->SetThreshold((unsigned int)Tag, pswe->m_vdThreshold[(unsigned int)Tag]);
         
      vvd vvdData = pswe->GetData();
      Plot(vvdData);
      }
   __finally
      {
      m_nPlotCounter--;
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots averaged double values of epoche (called by Plot(TSWEpoche *pswe))
//------------------------------------------------------------------------------
void TformEpoches::Plot(vvd &rvvdData)
{
   if ((int)rvvdData.size() <= Tag)
      return;

   if (!Visible)
      return;
   if (!TryEnterCriticalSection(&m_cs))
      return;
   try
      {
      if (m_vad.size() != rvvdData[(unsigned int)Tag].size())
         m_vad.resize(rvvdData[(unsigned int)Tag].size());

      if (tbtnAverage->Down && formSpikeWare->m_smp.Playing())
         {
         m_dAverageCounter += 1.0;
         m_vad *= (m_dAverageCounter-1.0)/m_dAverageCounter;
         m_vad += rvvdData[(unsigned int)Tag]/m_dAverageCounter;
         }
      else
         m_vad = rvvdData[(unsigned int)Tag];
      // NOTE: the second parameter must be the index of the last item rather than the size
      // of the array (despite it's name). For this purpose we can use the SLICE macro
      csEpoche->Clear();
      csEpoche->AddArray(SLICE((double const*)&m_vad[0], (int)m_vad.size()));
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// copies data of "own" channel into member m_vad
//------------------------------------------------------------------------------
void TformEpoches::SetData(vvd &rvvdData)
{
   if ((int)rvvdData.size() <= Tag)
      return;

   if (!TryEnterCriticalSection(&m_cs))
      return;
   try
      {
      if (m_vad.size() != rvvdData[(unsigned int)Tag].size())
         m_vad.resize(rvvdData[(unsigned int)Tag].size());
      m_vad = rvvdData[(unsigned int)Tag];
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots epoche data directly
//------------------------------------------------------------------------------
void TformEpoches::PlotData()
{
   csEpoche->Clear();
   csEpoche->AddArray(SLICE((double const*)&m_vad[0], (int)m_vad.size()));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears averaging buffer
//------------------------------------------------------------------------------
void TformEpoches::ClearAverage()
{
   EnterCriticalSection(&m_cs);
   try
      {
      m_dAverageCounter = 0.0;
      m_vad = 0.0;
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates threshold series
//------------------------------------------------------------------------------
void TformEpoches::UpdateThreshold(double dThreshold)
{
   csThreshold->YValues->Value[0] = dThreshold;
   csThreshold->YValues->Value[1] = dThreshold;
   csThreshold->Repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears epoche chart series
//------------------------------------------------------------------------------
void TformEpoches::Clear()
{
   csEpoche->Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseDown callback for chart: sets new threhold value, if Shift AND Ctrl 
/// are pressed
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::chrtMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
   if (!PtInRect(chrt->ChartRect, TPoint(X,Y)) || !formSpikeWare->FormsCreated())
      return;

   int nVirtKey = GetKeyState(VK_CONTROL);
   if ((nVirtKey & SHIFTED))
      return;
   nVirtKey = GetKeyState(VK_SHIFT);
   if (!(nVirtKey & SHIFTED))
      return;
   m_bMouseDown = true;

   if (formSpikeWare->m_pformEpoches->cbEpocheThreshold->Checked)
      {
      int nEpoche = formSpikeWare->m_pformEpoches->tbEpoches->Position;
      UnicodeString us = (int)formSpikeWare->m_swsSpikes.GetNumSpikes(0);
      us += ", ";

      // remove spikes of current epoche
      TSWEpoche* pswe = formSpikeWare->m_sweEpoches.Get(nEpoche);
      formSpikeWare->m_swsSpikes.Remove((unsigned int)Tag, (unsigned int)nEpoche);

      us += (int)formSpikeWare->m_swsSpikes.GetNumSpikes((unsigned int)Tag);
      us += ", ";

      // get thresholds of current epoche
      pswe->m_vdThreshold[(unsigned int)Tag] = csThreshold->YScreenToValue(Y);
      formSpikeWare->SetXMLEpocheThreshold(nEpoche, pswe->m_vdThreshold);


      formSpikeWare->m_swsSpikes.Add(pswe);

      us += (int)formSpikeWare->m_swsSpikes.GetNumSpikes((unsigned int)Tag);

      formSpikeWare->PlotSpikes();
      formSpikeWare->PlotClusters();
      formSpikeWare->PlotBubblePlots();
      formSpikeWare->PlotEpoches(pswe);

      }
   else
      formSpikeWare->SetThreshold((unsigned int)Tag, csThreshold->YScreenToValue(Y));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseLeave callback for chart: resets m_bMouseDown
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::chrtMouseLeave(TObject *Sender)
{
   m_bMouseDown = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseMove callback of chart: if in "set-threshold-mode" (see chrtMouseDown
/// then threshold is adjusted
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::chrtMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
   if (!formSpikeWare->FormsCreated())
      return;
   if (m_bMouseDown && !formSpikeWare->m_pformEpoches->cbEpocheThreshold->Checked)
      formSpikeWare->SetThreshold((unsigned int)Tag, csThreshold->YScreenToValue(Y));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseUp callback of chart: releases m_bMouseDown and tells main class that
/// measurement has changed
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::chrtMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
   m_bMouseDown = false;
   formSpikeWare->SetMeasurementChanged();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused#pragma argsused
void __fastcall TformEpoches::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if (!formSpikeWare->FormsCreated())
      return;

   if (Axis == chrt->BottomAxis)
      {
      // adjust top axis as well if bottom changed!
      if (formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis, 0, formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0, this))
         chrt->TopAxis->SetMinMax(  MsToSamples(Axis->Minimum, formSpikeWare->m_swsSpikes.GetSampleRate()),
                                    MsToSamples(Axis->Maximum, formSpikeWare->m_swsSpikes.GetSampleRate())
                                    );
      formSpikeWare->m_pformEpoches->SetAllAxis(this, false);
      }
   else if (Axis == chrt->LeftAxis)
      {
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wcomma"
      if (formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis, -1, 1), this)
         {
         formSpikeWare->m_pformEpoches->SetAllAxis(this, true);
         // if changed, then set identical on spikes form (only if spikes form shows
         // THIS channel!) OR if we adjust all axis anyway
         if (Tag == formSpikeWare->m_pformSpikes->Tag || formSpikeWare->m_pformEpoches->cbAllAxis->Checked)
            formSpikeWare->m_pformSpikes->chrt->LeftAxis->SetMinMax(chrt->LeftAxis->Minimum, chrt->LeftAxis->Maximum);

         // if changed, then set identical on all cluster plots
         formSpikeWare->ScaleClusterPlots((int)Tag);
         }
      #pragma clang diagnostic pop
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of chart: sets threshold directly if Shift + Ctrl are pressed
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::chrtClick(TObject *Sender)
{
   int nVirtKey = GetKeyState(VK_CONTROL);
   if ((nVirtKey & SHIFTED))
      {
      double d = formSpikeWare->GetThreshold((unsigned int)Tag);
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wcomma"
      if (formSpikeWare->m_pformSetParameters->SetParameter("Threshold", "rel.", d), this)
         {
         formSpikeWare->SetThreshold((unsigned int)Tag, d);
         formSpikeWare->SetMeasurementChanged();
         }
      #pragma clang diagnostic pop
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnClear: clears average
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::tbtnClearClick(TObject *Sender)
{
   ClearAverage();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// paints chartrect with special color, if tbtnEvalActive is pressed
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::chrtBeforeDrawAxes(TObject *Sender)
{
   if (!tbtnEvalActive->Down)
      return;
   chrt->Canvas->Brush->Color = (TColor)RGB(211, 230, 255);
   chrt->Canvas->Pen->Color   = chrt->Canvas->Brush->Color;
   chrt->Canvas->Rectangle(chrt->ChartRect);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnEvalActive: toggles tbtnEvalActive status and calls 
/// formSpikeWare->SetEvalWindow
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::tbtnEvalActiveClick(TObject *Sender)
{
   if (tbtnEvalActive->Down)
      formSpikeWare->SetEvalWindow(this);
   // if it's down already, don't allow up!!
   else
      tbtnEvalActive->Down = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbnListen: calls formSpikeWare->SetMonitor
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpoches::tbnListenClick(TObject *Sender)
{
   formSpikeWare->SetMonitor(this);
}
//------------------------------------------------------------------------------


