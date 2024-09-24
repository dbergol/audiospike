//------------------------------------------------------------------------------
/// \file SWStim.cpp
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
#pragma hdrstop

#include "SWStim.h"
#include "SWTools.h"
#include <math.h>
#include <algorithm>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundef"
#include "sndfile.h"
#pragma clang diagnostic pop
#include "SpikeWareMain.h"


//------------------------------------------------------------------------------

#pragma package(smart_init)

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor - empty
//------------------------------------------------------------------------------
TSWStimuli::TSWStimuli()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears internal members
//------------------------------------------------------------------------------
void TSWStimuli::Clear()
{
   m_swspStimPars.Clear();
   m_swstStimuli.clear();
   m_vSWAudioData.clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns the node name for the XML node holding ALL stimuli. Up t now this
/// is "AllStimuli" (fix)
//------------------------------------------------------------------------------
UnicodeString TSWStimuli::GetStimuliNodeName()
{
   return "AllStimuli";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds stimuli + stimulus parameters from passed XML node to class 
//------------------------------------------------------------------------------
void TSWStimuli::Add(_di_IXMLNode xmlDoc,
                     double dAvailableLength,
                     unsigned int nNumChannels,
                     int nMode)
{
   Clear();

   // check Parameters and Stimuli nodes (mandatory)
   _di_IXMLNode xmlParams = xmlDoc->ChildNodes->FindNode("Parameters");
   if (!xmlParams)
      throw Exception("Parameters missing");

   _di_IXMLNode xmlStimuli = xmlDoc->ChildNodes->FindNode("Stimuli");
   if (!xmlStimuli)
      throw Exception("Stimuli missing");

   m_nNumChannels = nNumChannels;
   AddParams(xmlParams);

   if (nMode == SWLM_TEMPLATE)
      CreateLevelStimuli(xmlDoc);
   _di_IXMLNode xmlAllStimuli = xmlDoc->ChildNodes->FindNode("AllStimuli");
   if (!xmlAllStimuli)
      throw Exception("AllStimuli missing");

   AddStimuli(xmlAllStimuli, dAvailableLength, nMode);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds stimulus parameters from passed XML node to member
//------------------------------------------------------------------------------
void TSWStimuli::AddParams(_di_IXMLNode xmlParams)
{
   int nNumPars = xmlParams->ChildNodes->Count;
   if (!nNumPars)
      throw Exception("no Parameter nodes found");

   int nVal, nNode, nNumValues;
   UnicodeString usName, usUnit, usPar, us;
   bool bLog, bString;
   for (nNode = 0; nNode < nNumPars; nNode++)
      {
      usName = "";
      usUnit = "";
      bLog = false;
      bString = false;

      _di_IXMLNode xmlPar = xmlParams->ChildNodes->Nodes[nNode];
      if (xmlPar->GetNodeName() != "Parameter")
         throw Exception("Invalid subnode name in Parameters");

      nNumValues = xmlPar->ChildNodes->Count;
      for (nVal = 0; nVal < nNumValues; nVal++)
         {
         _di_IXMLNode xmlVal = xmlPar->ChildNodes->Nodes[nVal];
         us = xmlVal->GetNodeName();
         if (us == "Name")
            usName = xmlVal->GetText();
         else if (us == "Unit")
            usUnit = xmlVal->GetText();
         else if (us == "Log")
            bLog = (xmlVal->GetText() == "1");
         else if (us == "String")
            bString = (xmlVal->GetText() == "1");
         }
      // name must not be empty
      if (usName.IsEmpty())
         throw Exception("Name of parameter " + IntToStr(nVal + 1) + " missing");
      // duplicates forbidden
      if (m_swspStimPars.IndexFromName(usName) != -1)
         throw Exception("Parameter doublette detected: " + usName);


      // parameter "Level" gets special handling: here we generate all
      // occuring values directly!
      if (usName == "Level")
         {
         UnicodeString usLevels = GetXMLValue(xmlPar, "Level");
         if (usLevels.IsEmpty())
            throw Exception("'Level' values missing in 'Level' parameter");

         vved vvedLevels   = ParseMLVector(usLevels, "Level");
         m_nChannelLevels  = (unsigned int)vvedLevels.size();
         if (!m_nChannelLevels)
            throw Exception("'Level' does not contain valid lavels in 'Level' parameter");
         // number of channels must be either 1 (then identical level is used
         // for ALL channels) or identical to number of channels
         if (m_nChannelLevels != 1 && m_nChannelLevels != m_nNumChannels)
            throw Exception("'Level' must contain either exactly one column or one column for each used output channel");
         UnicodeString usLevelName;
         unsigned int n;
         for (n = 0; n < m_nChannelLevels; n++)
            {
            usLevelName = usName + "_" + IntToStr((int)n+1);
            m_swspStimPars.Add(usLevelName, usUnit, bLog, false);
            m_swspStimPars.m_vvdValues[(unsigned int)m_swspStimPars.IndexFromName(usLevelName)] = vvedLevels[n];
            }
         }
      else
         m_swspStimPars.Add(usName, usUnit, bLog, bString);
      }
   // NOTE: "Level_1" is MANDATORY!!
   if (m_swspStimPars.IndexFromName("Level_1") == -1)
      throw Exception("Parameter 'Level' missing");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates stimuli from one stimulus and level-list description for all levels
//------------------------------------------------------------------------------
void TSWStimuli::CreateLevelStimuli(_di_IXMLNode xmlDoc, _di_IXMLNode xmlAppend)
{
   int nNode, nVals;

   // if brandnew (no stimuli to be appended) generate node AllStumli completely new
   _di_IXMLNode xmlAllStimuli = xmlDoc->ChildNodes->FindNode("AllStimuli");
   if (!xmlAppend)
      {
      if (!!xmlAllStimuli)
         xmlDoc->ChildNodes->Remove(xmlAllStimuli);
      xmlAllStimuli = xmlDoc->AddChild("AllStimuli");
      }
   if (!xmlAllStimuli)
      throw Exception("AllStimuli missing");


   _di_IXMLNode xmlStimuli = xmlDoc->ChildNodes->FindNode("Stimuli");
   if (!xmlStimuli)
      throw Exception("Stimuli missing");

   // number of stimuli
   int nNumStim = xmlStimuli->ChildNodes->Count;
   if (!nNumStim)
      throw Exception("no Stimulus nodes found");

   int nStartSimulusNode = 0;

   // append new stimuli?
   if (!!xmlAppend)
      {
      // patch them to original Stimuli node
      _di_IXMLNode xmlNewStimuli = xmlAppend->ChildNodes->FindNode("Stimuli");
      if (!xmlNewStimuli)
         throw Exception("Stimuli missing");

      // number of stimuli
      int nNumNewStim = xmlNewStimuli->ChildNodes->Count;
      if (!nNumNewStim)
         throw Exception("no new Stimulus nodes found");

      // adjust start index and total number for patching them to 'AllStimuli below
      nStartSimulusNode = nNumStim;
      nNumStim += nNumNewStim;

      for (nNode = 0; nNode < nNumNewStim; nNode++)
         {
         _di_IXMLNode xmlStim = xmlNewStimuli->ChildNodes->Nodes[nNode];
         if (xmlStim->GetNodeName() != "Stimulus")
            throw Exception("Invalid subnode name in new Stimuli " + xmlNewStimuli->GetNodeName());

         // add a new Stimulus
         _di_IXMLNode xmlNewStim = xmlStimuli->AddChild("Stimulus");
         nVals = xmlStim->ChildNodes->Count;
         for (int x = 0; x < nVals; x++)
            {
            _di_IXMLNode xmlParam  = xmlStim->ChildNodes->Nodes[x];
            xmlNewStim->ChildValues[xmlParam->GetNodeName()] = xmlParam->GetText();
            }
         }
      }

   // get all level values
   // NOTE: "Level_1" is mandatory (already tested in AddParams, but to be sure...)
   int nLevelIndex = m_swspStimPars.IndexFromName("Level_1");
   if (nLevelIndex == -1)
      throw Exception("Parameter 'Level' missing");

   // get number of different levels per channel
   unsigned int nNumLevels = (unsigned int)m_swspStimPars.m_vvdValues[(unsigned int)nLevelIndex].size();
   if (!nNumLevels)
         throw Exception("Parameter values for 'Level' empty!");


   unsigned int nChannelLevel, nLevel;
   UnicodeString usLevelName;

   // loop through levels
   for (nLevel = 0; nLevel < nNumLevels; nLevel++)
      {
      for (nNode = nStartSimulusNode; nNode < nNumStim; nNode++)
         {
         _di_IXMLNode xmlStim = xmlStimuli->ChildNodes->Nodes[nNode];
         if (xmlStim->GetNodeName() != "Stimulus")
            throw Exception("Invalid subnode name in Stimuli " + xmlStim->GetNodeName());

         // add a new Stimulus
         _di_IXMLNode xmlNewStim = xmlAllStimuli->AddChild("Stimulus");
         nVals = xmlStim->ChildNodes->Count;

         for (int x = 0; x < nVals; x++)
            {
            _di_IXMLNode xmlParam  = xmlStim->ChildNodes->Nodes[x];
            xmlNewStim->ChildValues[xmlParam->GetNodeName()] = xmlParam->GetText();
            }

         for (nChannelLevel = 0; nChannelLevel < m_nChannelLevels; nChannelLevel++)
            {
            usLevelName = "Level_" + IntToStr((int)nChannelLevel+1);
            nLevelIndex = m_swspStimPars.IndexFromName(usLevelName);
            if (nLevelIndex < 0)
               throw Exception("watt");

            xmlNewStim->ChildValues[usLevelName] = DoubleToStr(m_swspStimPars.m_vvdValues[(unsigned int)nLevelIndex][(unsigned int)nLevel]);
            }
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// append additional level(s)
//------------------------------------------------------------------------------
void TSWStimuli::AddLevelStimuli(_di_IXMLNode xmlDoc, vved& vvedLevels)
{
   _di_IXMLNode xmlStimuli = xmlDoc->ChildNodes->FindNode("Stimuli");
   if (!xmlStimuli)
      throw Exception("Stimuli missing");

   // number of (original) stimuli
   unsigned int nNumStim = (unsigned int)xmlStimuli->ChildNodes->Count;
   if (!nNumStim)
      throw Exception("no Stimulus nodes found");

   _di_IXMLNode xmlAllStimuli = xmlDoc->ChildNodes->FindNode("AllStimuli");
   if (!xmlAllStimuli)
      throw Exception("AllStimuli missing");

   unsigned int nChannelLevels = (unsigned int)vvedLevels.size();
   if (nChannelLevels != m_nChannelLevels && nChannelLevels != 1)
      throw Exception("levels to add must have either one channel or number of channels of result (" + IntToStr((int)m_nChannelLevels) + ")");
   // get number of levels in first channel
   unsigned int nNumLevels = (unsigned int)vvedLevels[0].size();
   if (!nNumLevels)
      throw Exception("No levels to add: nothing to do!");

   UnicodeString usLevelName;
   int nVals;
   unsigned int nChannelLevel, nNumNewLevels, nNode, nLevel;
   // loop through levels
   for (nLevel = 0; nLevel < nNumLevels; nLevel++)
      {
      for (nNode = 0; nNode < nNumStim; nNode++)
         {
         _di_IXMLNode xmlStim = xmlStimuli->ChildNodes->Nodes[nNode];
         if (xmlStim->GetNodeName() != "Stimulus")
            throw Exception("Invalid subnode name in Stimuli " + xmlStim->GetNodeName());

         // add a new Stimulus
         _di_IXMLNode xmlNewStim = xmlAllStimuli->AddChild("Stimulus");
         nVals = xmlStim->ChildNodes->Count;
         for (int x = 0; x < nVals; x++)
            {
            _di_IXMLNode xmlParam  = xmlStim->ChildNodes->Nodes[x];
            xmlNewStim->ChildValues[xmlParam->GetNodeName()] = xmlParam->GetText();
            }

         for (nChannelLevel = 0; nChannelLevel < nChannelLevels; nChannelLevel++)
            {
            usLevelName = "Level_" + IntToStr((int)nChannelLevel+1);
            xmlNewStim->ChildValues[usLevelName] = DoubleToStr(vvedLevels[nChannelLevel][nLevel]);
            }
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
////// adds stimuli from passed XML node to class 
//------------------------------------------------------------------------------
void TSWStimuli::AddStimuli(_di_IXMLNode xmlStimuli, double dAvailableLength, int nMode)
{
   bool bResult = (nMode == SWLM_RESULT);
   m_bRMSMissing = false;
   m_swstStimuli.clear();
   //
   int nNumStim = xmlStimuli->ChildNodes->Count;
   if (!nNumStim)
      throw Exception("no Stimulus nodes found");


   // re-read number of stimuli: CreateLevelStimuli may have created new nodes!
   nNumStim = xmlStimuli->ChildNodes->Count;


   unsigned int nNumPars = m_swspStimPars.Count();
   if (!nNumPars)
      throw Exception("cannot add stimuli: no parameters available");


   // vector for parameters of ONE stimulus
   std::vector<double > vdParams(nNumPars);
   std::vector<UnicodeString > vusParams(nNumPars);

   unsigned int nChannelLevel;
   int nLevelIndex;

   unsigned int nNumChannels;
   unsigned int nNumSamples = 0;
   double       dSampleRate;

   UnicodeString usName, usFileName, usPar, usVal, usRMS;
   double d, dStimLength;
   int nNode, nLevel;
   unsigned int nPar;

   for (nNode = 0; nNode < nNumStim; nNode++)
      {

      _di_IXMLNode xmlStim = xmlStimuli->ChildNodes->Nodes[nNode];
      if (xmlStim->GetNodeName() != "Stimulus")
         throw Exception("Invalid subnode name in Stimuli " + xmlStim->GetNodeName());

      usName = GetXMLValue(xmlStim, "Name");
      // default name is index
      if (usName.IsEmpty())
         usName = IntToStr(nNode+1);

      usFileName = GetXMLValue(xmlStim, "FileName");
      if (usFileName.Length() < 2)
         throw Exception("FileName missing in stimulus '" + usName + "', subnode " + IntToStr(nNode + 1));

      usFileName = ExpandFileName(usFileName);
      if (!bResult)
         {
         if (!FileExists(usFileName))
            throw Exception("Sound file '" + usFileName + "' not found specified in Stimulus '" + usName + "', subnode " + IntToStr(nNode + 1));
         xmlStim->ChildValues["FileName"] = usFileName;
         // check properties of file
         AudioFileProperties(usFileName, nNumChannels, nNumSamples, dSampleRate);
         dStimLength = (double)nNumSamples / m_dDeviceSampleRate;
         #pragma clang diagnostic push
         #pragma clang diagnostic ignored "-Wfloat-equal"
         if (dSampleRate != m_dDeviceSampleRate)
            throw Exception("Sound file '" + usFileName + "' has SampleRate " + DoubleToStr(dSampleRate) + " (expected SampleRate: " + DoubleToStr(m_dDeviceSampleRate) +  ")");
         #pragma clang diagnostic pop
         // allow one channel (used for ALL output channels) or channel number identical to used output channels
         if (nNumChannels != 1 && nNumChannels != m_nNumChannels)
            throw Exception("Sound file '" + usFileName + "' has " + IntToStr((int)nNumChannels) + " channels (expected channels: " + IntToStr((int)m_nNumChannels) +  ")");
         if (dStimLength > dAvailableLength)
            throw Exception("Sound file '" + usFileName + "' too long: " + DoubleToStr(dStimLength) + " seconds (maximum allowed seconds: " + DoubleToStr(dAvailableLength) +  " ( == EpocheLength - PreStimulus))");

         // read optional passed RMS (if empty, i.e. 0.0, then LoadAudioData will calculate RMS)
         vved vvedRMS;
         usRMS = GetXMLValue(xmlStim, "RMS");
         if (!usRMS.IsEmpty())
            {
            vvedRMS   = ParseMLVector(usRMS, "RMS");
            if (vvedRMS.size() != nNumChannels)
               throw Exception("invalid number of RMS found in for stimulus");
            }
         else
            m_bRMSMissing = true;

         // load it if necessary
         if (!GetAudioData(usFileName))
            LoadAudioData(usFileName, vvedRMS);
         }
      // values for ALL parameters MUST exist!
      for (nPar = 0; nPar < nNumPars; nPar++)
         {
         usPar = m_swspStimPars.m_vusNames[nPar];
         usVal = GetXMLValue(xmlStim, usPar);
         if (!m_swspStimPars.m_vbString[nPar])
            {
            if (!TryStrToDouble(usVal, d))
               throw Exception("Parameter " + usPar + " missing or not a float for stimulus " + usName);
            // store it in temporary vector for creating stimuli below
            vdParams[nPar] = d;
            vusParams[nPar] = "";
            // add only unique parameter values
            if ( std::find(m_swspStimPars.m_vvdValues[nPar].begin(), m_swspStimPars.m_vvdValues[nPar].end(), d) == m_swspStimPars.m_vvdValues[nPar].end() )
               {
               m_swspStimPars.m_vvdValues[nPar].push_back(d);
               // parameter as string empty
               m_swspStimPars.m_vvusValues[nPar].push_back("");
               }
            }
         else
            {
            // pass index as double param
            vdParams[nPar] = m_swspStimPars.m_vvdValues[nPar].size();
            vusParams[nPar] = usVal;
            // add only unique parameter values
            if ( std::find(m_swspStimPars.m_vvusValues[nPar].begin(), m_swspStimPars.m_vvusValues[nPar].end(), usVal) == m_swspStimPars.m_vvusValues[nPar].end() )
               {
               m_swspStimPars.m_vvdValues[nPar].push_back(m_swspStimPars.m_vvdValues[nPar].size());
               m_swspStimPars.m_vvusValues[nPar].push_back(usVal);
               }
            }
         }

      m_swstStimuli.push_back(TSWStimulus(usName, usFileName, nNumSamples, vdParams, vusParams));
      m_swstStimuli.back().m_nIndex          = (unsigned int)m_swstStimuli.size()-1;
      m_swstStimuli.back().m_nFileStimIndex  = (unsigned int)nNode;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns pointer do TSWAudioData by filename
//------------------------------------------------------------------------------
TSWAudioData* TSWStimuli::GetAudioData(UnicodeString usFileName)
{
   unsigned int n;
   // load it if necessary
   for (n = 0; n < m_vSWAudioData.size(); n++)
      {
      if (m_vSWAudioData[n].m_usFileName == usFileName)
         return &m_vSWAudioData[n];
      }
   return NULL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads audiodata from file and determines audio file properties (RMS, Peak ...)
//------------------------------------------------------------------------------
void TSWStimuli::LoadAudioData(UnicodeString usFileName, vved& rvvedRMS)
{
   SNDFILE* pSndFile = NULL;
   TMemoryStream* pms = new TMemoryStream();
   try
      {
      SF_INFO sfi;
      ZeroMemory(&sfi, sizeof(sfi));

      pSndFile = sf_open(AnsiString(usFileName).c_str(), SFM_READ, &sfi);

      if (!pSndFile)
         throw Exception("cannot open file " + usFileName);

      // create buffer for reading ALL frames
      pms->Size = sfi.channels * sfi.frames * (int)sizeof(double);
      if (sfi.frames != sf_readf_double(pSndFile, (double*)pms->Memory, sfi.frames))
         throw Exception("error reading file " + usFileName);

      // create audio data
      m_vSWAudioData.push_back(TSWAudioData());
      // get reference to newly created data
      TSWAudioData& rswad = m_vSWAudioData.back();
      rswad.m_usFileName = usFileName;
      // create audio buffer: we need it non-interleaved!!
      // at the same time we calculate the file RMS and file peak
      unsigned int nChannels = (unsigned int)sfi.channels;
      rswad.m_vvdData.resize(nChannels);
      rswad.m_vadRMS.resize(nChannels);
      rswad.m_vadFilePeak.resize(nChannels);
      rswad.m_vadFileRMS.resize(nChannels);
      rswad.m_vadProcessedPeak.resize(nChannels);
      rswad.m_vadRMS             = 0.0;
      rswad.m_vadFilePeak        = 0.0;
      rswad.m_vadFileRMS         = 0.0;
      rswad.m_vadProcessedPeak   = 0.0;
      unsigned int nChannel, nFrame;
      double d;
      for (nChannel = 0; nChannel < nChannels; nChannel++)
         rswad.m_vvdData[nChannel].resize((unsigned int)sfi.frames);
      double* pd = (double*)pms->Memory;
      for (nFrame = 0; nFrame < sfi.frames; nFrame++)
         {
         for (nChannel = 0; nChannel < nChannels; nChannel++)
            {
            d = *pd++;
            // store sample
            rswad.m_vvdData[nChannel][nFrame] = d;

            // store peak
            d = fabs(d);
            if (d > rswad.m_vadFilePeak[nChannel])
               rswad.m_vadFilePeak[nChannel] = d;
            // add up squared values for RMS (see below)
            rswad.m_vadFileRMS[nChannel] += d*d;
            }
         }

      // convert ti dB AND RMS calculation (mean and root)
      for (nChannel = 0; nChannel < nChannels; nChannel++)
         {
         rswad.m_vadFilePeak[nChannel] = FactorTodB(rswad.m_vadFilePeak[nChannel]);
         rswad.m_vadFileRMS[nChannel]  = FactorTodB(sqrt(rswad.m_vadFileRMS[nChannel] / (double) sfi.frames));
         // then copy calculated or passed RMS to m_vadRMS (passed wins)

         if (rvvedRMS.size())
            {
            if (rvvedRMS[nChannel][0] >= 0.0)
               throw Exception("Invalid RMS for a signal detected (value >= 0.0)");
            rswad.m_vadRMS[nChannel] = rvvedRMS[nChannel][0];
            }
         else
            rswad.m_vadRMS[nChannel] = rswad.m_vadFileRMS[nChannel];

         // uncomment for debugging)
         /*
         if (formSpikeWare->m_bLevelDebug)
            {
            UnicodeString us;
            us.printf(L"%ls|%d, %lf, %lf, %lf", usFileName.w_str(), nChannel, rswad.m_vadFilePeak[nChannel], rswad.m_vadFileRMS[nChannel], rswad.m_vadRMS[nChannel] );
            OutputDebugStringW(us.w_str());
            }
         */
         }

      }
   __finally
      {
      TRYDELETENULL(pms);
      if (pSndFile)
         sf_close(pSndFile);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// retrieves properties of a file (static function)
//------------------------------------------------------------------------------
void TSWStimuli::AudioFileProperties(UnicodeString usFileName,
                                     unsigned int &nNumChannels,
                                     unsigned int &nNumSamples,
                                     double       &dSampleRate
                                     )
{
   SNDFILE* pSndFile = NULL;
   try
      {
      SF_INFO sfi;
      ZeroMemory(&sfi, sizeof(sfi));


      pSndFile = sf_open(AnsiString(usFileName).c_str(), SFM_READ, &sfi);

      if (!pSndFile)
         throw Exception("cannot open file " + usFileName);

      nNumChannels = (unsigned int)sfi.channels;
      dSampleRate  = sfi.samplerate;
      nNumSamples  = (unsigned int)sfi.frames;
      }
   __finally
      {
      if (pSndFile)
         sf_close(pSndFile);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor copies passed values to members 
//------------------------------------------------------------------------------
TSWStimulus::TSWStimulus(  UnicodeString usName,
                           UnicodeString usFileName,
                           unsigned int  nLength,
                           std::vector<double >& rvdParams,
                           std::vector<UnicodeString > & rvusParams)
{
   m_usName       = usName;
   m_usFileName   = usFileName;
   m_nLength      = nLength;
   m_vdParams     = rvdParams;
   m_vusParams    = rvusParams;
}
//------------------------------------------------------------------------------



