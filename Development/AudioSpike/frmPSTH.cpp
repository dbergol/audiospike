//------------------------------------------------------------------------------
/// \file frmPSTH.cpp
///
/// \author Berg
/// \brief Implementation of a form to display a PSTH (histogram) for a cluster
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

#include "frmPSTH.h"
#include "SpikeWareMain.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VCLTee.StatChar"
#pragma link "frmASUI"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Calls Clear() to initialize members
//------------------------------------------------------------------------------
TSWPSTHSelection::TSWPSTHSelection()
{
   Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resets members
//------------------------------------------------------------------------------
void TSWPSTHSelection::Clear()
{
   dX0      = 0.0;
   dX1      = 0.0;
   bActive  = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. creates THistogramFunction and initializes members
//------------------------------------------------------------------------------
__fastcall TformPSTH::TformPSTH(TComponent* Owner, TMenuItem* pmi)
   : TformASUI(Owner, pmi)
{
   m_pthf = new THistogramFunction(this);
   m_pthf->DataStyle = hdsTruncate;
   m_pthf->Cumulative = false;
   m_pthf->NumBins = 100;

   csData->FunctionType = m_pthf;
   csData->SetFunction(m_pthf);
   csData->Marks->Visible = false;

   csSelection->Y0 = 0;
   csNoiseSelection->Y0 = 0;
   csSelection->Y1 = 1;
   csNoiseSelection->Y1 = 1;

   m_nPlotCounter = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destuctor. saves settings
//------------------------------------------------------------------------------
__fastcall TformPSTH::~TformPSTH()
{
   formSpikeWare->m_pIni->WriteString(Name, "SelectionX0", DoubleToStr(csSelection->X0));
   formSpikeWare->m_pIni->WriteString(Name, "SelectionX1", DoubleToStr(csSelection->X1));
   formSpikeWare->m_pIni->WriteString(Name, "NoiseSelectionX0", DoubleToStr(csNoiseSelection->X0));
   formSpikeWare->m_pIni->WriteString(Name, "NoiseSelectionX1", DoubleToStr(csNoiseSelection->X1));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// initializes chart, selections and histogram function
//------------------------------------------------------------------------------
void TformPSTH::Initialize(unsigned int nChannels)
{
   chrt->BottomAxis->SetMinMax(0, formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0);

   int nBinLen = formSpikeWare->m_pIni->ReadInteger("Settings", "PSTHBinSize", 1);
   m_pthf->NumBins = (int)floor(formSpikeWare->m_sweEpoches.m_dEpocheLength*1000) / nBinLen;
   ShowBinSize();
   csSelection->Active      = false;
   csNoiseSelection->Active = false;

   m_vSWSelections.clear();
   m_vSWNoiseSelections.clear();
   unsigned int n;
   for (n = 0; n < nChannels; n++)
      {
      m_vSWSelections.push_back(TSWPSTHSelection());
      m_vSWNoiseSelections.push_back(TSWPSTHSelection());
      }

   // read region from ini.... UNCOMMENT FOR DEBUGGING ONLY
   /*
   // read region from ini.... debug only
   if (!formSpikeWare->m_pIni->ReadBool(Name, "ReadSelections", false))
      return;
   double d1, d2;
   UnicodeString us1 = formSpikeWare->m_pIni->ReadString(Name, "SelectionX0", "");
   UnicodeString us2 = formSpikeWare->m_pIni->ReadString(Name, "SelectionX1", "");
   if (TryStrToDouble(us1, d1) && TryStrToDouble(us2, d2))
      {
      csSelection->X0 = d1;
      csSelection->X1 = d2;
      csSelection->Active = true;

      }
   us1 = formSpikeWare->m_pIni->ReadString(Name, "NoiseSelectionX0", "");
   us2 = formSpikeWare->m_pIni->ReadString(Name, "NoiseSelectionX1", "");
   if (TryStrToDouble(us1, d1) && TryStrToDouble(us2, d2))
      {
      csNoiseSelection->X0 = d1;
      csNoiseSelection->X1 = d2;
      csNoiseSelection->Active = true;
      }
   */
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots the PSTH
//------------------------------------------------------------------------------
void TformPSTH::Plot(unsigned int nChannelIndex, bool bForce)
{
   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;
   try
      {
      m_nPlotCounter++;
      Caption = "PSTH - Channel " + IntToStr((int)nChannelIndex+1);
      if (Tag != (int)nChannelIndex || bForce)
         {
         Tag = (NativeInt)nChannelIndex;
         csSelection->Active  = m_vSWSelections[(unsigned int)Tag].bActive;
         csSelection->X0      = m_vSWSelections[(unsigned int)Tag].dX0;
         csSelection->X1      = m_vSWSelections[(unsigned int)Tag].dX1;
         csNoiseSelection->Active  = m_vSWNoiseSelections[(unsigned int)Tag].bActive;
         csNoiseSelection->X0 = m_vSWNoiseSelections[(unsigned int)Tag].dX0;
         csNoiseSelection->X1 = m_vSWNoiseSelections[(unsigned int)Tag].dX1;
         }

      csData->Clear();
      csPoints->Clear();
      chrt->LeftAxis->Minimum = 0;
      chrt->LeftAxis->AutomaticMaximum = true;
      unsigned int nNum = formSpikeWare->m_swsSpikes.GetNumSpikes(nChannelIndex);
      if (!nNum)
         return;
      unsigned int n;
      for (n = 0; n < nNum; n++)
         {
         if (formSpikeWare->m_swsSpikes.GetSpikeGroup(nChannelIndex, n) >= 0)
            csPoints->AddY(formSpikeWare->m_swsSpikes.GetSpikeTime(nChannelIndex, n)*1000.0);
         }

      csData->DataSources->Clear();
      csData->DataSources->Add(csPoints);
      }
   __finally
      {
      m_nPlotCounter--;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears chart series and resets axis
//------------------------------------------------------------------------------
void TformPSTH::Clear()
{
   csPoints->Clear();
   csData->Clear();
   chrt->LeftAxis->Minimum = 0;
   chrt->LeftAxis->AutomaticMaximum = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for ZoomIn button. Raises binsize
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::tbtnZoomOutClick(TObject *Sender)
{
   if (m_pthf->NumBins < 1000)
      m_pthf->NumBins *= 2;
   ShowBinSize();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for ZoomIn button. Lowers binsize
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::tbtnZoomInClick(TObject *Sender)
{
   if (m_pthf->NumBins > 2)
      m_pthf->NumBins /= 2;
   ShowBinSize();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// stores selection in rvSel
//------------------------------------------------------------------------------
void TformPSTH::StoreSelection(TChartShape* pcs)
{
   std::vector<TSWPSTHSelection > &rvSel  = pcs == csNoiseSelection ? m_vSWNoiseSelections : m_vSWSelections;
   rvSel[(unsigned int)Tag].dX0 = pcs->X0;
   rvSel[(unsigned int)Tag].dX1 = pcs->X1;
   rvSel[(unsigned int)Tag].bActive = pcs->Active;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseDown callback of chart. Starts setting a selection
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
   if (!PtInRect(chrt->ChartRect, TPoint(X,Y)))
      return;

   TChartShape* pcs = Button == mbRight ? csNoiseSelection : csSelection;

   double dX, dY;
   m_dLastX0 = pcs->X0;
   m_dLastX1 = pcs->X1;
   pcs->GetCursorValues(dX, dY);
   pcs->X0 = dX;
   pcs->X1 = dX;
   pcs->Active = true;
   StoreSelection(pcs);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseMove callback of chart. Updates selection
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::chrtMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
   if (  !PtInRect(chrt->ChartRect, TPoint(X,Y))
      || (!Shift.Contains(ssRight) && !Shift.Contains(ssLeft))
      )
      return;

   TChartShape* pcs = Shift.Contains(ssRight) ? csNoiseSelection : csSelection;

   double dX, dY;
   pcs->GetCursorValues(dX, dY);
   pcs->X1 = dX;
   StoreSelection(pcs);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseUp callback of chart. Ends setting a selection and updates plots
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::chrtMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wfloat-equal"
   bool bNoUpdate = true;
   if (Button == mbLeft)
      bNoUpdate = (csSelection->X0 == m_dLastX0 && csSelection->X1 == m_dLastX1);
   else if (Button == mbRight)
      bNoUpdate = (csNoiseSelection->X0 == m_dLastX0 && csNoiseSelection->X1 == m_dLastX1);
   #pragma clang diagnostic pop


   if (!bNoUpdate)
      {
      formSpikeWare->SetMeasurementChanged();
      formSpikeWare->PlotBubblePlots();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
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
/// Onclick callback for tbtnSel enabled selection and applies it
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::tbtnSelClick(TObject *Sender)
{
   csSelection->Active = true;
   if (formSpikeWare->m_pformSetParameters->SetChartShapeMinMax(chrt->BottomAxis, csSelection, this))
      {
      StoreSelection(csSelection);
      formSpikeWare->PlotBubblePlots();
      formSpikeWare->SetMeasurementChanged();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Onclick callback for tbtnNoiseSel enabled noise selection and applies it
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::tbtnNoiseSelClick(TObject *Sender)
{
   csNoiseSelection->Active = true;
   if (formSpikeWare->m_pformSetParameters->SetChartShapeMinMax(chrt->BottomAxis, csNoiseSelection, this))
      {
      StoreSelection(csNoiseSelection);
      formSpikeWare->PlotBubblePlots();
      formSpikeWare->SetMeasurementChanged();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Onclick callback for tbtnSelClear disables selection and applies the change
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::tbtnSelClearClick(TObject *Sender)
{
   if (csSelection->Active)
      {
      csSelection->Active = false;
      StoreSelection(csSelection);
      formSpikeWare->PlotBubblePlots();
      formSpikeWare->SetMeasurementChanged();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Onclick callback for tbtnNoiseSelClear disables noiseselection and applies 
/// the change
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::tbtnNoiseSelClearClick(TObject *Sender)
{
   if (csNoiseSelection->Active)
      {
      csNoiseSelection->Active   = false;
      StoreSelection(csNoiseSelection);
      formSpikeWare->PlotBubblePlots();
      formSpikeWare->SetMeasurementChanged();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns enabled status of selection
//------------------------------------------------------------------------------
bool TformPSTH::Selected()
{
   return csSelection->Active;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns enabled status of noise selection
//------------------------------------------------------------------------------
bool TformPSTH::NoiseSelected()
{
   return csNoiseSelection->Active;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// writes current selections to passed variables
//------------------------------------------------------------------------------
void  TformPSTH::GetSelections(double& dMin, double& dMax, double& dNoiseMin, double& dNoiseMax)
{
   dMin = Min(csSelection->X0, csSelection->X1)/1000.0;
   dMax = Max(csSelection->X0, csSelection->X1)/1000.0;
   dNoiseMin = Min(csNoiseSelection->X0, csNoiseSelection->X1)/1000.0;
   dNoiseMax = Max(csNoiseSelection->X0, csNoiseSelection->X1)/1000.0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows binsize on label
//------------------------------------------------------------------------------
void  TformPSTH::ShowBinSize()
{
  lblBinSize->Caption = "  binsize: " + FormatFloat("0.00", (Extended)(formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / m_pthf->NumBins)) + " ms";;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Shows dialog to set binsize by value
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformPSTH::tbtnBinSizeClick(TObject *Sender)
{
   //
   double dValue = formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / (double)m_pthf->NumBins;
   dValue = (double)StrToFloat(FormatFloat("0.00", (Extended)dValue));

   if (!formSpikeWare->m_pformSetParameters->SetParameter("Binsize", "ms", dValue, this))
      return;

   m_pthf->NumBins = (int)floor(formSpikeWare->m_sweEpoches.m_dEpocheLength*1000.0 / dValue);
   ShowBinSize();

}
//------------------------------------------------------------------------------




