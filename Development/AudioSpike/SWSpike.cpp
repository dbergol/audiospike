//------------------------------------------------------------------------------
/// \file SWSpike.cpp
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

#pragma hdrstop

#include "SWSpike.h"
#include "SpikeWareMain.h"
#include "SWEpoches.h"
#include <math.h>
#include "Encddecd.hpp"

//------------------------------------------------------------------------------
#pragma warn -aus
#pragma package(smart_init)

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// CLASS TSWSpikes containing info about multiple spikes with identical
/// parameters
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor initializes members
//------------------------------------------------------------------------------
TSWSpikes::TSWSpikes()
{
   InitializeCriticalSection(&m_cs);
   m_bInitialized = false;
   m_dSampleRate = 44100.0;
   m_dSampleRateDevider = 1.0;
   m_nPostThreshold = 0;
   m_dPostThreshold = 0.0;
   SetNumChannels(1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, does cleanup
//------------------------------------------------------------------------------
TSWSpikes::~TSWSpikes()
{
   Clear();
   DeleteCriticalSection(&m_cs);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reurns true if completetly empty
//------------------------------------------------------------------------------
bool TSWSpikes::IsEmpty()
{
   unsigned int n;
   for (n = 0; n < m_vvSpikes.size(); n++)
      {
      if (m_vvSpikes[n].size())
         return false;
      }
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// assertion for channel index, raises exception on assertion error
//------------------------------------------------------------------------------
void TSWSpikes::AssertIndex(unsigned int nChannelIndex)
{
   if (nChannelIndex > m_vvSpikes.size())
      throw Exception("spike channel index exceeded");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns nmber of channels
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetNumChannels()
{
   return (unsigned int)m_vvSpikes.size();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets numper of channels, only allowed if empty
//------------------------------------------------------------------------------
void TSWSpikes::SetNumChannels(unsigned int nNum)
{
   if (!IsEmpty())
      throw Exception("number of channels cannot be set if spikes are not empty!");
   EnterCriticalSection(&m_cs);
   try
      {
      Clear();
      m_vvSpikes.resize(nNum);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns internal samplerate, which is samplerate/SampleRateDevider and thus 
/// NOT necessarily identical to device sample rate!
//------------------------------------------------------------------------------
double TSWSpikes::GetSampleRate()
{
   return m_dSampleRate;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets internal samplerate and devider
//------------------------------------------------------------------------------
void TSWSpikes::SetSampleRate(double dSampleRate, double dSampleRateDevider)
{
   if (!IsEmpty())
      throw Exception("Sanplerate cannot be changed if spikes are stored");

   EnterCriticalSection(&m_cs);
   try
      {
      m_dSampleRateDevider = dSampleRateDevider;
      m_dSampleRate     = dSampleRate/dSampleRateDevider;
      m_nPreThreshold   = (int)(m_dPreThreshold * m_dSampleRate);
      m_nSpikeLength    = (int)(m_dSpikeLength * m_dSampleRate);
      m_nPostThreshold  = (int)(m_dPostThreshold * m_dSampleRate);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets pre- and postthreshold and spike length (for analysis)
//------------------------------------------------------------------------------
void TSWSpikes::SetSpikeLength(double dPreThreshold, double dPostThreshold, double dSpikeLength)
{
   if (!IsEmpty())
      throw Exception("PreThreshold, PostThreshold or SpikeLength cannot be changed if spikes are stored");

   EnterCriticalSection(&m_cs);
   try
      {
      m_dPreThreshold   = dPreThreshold;
      m_dPostThreshold  = dPostThreshold;
      m_dSpikeLength    = dSpikeLength;

      m_nPreThreshold   = (int)(m_dPreThreshold*m_dSampleRate);
      m_nPostThreshold  = (int)(m_dPostThreshold*m_dSampleRate);
      m_nSpikeLength    = (int)(m_dSpikeLength*m_dSampleRate);

      // set displayed total peak length in microseconds
      m_swspSpikePars.SetPeakLength(m_dSpikeLength * 1000000.0);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears all spikes
//------------------------------------------------------------------------------
void TSWSpikes::Clear()
{
   EnterCriticalSection(&m_cs);
   try
      {
      unsigned int n, m;
      for (n = 0; n < m_vvSpikes.size(); n++)
         {
         for (m = 0; m < m_vvSpikes[n].size(); m++)
            {
            TRYDELETENULL(m_vvSpikes[n][m]);
            }
         m_vvSpikes[n].clear();
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns number of spikes stored for one channel
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetNumSpikes(unsigned int nChannelIndex)
{
   AssertIndex(nChannelIndex);
   return (unsigned int)m_vvSpikes[nChannelIndex].size();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Resets a spike group index for one channel: the group index is a grouping 
/// index by selection in a cluster plot (see also frmCluster)
//------------------------------------------------------------------------------
void TSWSpikes::SpikeGroupReset(unsigned int nChannelIndex)
{
   AssertIndex(nChannelIndex);
   EnterCriticalSection(&m_cs);
   try
      {
      unsigned int n;
      for (n = 0; n < m_vvSpikes[nChannelIndex].size(); n++)
         m_vvSpikes[nChannelIndex][n]->m_nGroupIndex = -1;
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes all spikes belonging to a particular epoche from all channels
//------------------------------------------------------------------------------
void TSWSpikes::Remove(unsigned int nEpocheIndex)
{
   unsigned int nChannel;
   for (nChannel = 0; nChannel < GetNumChannels(); nChannel++)
      Remove(nChannel, nEpocheIndex);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes all spikes belonging to a particular epoche from one channels
//------------------------------------------------------------------------------
void TSWSpikes::Remove(unsigned int nChannelIndex, unsigned int nEpocheIndex)
{
   if (m_vvSpikes.size() <= nChannelIndex)
      throw Exception("channel index error in " + UnicodeString(__FUNC__));
   unsigned int n, nIndex;
   unsigned int nNumSpikes = (unsigned int)m_vvSpikes[nChannelIndex].size();
   for (n = 0; n < nNumSpikes; n++)
      {
      TSWSpike *psms = m_vvSpikes[nChannelIndex][nNumSpikes - n - 1];
      if (psms->m_nEpocheIndex == nEpocheIndex)
         {
         m_vvSpikes[nChannelIndex].erase(m_vvSpikes[nChannelIndex].begin() + (int)(nNumSpikes - n - 1));
         TRYDELETENULL(psms);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds spikes from one epoche by threshold evaluation. if a pointer to epoche
/// audio pcm data is passed, it is used instead of passed epoches data
//------------------------------------------------------------------------------
void TSWSpikes::Add(TSWEpoche *pswe, vvd *pvvd)
{
   vvd vvdData = pvvd ? *pvvd : pswe->GetData();
   if (!vvdData.size())
      return;
   EnterCriticalSection(&m_cs);
   try
      {
      unsigned int n, nChannel;
      // use fix PostThreshold if set at all
      unsigned int nPostThreshold = (unsigned int)m_nPostThreshold;
      // otherwise use spikelength and prethreshold to calculate it
      if (!nPostThreshold)
         nPostThreshold =(unsigned int)( m_nSpikeLength - m_nPreThreshold);
      unsigned int nSize = (unsigned int)vvdData[0].size();
      unsigned int nStopLoop = nSize - (unsigned int)(m_nSpikeLength - m_nPreThreshold);
      for (nChannel = 0; nChannel < vvdData.size(); nChannel++)
         {
         // decide only once about pos or neg threshold...
         if (pswe->m_vdThreshold[nChannel] > 0)
            {
            for (n = (unsigned int)m_nPreThreshold; n < nStopLoop; n++)
               {
               if (vvdData[nChannel][n] > pswe->m_vdThreshold[nChannel])
                  {
                  TSWSpike *psms = new TSWSpike(this, pswe, vvdData, n, nChannel);
                  m_vvSpikes[nChannel].push_back(psms);
                  n += nPostThreshold;
                  }
               }
            }
         else
            {
            for (n = (unsigned int)m_nPreThreshold; n < nStopLoop; n++)
               {
               if (vvdData[nChannel][n] < pswe->m_vdThreshold[nChannel])
                  {
                  TSWSpike *psms = new TSWSpike(this, pswe, vvdData, n, nChannel);
                  m_vvSpikes[nChannel].push_back(psms);
                  n += nPostThreshold;
                  }
               }
            }
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds spikes from a Spikes XML subnode
//------------------------------------------------------------------------------
void TSWSpikes::Add(_di_IXMLNode xmlSpikes)
{
   EnterCriticalSection(&m_cs);
   try
      {
      AnsiString asData;
      double d;
      int n, nSpike;
      for (nSpike = 0; nSpike < xmlSpikes->ChildNodes->Count; nSpike++)
         {
         _di_IXMLNode xmlSpike = xmlSpikes->ChildNodes->Nodes[nSpike];

         TSWSpike *psms = new TSWSpike(this);
         if (!TryStrToDouble(GetXMLValue(xmlSpike, "SpikeTime"), d))
            {
            throw Exception("invalid SpikeTime found in a spike");
            }

         // NOTE: values were written 1-based !!!
         psms->m_dSpikeTime = d;

         if (!TryStrToInt(GetXMLValue(xmlSpike, "SpikePosition"), n))
            throw Exception("invalid SpikePosition found in a spike");
         psms->m_nSpikePos = (unsigned int)n-1;
         if (!TryStrToInt(GetXMLValue(xmlSpike, "StimIndex"), n))
            throw Exception("invalid StimIndex found in a spike");
         psms->m_nStimIndex = (unsigned int)n-1;
         if (!TryStrToInt(GetXMLValue(xmlSpike, "EpocheIndex"), n))
            throw Exception("invalid EpocheIndex found in a spike");
         psms->m_nEpocheIndex = (unsigned int)n-1;
         if (!TryStrToInt(GetXMLValue(xmlSpike, "RepetitionIndex"), n))
            throw Exception("invalid Repetition found in a spike");
         psms->m_nRepetitionIndex = (unsigned int)n-1;
         if (!TryStrToInt(GetXMLValue(xmlSpike, "Channel"), n))
            throw Exception("invalid Channel found in a spike");
         psms->m_nChannelIndex = (unsigned int)n-1;
         if (!TryStrToDouble(GetXMLValue(xmlSpike, "Threshold"), d))
            throw Exception("invalid Threshold found in a spike");
         psms->m_dThreshold = d;

         // decode data
         asData = GetXMLValue(xmlSpike, "Data");
         if (asData.IsEmpty())
            throw Exception("empty Data found in a spike");

         Sysutils::TBytes tbData = DecodeBase64(asData);
         if (tbData.Length != (int)(m_nSpikeLength *(int)sizeof(double)))
            throw Exception("Data with invalid length found in a spike (expected length: " +
                     IntToStr((int)(m_nSpikeLength *(int)sizeof(double))) +
                     ", current length: " +
                     IntToStr((int)tbData.Length)
            );
         CopyMemory(&psms->m_vadData[0], &tbData[0], (unsigned int)m_nSpikeLength*sizeof(double));
         psms->Init(m_dSampleRate);
         m_vvSpikes[psms->m_nChannelIndex].push_back(psms);
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns a spike by channel and index
//------------------------------------------------------------------------------
std::valarray<double>& TSWSpikes::GetSpike(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_vadData;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns a spike parameter bei channel, index and parameter type
//------------------------------------------------------------------------------
double   TSWSpikes::GetSpikeParam(unsigned int nChannelIndex, unsigned int nIndex, TSpikeParam sp)
{
   AssertIndex(nChannelIndex);
   EnterCriticalSection(&m_cs);
   double d = 0.0;
   try
      {
      // SP_LAST not handled by purpose
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wswitch-enum"
      switch (sp)
         {
         case SP_TOTALAMPLITUDE: d = m_vvSpikes[nChannelIndex][nIndex]->TotalAmplitude(); break;
         case SP_PEAK1:          d = m_vvSpikes[nChannelIndex][nIndex]->Peak1(); break;
         case SP_PEAK2:          d = m_vvSpikes[nChannelIndex][nIndex]->Peak2(); break;
         case SP_PEAKPOS:        d = m_vvSpikes[nChannelIndex][nIndex]->PeakPos(); break;
         case SP_PEAKNEG:        d = m_vvSpikes[nChannelIndex][nIndex]->PeakNeg(); break;
         case SP_PEAK2PEAK:      d = m_vvSpikes[nChannelIndex][nIndex]->PeakToPeak(); break;
         case SP_THRS2PEAK2:     d = m_vvSpikes[nChannelIndex][nIndex]->ThresholdToPeak2(); break;
         default: throw Exception("unknown spike patrameter requested");
         }
      #pragma clang diagnostic pop
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
   return d;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spike time by channel and index
//------------------------------------------------------------------------------
double   TSWSpikes::GetSpikeTime(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_dSpikeTime;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// returns spike threshold by channel and index
//------------------------------------------------------------------------------
double   TSWSpikes::GetThreshold(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_dThreshold;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spike position by channel and index
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetSpikePosition(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nSpikePos;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spike group by channel and index
//------------------------------------------------------------------------------
int      TSWSpikes::GetSpikeGroup(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nGroupIndex;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets spike group by channel and index
//------------------------------------------------------------------------------
void     TSWSpikes::SetSpikeGroup(unsigned int nChannelIndex, unsigned int nIndex, int nGroup)
{
   AssertIndex(nChannelIndex);
   m_vvSpikes[nChannelIndex][nIndex]->m_nGroupIndex = nGroup;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spike stimulus index by channel and index
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetStimIndex(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nStimIndex;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spike epoche index by channel and index
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetEpocheIndex(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nEpocheIndex;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spike repetition index by channel and index
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetRepetitionIndex(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nRepetitionIndex;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// CLASS TSWSpike containing info about one spike
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initializes members, copies passed data
//------------------------------------------------------------------------------
TSWSpike::TSWSpike(  TSWSpikes* pSpikes,
                     TSWEpoche *pswe,
                     vvd   &rvvdEpocheData,
                     unsigned int nPos,
                     unsigned int nChannelIndex)
   : m_nGroupIndex(-1)
{
   m_nStimIndex         = pswe->m_nStimIndex;
   m_nEpocheIndex       = pswe->m_nIndex;
   m_nRepetitionIndex   = pswe->m_nRepetitionIndex;
   m_dThreshold      = pswe->m_vdThreshold[nChannelIndex];
   m_nChannelIndex   = nChannelIndex;
   m_nSpikePos       = nPos;
   // copy the pure spike data
   m_vadData.resize((unsigned int)pSpikes->m_nSpikeLength);
   CopyMemory(&m_vadData[0], &rvvdEpocheData[nChannelIndex][nPos-(unsigned int)pSpikes->m_nPreThreshold], (unsigned int)pSpikes->m_nSpikeLength*sizeof(double));
   m_dTrigT          = (double)pSpikes->m_nPreThreshold / pSpikes->m_dSampleRate;
   m_dSpikeTime      = (double)nPos / pSpikes->m_dSampleRate;

   Init(pSpikes->m_dSampleRate);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor creating an empty spike
//------------------------------------------------------------------------------
TSWSpike::TSWSpike(TSWSpikes* pSpikes)
   : m_nGroupIndex(-1)
{
   m_vadData.resize((unsigned int)pSpikes->m_nSpikeLength);
   m_dTrigT = (double)pSpikes->m_nPreThreshold / pSpikes->m_dSampleRate;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Init routine: determines spike parameters
//------------------------------------------------------------------------------
void TSWSpike::Init(double dSampleRate)
{
   // determine general spike parameters
   m_dPeakUA = 0.0;
   m_dPeakDA = 0.0;
   m_dPeakUT = 0.0;
   m_dPeakDT = 0.0;
   unsigned int n;
   double d;
   for (n = 0; n < m_vadData.size(); n++)
      {
      d = m_vadData[n];
      if (d > m_dPeakUA)
         {
         m_dPeakUA = d;
         m_dPeakUT = (double)n / dSampleRate;
         }
      else if (m_vadData[n] < m_dPeakDA)
         {
         m_dPeakDA = d;
         m_dPeakDT = (double)n / dSampleRate;
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns total amplitude of spike
//------------------------------------------------------------------------------
double   TSWSpike::TotalAmplitude()
{
   return fabs(m_dPeakUA) + fabs(m_dPeakDA);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns amplitude of the 1st component (“phase”).
//------------------------------------------------------------------------------
double   TSWSpike::Peak1()
{
   return m_dPeakUT < m_dPeakDT ? m_dPeakUA : m_dPeakDA;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns amplitude of the 2nd component (“phase”).
//------------------------------------------------------------------------------
double   TSWSpike::Peak2()
{
   return m_dPeakUT > m_dPeakDT ? m_dPeakUA : m_dPeakDA;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns amplitude of positive peak
//------------------------------------------------------------------------------
double   TSWSpike::PeakPos()
{
   return m_dPeakUA;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns amplitude of negative peak
//------------------------------------------------------------------------------
double   TSWSpike::PeakNeg()
{
   return m_dPeakDA;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// peak to peak time in microseconds
//------------------------------------------------------------------------------
double   TSWSpike::PeakToPeak()
{
   return fabs(m_dPeakUT - m_dPeakDT)*1000000.0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// peak to threshold to peak 2 time in microseconds
//------------------------------------------------------------------------------
double   TSWSpike::ThresholdToPeak2()
{
   double d = m_dPeakUT > m_dPeakDT ? fabs(m_dPeakUT - m_dTrigT) : fabs(m_dPeakDT - m_dTrigT);

   return d*1000000.0;
}
//------------------------------------------------------------------------------

