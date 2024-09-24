//------------------------------------------------------------------------------
/// \file SWSpikeParamaters.cpp
///
/// \author Berg
/// \brief Implementation of class TSWSpikeParamaters containing a variable list
/// of spike parameter names and values
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

#include "SWSpikeParameters.h"
#include <math.h>
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Adds all known parameters
//------------------------------------------------------------------------------
SWSpikeParameters::SWSpikeParameters()
{
   Add("TotalAmp", "Total amplitude", "rel.", 0.0, 2.0);
   Add("Peak1", "Peak 1", "rel.", -1.0, 1.0);
   Add("Peak2", "Peak 2", "rel.", -1.0, 1.0);
   Add("PeakPos", "+ve Peak", "rel.", -1.0, 1.0);
   Add("PeakNeg", "-ve Peak", "rel.", -1.0, 1.0);
   Add("PeakToPeak", "Peak to Peak", "µs", 0.0, 2000.0);
   Add("TrigToPeak", "Trig to Peak", "µs", 0.0, 2000.0);

   if (m_vusNames.size() != SP_LAST)
      throw Exception("inconsistant spike paramater sizes");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds a new parameter
//------------------------------------------------------------------------------
void SWSpikeParameters::Add(  UnicodeString usID,
                              UnicodeString usName,
                              UnicodeString usUnit,
                              double dMin,
                              double dMax)
{
   m_vusIDs.push_back(usID);
   m_vusNames.push_back(usName);
   m_vusUnits.push_back(usUnit);
   m_vdMin.push_back(dMin);
   m_vdMax.push_back(dMax);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Sets peaklength by initializing maxima of peak2peak and thrs2peak to passed
/// peak length
//------------------------------------------------------------------------------
void SWSpikeParameters::SetPeakLength(double dLen)
{
   m_vdMax[SP_PEAK2PEAK]   = dLen;
   m_vdMax[SP_THRS2PEAK2]  = dLen;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns parameter index by parameter name
//------------------------------------------------------------------------------
int SWSpikeParameters::IndexFromID(UnicodeString us)
{
   unsigned int n;
   for (n = 0; n < m_vusIDs.size(); n++)
      {
      if (m_vusIDs[n] == us)
         return (int)n;
      }
   throw Exception("SpikParameter with ID '" + us + "' not found");
}
//------------------------------------------------------------------------------
