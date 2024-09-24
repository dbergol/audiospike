//------------------------------------------------------------------------------
/// \file SWFilters.h
///
/// \author Berg
/// \brief Implementation of class TSWFilters for reading/writing filters
/// to/from ini-files
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
#ifndef SWFiltersH
#define SWFiltersH
//------------------------------------------------------------------------------

#include "SWTools.h"

//------------------------------------------------------------------------------
/// class for reading/writing filters to/from ini-files
//------------------------------------------------------------------------------
class TSWFilters
{
   public:
      TSWFilters(UnicodeString usFileName);
      ~TSWFilters();
      void                    SaveHiPass(UnicodeString usChannel, float f);
      void                    SaveBandPass(UnicodeString usChannel, float fLowFreq, float fHiFreq, float fMaxFreq);
      void                    RemoveFilter(UnicodeString usChannel);
      void                    RemoveHiPass(UnicodeString usChannel);
      void                    RemoveBandPass(UnicodeString usChannel);
      void                    GetHiPass(UnicodeString usChannel, float& rfHiPass);
      void                    GetBandPass(UnicodeString usChannel, float& rfLoFreq, float& rfHiFreq);
      void                    GetFilterEdges(UnicodeString usChannel, float& rfLoFreq, float& rfHiFreq);
      void                    GetChannelFreqs(UnicodeString usChannel, float& rfLoFreq, float& rfHiFreq, float fMaxFreq, bool bInSitu);
      TIniFile*               m_pFilterIni;
   private:
};
//------------------------------------------------------------------------------


#endif
