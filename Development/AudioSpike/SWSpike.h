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
#include "SWSpikeDetectionMethods.h"
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

      double                  m_dSpikeLength;
      unsigned int            m_nSpikeLength;
      double                  m_dSampleRate;
      bool                    m_bInitialized;

      bool                    IsEmpty();
      void                    SetSpikeLength(double dSpikeLength);
      void                    AssertIndex(unsigned int nChannelIndex);

   public:
      TSWSpikes();
      ~TSWSpikes();

      std::vector<std::vector<TSWSpike* > > m_vvSpikes;     // vector of vectors with spikes per channel

      SWSpikeParameters          m_swspSpikePars;
      TSpikeDetectionMethods     m_sdmDetectionMethods;
      bool                       m_bRejectSingleSignSpikes;
      double                     m_dSampleRateDevider;

      int            m_nTestFlag;
      void           DebugSave(int x);

      void           Clear();
      double         GetSampleRate();
      void           SetSampleRate(double dSampleRate, double dSampleRateDevider);

      unsigned int   GetSpikeLengthSamples(void);
      double         GetSpikeLength(void);

      void           SetDetectionMethod1(double dSpikeLength, double dPreThreshold, double dPostThreshold);
      void           SetDetectionMethod2(double dRefractoryTime, double dRefractoryTimeTailFactor);


      void           SetNumChannels(unsigned int nNum);
      unsigned int   GetNumChannels();

      void           Remove(unsigned int nEpocheIndex);
      void           Remove(unsigned int nChannelIndex, unsigned int nEpocheIndex);
      void           Add(TSWEpoche *pswe, vvd *pvvd = NULL);
      void           Add(_di_IXMLNode xmlSpikes);
      unsigned int   CallSpikeDetectionMethod(  TSWSpike* psms,
                                                std::valarray<double >* pvadEpoche = NULL);
      unsigned int   GetNumSpikes(unsigned int nChannelIndex);
      double         GetSpikeParam(unsigned int nChannelIndex, unsigned int nIndex, TSpikeParam sp);
      double         GetSpikeTime(unsigned int nChannelIndex, unsigned int nIndex);
      double         GetThreshold(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int   GetSpikePosition(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int   GetSpikePeakPosPosition(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int   GetSpikePeakNegPosition(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int   GetThresholdCrossingPosition(unsigned int nChannelIndex, unsigned int nIndex);
      int            GetSpikeGroup(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int   GetStimIndex(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int   GetEpocheIndex(unsigned int nChannelIndex, unsigned int nIndex);
      unsigned int   GetRepetitionIndex(unsigned int nChannelIndex, unsigned int nIndex);
      void           SetSpikeGroup(unsigned int nChannelIndex, unsigned int nIndex, int nGroup);
      void           SpikeGroupReset(unsigned int nChannelIndex);
      std::valarray<double>& GetSpike(unsigned int nChannelIndex, unsigned int nIndex);
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for storing properties of one spike
//------------------------------------------------------------------------------
class TSWSpike
{
   friend class TSWSpikes;
   friend class TSpikeDetectionMethodVersion1;
   friend class TSpikeDetectionMethodVersion2;
   public:
      TSWSpike(unsigned int nSpikeLength);
   private:
      int            m_nGroupIndex;
      double         m_dThreshold;
      double         m_dPeakUA;
      double         m_dPeakDA;
      double         m_dPeakUT;
      double         m_dPeakDT;
      unsigned int   m_nPeakUT;
      unsigned int   m_nPeakDT;
      double         m_dTrigT;
      double         m_dSpikeTime;
      unsigned int   m_nThresholdCrossingPosition;
      unsigned int   m_nSpikePos;
      unsigned int   m_nStimIndex;
      unsigned int   m_nEpocheIndex;
      unsigned int   m_nRepetitionIndex;
      unsigned int   m_nChannelIndex;
      std::valarray<double >  m_vadData;
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
