//------------------------------------------------------------------------------
/// \file frmBubblePlot.cpp
///
/// \author Berg
/// \brief Implementation of a form to show a bubble plot
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
#include <algorithm>
#pragma hdrstop

#include "frmBubblePlot.h"
#include "SpikeWareMain.h"
#include "frmPSTH.h"
#include "frmVectorStrength.h"
#include "frmStimuli.h"
#include "frmBubbleData.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initilizes members and sets chart properties
//------------------------------------------------------------------------------
__fastcall TformBubblePlot::TformBubblePlot( TComponent* Owner, 
                                             unsigned int nParamX, 
                                             unsigned int nParamY, 
                                             bool bSearch)
   :  TformASUI(Owner), 
      m_bSearch(bSearch), 
      m_nParamX(nParamX), 
      m_nParamY(nParamY), 
      m_nPlotCounter(0)
{
   Name =  formSpikeWare->ParameterWindowName(nParamX, nParamY);
   // if the index of the Y-axis is one higher (!) than the last parameter, then
   // it is the 'response' ather than a read stimulus parameter!
   m_bResponseYAxis = (m_nParamY == formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusNames.size());
   csResponseData->Active = m_bResponseYAxis;
   csDisplayData->Active  = !m_bResponseYAxis;

   cbOnlyActiveBubble->Visible = m_bSearch;

   if (m_bSearch)
      HelpFile = "formSearchParam";
   else
      HelpFile = Name;

   // don't remove order!!!
   csDisplayData->XValues->Order       = loNone;
   csDisplayData->YValues->Order       = loNone;
   csDisplayData->RadiusValues->Order  = loNone;
   m_clBubbleColor = m_bSearch ? clBlue : clGreen;

   csResponseData->XValues->Order = loNone;
   csResponseData->YValues->Order = loNone;
   csResponseData->Pointer->Visible         = true;
   csResponseData->Pointer->Style           = psCircle;
   csResponseData->Pointer->VertSize        = 5;
   csResponseData->Pointer->HorizSize       = 5;
   csResponseData->Pointer->InflateMargins  = True;
   csResponseData->Pointer->Visible         = true;

   m_nLastBubbleIndex = -1;

   // call RestoreFormPos again due to 'HelpFile' that corresponds to section name!
   formSpikeWare->RestoreFormPos(this);
   chrt->Foot->Text->Text = " ";
   Initialize();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor. empty ...
//------------------------------------------------------------------------------
__fastcall TformBubblePlot::~TformBubblePlot()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnClose callback. Calls baseclass and calls window removing function
//------------------------------------------------------------------------------
void __fastcall TformBubblePlot::FormClose(TObject *Sender, TCloseAction &Action)
{
   TformASUI::FormClose(Sender, Action);
   if (!m_bSearch)
      formSpikeWare->RemoveParamWindow(this);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes plot with properties from measurment
//------------------------------------------------------------------------------
void TformBubblePlot::Initialize()
{
   m_dSelLen = -1.0;

   chrt->BottomAxis->Title->Caption = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusNames[m_nParamX]
                                      + " [" + formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamX] + "]";
   chrt->BottomAxis->Logarithmic =  formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbLog[m_nParamX]
                                 && !formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbString[m_nParamX];

   if (formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbString[m_nParamX])
      chrt->BottomAxis->Increment = 1;


   if (m_bResponseYAxis)
      {
      chrt->LeftAxis->Title->Caption   = "Response";
      chrt->LeftAxis->Logarithmic      = false;
      chrt->LeftAxis->AutomaticMaximum = true;
      chrt->LeftAxis->Minimum = 0;

      chrt->BottomAxis->SetMinMax(m_dXMinDefault, m_dXMaxDefault);

      csResponseData->Clear();
      unsigned int n;
      unsigned int nXSize = (unsigned int)formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX].size();
      for (n = 0; n < nXSize; n++)
         csResponseData->AddXY(formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX][n], 0);
      m_dXMinDefault = csResponseData->XValues->MinValue;
      m_dXMaxDefault = csResponseData->XValues->MaxValue;
      chrt->BottomAxis->SetMinMax(m_dXMinDefault, m_dXMaxDefault);
      csResponseData->Visible = false;
      }
   else
      {
      chrt->LeftAxis->Title->Caption   = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusNames[m_nParamY]
                                         + " [" + formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamY] + "]";
      chrt->LeftAxis->Logarithmic   =  formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbLog[m_nParamY]
                                    && !formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbString[m_nParamY];

      if (formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbString[m_nParamY])
         chrt->LeftAxis->Increment = 1;


      // set properties of bubbledata
      double dFreqMultiplier = 1.0;
      m_bpd.Reset();
      m_bpd.m_nFreqAxis = formSpikeWare->m_swsStimuli.m_swspStimPars.IndexFromName("FREQUENCY");
      if (m_bpd.m_nFreqAxis != (int)m_nParamX && m_bpd.m_nFreqAxis != (int)m_nParamY)
         m_bpd.m_nFreqAxis = -1;
      if (m_bpd.m_nFreqAxis > -1)
         {
         // try try check for kHz or MHz .....
         UnicodeString usUnit = UpperCase(formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[(unsigned int)m_bpd.m_nFreqAxis]);
         if (usUnit == "KHZ")
            dFreqMultiplier = 1000.0;
         else if (usUnit == "MHZ")
            dFreqMultiplier = 1000000.0;
         }

      // add all possible bubbles with radius 0: needed to show axis fine and to reverse lookup
      // X and Y for a particular buble
      unsigned int n, m;
      double dX, dY;
      for (n = 0; n < formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX].size(); n++)
         {
         for (m = 0; m < formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamY].size(); m++)
            {
            csDisplayData->AddBubble(  formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX][n],
                                       formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamY][m],
                                       0);

            m_bpd.m_vBubbleData.push_back(TBubbleData());

            if (m_bpd.m_nFreqAxis == (int)m_nParamX)
               {
               m_bpd.m_vBubbleData.back().m_dFrequency      = dFreqMultiplier * formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX][n];
               m_bpd.m_vBubbleData.back().m_dNonFreqValue   = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamY][m];
               }
            else if (m_bpd.m_nFreqAxis == (int)m_nParamY)
               {
               m_bpd.m_vBubbleData.back().m_dFrequency = dFreqMultiplier * formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamY][m];
               m_bpd.m_vBubbleData.back().m_dNonFreqValue   = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX][n];
               }
            }
         }
      // set size of data valarray
      m_vadData.resize( formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX].size()
                       *formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamY].size());

      if (m_bSearch)
         {
         m_nSearchStim  = 0;
         m_vadData      = -1.0;
         m_vadData[0]   = 1.0;
         }
      m_dXMinDefault = csDisplayData->XValues->MinValue;
      m_dXMaxDefault = csDisplayData->XValues->MaxValue;
      m_dYMinDefault = csDisplayData->YValues->MinValue;
      m_dYMaxDefault = csDisplayData->YValues->MaxValue;
      chrt->LeftAxis->SetMinMax(m_dYMinDefault, m_dYMaxDefault);
      chrt->BottomAxis->SetMinMax(m_dXMinDefault, m_dXMaxDefault);

      }

   AdjustLogAxis(chrt->LeftAxis);
   AdjustLogAxis(chrt->BottomAxis);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots/updates the bubbles
//------------------------------------------------------------------------------
void TformBubblePlot::Plot(unsigned int nChannelIndex)
{
   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;
   m_nPlotCounter++;

   TStringList *psl = NULL;
   try
      {
      if (m_bResponseYAxis)
         {
         PlotResponse(nChannelIndex);
         csResponseData->Visible = true;
         return;
         }

      psl = new TStringList();

      double dPreStimulus = formSpikeWare->m_sweEpoches.m_dPreStimulus;
      Tag = (NativeInt)nChannelIndex;

      m_bpd.Clear();

      double d;
      try
         {
         Caption = "Parameters - Channel " + IntToStr((int)nChannelIndex+1);

         // clear radius data
         m_vadData  = 0.0;
         if (!formSpikeWare->m_pformPSTH->Selected() && !formSpikeWare->m_pformPSTH->NoiseSelected())
            return;

         // retrieve selections from PSTH plot
         double dMin, dMax, dNoiseMin, dNoiseMax;
         formSpikeWare->m_pformPSTH->GetSelections(dMin, dMax, dNoiseMin, dNoiseMax);
         m_dSelLen = dMax - dMin;


         // get references to stimulus values and their sizes once before loop
         std::vector<double >& rvdXParamValues = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX];
         std::vector<double >& rvdYParamValues = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamY];
         unsigned int nXSize = (unsigned int)formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX].size();
         unsigned int nYSize = (unsigned int)formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamY].size();

         double dSpikeTime, dMod;
         unsigned int nSpike, nX, nY, nBubbleIndex;
         unsigned int nNum = formSpikeWare->m_swsSpikes.GetNumSpikes(nChannelIndex);

         for (nSpike = 0; nSpike < nNum; nSpike++)
            {
            // check if spike selected at all!
            if (formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, nSpike) < 0)
               continue;

            // retrieve spiketime
            dSpikeTime = formSpikeWare->m_swsSpikes.GetSpikeTime(nChannelIndex, nSpike);

            // in the spike we have stored m_nStimIndex which points to the stimulus, that evoked
            // this spike. In this stimulus the corresponding stimulus parameters are stored.
            // Get a reference to the stimulus parameters
            std::vector<double >& rvdParams =
               formSpikeWare->m_swsStimuli.m_swstStimuli[formSpikeWare->m_swsSpikes.GetStimIndex(nChannelIndex, nSpike)].m_vdParams;

            // now find the indices within the parameter values
            nX = (unsigned int)(int)(std::find(rvdXParamValues.begin(), rvdXParamValues.end(), rvdParams[m_nParamX]) - rvdXParamValues.begin());
            nY = (unsigned int)(int)(std::find(rvdYParamValues.begin(), rvdYParamValues.end(), rvdParams[m_nParamY]) - rvdYParamValues.begin());
            // not found? (should not happen)
            if (nX >= nXSize || nY >= nYSize)
               continue;

            nBubbleIndex = nX*nYSize + nY;
            
            // push all (!) spiketimes (not only those selected in PSTH)
            m_bpd.m_vBubbleData[nBubbleIndex].m_vdSpikeTimes.push_back(dSpikeTime);

            if (formSpikeWare->m_pformPSTH->Selected())
               {
               // increase bubble size and add bubble values
               if (dSpikeTime >= dMin && dSpikeTime <= dMax)
                  {
                  // push only spike cycles selected in PSTH!!
                  // NOTE: here we subtract PreStimulus from Spiketime to get correct phase!!
                  if (m_bpd.HasFrequency())
                     {
                     m_bpd.m_vBubbleData[nBubbleIndex].m_vdSpikeCycles.push_back(
                        modf((dSpikeTime - dPreStimulus)* m_bpd.m_vBubbleData[nBubbleIndex].m_dFrequency, &dMod)
                        );
                     }

                  //increase bubble size
                  m_vadData[nBubbleIndex] += 1;
                  }
               }
            if (formSpikeWare->m_pformPSTH->NoiseSelected())
               {
               // decrease bubble size
               if (dSpikeTime >= dNoiseMin && dSpikeTime <= dNoiseMax)
                  m_vadData[nBubbleIndex] -= 1;
               }
            }

         if (m_bpd.HasFrequency())
            m_bpd.CalculateVectorStrength();
         }
      __finally
         {

         if (!m_bResponseYAxis)
            {
            double dEpoches = formSpikeWare->m_swsStimuli.m_nNumRepetitions;
            // show value of maximum bubble
            double dValue = GetAbsMax();
            UnicodeString us1, us2;
            us1 = IntToStr((int)dValue);
            if (dEpoches > 0)
               {
               us2.printf(L"%.1f", (dValue / dEpoches));
               if (m_dSelLen > 0.0)
                  {
                  us1 = us1 + " (" + IntToStr((int)floor(dValue / m_dSelLen)) + "/s)";
                  us2.printf(L"%ls (%.1f/s)", us2.w_str(), (double)(dValue / m_dSelLen / dEpoches));
                  }
               }
            psl->Add("Maximum Bubble (sum): " + us1);
            psl->Add("Maximum Bubble (per repetition): " + us2);

            chrt->Title->Text->Text = psl->Text;
            }
         }

      // we have to re-plot period histogram only, since bubble PSTH uses all
      // spikes, not only PSTH selection...
      if (m_bpd.HasFrequency() && m_nLastBubbleIndex >= 0)
         formSpikeWare->m_pformBubbleData->UpdatePeriodHistogram(m_bpd.m_vBubbleData[(unsigned int)m_nLastBubbleIndex]);
      }
   __finally
      {
      m_nPlotCounter--;
      TRYDELETENULL(psl);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// called by Plot() to plot special things if Y-Axis is a response axis
//------------------------------------------------------------------------------
void TformBubblePlot::PlotResponse(unsigned int nChannelIndex)
{
   Tag = (NativeInt)nChannelIndex;

   double d;
   Caption = "Parameters - Channel " + IntToStr((int)nChannelIndex+1);

   if (!formSpikeWare->m_pformPSTH->Selected() && !formSpikeWare->m_pformPSTH->NoiseSelected())
      return;

   // reset data
   int n;
   for (n = 0; n < csResponseData->YValues->Count; n++)
      csResponseData->YValues->Value[n] = 0;

   // retrieve selections from PSTH plot
   double dMin, dMax, dNoiseMin, dNoiseMax;
   formSpikeWare->m_pformPSTH->GetSelections(dMin, dMax, dNoiseMin, dNoiseMax);
   m_dSelLen = dMax - dMin;

   // get references to stimulus value and the sizes once before loop
   std::vector<double >& rvdXParamValues = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX];
   unsigned int nXSize = (unsigned int)formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[m_nParamX].size();

   double dSpikeTime;
   unsigned int nSpike, nX;
   unsigned int nNum = formSpikeWare->m_swsSpikes.GetNumSpikes(nChannelIndex);
   for (nSpike = 0; nSpike < nNum; nSpike++)
      {
      // check if spike selected at all!
      if (formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, nSpike) < 0)
         continue;

      // retrieve spiketime
      dSpikeTime = formSpikeWare->m_swsSpikes.GetSpikeTime(nChannelIndex, nSpike);

      // in the spike we have stored m_nStimIndex which points to the stimulus, that evoked
      // this spike. In this stimulus the corresponding stimulus parameters are stored.
      // Get a reference to the stimulus parameters
      std::vector<double >& rvdParams =
         formSpikeWare->m_swsStimuli.m_swstStimuli[formSpikeWare->m_swsSpikes.GetStimIndex(nChannelIndex, nSpike)].m_vdParams;

      // now find the indices within the parameter values
      nX = (unsigned int)(int)(std::find(rvdXParamValues.begin(), rvdXParamValues.end(), rvdParams[m_nParamX]) - rvdXParamValues.begin());
      // not found? (should not happen
      if (nX >= nXSize)
         continue;

      if (formSpikeWare->m_pformPSTH->Selected())
         {
         // increase bubble size
         if (dSpikeTime >= dMin && dSpikeTime <= dMax)
            {
            csResponseData->YValues->Value[(int)nX] += 1;
            }
         }
      if (formSpikeWare->m_pformPSTH->NoiseSelected())
         {
         // increase bubble size
         if (dSpikeTime >= dNoiseMin && dSpikeTime <= dNoiseMax)
            {
            csResponseData->YValues->Value[(int)nX] -= 1;
            }
         }
      }
      csResponseData->Repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears data member
//------------------------------------------------------------------------------
void TformBubblePlot::Clear()
{
   m_vadData.resize(0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubblePlot::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   double      dMin = Axis == chrt->BottomAxis ? m_dXMinDefault : m_dYMinDefault;
   double      dMax = Axis == chrt->BottomAxis ? m_dXMaxDefault : m_dYMaxDefault;

   formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis, dMin, dMax, this);
   AdjustLogAxis(Axis);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns may of fabs of data
//------------------------------------------------------------------------------
double TformBubblePlot::GetAbsMax()
{
   double dMin = fabs(m_vadData.min());
   double dMax = fabs(m_vadData.max());
   return dMin > dMax ? dMin : dMax;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// here the real bubble is painted to have custom sizes
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubblePlot::chrtAfterDraw(TObject *Sender)
{
   if (!m_vadData.size())
      return;

   int nAvail = chrt->ChartRect.Width();
   if (nAvail > chrt->ChartRect.Height())
      nAvail = chrt->ChartRect.Height();

   // largest bubble should be 20% of space
   nAvail /= 20;
   if (!nAvail)
      return;

   double dMax = GetAbsMax();
   if (dMax == 0.0)
      return;

   double dScale = (double)nAvail / dMax;
   if (m_bSearch)
      dScale /= 2.0;
   int n, nRadius;
   int nX, nY;
   for (n = 0; n < csDisplayData->XValues->Count; n++)
      {
      if (cbOnlyActiveBubble->Visible && cbOnlyActiveBubble->Checked && m_vadData[(unsigned int)n] <= 0)
         continue;
      nX = chrt->BottomAxis->CalcXPosValue(csDisplayData->XValues->Value[n]);
      nY = chrt->LeftAxis->CalcYPosValue(csDisplayData->YValues->Value[n]);
      nRadius = (int)(dScale * fabs(m_vadData[(unsigned int)n]));
      chrt->Canvas->Brush->Color = m_vadData[(unsigned int)n] > 0 ? m_clBubbleColor :  clWhite;
      chrt->Canvas->Ellipse(nX - nRadius, nY - nRadius, nX + nRadius, nY + nRadius);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// draw custom ticks/lines
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubblePlot::chrtBeforeDrawSeries(TObject *Sender)
{
   if (!m_vadData.size())
      return;
   chrt->Canvas->Pen->Color = chrt->BottomAxis->MinorGrid->Color;
   int n, nValue;
   double dValue;
   if (chrt->BottomAxis->Logarithmic)
      {
      for (n = 0; n < chrt->BottomAxis->Items->Count; n++)
         {
         dValue = chrt->BottomAxis->Items->Item[n]->Value;
         if (dValue <= chrt->BottomAxis->Minimum || dValue >= chrt->BottomAxis->Maximum)
            continue;
         nValue = chrt->BottomAxis->CalcPosValue(dValue);
         chrt->Canvas->MoveTo(nValue , chrt->ChartRect.Top);
         chrt->Canvas->LineTo(nValue , chrt->ChartRect.Bottom);
         }
      }
   if (chrt->LeftAxis->Logarithmic)
      {
      for (n = 0; n < chrt->LeftAxis->Items->Count; n++)
         {
         dValue = chrt->LeftAxis->Items->Item[n]->Value;
         if (dValue <= chrt->LeftAxis->Minimum || dValue >= chrt->LeftAxis->Maximum)
            continue;
         nValue = chrt->LeftAxis->CalcPosValue(dValue);
         chrt->Canvas->MoveTo(chrt->ChartRect.Left, nValue);
         chrt->Canvas->LineTo(chrt->ChartRect.Right, nValue);
         }
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adjusts tick labels/values for log axis
//------------------------------------------------------------------------------
void TformBubblePlot::AdjustLogAxis(TChartAxis* pca)
{
   pca->AdjustMaxMin();
   if (!pca->Logarithmic)
      return;
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wfloat-equal"
   #pragma clang diagnostic ignored "-Wundefined-func-template"
   if (pca->Maximum == pca->Minimum)
      return;
   Application->ProcessMessages();
   int nStep = (int)pow(10, (ceil(log10(pca->Maximum - pca->Minimum)) - 1));
   int nValue = ((int)pca->Minimum / nStep) * nStep;

   pca->Items->Clear();
   while (1)
      {
      pca->Items->Add(nValue, IntToStr(nValue));
      nValue += nStep;
      if (nValue > pca->Maximum)
         break;
      }
   #pragma clang diagnostic pop
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows info on clicked/nearest bubble and selects corresponding stimulus in main form
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubblePlot::chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
   if (!m_vadData.size() || !formSpikeWare->FormsCreated())
      return;
   // find nearest 'bubble'
   double dMin = 10000000;
   double dDist;
   int nIndex = 0;
   int n, nX, nY;
   for (n = 0; n < csDisplayData->XValues->Count; n++)
      {
      nX = chrt->BottomAxis->CalcXPosValue(csDisplayData->XValues->Value[n]);
      nY = chrt->LeftAxis->CalcYPosValue(csDisplayData->YValues->Value[n]);
      // calculate distance
      dDist = sqrt((double)(abs(X - nX)*abs(X - nX) + abs(Y - nY)*abs(Y - nY)));
      if (dDist < dMin)
         {
         dMin = dDist;
         nIndex = n;
         }
      }

   // in non-search mode we have a bubble-plot based on PSTH.
   // Then we want to show a PSTH and period histogram for this bubble only!!
   if (!m_bSearch)
      {
      if (PointInRect(chrt->ChartRect,TPoint(X,Y)))
         {
         UnicodeString usInfo;
         // show channel, and stimulus parameters on BubbleBSTH
         usInfo.printf(L"Channel %d: %ls %ls - %ls %ls",
            Tag+1,
            FloatToStr((Extended)csDisplayData->XValues->Value[nIndex]).w_str(),
            formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamX].w_str(),
            FloatToStr((Extended)csDisplayData->YValues->Value[nIndex]).w_str(),
            formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamY].w_str()
            );

         formSpikeWare->SetWindowVisible(formSpikeWare->m_pformBubbleData, true);
         formSpikeWare->m_pformBubbleData->Plot(m_bpd.m_vBubbleData[(unsigned int)nIndex], m_bpd.HasFrequency(), usInfo);
         formSpikeWare->m_pformBubbleData->BringToFront();

         if (m_bpd.HasFrequency())
            {
            // show channel, and non-frequency stimulus parameter on vectro strength plot
            if (m_bpd.m_nFreqAxis == (int)m_nParamX)
               usInfo.printf(L"Channel %d: %ls %ls",
                  Tag+1,
                  FloatToStr((Extended)csDisplayData->YValues->Value[nIndex]).w_str(),
                  formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamY].w_str()
                  );
            else
               usInfo.printf(L"Channel %d: %ls %ls",
                  Tag+1,
                  FloatToStr((Extended)csDisplayData->XValues->Value[nIndex]).w_str(),
                  formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamX].w_str()
                  );
            formSpikeWare->m_pformVectorStrength->Plot(m_bpd, nIndex);
            formSpikeWare->m_pformVectorStrength->chrt->Title->Text->Text = usInfo;
            formSpikeWare->SetWindowVisible(formSpikeWare->m_pformVectorStrength, true);
            }


         m_nLastBubbleIndex = nIndex;
         }
      }


   // search the corresponding stimulus
   if (m_bSearch && m_nSearchStim != nIndex)
      {
      for (n = 0; n < csDisplayData->XValues->Count; n++)
         m_vadData[(unsigned int)n] = nIndex == n ? 1.0 : -1.0;
      chrt->Invalidate();
      }

   m_nSearchStim = nIndex;

   int nStimIndex = -1;
   // search corresponding stimulus
   for (n = 0; n < (int)formSpikeWare->m_swsStimuli.m_swstStimuli.size(); n++)
      {
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wfloat-equal"
      if (  formSpikeWare->m_swsStimuli.m_swstStimuli[(unsigned int)n].m_vdParams[m_nParamX] == csDisplayData->XValues->Value[nIndex]
         && formSpikeWare->m_swsStimuli.m_swstStimuli[(unsigned int)n].m_vdParams[m_nParamY] == csDisplayData->YValues->Value[nIndex]
         )
         {
         nStimIndex = n;
         break;
         }
      #pragma clang diagnostic pop
      }


   chrt->Enabled = false;
   if (nStimIndex > -1)
      {
      if (m_bSearch)
         formSpikeWare->PlaySearchStimulus(nStimIndex);
      else
         formSpikeWare->m_pformStimuli->SelectItem(nStimIndex);
      }
   chrt->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// upddates bubble data of bubble with passed index
//------------------------------------------------------------------------------
void TformBubblePlot::UpdateBubble(int nIndex)
{
   if (!m_bSearch)
      return;
   int n;
   int nBubble = -1;
   for (n = 0; n < csDisplayData->XValues->Count; n++)
      {
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wfloat-equal"
      if (  formSpikeWare->m_swsStimuli.m_swstStimuli[(unsigned int)nIndex].m_vdParams[m_nParamX] == csDisplayData->XValues->Value[n]
         && formSpikeWare->m_swsStimuli.m_swstStimuli[(unsigned int)nIndex].m_vdParams[m_nParamY] == csDisplayData->YValues->Value[n]
         )
         {
         nBubble = n;
         break;
         }
      #pragma clang diagnostic pop
      }
   if (nBubble > -1)
      {
      for (n = 0; n < (int)m_vadData.size(); n++)
         m_vadData[(unsigned int)n] = (n == nBubble) ? 1.0 : -1.0;
      csDisplayData->Repaint();
      Application->ProcessMessages();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adjust text of axis labels on the fly
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubblePlot::chrtGetAxisLabel(TChartAxis *Sender, TChartSeries *Series,
          int ValueIndex, UnicodeString &LabelText)
{
   int n;
   if (Sender == chrt->LeftAxis)
      {
      if (!formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbString[m_nParamY])
         return;
      if (TryStrToInt(LabelText, n))
         LabelText = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvusValues[m_nParamY][(unsigned int)n];

      }
   if (Sender == chrt->BottomAxis)
      {
      if (!formSpikeWare->m_swsStimuli.m_swspStimPars.m_vbString[m_nParamX])
         return;
      if (TryStrToInt(LabelText, n))
         LabelText = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvusValues[m_nParamX][(unsigned int)n];
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// callback if 'Show only active....' checkbox is toggled: forces repaint
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubblePlot::cbOnlyActiveBubbleClick(TObject *Sender)
{
   chrt->Repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// show mouse X/Y pos of mouse in footer
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBubblePlot::chrtMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
   if (m_bResponseYAxis)
      return;
   if ( PtInRect((RECT*)&chrt->ChartRect, Point(X-chrt->Width3D,Y+chrt->Height3D)))
      {
      double tmpX,tmpY;
      // set label text
      csDisplayData->GetCursorValues(tmpX,tmpY);  // <-- get values under mouse cursor
      UnicodeString us;
      us.printf(L"%d %ls, %d %ls",

      (int)floor(tmpX),
      formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamX].w_str(),
      (int)floor(tmpY),
      formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusUnits[m_nParamY].w_str()
      );


      chrt->Foot->Text->Text = us;
      }
}
//------------------------------------------------------------------------------




