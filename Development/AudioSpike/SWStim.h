//------------------------------------------------------------------------------
/// \file SWStim.h
///
/// \author Berg
/// \brief Implementation of classes TSWStimulus, TSWStimuli and TSWAudioData to
/// store audio stimulus data
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
#ifndef SWStimH
#define SWStimH
//------------------------------------------------------------------------------
#include <vcl.h>
#include <vector>
#include <valarray>
#include <msxmldom.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include "SWStimParameters.h"
#include "SWTools.h"

/// forward declaration
class TSWStimulus;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for storing audio data and characteristics of an audiofile per channel
//------------------------------------------------------------------------------
class TSWAudioData
{
   public:
      UnicodeString           m_usFileName;
      vvd                     m_vvdData;
      std::valarray<double >  m_vadFilePeak;
      std::valarray<double >  m_vadFileRMS;
      std::valarray<double >  m_vadRMS;
      std::valarray<double >  m_vadProcessedPeak;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class for handling multiple instances of TSWStimulus and TSWAudioData
//------------------------------------------------------------------------------
class TSWStimuli
{
   public:
      TSWStimuli();
      double                     m_dDeviceSampleRate;
      unsigned int               m_nNumChannels;
      unsigned int               m_nChannelLevels;
      unsigned int               m_nNumRepetitions;
      unsigned int               m_nRandom;
      SWStimParameters           m_swspStimPars;
      std::vector<TSWStimulus >  m_swstStimuli;
      std::vector<TSWAudioData > m_vSWAudioData;
      bool                       m_bRMSMissing;

      void           Clear();
      UnicodeString  GetStimuliNodeName();
      void           Add(  _di_IXMLNode xmlDoc,
                           double dAvailableLength,
                           unsigned int nNumChannels,
                           int nMode);
      void           CreateLevelStimuli(_di_IXMLNode xmlDoc, _di_IXMLNode xmlAppend = NULL);
      void           AddLevelStimuli(_di_IXMLNode xmlDoc, vved& vvedLevels);
      static void    AudioFileProperties( UnicodeString  usFileName,
                                          unsigned int   &nNumChannels,
                                          unsigned int   &nNumSamples,
                                          double         &dSampleRate
                                          );
      TSWAudioData*  GetAudioData(UnicodeString usFileName);
   private:
      void LoadAudioData(UnicodeString usFileName, vved& rvvedRMS);
      void AddParams(_di_IXMLNode xmlParams);
      void AddStimuli(_di_IXMLNode xmlStimuli, double dAvailableLength, int nMode);
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// class storing properties of a stimulus
//------------------------------------------------------------------------------
class TSWStimulus
{
   friend class TSWStimuli;
   public:
      TSWStimulus(UnicodeString usName,
                  UnicodeString usFileName,
                  unsigned int  nLength,
                  std::vector<double >& rvdParams,
                  std::vector<UnicodeString > & rvusParams);
      UnicodeString           m_usName;
      UnicodeString           m_usFileName;
      unsigned int            m_nFileStimIndex;
      unsigned int            m_nRepetition;
      unsigned int            m_nIndex;
      unsigned int            m_nLength;
      std::vector<double >    m_vdParams;
      std::vector<UnicodeString >    m_vusParams;
   private:
};
//------------------------------------------------------------------------------
#endif
