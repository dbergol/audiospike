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
#include <algorithm>
#include "Encddecd.hpp"

//------------------------------------------------------------------------------
#pragma warn -aus
#pragma package(smart_init)

//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
/// constructor initializes members
//------------------------------------------------------------------------------
TSWSpikes::TSWSpikes()
{
   InitializeCriticalSection(&m_cs);
   m_nTestFlag = 0;
   m_bInitialized = false;
   m_dSampleRate = 44100.0;
   m_dSampleRateDevider = 1.0;
   m_bRejectSingleSignSpikes = false;
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
/// sets number of channels, only allowed if empty
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
      throw Exception("Samplerate cannot be changed if spikes are stored");

   EnterCriticalSection(&m_cs);
   try
      {
      m_dSampleRateDevider = dSampleRateDevider;
      m_dSampleRate     = dSampleRate/dSampleRateDevider;

      // to be sure: recalculate everything for ALL methods
      unsigned int n;
      for (n = 0; n < m_sdmDetectionMethods.m_vsdmMethods.size(); n++)
         m_sdmDetectionMethods.m_vsdmMethods[n]->RecalculatePositionsFromTimes(this);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// returns spike length in samples
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetSpikeLengthSamples(void)
{
   return  m_nSpikeLength;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spike length
//------------------------------------------------------------------------------
double TSWSpikes::GetSpikeLength(void)
{
   return  m_dSpikeLength;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets spike length
//------------------------------------------------------------------------------
void TSWSpikes::SetSpikeLength(double dSpikeLength)
{
   if (!IsEmpty())
      throw Exception("SpikeLength cannot be changed if spikes are stored");

   EnterCriticalSection(&m_cs);
   try
      {
      m_dSpikeLength    = dSpikeLength;
      m_nSpikeLength    = (unsigned int)(m_dSpikeLength * m_dSampleRate);

      // set displayed total peak length in microseconds
      m_swspSpikePars.SetSpikeLength(m_dSpikeLength * 1000000.0);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

// NOTE: the next two methods are sort of "hard coded", but there has to be one
// point where caller needs to know what he's calling!!


//------------------------------------------------------------------------------
/// sets spikelength and pre- and postthreshold for Version 1
//------------------------------------------------------------------------------
void TSWSpikes::SetDetectionMethod1(double dSpikeLength, double dPreThreshold, double dPostThreshold)
{
  if (!IsEmpty())
      throw Exception("DetectionMethod values cannot be changed if spikes are stored");

   EnterCriticalSection(&m_cs);
   try
      {
      m_sdmDetectionMethods.SetMethodIndex(SDM_VERSION_1);
      SetSpikeLength(dSpikeLength);
      m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_1]->SetVariable("PreThreshold", dPreThreshold);
      m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_1]->SetVariable("PostThreshold", dPostThreshold);
      m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_1]->RecalculatePositionsFromTimes(this);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets RefractoryTime, RefractoryTimeTailFactor and calculates SpikeLength
/// for Version 2
//------------------------------------------------------------------------------
void  TSWSpikes::SetDetectionMethod2(double dRefractoryTime, double dRefractoryTimeTailFactor)
{
  if (!IsEmpty())
      throw Exception("DetectionMethod values cannot be changed if spikes are stored");

   EnterCriticalSection(&m_cs);
   try
      {
      m_sdmDetectionMethods.SetMethodIndex(SDM_VERSION_2);
      m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_2]->SetVariable("RefractoryTime", dRefractoryTime);
      m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_2]->SetVariable("RefractoryTimeTailFactor", dRefractoryTimeTailFactor);

      // calculate
      SetSpikeLength((2.0+dRefractoryTimeTailFactor)*dRefractoryTime);

      m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_2]->RecalculatePositionsFromTimes(this);
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
/// Calls different available spike detection methods
//------------------------------------------------------------------------------
unsigned int TSWSpikes::CallSpikeDetectionMethod( TSWSpike* psms,
                                          std::valarray<double >* pvadEpoche)
{
   // set default for refractoryTime
   unsigned int nRefractoryTime = GetSpikeLengthSamples();

   TSpikeDetectionMethodIndex sdmi = m_sdmDetectionMethods.GetMethodIndex();
   //---------------------------------------------------------------------------
   if (sdmi == SDM_VERSION_1)
      nRefractoryTime = m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_1]->SpikeDetection(this, psms, pvadEpoche);
   //---------------------------------------------------------------------------
   // Version 2: "new" AudioSpike Spike Detection Method
   // More complicated, done in separate function....
   //---------------------------------------------------------------------------
   else if (sdmi == SDM_VERSION_2)
      {
      nRefractoryTime = m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_2]->SpikeDetection(this, psms, pvadEpoche);
      }
   //---------------------------------------------------------------------------
   else
      throw Exception("Unknown spike detection method passed to TSWSpikes::CallSpikeDetectionMethod");

   // If corresponding flag is set, reject spikes without a negative Peak+ or
   // a positive Peak-
   if (m_bRejectSingleSignSpikes)
      {
      if (psms->m_dPeakDA > 0.0 || psms->m_dPeakUA < 0.0)
         return 0;
      }

   return nRefractoryTime;
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
      unsigned int nStopLoop = (unsigned int)vvdData[0].size() - GetSpikeLengthSamples();

      // NOTE: we only consider a sample a "trigger", if the course of the spike
      // crosses the threshold "in the correct direction"
      // This means:
      //   - when searching for a trigger, we wait initially until the current value
      //     is BELOW the threshold (again - maybe multitple times??)
      //   - afterwards we continue the loop untile the threshold is exceeded again
      double         dSample;
      int            nSamplesFoundBelowThreshold = 0;
      bool           bAboveThreshold;
      bool           bPositiveThreshold;

      for (nChannel = 0; nChannel < vvdData.size(); nChannel++)
         {
         nSamplesFoundBelowThreshold = 0;

         bPositiveThreshold = pswe->m_vdThreshold[nChannel] > 0;

         for (n = 0; n < nStopLoop; n++)
            {
            dSample = vvdData[nChannel][n];
            if (bPositiveThreshold)
               bAboveThreshold = dSample > pswe->m_vdThreshold[nChannel];
            else
               bAboveThreshold = dSample < pswe->m_vdThreshold[nChannel];

            // no values below threshold found? Check if we have to simply continue
            if (!nSamplesFoundBelowThreshold) // maybe nSamplesFoundBelowThreshold < X instead?
               {
               // if still above threshold: continue directly
               if (bAboveThreshold)
                  continue;
               }

            // reaching this point, exceeding the threshold means: it's a threshold crossing point!
            if (bAboveThreshold)
               {
               // create speak and set all values, that do NOT depend on
               // current detection method
               TSWSpike *psms             = new TSWSpike(GetSpikeLengthSamples());
               psms->m_nStimIndex         = pswe->m_nStimIndex;
               psms->m_nEpocheIndex       = pswe->m_nIndex;
               psms->m_nRepetitionIndex   = pswe->m_nRepetitionIndex;
               psms->m_dThreshold         = pswe->m_vdThreshold[nChannel];
               psms->m_nChannelIndex      = nChannel;
               // threshold crossing point is stored as absoute position within epoche (!)
               psms->m_nThresholdCrossingPosition = n;
               psms->m_dTrigT             = (double)n / m_dSampleRate;


               // set "default" spike position: to m_nThresholdCrossingPosition:
               // must be adjusted by detection method!
               psms->m_nSpikePos          = n;


               // DEPENDS ON METHOD
               // call particular spike detection method. It will
               // - copies the raw spike data from epoche to spike
               // - calculate more spike values depending on method
               // - applies optional constraints
               // - return refractory time (number of samples to advance the search window after this peak).

               // call spike detection method
               unsigned int nRefractoryTime = CallSpikeDetectionMethod(psms, &vvdData[nChannel]);

               // will return RefractoryTime > 0 on success
               if (nRefractoryTime)
                  {

                  // NOTE: before storing a spike, a spike detection  method might like to
                  // "touch" the spike again, e.g. for "moving" it by copying from a different
                  // position within the epoche including adjusting other values.This is currently
                  // done only for "Version 2". This is NOT done within CallSpikeDetectionMethod
                  //
                  m_vvSpikes[nChannel].push_back(psms);
                  }
               // otherwise delete the spike and continue
               else
                  {
                  // delete spike on failure
                  delete psms;
                  }

               // reset nSamplesFoundBelowThreshold and advance window
               nSamplesFoundBelowThreshold = 0;
               n += nRefractoryTime;
               }
            // otherwise increase number of values found below threshold
            else
               nSamplesFoundBelowThreshold++;
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
      double   dSpikeTime, dThreshold;
      int      nSpikePos, nThresholdCrossingPosition, nStimIndex, nEpocheIndex,
               nRepetitionIndex, nChannelIndex;
      AnsiString asData;

      // retrieve PreThreshold for Version 1 peak detection ONCE before the loops, in case we need it
      unsigned int nPreThreshold = m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_1]->GetVariableInt("PreThreshold");


      // NOTE: accessing nodes in XML-Interface is very slow. But we may have
      // maaaaany spikes childnodes (single spikes) here. Therefor we use the
      // low level for the DOM-Interface here node
      _di_IDOMNode xmlSpike = xmlSpikes->GetDOMNode()->childNodes->item[0];
      while (xmlSpike)
         {
         if (!TryStrToDouble(GetNodeChildValue(xmlSpike, "SpikeTime"), dSpikeTime))
            throw Exception("invalid SpikeTime found in a spike");
         if (!TryStrToInt(GetNodeChildValue(xmlSpike, "SpikePosition"), nSpikePos))
            throw Exception("invalid SpikePosition found in a spike");
         if (!TryStrToInt(GetNodeChildValue(xmlSpike, "StimIndex"), nStimIndex))
            throw Exception("invalid StimIndex found in a spike");
         if (!TryStrToInt(GetNodeChildValue(xmlSpike, "EpocheIndex"), nEpocheIndex))
            throw Exception("invalid EpocheIndex found in a spike");
         if (!TryStrToInt(GetNodeChildValue(xmlSpike, "RepetitionIndex"), nRepetitionIndex))
            throw Exception("invalid Repetition found in a spike");
         if (!TryStrToInt(GetNodeChildValue(xmlSpike, "Channel"), nChannelIndex))
            throw Exception("invalid Channel found in a spike");
         if (!TryStrToDouble(GetNodeChildValue(xmlSpike, "Threshold"), dThreshold))
            throw Exception("invalid Threshold found in a spike");

         // decode raw spike waveform
         asData = GetNodeChildValue(xmlSpike, "Data");
         if (asData.IsEmpty())
            throw Exception("empty Data found in a spike");

         Sysutils::TBytes tbData = DecodeBase64(asData);
         if (tbData.Length != (int)(GetSpikeLengthSamples() *sizeof(double)))
            throw Exception("Data with invalid length found in a spike (expected length: " +
                     IntToStr((int)GetSpikeLengthSamples()) +
                     ", current length: " +
                     IntToStr((int)tbData.Length / (int)sizeof(double))
            );

         // create the new spike
         TSWSpike *psms             = new TSWSpike(GetSpikeLengthSamples());
         // NOTE: values were written 1-based !!!
         psms->m_dSpikeTime         = dSpikeTime;
         psms->m_nSpikePos          = (unsigned int)nSpikePos-1;
         psms->m_nStimIndex         = (unsigned int)nStimIndex-1;
         psms->m_nEpocheIndex       = (unsigned int)nEpocheIndex-1;
         psms->m_nRepetitionIndex   = (unsigned int)nRepetitionIndex-1;
         psms->m_nChannelIndex      = (unsigned int)nChannelIndex-1;
         psms->m_dThreshold         = dThreshold;

         // copy the raw spike data
         CopyMemory(&psms->m_vadData[0], &tbData[0], GetSpikeLengthSamples()*sizeof(double));


         // compatibility with versions before 2.5: these results were ALWAYS processed
         // using spike detection "Version 1" and they differ with respect to
         // - ThresholdCrossingPosition does not exist
         // - meaning of SpikePosition has changed
         // - PeakPosPosition and PeakNegPosition were NOT stored

         // First: adjust ThresholdCrossingPosition and SpikePosition if needed
         if (!TryStrToInt(GetNodeChildValue(xmlSpike, "ThresholdCrossingPosition"), nThresholdCrossingPosition))
            {
            // set new absolute trigger crossing position
            nThresholdCrossingPosition = (int)psms->m_nSpikePos;
            // afterwards adjust spike position by m_nPreThreshold
            psms->m_nSpikePos -= nPreThreshold;
            }
         else
            // see above: values were written 1-based !!!
            nThresholdCrossingPosition -= 1;

         psms->m_nThresholdCrossingPosition = (unsigned int)nThresholdCrossingPosition;
         psms->m_dTrigT             = (double)psms->m_nThresholdCrossingPosition / m_dSampleRate;


         // Second: check, if peak positions are available
         int nPeakPosPosition, nPeakNegPosition = 0;
         if (  TryStrToInt(GetNodeChildValue(xmlSpike, "PeakPosPosition"), nPeakPosPosition)
            && TryStrToInt(GetNodeChildValue(xmlSpike, "PeakNegPosition"), nPeakNegPosition)
            )
            {
            // calculate/retrieve values from positions
            psms->m_nPeakUT   = (unsigned int)nPeakPosPosition-1;
            psms->m_nPeakDT   = (unsigned int)nPeakNegPosition-1;
            psms->m_dPeakUA   = psms->m_vadData[psms->m_nPeakUT - psms->m_nSpikePos];
            psms->m_dPeakDA   = psms->m_vadData[psms->m_nPeakDT - psms->m_nSpikePos];
            psms->m_dPeakUT   = (double)psms->m_nPeakUT / m_dSampleRate;
            psms->m_dPeakDT   = (double)psms->m_nPeakDT / m_dSampleRate;
            }
         // otherwise call old (Version 1) peak extraction method
         else
            {
            // call 'old' spikedetection WITHOUT passing epoche data: will only
            // re-calculate the peak+/peak- values!
            m_sdmDetectionMethods.m_vsdmMethods[SDM_VERSION_1]->SpikeDetection(this, psms);
            }

         // add it to spikes array
         m_vvSpikes[psms->m_nChannelIndex].push_back(psms);

         xmlSpike = xmlSpike->nextSibling;
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
         default: throw Exception("unknown spike parameter requested");
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
/// returns position of positive peak in samples
//------------------------------------------------------------------------------
unsigned int   TSWSpikes::GetSpikePeakPosPosition(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nPeakUT;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns position of negative peak in samples
//------------------------------------------------------------------------------
unsigned int   TSWSpikes::GetSpikePeakNegPosition(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nPeakDT;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns spikes threshold crossing  position by channel and index
//------------------------------------------------------------------------------
unsigned int TSWSpikes::GetThresholdCrossingPosition(unsigned int nChannelIndex, unsigned int nIndex)
{
   AssertIndex(nChannelIndex);
   return m_vvSpikes[nChannelIndex][nIndex]->m_nThresholdCrossingPosition;
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
/// CLASS TSWSpike containing info about one spike. NOTE: all members are
/// filled after
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor initializes spike data valarray and group index
//------------------------------------------------------------------------------
TSWSpike::TSWSpike(unsigned int nSpikeLength)
   : m_nGroupIndex(-1)
{
   m_vadData.resize(nSpikeLength);
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

