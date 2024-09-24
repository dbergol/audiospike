//------------------------------------------------------------------------------
/// \file SWFilters.cpp
///
/// \author Berg
/// \brief Implementation of classes TSWFilters for reading/writing filters
/// to/from ini-files
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
//---------------------------------------------------------------------------------
#pragma hdrstop
#include <math.h>
#include "SWFilters.h"
#include "SpikeWareMain.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Creates filter inifile
//------------------------------------------------------------------------------
TSWFilters::TSWFilters(UnicodeString usFileName)
   : m_pFilterIni(NULL)
{
   m_pFilterIni = new TIniFile(usFileName);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor cleans up inifile
//------------------------------------------------------------------------------
TSWFilters::~TSWFilters()
{
   TRYDELETENULL(m_pFilterIni);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// saves a HiPass filter to ini
//------------------------------------------------------------------------------
void TSWFilters::SaveHiPass(UnicodeString usChannel, float f)
{
   // remove existing filter
   RemoveHiPass(usChannel);

   usChannel += " - HIPASS";
   int nFreq      = (int)floor(f);

   // write HiPass frequency

   m_pFilterIni->WriteInteger(usChannel, "Lin", 1);
   m_pFilterIni->WriteInteger(usChannel, "HiPass", nFreq);

   int ndBPerOctave = formSpikeWare->m_pIni->ReadInteger("Settings", "FilterdBperOctaveOutput", 12);
   // create ndB/Octave-filter (for one octave): below hard 0
   m_pFilterIni->WriteInteger(usChannel, IntToStr(nFreq/2-1), 0);
   m_pFilterIni->WriteFloat(usChannel, IntToStr(nFreq/2), dBToFactor(-ndBPerOctave));
   m_pFilterIni->WriteInteger(usChannel, IntToStr(nFreq), 1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// saves a BandPass filter to ini
//------------------------------------------------------------------------------
#pragma argsused
void TSWFilters::SaveBandPass(UnicodeString usChannel, float fLowFreq, float fHiFreq, float fMaxFreq)
{
   // remove existing filter
   RemoveBandPass(usChannel);

   usChannel += " - BANDPASS";

   int nFreqLower = (int)floor(fLowFreq);
   int nFreqUpper = (int)floor(fHiFreq);


   // write edge frequencies
   m_pFilterIni->WriteInteger(usChannel, "Lin", 1);
   m_pFilterIni->WriteInteger(usChannel, "MinFreq", nFreqLower);
   m_pFilterIni->WriteInteger(usChannel, "MaxFreq", nFreqUpper);

   // create ndB/Octave-filter
   int ndBPerOctave = formSpikeWare->m_pIni->ReadInteger("Settings", "FilterdBperOctaveInput", 12);

   // create ndB/Octave-filter (for one octave): below and above hard 0
   m_pFilterIni->WriteInteger(usChannel, IntToStr(nFreqLower/2-1), 0);
   m_pFilterIni->WriteFloat(usChannel, IntToStr(nFreqLower/2), dBToFactor(-ndBPerOctave));
   m_pFilterIni->WriteInteger(usChannel, IntToStr(nFreqLower), 1);
   m_pFilterIni->WriteInteger(usChannel, IntToStr(nFreqUpper), 1);
   m_pFilterIni->WriteFloat(usChannel, IntToStr(2*nFreqUpper), dBToFactor(-ndBPerOctave));
   m_pFilterIni->WriteFloat(usChannel, IntToStr(2*nFreqUpper+1), 0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes a filter from ini
//------------------------------------------------------------------------------
void TSWFilters::RemoveFilter(UnicodeString usChannel)
{
   m_pFilterIni->EraseSection(usChannel);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes a HiPass filter from ini
//------------------------------------------------------------------------------
void TSWFilters::RemoveHiPass(UnicodeString usChannel)
{
   RemoveFilter(usChannel + " - HIPASS");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes a BandPass filter from ini
//------------------------------------------------------------------------------
void TSWFilters::RemoveBandPass(UnicodeString usChannel)
{
   RemoveFilter(usChannel + " - BANDPASS");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns edge frequency of HiPass filter
//------------------------------------------------------------------------------
void TSWFilters::GetHiPass(UnicodeString usChannel, float& rfHiPass)
{
   rfHiPass = -1;
   usChannel += " - HIPASS";
   UnicodeString us = m_pFilterIni->ReadString(usChannel, "HiPass", "");
   double d;
   if (TryStrToDouble(us, d))
      rfHiPass = (float)d;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns edge frequencies of BandPass filter
//------------------------------------------------------------------------------
void TSWFilters::GetBandPass(UnicodeString usChannel, float& rfLoFreq, float& rfHiFreq)
{
   GetFilterEdges(usChannel + " - BANDPASS", rfLoFreq, rfHiFreq);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns edge frequencies of BandPass filter
//------------------------------------------------------------------------------
void TSWFilters::GetFilterEdges(UnicodeString usChannel, float& rfLoFreq, float& rfHiFreq)
{
   rfLoFreq = -1;
   rfHiFreq = -1;
   double d;
   UnicodeString us = m_pFilterIni->ReadString(usChannel, "MinFreq", "");
   if (TryStrToDouble(us, d))
      rfLoFreq = (float)d;
   us = m_pFilterIni->ReadString(usChannel, "MaxFreq", "");
   if (TryStrToDouble(us, d))
      rfHiFreq = (float)d;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns edge frequencies of HiPass filter
//------------------------------------------------------------------------------
void TSWFilters::GetChannelFreqs(UnicodeString usChannel,
                                 float& rfLoFreq,
                                 float& rfHiFreq,
                                 float fMaxFreq,
                                 bool bInSitu )
{
   float f;
   GetHiPass(usChannel, f);

   if (bInSitu)
      usChannel += " - INSITU";

   GetFilterEdges(usChannel, rfLoFreq, rfHiFreq);

   if (f > rfLoFreq)
      rfLoFreq = f;
   if (rfLoFreq < 0)
      rfLoFreq = 50.0f;
   if (rfHiFreq < 0)
      rfHiFreq = fMaxFreq - 50.0f;
}
//------------------------------------------------------------------------------
