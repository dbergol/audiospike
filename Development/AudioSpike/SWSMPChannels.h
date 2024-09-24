//------------------------------------------------------------------------------
/// \file SWSMPChannels.h
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
#ifndef SWSMPChannelsH
#define SWSMPChannelsH
//------------------------------------------------------------------------------

#include <vcl.h>
#include <limits.h>
#include <vector>
#include <valarray>
#include "SWTools.h"

#define FFTLEN_DEFAULT     2048
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// channel usage types
/// NOTE: reference microphone has NO type by purpose: might be probe mic channel at
/// the same time!
//------------------------------------------------------------------------------
enum TChannelType{
   AS_SMP_NONE = 0,     ///< unused channel
   AS_SMP_OUTPUT,       ///< playback (out only)
   AS_SMP_MONITOR,      ///< monitor (out only)
   AS_SMP_TRIGGER,      ///< trigger channel (in or out)
   AS_SMP_ELECTRODE,    ///< electrode (in only)
   AS_SMP_PROBEMIC      ///< probe microphone (in-situ, in only)
};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// Struct for sorting by absoulte hardware indices
//------------------------------------------------------------------------------
struct SWChannelSort {
   unsigned int   m_nAbsolutIndex;
   unsigned int   m_nIndex;
   UnicodeString  m_usName;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// helper struct fors sorting SWChannelSort
//------------------------------------------------------------------------------
struct by_AbsoluteIndex {
    bool operator()(SWChannelSort const &a, SWChannelSort const &b) const  {
        return a.m_nAbsolutIndex < b.m_nAbsolutIndex;
    }
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Struct for storing properties of a hardware audio channel. Here we store
/// the channel type as well!
/// NOTE: reference microphone has NO type by purpose: might be probe mic channel at
/// the same time!
//------------------------------------------------------------------------------
struct SWSMPHWChannel {
      UnicodeString  m_usName;
      unsigned int   m_nHWIndex;
      TChannelType   m_tct;
      bool           m_bRawOutput;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// enum for channel directions 
//------------------------------------------------------------------------------
enum TChannelDir {
   SWSMPHWCDIR_IN = 0,
   SWSMPHWCDIR_OUT
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Class for storing properties of an audio channel
//------------------------------------------------------------------------------
class SWSMPHWChannels {
   public:
      SWSMPHWChannels();
      std::vector<std::vector<SWSMPHWChannel > > m_vvswcChannels;

      void Clear();
      void Add(UnicodeString usName, TChannelDir cd);
      void AddChannel(SWSMPHWChannel &rswc, TChannelDir cd);
      TChannelType GetChannelType(unsigned int nIndex, TChannelDir cd);
      bool IsOutput(unsigned int nIndex);
      bool IsOutputRaw(unsigned int nIndex);
      bool IsTrigger(unsigned int nIndex, TChannelDir cd);
      bool IsElectrode(unsigned int nIndex);
      bool IsProbeMic(unsigned int nIndex);
      bool IsMonitor(unsigned int nIndex);
      void SetOutputRaw(unsigned int nIndex, bool b);
      std::vector<int > GetOutputs();
      std::vector<int > GetOutputIndices();
      std::vector<int > GetElectrodes();
      std::vector<int > GetElectrodeIndices();
      std::vector<int > GetProbeMics();
      std::vector<int > GetProbeMicIndices();
      bool SetOutputs(std::vector<int > &rvi);
      bool SetOutputsRaw(std::vector<int > &rvi);
      bool SetElectrodes(std::vector<int > &rvi);
      bool SetProbeMics(std::vector<int > &rvi);
      int  GetMonitor();
      int  GetTrigger(TChannelDir cd);
      int  GetRefMic();
      bool SetMonitor(int nIndex);
      bool SetTrigger(int nIndex, TChannelDir cd);
      bool SetRefMic(int nIndex);
      bool SetChannelType(int nIndex, TChannelType tct, TChannelDir cd);
      unsigned int  GetNumChannels(TChannelDir cd);
      UnicodeString GetChannelName(unsigned int nIndex, TChannelDir cd);
      int GetChannelIndex(UnicodeString usName, TChannelDir cd);
      void  SortByHWIndex(TChannelDir cd);
   private:
      int   m_nReferenceMicrophoneIndex;
      bool  IsChannelType(unsigned int nIndex, TChannelType tct, TChannelDir cd);
      int   GetChannelTypeIndex(TChannelType tct, TChannelDir cd);
};
//------------------------------------------------------------------------------
#endif
