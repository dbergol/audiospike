//------------------------------------------------------------------------------
/// \file SWSMPChannels.cpp
///
/// \author Berg
/// \brief Implementation of a class SWSMPHWChannels, a helper class for SWSMP
/// to store information about channels
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

#include "SWSMPChannels.h"
#include <math.h>
#include <string>
#include <algorithm>
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma warn -aus
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Calls Clear to initialize members
//------------------------------------------------------------------------------
SWSMPHWChannels::SWSMPHWChannels()
{
   Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets defaults for members
//------------------------------------------------------------------------------
void SWSMPHWChannels::Clear()
{
   m_nReferenceMicrophoneIndex = -1;
   m_vvswcChannels.clear();
   m_vvswcChannels.resize(2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sorts channel vector by their hardware indices
//------------------------------------------------------------------------------
void SWSMPHWChannels::SortByHWIndex(TChannelDir cd)
{
   int nRefMicHWIndex = -1;
   if (m_nReferenceMicrophoneIndex >= 0)
      nRefMicHWIndex = (int)m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)m_nReferenceMicrophoneIndex].m_nHWIndex;

   // create vector with hardware indices
   std::vector<int > vi;
   unsigned int n, m;
   for (n = 0; n < m_vvswcChannels[cd].size(); n++)
      vi.push_back((int)m_vvswcChannels[cd][n].m_nHWIndex);

   // sort them ascending
   std::sort(vi.begin(), vi.end());

   // create a copy of the original channels
   std::vector<SWSMPHWChannel >  vswcChannels = m_vvswcChannels[cd];
   // clear original channels
   m_vvswcChannels[cd].clear();
   for (n = 0; n < vi.size(); n++)
      {
      // search channel with corresponding HWIndex
      for (m = 0; m < vi.size(); m++)
         {
         if ((int)vswcChannels[m].m_nHWIndex == vi[n])
            AddChannel(vswcChannels[m], cd);
         }
   // adjust reference mic index if necessary
   if (cd == SWSMPHWCDIR_IN && (int)m_vvswcChannels[cd].back().m_nHWIndex == nRefMicHWIndex)
      SetRefMic((int)n);
   }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds a channel by name
//------------------------------------------------------------------------------
void SWSMPHWChannels::Add(UnicodeString usName, TChannelDir cd)
{
   SWSMPHWChannel swc;
   swc.m_usName   = usName;
   swc.m_tct      = AS_SMP_NONE;
   swc.m_nHWIndex = (unsigned int)m_vvswcChannels[cd].size();
   swc.m_bRawOutput = false;

   m_vvswcChannels[cd].push_back(swc);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds one channel directly
//------------------------------------------------------------------------------
void SWSMPHWChannels::AddChannel(SWSMPHWChannel &rswc, TChannelDir cd)
{
   m_vvswcChannels[cd].push_back(rswc);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns type of channel
//------------------------------------------------------------------------------
TChannelType SWSMPHWChannels::GetChannelType(unsigned int nIndex, TChannelDir cd)
{
   if (nIndex >= m_vvswcChannels[cd].size())
      {
      UnicodeString us;
      us.sprintf(L"invalid index passed to %hs (%u:%u)", __FUNC__, nIndex, m_vvswcChannels[cd].size());
      throw Exception(us);
      }
   return m_vvswcChannels[cd][nIndex].m_tct;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns if a channel is an output channel
//------------------------------------------------------------------------------
bool SWSMPHWChannels::IsOutput(unsigned int nIndex)
{
   return IsChannelType(nIndex, AS_SMP_OUTPUT, SWSMPHWCDIR_OUT);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns if a channel is the trigger channel
//------------------------------------------------------------------------------
bool SWSMPHWChannels::IsTrigger(unsigned int nIndex, TChannelDir cd)
{
   return IsChannelType(nIndex, AS_SMP_TRIGGER, cd);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns if a channel is an electrode recording channel 
//------------------------------------------------------------------------------
bool SWSMPHWChannels::IsElectrode(unsigned int nIndex)
{
   return IsChannelType(nIndex, AS_SMP_ELECTRODE, SWSMPHWCDIR_IN);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns if a channel is a probe microphone channel 
//------------------------------------------------------------------------------
bool SWSMPHWChannels::IsProbeMic(unsigned int nIndex)
{
   return IsChannelType(nIndex, AS_SMP_PROBEMIC, SWSMPHWCDIR_IN);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns if a channel is the monitor channel 
//------------------------------------------------------------------------------
bool SWSMPHWChannels::IsMonitor(unsigned int nIndex)
{
   return IsChannelType(nIndex, AS_SMP_MONITOR, SWSMPHWCDIR_OUT);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Sets 'raw output' flag for a channel: this chanel will play audio data 'raw'
/// i.e. without calibration, equalisation, level information
//------------------------------------------------------------------------------
void SWSMPHWChannels::SetOutputRaw(unsigned int nIndex, bool b)
{
   if (nIndex < m_vvswcChannels[SWSMPHWCDIR_OUT].size())
      m_vvswcChannels[SWSMPHWCDIR_OUT][nIndex].m_bRawOutput = b;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns if an output channel is in 'raw' mode
//------------------------------------------------------------------------------
bool SWSMPHWChannels::IsOutputRaw(unsigned int nIndex)
{
   if (nIndex < m_vvswcChannels[SWSMPHWCDIR_OUT].size())
      return m_vvswcChannels[SWSMPHWCDIR_OUT][nIndex].m_bRawOutput;
   return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns if a channel type of a channel is identical to passed type
//------------------------------------------------------------------------------
bool SWSMPHWChannels::IsChannelType(unsigned int nIndex, TChannelType tct, TChannelDir cd)
{
   return GetChannelType(nIndex, cd) == tct;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns index of first channel is of passed type
//------------------------------------------------------------------------------
int  SWSMPHWChannels::GetChannelTypeIndex(TChannelType tct, TChannelDir cd)
{
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[cd].size(); n++)
      {
      if (m_vvswcChannels[cd][n].m_tct == tct)
         return (int)n;
      }
   return -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns vector with all output indices
//------------------------------------------------------------------------------
std::vector<int > SWSMPHWChannels::GetOutputs()
{
   std::vector<int > vi;
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[SWSMPHWCDIR_OUT].size(); n++)
      {
      if (IsOutput(n))
         vi.push_back((int)n);
      }
   return vi;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns vector with all output HARDWARE (!) indices
//------------------------------------------------------------------------------
std::vector<int > SWSMPHWChannels::GetOutputIndices()
{
   std::vector<int > vi;
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[SWSMPHWCDIR_OUT].size(); n++)
      {
      if (IsOutput(n))
         vi.push_back((int)m_vvswcChannels[SWSMPHWCDIR_OUT][n].m_nHWIndex);
      }
   return vi;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns vector with all electrode indices
//------------------------------------------------------------------------------
std::vector<int > SWSMPHWChannels::GetElectrodes()
{
   std::vector<int > vi;
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[SWSMPHWCDIR_IN].size(); n++)
      {
      if (IsElectrode(n))
         vi.push_back((int)n);
      }
   return vi;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns vector with all electrode HARDWARE (!) indices
//------------------------------------------------------------------------------
std::vector<int > SWSMPHWChannels::GetElectrodeIndices()
{
   std::vector<int > vi;
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[SWSMPHWCDIR_IN].size(); n++)
      {
      if (IsElectrode(n))
         vi.push_back((int)m_vvswcChannels[SWSMPHWCDIR_IN][n].m_nHWIndex);
      }
   return vi;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns vector with all probe microphone indices
//------------------------------------------------------------------------------
std::vector<int > SWSMPHWChannels::GetProbeMics()
{
   std::vector<int > vi;
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[SWSMPHWCDIR_IN].size(); n++)
      {
      if (IsProbeMic(n))
         vi.push_back((int)n);
      }
   return vi;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// returns vector with all probe microphone HARDWARE (!) indices
//------------------------------------------------------------------------------
std::vector<int > SWSMPHWChannels::GetProbeMicIndices()
{
   std::vector<int > vi;
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[SWSMPHWCDIR_IN].size(); n++)
      {
      if (IsProbeMic(n))
         vi.push_back((int)m_vvswcChannels[SWSMPHWCDIR_IN][n].m_nHWIndex);
      }
   return vi;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets type of all passed channels to AS_SMP_OUTPUT
//------------------------------------------------------------------------------
bool SWSMPHWChannels::SetOutputs(std::vector<int > &rvi)
{
   unsigned int n;
   for (n = 0; n < rvi.size(); n++)
      SetChannelType(rvi[n], AS_SMP_OUTPUT, SWSMPHWCDIR_OUT);
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets mode of all passed output channels to 'raw'
//------------------------------------------------------------------------------
bool SWSMPHWChannels::SetOutputsRaw(std::vector<int > &rvi)
{
   // switch OFF raw for all channels before
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[SWSMPHWCDIR_OUT].size(); n++)
      {
      if (IsOutput(n))
         SetOutputRaw(n, false);
      }
   // then set passed channels
   for (n = 0; n < rvi.size(); n++)
      SetOutputRaw((unsigned int)rvi[n], true);
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets type of all passed channels to AS_SMP_ELECTRODE
//------------------------------------------------------------------------------
bool SWSMPHWChannels::SetElectrodes(std::vector<int > &rvi)
{
   unsigned int n;
   for (n = 0; n < rvi.size(); n++)
      SetChannelType(rvi[n], AS_SMP_ELECTRODE, SWSMPHWCDIR_IN);
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets type of all passed channels to AS_SMP_PROBEMIC
//------------------------------------------------------------------------------
bool SWSMPHWChannels::SetProbeMics(std::vector<int > &rvi)
{
   unsigned int n;
   for (n = 0; n < rvi.size(); n++)
      SetChannelType(rvi[n], AS_SMP_PROBEMIC, SWSMPHWCDIR_IN);
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns index of monitor channel
//------------------------------------------------------------------------------
int  SWSMPHWChannels::GetMonitor()
{
   return GetChannelTypeIndex(AS_SMP_MONITOR, SWSMPHWCDIR_OUT);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns index of trigger channel
//------------------------------------------------------------------------------
int  SWSMPHWChannels::GetTrigger(TChannelDir cd)
{
   return GetChannelTypeIndex(AS_SMP_TRIGGER, cd);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns index of reference micorphone channel
//------------------------------------------------------------------------------
int  SWSMPHWChannels::GetRefMic()
{
   return m_nReferenceMicrophoneIndex;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets type of a channel to passed type
//------------------------------------------------------------------------------
bool  SWSMPHWChannels::SetChannelType(int nIndex, TChannelType tct, TChannelDir cd)
{
   if (nIndex >= (int)m_vvswcChannels[cd].size())
      throw Exception("invalid index passed to " + UnicodeString(__FUNC__));
   if (tct == AS_SMP_TRIGGER || tct == AS_SMP_MONITOR || nIndex < 0)
      {
      int nOld = GetChannelTypeIndex(tct, cd);
      if (nOld > -1)
         m_vvswcChannels[cd][(unsigned int)nOld].m_tct = AS_SMP_NONE;
      }
   if (nIndex >= 0)
      m_vvswcChannels[cd][(unsigned int)nIndex].m_tct = tct;

   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets passed channel to AS_SMP_MONITOR
//------------------------------------------------------------------------------
bool SWSMPHWChannels::SetMonitor(int nIndex)
{
   return SetChannelType(nIndex, AS_SMP_MONITOR, SWSMPHWCDIR_OUT);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets passed channel to AS_SMP_TRIGGER
//------------------------------------------------------------------------------
bool SWSMPHWChannels::SetTrigger(int nIndex, TChannelDir cd)
{
   return SetChannelType(nIndex, AS_SMP_TRIGGER, cd);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// setsm_nReferenceMicrophoneIndex to passed channel 
//------------------------------------------------------------------------------
bool SWSMPHWChannels::SetRefMic(int nIndex)
{
   if (nIndex < (int)m_vvswcChannels[SWSMPHWCDIR_IN].size())
      {
      if (nIndex >= 0)
         m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)nIndex].m_tct = AS_SMP_NONE;
      m_nReferenceMicrophoneIndex = nIndex;
      return true;
      }

   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns number of channels
//------------------------------------------------------------------------------
unsigned int  SWSMPHWChannels::GetNumChannels(TChannelDir cd)
{
   return  (unsigned int)m_vvswcChannels[cd].size();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns a channel name
//------------------------------------------------------------------------------
UnicodeString SWSMPHWChannels::GetChannelName(unsigned int nIndex, TChannelDir cd)
{
   if (nIndex >= m_vvswcChannels[cd].size())
      throw Exception("invalid index passed to " + UnicodeString(__FUNC__));
   return m_vvswcChannels[cd][nIndex].m_usName;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// returns a channel index
//------------------------------------------------------------------------------
int SWSMPHWChannels::GetChannelIndex(UnicodeString usName, TChannelDir cd)
{
   unsigned int n;
   for (n = 0; n < m_vvswcChannels[cd].size(); n++)
      {
      if (m_vvswcChannels[cd][n].m_usName == usName)
         return (int)n;
      }
    return -1;
}
//------------------------------------------------------------------------------


