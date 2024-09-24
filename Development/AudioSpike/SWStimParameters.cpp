//------------------------------------------------------------------------------
/// \file SWStimParamaters.cpp
///
/// \author Berg
/// \brief Implementation of class TSWStimParamaters containing a variable list
/// of stimulus parameter names and values
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

#include "SWStimParameters.h"
#include <math.h>
#include <algorithm>
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor - empty
//------------------------------------------------------------------------------
SWStimParameters::SWStimParameters()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears internal members
//------------------------------------------------------------------------------
void SWStimParameters::Clear()
{
   m_vusNames.clear();
   m_vusUnits.clear();
   m_vbLog.clear();
   m_vbString.clear();
   m_vvdValues.clear();
   m_vvusValues.clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds a stimulus paramater
//------------------------------------------------------------------------------
void SWStimParameters::Add(UnicodeString usName, UnicodeString usUnit, bool bLog, bool bString)
{
   if (IndexFromName(usName) > -1)
      throw Exception("parameter '" + usName + "' already registered!");
   m_vusNames.push_back(usName);
   m_vusUnits.push_back(usUnit);
   m_vbLog.push_back(bLog);
   m_vbString.push_back(bString);
   m_vvdValues.resize(m_vvdValues.size()+1);
   m_vvusValues.resize(m_vvusValues.size()+1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns number of parameters
//------------------------------------------------------------------------------
unsigned int SWStimParameters::Count()
{
   return (unsigned int)m_vusNames.size();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns parameter index by name
//------------------------------------------------------------------------------
int SWStimParameters::IndexFromName(UnicodeString us)
{
   unsigned int n;
   for (n = 0; n < m_vusNames.size(); n++)
      {
      if (UpperCase(m_vusNames[n]) == UpperCase(us))
         return (int)n;
      }
   return -1;
}
//------------------------------------------------------------------------------
