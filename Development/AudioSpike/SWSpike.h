//------------------------------------------------------------------------------
/// \file SWSpike.h
///
/// \author Berg
/// \brief Implementation of classes TSWSpike and TSWSpikes to store spike data
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
#ifndef SWSpikeH
#define SWSpikeH
//------------------------------------------------------------------------------

#include <vcl.h>
#include <msxmldom.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include <vector>
#include <valarray>
#include "SWSpikeParameters.h"
#include "SWStimParameters.h"
#include "SWTools.h"

//------------------------------------------------------------------------------

class TSWSpike;
class TSWEpoche;
//------------------------------------------------------------------------------
/// class for storing multiple multiple spikes with identical
/// parameters
//------------------------------------------------------------------------------
class TSWSpikes
{
   friend class TSWSpike;
   private:
      CRITICAL_SECTION        m_cs;
      int                     m_nPreThreshold;
      double                  m_dSampleRate;
      bool                    m_bInitialized;
      bool                    IsEmpty();
   public:
      TSWSpikes();
      ~TSWSpikes();
      SWSpikeParameters       m_swspSpikePars;
      double                  m_dPreThreshold;
      double                  m_dSpikeLength;
      int                     m_nSpikeLength;
      double                  m_dPostThreshold;
      int                     m_nPostThreshold;
      double                  m_dSampleRateDevider;
      void                    AssertIndex(unsigned int nChannelIndex);
      std::vector<std::vector<TSWSpike* > > m_vvSpikes;
      void     Clear();
      double   GetSampleRate();
      void     SetSampleRate(double dSampleRate, double dSampleRateDevider);
      void     SetSpikeLength(double dPreThreshold, double dPostThreshold, double dSpikeLength);

      unsigned int GetNumChannels();
      void     SetNumChannels(unsigned int nNum);

      void     Remove(unsigned int nEpocheIndex);
      void     Remove(unsigned int nChannelIndex, unsigned int nEpocheIndex);
      void     Add(TSWEpoche *pswe, vvd *pvvd = NULL);
      void     Add(_di_IXMLNode xmlSpikes);
      unsigned int GetNumSpikes(unsigned int nChannelIndex);
      double   GetSpikeParam(unsigned int nChannelIndex, unsigned int nIndex, TSpikeParam sp);
      double   GetSpikeTime(unsigned int nChannelIndex, unsigned int nIndex);
      double   GetThreshold(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int GetSpikePosition(unsigned int nChannelIndex, unsigned int nIndex);
      int      GetSpikeGroup(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int GetStimIndex(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int GetEpocheIndex(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int GetRepetitionIndex(unsigned int nChannelIndex, unsigned int nIndex);
      void     SetSpikeGroup(unsigned int nChannelIndex, unsigned int nIndex, int nGroup);
      void     SpikeGroupReset(unsigned int nChannelIndex);
      std::valarray<double>& GetSpike(unsigned int nChannelIndex, unsigned int nIndex);
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for storing properties of one spike
//------------------------------------------------------------------------------
class TSWSpike
{
   friend class TSWSpikes;
   public:
      TSWSpike(TSWSpikes* pSpikes,
               TSWEpoche *pswe,
               vvd   &rvvdEpocheData,
               unsigned int nPos,
               unsigned int nChannelIndex
               );
      TSWSpike(TSWSpikes* pSpikes);
   private:
      int      m_nGroupIndex;
      double   m_dThreshold;
      double   m_dPeakUA;
      double   m_dPeakDA;
      double   m_dPeakUT;
      double   m_dPeakDT;
      double   m_dTrigT;
      double   m_dSpikeTime;
      unsigned int  m_nSpikePos;
      unsigned int   m_nStimIndex;
      unsigned int   m_nEpocheIndex;
      unsigned int   m_nRepetitionIndex;
      unsigned int   m_nChannelIndex;
      std::valarray<double >  m_vadData;
      void     Init(double dSampleRate);
   public:
      double   TotalAmplitude();
      double   Peak1();
      double   Peak2();
      double   PeakPos();
      double   PeakNeg();
      double   PeakToPeak();
      double   ThresholdToPeak2();
};
//------------------------------------------------------------------------------

#endif
