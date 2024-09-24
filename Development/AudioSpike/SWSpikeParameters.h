//------------------------------------------------------------------------------
/// \file SWSpikeParamaters.h
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
#ifndef SWSpikeParametersH
#define SWSpikeParametersH
//------------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
#include <valarray>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for "known" peak parameters
//------------------------------------------------------------------------------
enum TSpikeParam
{
   SP_TOTALAMPLITUDE = 0,  ///< Total action potential amplitude
   SP_PEAK1,               ///< amplitude of the 1st component (“phase”).
   SP_PEAK2,               ///< amplitude of the 2nd component (“phase”).
   SP_PEAKPOS,             ///< amplitude of the positive peak.
   SP_PEAKNEG,             ///< amplitude of the negative peak.
   SP_PEAK2PEAK,           ///< time in µsec between the 1st and 2nd component
   SP_THRS2PEAK2,          ///< time in µsec between the “trigger” (threshold crossing) and 2nd component
   SP_LAST                 ///< dummy to mark last entry
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class containing a variable list of spike parameter names and values
//------------------------------------------------------------------------------
class SWSpikeParameters
{
   public:
      std::vector<UnicodeString > m_vusIDs;
      std::vector<UnicodeString > m_vusNames;
      std::vector<UnicodeString > m_vusUnits;
      std::vector<double > m_vdMin;
      std::vector<double > m_vdMax;
      SWSpikeParameters();
      void Add(UnicodeString usID, UnicodeString usName,
               UnicodeString usUnit, double dMin, double dMax);
      void SetPeakLength(double dLen);
      int IndexFromID(UnicodeString us);
};
//------------------------------------------------------------------------------
#endif
