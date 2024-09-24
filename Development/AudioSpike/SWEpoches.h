//------------------------------------------------------------------------------
/// \file SWEpcoches.h
///
/// \author Berg
/// \brief Implementation of classes TSWEpoche and TSWEpoches to store epoche data
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
#ifndef SWEpochesH
#define SWEpochesH
//------------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#include <valarray>
#include <SWTools.h>


class TSWEpoches;

//------------------------------------------------------------------------------
/// class to store data of one epoche
//------------------------------------------------------------------------------
class TSWEpoche
{
   friend class TSWEpoches;
   public:
      TSWEpoche(  unsigned int nNumChannels,
                  unsigned int nNumSamples,
                  unsigned int nStimIndex,
                  unsigned int nRepetitionIndex,
                  const std::vector<double >& rvdThreshold,
                  UnicodeString usFileName);
      unsigned int      m_nNumChannels;
      unsigned int      m_nNumSamples;
      unsigned int      m_nStimIndex;
      unsigned int      m_nIndex;
      unsigned int      m_nRepetitionIndex;
      UnicodeString     m_usFileName;
      std::vector<double >    m_vdThreshold;
      vvd            GetData();
      void           ClearData();
   private:
      vvd            m_vvdData;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class to store and handle multiple instances of TSWEpoche
//------------------------------------------------------------------------------
class TSWEpoches
{
   public:
   private:
      CRITICAL_SECTION        m_cs;
      CRITICAL_SECTION        m_csReset;
      #ifdef CHKCHNLS
      unsigned int            m_nTriggerChannel;
      #endif
      int                     m_nRecEpochePos;
      unsigned int            m_nNumTriggerSamplesInNextBuffer;
      unsigned int            m_nNumTriggerSamplesInNextBufferPlay;
      int                     m_nDoubleTriggerDistance;
      TFileStream*            m_pfsWrite;
      TFileStream*            m_pfsWriteProbeMic;
   public:
      TList*                  m_ptl;
      vvf            m_vvfEpoche;
      vvf            m_vvfEpocheProbeMic;
      double         m_dEpocheLength;
      double         m_dPreStimulus;
      int            m_nRepetitionPeriod;
      double         m_dStimulusPause;
      unsigned int   m_nEpochesTotal;
      int            m_nTriggersDetected;
      double         m_dTriggerTestLastTriggerValue;
      int            m_nTriggerTestTriggersPlayed;
      int            m_nStimIndexAtStart;
      __int64        m_nSamplesPlayed;
      __int64        m_nLastTriggerPos;
      int            m_nLastTriggerDistance;
      bool           m_bTriggerError;
      int            m_nFirstTriggerError;
      UnicodeString  m_usTriggerError;
      std::vector<double >    m_vdThreshold;

      TSWEpoches();
      ~TSWEpoches();
      void           AssertIndex(unsigned int nChannelIndex);
      void           Reset();
      void           Clear();
      void           Start();
      void           Initialize(unsigned int nNumChannels, unsigned int nSize);
      void           InitSave();
      void           AppendSave();
      void           DoneSave();
      unsigned int   GetNumChannels();
      double         GetThreshold(unsigned int nChannelIndex);
      void           SetThreshold(unsigned int nChannelIndex, double dThreshold);
      #ifdef CHKCHNLS
      void           SetTriggerChannel(unsigned int nTriggerChannel);
      #endif
      TSWEpoche*     Push( vvf& rvvfData,
                           const std::vector<double >& rvdThreshold,
                           unsigned int nStimIndex,
                           unsigned int nRepetitionIndex);
      TSWEpoche*     Pop(bool &bLast);
      TSWEpoche*     Get(int nIndex = -1);

      unsigned int   Count();
      void           SoundProc(vvf &vvfBuffers, bool bTriggerTest);
      void           SoundProcTriggerTest(vvf &vvfBuffers);
      void           SoundProcTriggerTestPlay(vvf &vvfBuffers);

};
//------------------------------------------------------------------------------
#endif
