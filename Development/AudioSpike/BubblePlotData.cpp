//------------------------------------------------------------------------------
/// \file BubblePlotData.cpp
///
/// \author Berg
/// \brief Implementation of helper classes TBubblePlotData and TBubbleData
/// to handle data for bubble plots
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
#pragma hdrstop

#include "BubblePlotData.h"

#include <valarray>
#include <complex>

//------------------------------------------------------------------------------
#pragma package(smart_init)

//------------------------------------------------------------------------------
/// constructor. initializes members
//------------------------------------------------------------------------------
TBubbleData::TBubbleData()
   : m_dFrequency(0.0), m_dVectorStrength(0.0), m_dPUniform(0.0)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears internal vectors
//------------------------------------------------------------------------------
void TBubbleData::Clear()
{
   m_vdSpikeTimes.clear();
   m_vdSpikeCycles.clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. initializes members
//------------------------------------------------------------------------------
TBubblePlotData::TBubblePlotData()
   : m_nFreqAxis(-1)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears internal vector of TBubbleData
//------------------------------------------------------------------------------
void TBubblePlotData::Reset()
{
   m_vBubbleData.clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls clears for all TBubbleData instances in m_vBubbleData
//------------------------------------------------------------------------------
void TBubblePlotData::Clear()
{
   unsigned int n;
   for (n = 0; n < m_vBubbleData.size(); n++)
      m_vBubbleData[n].Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns true if m_nFreqAxis > 0
//------------------------------------------------------------------------------
bool TBubblePlotData::HasFrequency()
{
   return m_nFreqAxis > 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calculates vector strength
//------------------------------------------------------------------------------
void TBubblePlotData::CalculateVectorStrength()
{
   std::valarray<std::complex<double > > vac;

   // go through bubbles
   unsigned int nBubble, nNum, n;
   double d;
   for (nBubble = 0; nBubble < m_vBubbleData.size(); nBubble++)
      {
      nNum = (unsigned int)m_vBubbleData[nBubble].m_vdSpikeCycles.size();
      if (!nNum)
         continue;

      // calculate vector strength
      vac.resize(nNum);
      // - collect all complex vectors

      for (n = 0; n < nNum; n++)
         {
         d = 2.0*M_PI*m_vBubbleData[nBubble].m_vdSpikeCycles[n];
         vac[n].real(cos(d));
         vac[n].imag(sin(d));
         }
      m_vBubbleData[nBubble].m_dVectorStrength = std::abs(vac.sum() / (double)vac.size());

      // (approximate) Rayleigh test for circular uniformity
      // Zar, Biostatistical Analysis, 5th Ed., 2010, equation 27.4, p. 625
      double dN   = (double)nNum;
      double dRN  = m_vBubbleData[nBubble].m_dVectorStrength*dN;
      m_vBubbleData[nBubble].m_dPUniform = exp(sqrt((1.0+4.0*dN+4*(dN*dN-dRN*dRN))) - (1.0+2.0*dN));
      }
}
//------------------------------------------------------------------------------


