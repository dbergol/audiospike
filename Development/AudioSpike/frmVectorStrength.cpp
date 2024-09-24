//------------------------------------------------------------------------------
/// \file frmVectorStrength.cpp
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
#include <vcl.h>
#include <math.h>
#include <algorithm>
#pragma hdrstop

#include "frmVectorStrength.h"
#include "SpikeWareMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor, initializes members
//------------------------------------------------------------------------------
__fastcall TformVectorStrength::TformVectorStrength(TComponent* Owner)
   : TformASUI(Owner), m_nPlotCounter(0)
{

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor - empty
//------------------------------------------------------------------------------
__fastcall TformVectorStrength::~TformVectorStrength()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots vector strength
//------------------------------------------------------------------------------
void TformVectorStrength::Plot(TBubblePlotData& rbpd, int nIndex)
{

   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;
   try
      {
      m_nPlotCounter++;
      csVS->Clear();
      csP->Clear();
      unsigned int n;
      double dNonFreqValue = rbpd.m_vBubbleData[(unsigned int)nIndex].m_dNonFreqValue;
      for (n = 0; n < rbpd.m_vBubbleData.size(); n++)
         {
         // only use values that have the same 'non-freq-value'!!
         #pragma clang diagnostic push
         #pragma clang diagnostic ignored "-Wfloat-equal"
         if (rbpd.m_vBubbleData[n].m_dNonFreqValue != dNonFreqValue)
            continue;
         #pragma clang diagnostic pop
         // add vector strength to visible series ...
         csVS->AddXY(rbpd.m_vBubbleData[n].m_dFrequency, rbpd.m_vBubbleData[n].m_dVectorStrength);
         // .. and corresponding p-Values to invisible series
         csP->AddXY(rbpd.m_vBubbleData[n].m_dFrequency, rbpd.m_vBubbleData[n].m_dPUniform);
         }
      }
   __finally
      {
      m_nPlotCounter--;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnGetPointerStyle callback: sets circle or diagonal cross depending on value
/// of vector strength
//------------------------------------------------------------------------------
#pragma argsused
TSeriesPointerStyle __fastcall TformVectorStrength::csVSGetPointerStyle(TChartSeries *Sender,
          int ValueIndex)
{
   if (csP->YValues->Value[ValueIndex] < 0.05)
      return psCircle;
   else
      return psDiagCross;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets legend text fix
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformVectorStrength::chrtGetLegendText(TCustomAxisPanel *Sender, TLegendStyle LegendStyle,
          int Index, UnicodeString &LegendText)
{
   LegendText = "significant values";
}
//------------------------------------------------------------------------------

