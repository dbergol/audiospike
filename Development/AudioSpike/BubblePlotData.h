//------------------------------------------------------------------------------
/// \file BubblePlotData.h
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
#ifndef BubblePlotDataH
#define BubblePlotDataH
#include <vcl.h>
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for storing data for a bubble in a bubble plot
//------------------------------------------------------------------------------
class TBubbleData
{
   public:
      std::vector<double > m_vdSpikeTimes;
      std::vector<double > m_vdSpikeCycles;
      double               m_dFrequency;
      double               m_dNonFreqValue;
      double               m_dVectorStrength;
      double               m_dPUniform;
      TBubbleData();
      void                 Clear();
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for storing multiple TBubbleData and calculating vector strength
//------------------------------------------------------------------------------
class TBubblePlotData
{
   public:
      std::vector<TBubbleData > m_vBubbleData;
      int                  m_nFreqAxis;
      TBubblePlotData();
      bool                 HasFrequency();
      void                 Reset();
      void                 Clear();
      void                 CalculateVectorStrength();
};
//------------------------------------------------------------------------------
#endif
