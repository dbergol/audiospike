//------------------------------------------------------------------------------
/// \file SWEpcoches.cpp
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
#pragma hdrstop

#include "SWEpoches.h"
#include "SWTools.h"
#include "SpikeWareMain.h"
#include "frmStimuli.h"
#include <math.h>
#include <algorithm>
//------------------------------------------------------------------------------

#pragma package(smart_init)
#pragma warn -aus
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// absolute value to be exceeded to be interpreted as a trigger
//------------------------------------------------------------------------------
#define TRIGGER_THRESHOLD 0.1f
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// constructor initializes members
//------------------------------------------------------------------------------
TSWEpoche::TSWEpoche(unsigned int nNumChannels,
                     unsigned int nNumSamples,
                     unsigned int nStimIndex,
                     unsigned int nRepetitionIndex,
                     const std::vector<double >& rvdThreshold,
                     UnicodeString usFileName)
   : m_usFileName(usFileName), m_vvdData(nNumChannels, std::valarray<double>(nNumSamples))
{
   m_nNumChannels       = nNumChannels;
   m_nNumSamples        = nNumSamples;
   m_nStimIndex         = nStimIndex;
   m_nRepetitionIndex   = nRepetitionIndex;
   m_vdThreshold        = rvdThreshold;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns epoch data. If m_vvdData is (still) empty it is read from file
//------------------------------------------------------------------------------
vvd TSWEpoche::GetData()
{
   if (m_vvdData.size())
      return m_vvdData;

   vvd vvdData(m_nNumChannels, std::valarray<double>(m_nNumSamples));

   std::valarray<float > vaf(m_nNumSamples);
   TFileStream* pfs = NULL;
   try
      {
      pfs = new TFileStream(m_usFileName, fmOpenRead | fmShareDenyNone);
      __int64 nPos = m_nIndex * m_nNumChannels * m_nNumSamples * sizeof(float);
      if (nPos > pfs->Size-1)
         throw Exception("cannot read epoche data: position exceeded");
      pfs->Seek(nPos, soBeginning);
      unsigned int n, m;
      for (n = 0; n < m_nNumChannels; n++)
         {
         pfs->ReadBuffer(&vaf[0], (NativeInt)(m_nNumSamples * sizeof(float)));
         // convert floats to double
         for (m = 0; m < m_nNumSamples; m++)
            vvdData[n][m] = (double)vaf[m];
         }
      }
   __finally
      {
      TRYDELETENULL(pfs);
      }
   return vvdData;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------ 
/// clears m_vvdData member
//------------------------------------------------------------------------------
void TSWEpoche::ClearData()
{
   m_vvdData.clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// CLASS TSWEpoches containing info about multiple epoches
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor initializes members
//------------------------------------------------------------------------------
TSWEpoches::TSWEpoches()
   : m_pfsWrite(NULL), m_pfsWriteProbeMic(NULL)
{
   InitializeCriticalSection(&m_cs);
   InitializeCriticalSection(&m_csReset);
   m_ptl = new TList();
   m_nEpochesTotal      = 0;
   m_nRecEpochePos      = -1;
   m_nTriggersDetected  = 0;
   m_nTriggerTestTriggersPlayed = 0;
   m_nSamplesPlayed     = 0;
   m_nLastTriggerPos    = 0;
   m_nLastTriggerDistance = 0;
   m_bTriggerError      = false;
   m_nFirstTriggerError = -1;
   m_nStimIndexAtStart  = 0;

   m_nNumTriggerSamplesInNextBuffer = 0;
   m_nNumTriggerSamplesInNextBufferPlay = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, does cleanup
//------------------------------------------------------------------------------
TSWEpoches::~TSWEpoches()
{
   Clear();
   TRYDELETENULL(m_ptl);
   TRYDELETENULL(m_pfsWrite);
   TRYDELETENULL(m_pfsWriteProbeMic);
   DeleteCriticalSection(&m_cs);
   DeleteCriticalSection(&m_csReset);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// does assertion on index of passed channel
//------------------------------------------------------------------------------
void TSWEpoches::AssertIndex(unsigned int nChannelIndex)
{
   if (nChannelIndex > m_vvfEpoche.size())
      throw Exception("spike channel index exceeded");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// initializes vectors to correct sizes
//------------------------------------------------------------------------------
void  TSWEpoches::Initialize(unsigned int nNumChannels, unsigned int nSize)
{
   Clear();

   m_vvfEpoche.resize(nNumChannels);
   m_vdThreshold.resize(nNumChannels);
   unsigned int nChannel;
   for (nChannel = 0; nChannel < nNumChannels; nChannel++)
      m_vvfEpoche[nChannel].resize(nSize);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns number of channels
//------------------------------------------------------------------------------
unsigned int TSWEpoches::GetNumChannels()
{
   return (unsigned int)m_vvfEpoche.size();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns threshold for a channel
//------------------------------------------------------------------------------
double TSWEpoches::GetThreshold(unsigned int nChannelIndex)
{
   AssertIndex(nChannelIndex);
   return m_vdThreshold[nChannelIndex];
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets threshold for a channel
//------------------------------------------------------------------------------
void TSWEpoches::SetThreshold(unsigned int nChannelIndex, double dThreshold)
{
   AssertIndex(nChannelIndex);
   EnterCriticalSection(&m_cs);
   try
      {
      m_vdThreshold[nChannelIndex] = dThreshold;
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// intializes saving to file of epoches and probe mic recording in overwrite mode
//------------------------------------------------------------------------------
void TSWEpoches::InitSave()
{
   TRYDELETENULL(m_pfsWrite);
   TRYDELETENULL(m_pfsWriteProbeMic);
   m_pfsWrite = new TFileStream(formSpikeWare->m_usResultPath + "epoches.pcm", fmCreate | fmShareDenyWrite);
   // for insitu AND 'save probemics' create second write stream
   if (formSpikeWare->IsInSitu() && formSpikeWare->m_smp.m_bSaveProbeMics)
      m_pfsWriteProbeMic = new TFileStream(formSpikeWare->m_usResultPath + "probemics.pcm", fmCreate | fmShareDenyWrite);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// intializes saving to file of epoches and probe mic recording in append mode
//------------------------------------------------------------------------------
void TSWEpoches::AppendSave()
{
   TRYDELETENULL(m_pfsWrite);
   TRYDELETENULL(m_pfsWriteProbeMic);
   m_pfsWrite = new TFileStream(formSpikeWare->m_usResultPath + "epoches.pcm", fmOpenWrite | fmShareDenyWrite);
   m_pfsWrite->Seek(0, soFromEnd);
   // for insitu AND 'save probemics' create second write stream
   if (formSpikeWare->IsInSitu() && formSpikeWare->m_smp.m_bSaveProbeMics)
      {
      m_pfsWriteProbeMic = new TFileStream(formSpikeWare->m_usResultPath + "probemics.pcm", fmOpenWrite | fmShareDenyWrite);
      m_pfsWriteProbeMic->Seek(0, soFromEnd);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// exits saving to file by closing/deleting TFileStreams
//------------------------------------------------------------------------------
void TSWEpoches::DoneSave()
{
   TRYDELETENULL(m_pfsWrite);
   TRYDELETENULL(m_pfsWriteProbeMic);
   Application->ProcessMessages();
}
//------------------------------------------------------------------------------

#ifdef CHKCHNLS
//------------------------------------------------------------------------------
/// debug function for checking channels
//------------------------------------------------------------------------------
void TSWEpoches::SetTriggerChannel(unsigned int nTriggerChannel)
{
   m_nTriggerChannel = nTriggerChannel;
}
//------------------------------------------------------------------------------
#endif

//------------------------------------------------------------------------------
/// cleans up all stored epoches
//------------------------------------------------------------------------------
void TSWEpoches::Clear()
{
   EnterCriticalSection(&m_cs);
   try
      {
      int n;
      for (n = 0; n < m_ptl->Count; n++)
         {
         TSWEpoche* p = (TSWEpoche*)m_ptl->Items[n];
         TRYDELETENULL(p);
         }
      m_ptl->Clear();
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
   m_nEpochesTotal = 0;
   m_nRecEpochePos = -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resets epoche data
//------------------------------------------------------------------------------
void TSWEpoches::Reset()
{
   EnterCriticalSection(&m_csReset);
   try
      {
      unsigned int nChannel;
      for (nChannel = 0; nChannel < m_vvfEpoche.size(); nChannel++)
         m_vvfEpoche[nChannel] = 0.0f;
      m_nRecEpochePos = 0;
      m_nTriggersDetected = 0;
      m_nTriggerTestTriggersPlayed = 0;
      }
   __finally
      {
      LeaveCriticalSection(&m_csReset);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// starts recording of epoches
//------------------------------------------------------------------------------
void TSWEpoches::Start()
{
   m_nRecEpochePos      = -1;
   m_nTriggersDetected  = 0;
   m_nTriggerTestTriggersPlayed = 0;
   m_nSamplesPlayed     = 0;
   m_nLastTriggerPos    = 0;
   m_nLastTriggerDistance = 0;
   m_bTriggerError      = false;
   m_nFirstTriggerError = -1;
   m_nDoubleTriggerDistance = 4*formSpikeWare->m_smp.m_nTriggerLength / (int)formSpikeWare->m_swsSpikes.m_dSampleRateDevider;
   m_nTriggerTestTriggersPlayed = 0;
   m_nStimIndexAtStart = formSpikeWare->m_nStimPlayIndex;
   // finally initialize buffers for probemics
   m_vvfEpocheProbeMic.clear();
   if (formSpikeWare->IsInSitu() && formSpikeWare->m_smp.m_bSaveProbeMics)
      {
      m_vvfEpocheProbeMic.resize(formSpikeWare->m_smp.m_swcUsedChannels.GetProbeMics().size());
      unsigned int n;
      for (n = 0; n < m_vvfEpocheProbeMic.size(); n++)
         m_vvfEpocheProbeMic[n].resize(m_vvfEpoche[0].size());
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// pushes passed data as new epoche to own buffer
//------------------------------------------------------------------------------
TSWEpoche* TSWEpoches::Push(  vvf& rvvfData,
                              const std::vector<double >& rvdThreshold,
                              unsigned int nStimIndex,
                              unsigned int nRepetitionIndex)
{
   if (!rvvfData.size())
      return NULL;

   TSWEpoche *pswe = NULL;
   vvd vvdData(rvvfData.size(), std::valarray<double>(rvvfData[0].size()));

   EnterCriticalSection(&m_cs);
   try
      {
      try
         {
         pswe = new TSWEpoche((unsigned int)rvvfData.size(),
                              (unsigned int)rvvfData[0].size(),
                              nStimIndex,
                              nRepetitionIndex,
                              rvdThreshold,
                              formSpikeWare->m_usResultPath + "epoches.pcm");


         pswe->m_nIndex = m_nEpochesTotal++;
         unsigned int n, m;
         for (n = 0; n < rvvfData.size(); n++)
            {
            // save data as raw floats
            if (!!m_pfsWrite)
               m_pfsWrite->WriteBuffer(&rvvfData[n][0], (NativeInt)(rvvfData[n].size()*sizeof(float)));
            // convert floats to double
            for (m = 0; m < rvvfData[n].size(); m++)
               pswe->m_vvdData[n][m] = (double)rvvfData[n][m];
            }

         m_ptl->Add(pswe);
         }
      catch (...)
         {
         TRYDELETENULL(pswe);
         throw;
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }

   return pswe;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns oldest epoche from stored data and removes it
//------------------------------------------------------------------------------
TSWEpoche*  TSWEpoches::Pop(bool &bLast)
{
   TSWEpoche* p = NULL;
   EnterCriticalSection(&m_cs);
   try
      {
      if (m_ptl->Count)
         {
         p = (TSWEpoche*)m_ptl->Items[0];
         m_ptl->Delete(0);
         bLast = m_ptl->Count == 0;
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
   return p;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns an epoche without removing it. Negative indices count from last epoche
//------------------------------------------------------------------------------
TSWEpoche* TSWEpoches::Get(int nIndex)
{
   TSWEpoche* p = NULL;
   EnterCriticalSection(&m_cs);
   try
      {
      if (nIndex < 0)
         nIndex = (int)m_ptl->Count - 1;
      if (nIndex >= 0 && nIndex < m_ptl->Count)
         p = (TSWEpoche*)m_ptl->Items[nIndex];
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
   return p;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns number of stored eoches
//------------------------------------------------------------------------------
unsigned int TSWEpoches::Count()
{
   return (unsigned int)m_ptl->Count;
}
//------------------------------------------------------------------------------

//******************************************************************************
/// BELOW: sound callbacks called on runtme on audio buffers for different purposes
//******************************************************************************
                                                                               
//------------------------------------------------------------------------------
/// Main SoundProc searching for trigger and stores epoche audio data 
//------------------------------------------------------------------------------
void TSWEpoches::SoundProc(vvf &vvfBuffers, bool bTriggerTest)
{
   try
      {
      unsigned int nTriggerChannel = (unsigned int)formSpikeWare->m_smp.m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN);
      #ifdef CHKCHNLS
      static bool bShown = false;
      if ((int)nTriggerChannel != formSpikeWare->m_smp.m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
         {
         if (!bShown)
            ShowMessage("error 1 " + UnicodeString(__FUNC__));
         bShown = true;
         }
      #endif

      if (bTriggerTest)
         {
         SoundProcTriggerTest(vvfBuffers);
         return;
         }

      EnterCriticalSection(&m_csReset);
      unsigned int nNumCopySamplesInBuf = (unsigned int)vvfBuffers[0].size();
      m_nSamplesPlayed += nNumCopySamplesInBuf;

      try
         {
         unsigned int   nEpocheLen = (unsigned int)m_vvfEpoche[0].size();
         unsigned int   nSourceStartSample   = 0;
         unsigned int   nNumCopySamples      = 0;


         // here we first have to check for the 'special double-trigger':
         // if m_nFirstTriggerError is still < 0, then the second peak is
         // expected in THIS buffer (see below)!
         // NOTE: we do NOT search the special trigger in free search (generator does
         // not create the special trigger)
         if (!formSpikeWare->m_bFreeSearchRunning)
            {
            if (m_nTriggersDetected == 1 && m_nFirstTriggerError < 0)
               {
               int nPos = (int)(m_nLastTriggerPos + m_nDoubleTriggerDistance - (m_nSamplesPlayed - nNumCopySamplesInBuf));
               if (nPos < 0 || nPos > (int)nNumCopySamplesInBuf-1)
                  {
                  // OutputDebugString("UNEXPECTED");
                  m_nFirstTriggerError = 1;
                  }
               else
                  {
                  // NOTE: here we check for TRIGGER_THRESHOLD/2.0 because the second pulse only has half the amplitude of first!
                  if (vvfBuffers[nTriggerChannel][(unsigned int)nPos] >= TRIGGER_THRESHOLD/2.0f)
                     {
                     // set flag, that first trigger was fine!
                     m_nFirstTriggerError = 0;
                     // OutputDebugStringW(L"trig OK B");
                     }
                  else
                     {
                     // set error flag
                     m_nFirstTriggerError = 1;
                     //OutputDebugStringW(L"trig NOT OK B");
                     }
                  }
               }
            }
         else
            m_nFirstTriggerError = 0;

         // check for 'no triggers at all'
         if (!formSpikeWare->m_bFreeSearchRunning)
            {
            if (formSpikeWare->m_nStimPlayIndex > m_nStimIndexAtStart && !m_nTriggersDetected)
               {
               m_nFirstTriggerError = 2;
               // OutputDebugString("ERR NO TR 1");
               }
            }
         else if (!m_nTriggersDetected)
            {
            if (  formSpikeWare->m_smp.m_nFreeSearchSamplesPlayed
                  > 3*floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate) + formSpikeWare->m_smp.m_nTriggerOffset
               )
               m_nFirstTriggerError = 2;
            }


         // NOTE: the measurement constraints guarantee, that not more than one trigger
         // can be found within one buffer (epoche size < ASIO buffer size is forbidden)
         // But what might happen is, that in the beginning of the buffer we have data related
         // to the last epoche, the epoche ends, AND we find the next trigger + beginning of
         // next epoche in the same buffer.
         // we manage this here by doing the following:
         //    do the loop 'look for trigger' - 'copy data' twice. Break conditions are commented below
         unsigned int n;
         for (n = 0; n < 2; n++)
            {
            // do we have to look for a trigger (first OR second loop)?
            if (m_nRecEpochePos < 0)
               {
               // find maximum
               float* pf = std::max_element(&vvfBuffers[nTriggerChannel][0], &vvfBuffers[nTriggerChannel][vvfBuffers[nTriggerChannel].size()]);
               // if below trigger threshold: nothing to do: break condiditon for BOTH loops
               if (*pf < TRIGGER_THRESHOLD)
                  return;

               m_nTriggersDetected++;

               // get index of sample were trigger was found
               nSourceStartSample = (unsigned int)(std::distance(&vvfBuffers[nTriggerChannel][0], pf));
               m_nLastTriggerDistance = (int)(m_nSamplesPlayed - nNumCopySamplesInBuf + nSourceStartSample - m_nLastTriggerPos);
               if (m_nTriggersDetected > 1 && abs(m_nLastTriggerDistance - m_nRepetitionPeriod) > 5)
                  {
                  m_usTriggerError = "expected/measured distance: " + IntToStr(m_nRepetitionPeriod) + "/" + IntToStr(m_nLastTriggerDistance);
                  m_bTriggerError = true;
                  }
               m_nLastTriggerPos = m_nSamplesPlayed - nNumCopySamplesInBuf + nSourceStartSample;
               nNumCopySamplesInBuf -= nSourceStartSample;
               m_nRecEpochePos = 0;

               // on the very first trigger we have to look for the special 'double-trigger'. This second
               // pulse may be found within this buffer (if enough room behind detected trigger) or
               // in the next buffer. Here we only look for ONE sample to exceed trigger threshold within
               // expected distance!
               // NOTE: we do NOT search the special trigger in free search (generator does
               // not create the special trigger)
               if (!formSpikeWare->m_bFreeSearchRunning)
                  {
                  if (m_nTriggersDetected == 1)
                     {
                     if ((int)nNumCopySamplesInBuf >= m_nDoubleTriggerDistance)
                        {
                        // NOTE: here we check for TRIGGER_THRESHOLD/2.0 because the second pulse only has half the amplitude of first!
                        if (vvfBuffers[nTriggerChannel][nSourceStartSample+(unsigned int)m_nDoubleTriggerDistance] >= TRIGGER_THRESHOLD/2.0f)
                           {
                           // set flag, that first trigger was fine!
                           m_nFirstTriggerError = 0;
                           // OutputDebugStringW(("trig OK A"  + IntToStr(nDistance)).w_str());
                           }
                        else
                           {
                           // set error flag
                           m_nFirstTriggerError = 1;
                           // OutputDebugStringW(("trig NOT OK A"  + IntToStr(nDistance)).w_str());
                           }
                        }
                     }
                  }
               else
                  m_nFirstTriggerError = 0;

               }

            if (m_nRecEpochePos >= 0)
               {
               // how many still to record?
               nNumCopySamples = nEpocheLen - (unsigned int)m_nRecEpochePos;
               if (nNumCopySamples > nNumCopySamplesInBuf)
                  nNumCopySamples = nNumCopySamplesInBuf;


               unsigned int nChannel;
               unsigned int nEpocheChannel = 0;
               #ifdef CHKCHNLS
               static bool bShown2 = false;
               UnicodeString us1, us2;
               #endif
               for (nChannel = 0; nChannel < vvfBuffers.size(); nChannel++)
                  {
                  if (!formSpikeWare->m_smp.m_swcUsedChannels.IsElectrode(nChannel))
                     continue;

                  #ifdef CHKCHNLS
                  us2 += IntToStr((int)nChannel) + ", ";
                  if (nChannel != nTriggerChannel)
                     us1 += IntToStr((int)nChannel) + ", ";
                  #endif

                  CopyMemory(&m_vvfEpoche[nEpocheChannel++][(unsigned int)m_nRecEpochePos], &vvfBuffers[nChannel][nSourceStartSample], nNumCopySamples*sizeof(float));
                  }

               if (formSpikeWare->IsInSitu() && formSpikeWare->m_smp.m_bSaveProbeMics)
                  {

                  // do ProbeMics in separate loop. Slightly inefficient, but easier to maintain (and number
                  // of channels is always small!)
                  unsigned int nProbeMicChannel = 0;
                  for (nChannel = 0; nChannel < vvfBuffers.size(); nChannel++)
                     {
                     if (!formSpikeWare->m_smp.m_swcUsedChannels.IsProbeMic(nChannel))
                        continue;
                     CopyMemory(&m_vvfEpocheProbeMic[nProbeMicChannel++][(unsigned int)m_nRecEpochePos], &vvfBuffers[nChannel][nSourceStartSample], nNumCopySamples*sizeof(float));
                     }
                  }

               #ifdef CHKCHNLS
               if (us1 != us2)
                  {
                  if (!bShown2)
                     ShowMessage("error 2 " + UnicodeString(__FUNC__));
                  bShown2 = true;
                  }
               #endif

               m_nRecEpochePos += nNumCopySamples;

               // done storing epoche?
               if (m_nRecEpochePos >= (int)nEpocheLen)
                  {
                  //
                  // add epoche. NOTE: in search modes m_viRepetitionSequence is empty and we always
                  // write '0' as RepetitionIndex
                  unsigned int nRepetitionIndex = 0;
                  if (formSpikeWare->m_viRepetitionSequence.size() > m_nEpochesTotal)
                     nRepetitionIndex = (unsigned int)formSpikeWare->m_viRepetitionSequence[m_nEpochesTotal];
                  Push(m_vvfEpoche, formSpikeWare->GetThresholds(), formSpikeWare->GetCurrentStimulus(m_nEpochesTotal), nRepetitionIndex);

                  unsigned int m;
                  for (m = 0; m < m_vvfEpoche.size(); m++)
                     m_vvfEpoche[m] = 0.0f;

                  if (!!m_pfsWriteProbeMic && formSpikeWare->IsInSitu() && formSpikeWare->m_smp.m_bSaveProbeMics)
                     {
                     // NOTE: here we do the sorting in a way, that the order of epoche data and probemic data
                     // is identical, i.e. first channel in probemic contains the data recorded by the probmic
                     // connected to first output channel! This order is stored in m_viProbeMicOutChannels!!
                     for (m = 0; m < m_vvfEpocheProbeMic.size(); m++)
                        {
                        m_pfsWriteProbeMic->WriteBuffer( &m_vvfEpocheProbeMic[(unsigned int)formSpikeWare->m_smp.m_viProbeMicOutChannels[m]][0], 
                                                         (NativeInt)(m_vvfEpocheProbeMic[m].size()*sizeof(float)));
                        m_vvfEpocheProbeMic[m] = 0.0f;
                        }
                     }
                  // reset m_nRecEpochePos:
                  m_nRecEpochePos = -1;
                  }
               // this break condition is needed, if we have copied data, but epoche is NOT complete within this ASIO
               // buffer: then we DON'T want the loop to be executed again with copying data again!!
               if (m_nRecEpochePos >= 0)
                  return;
               }
            }
         }
      __finally
         {
         LeaveCriticalSection(&m_csReset);
         }
      }
   catch(Exception &e)
      {
      OutputDebugStringW((UnicodeString(__FUNC__) + ": " + e.Message).w_str());
      formSpikeWare->m_smp.Stop();
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sound recording proc for trigger test simply detecting and counting triggers
//------------------------------------------------------------------------------
void TSWEpoches::SoundProcTriggerTest(vvf &vvfBuffers)
{
   EnterCriticalSection(&m_csReset);
   unsigned int nTriggerChannel = (unsigned int)formSpikeWare->m_smp.m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN);
   #ifdef CHKCHNLS
   static bool bShown = false;
   if ((int)nTriggerChannel != formSpikeWare->m_smp.m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
      {
      if (!bShown)
         OutputDebugString("NO");
      bShown = true;
      }
   #endif
   try
      {
      // here we search for total number of triggers regardless of epoche length AND
      // we determine the amplidtude rel fullscale
      unsigned int   nNumSamples = (unsigned int)vvfBuffers[0].size();
      unsigned int n;
      float fValue;
      unsigned int nTriggerLength = (unsigned int)formSpikeWare->m_smp.m_nTriggerLength / (unsigned int)formSpikeWare->m_swsSpikes.m_dSampleRateDevider;
      for (n = m_nNumTriggerSamplesInNextBuffer; n < nNumSamples; n++)
         {
         fValue = fabs(vvfBuffers[nTriggerChannel][n]);
         if (fValue >= TRIGGER_THRESHOLD)
            {
            m_dTriggerTestLastTriggerValue = (double)fValue;
            m_nTriggersDetected++;
            // now get ampitude of this sample
            n += nTriggerLength;
            if (n > nNumSamples)
               m_nNumTriggerSamplesInNextBuffer = n - nNumSamples;
            else
               m_nNumTriggerSamplesInNextBuffer = 0;
            }
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_csReset);
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sound playback proc for trigger test simply detecting and counting plaed triggers
//------------------------------------------------------------------------------
void TSWEpoches::SoundProcTriggerTestPlay(vvf &vvfBuffers)
{
   EnterCriticalSection(&m_csReset);
   unsigned int nTriggerChannel = (unsigned int)formSpikeWare->m_smp.m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN);
   #ifdef CHKCHNLS
   static bool bShown = false;
   if ((int)nTriggerChannel != formSpikeWare->m_smp.m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
      {
      if (!bShown)
         OutputDebugString("NO");
      bShown = true;
      }
   #endif
   try
      {
      // here we search for total number of triggers regardless of epoche length
      unsigned int   nNumSamples = (unsigned int)vvfBuffers[0].size();
      unsigned int n;
      unsigned int nTriggerLength = (unsigned int)formSpikeWare->m_smp.m_nTriggerLength / (unsigned int)formSpikeWare->m_swsSpikes.m_dSampleRateDevider;
      for (n = m_nNumTriggerSamplesInNextBufferPlay; n < nNumSamples; n++)
         {
         if (fabs(vvfBuffers[nTriggerChannel][n]) >= TRIGGER_THRESHOLD)
            {
            m_nTriggerTestTriggersPlayed++;
            n += nTriggerLength;
            if (n > nNumSamples)
               m_nNumTriggerSamplesInNextBufferPlay = n - nNumSamples;
            else
               m_nNumTriggerSamplesInNextBufferPlay = 0;
            }
         }
      m_nSamplesPlayed += nNumSamples;
      }
   __finally
      {
      LeaveCriticalSection(&m_csReset);
      }

}
//------------------------------------------------------------------------------

