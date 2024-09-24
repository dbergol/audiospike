//------------------------------------------------------------------------------
/// \file SWStimParamaters.h
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
#ifndef SWStimParametersH
#define SWStimParametersH
//------------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
#include <valarray>
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// class for storing a variable list of stimulus parameter names and values
//------------------------------------------------------------------------------
class SWStimParameters
{
   public:
      std::vector<UnicodeString > m_vusNames;
      std::vector<UnicodeString > m_vusUnits;
      std::vector<bool > m_vbLog;
      std::vector<bool > m_vbString;
      std::vector<std::vector<double > > m_vvdValues;
      std::vector<std::vector<UnicodeString > > m_vvusValues;

      SWStimParameters();
      unsigned int Count();
      void Add(UnicodeString usName, UnicodeString usUnit, bool bLog, bool bString);
      int IndexFromName(UnicodeString us);
      void Clear();
};
//------------------------------------------------------------------------------
#endif
