//------------------------------------------------------------------------------
/// \file AHtVSTEqAS.cpp
/// \author Berg
/// \brief Implementation of class CHtVSTEqAS.
///
/// Project AudioSpike
/// Module  HtVSTEqAS.dll
///
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
#include <vcl.h>

#include "AHtVSTEqAS.h"

#ifdef VISUAL_PLUGIN
   #include "eqinput.h"
#endif

#pragma package(smart_init)

#include <stdio.h>
#include <windows.h>
#include <System.math.hpp>


#ifdef EXTPROCS
CHtVSTEq*  CHtVSTEq::sm_peq = NULL;
#endif

//--------------------------------------------------------------------------
#ifndef TRYDELETENULL
   #define TRYDELETENULL(p) {if (p!=NULL) { try {delete p;} catch (...){;} p = NULL;}}
#endif



//--------------------------------------------------------------------------
/// constructor: Initializes members
//--------------------------------------------------------------------------
CHtVSTEq::CHtVSTEq (audioMasterCallback audioMaster)
   :  AudioEffectX (audioMaster, 1, 3), // programs, parameters
      m_bIsValid(false),
      m_nUpdateInterval(30),
      m_fEnabled(1.0f),
      m_fVisible(1.0f),
      m_bMuted(false),
      #ifdef VISUAL_PLUGIN
      m_pfrmVisual(NULL),
      #endif
      m_pOLA(NULL)
{
   try
      {
      FormatSettings.DecimalSeparator = '.';

      // factor for painting spectrum reasonable with respect to fullscale
      m_fVisSpecFactor = dBToFactor(15.0);


      InitializeCriticalSection(&csDataSection);

      m_nNumChannels = 1;

      setNumInputs ((VstInt32)m_nNumChannels);
      setNumOutputs ((VstInt32)m_nNumChannels);
      setUniqueID (CCONST('H','t','A','E'));
      canProcessReplacing ();    // supports both accumulating and replacing output

      // set default values
      m_nFFTLen      = 512;

      #ifdef VISUAL_PLUGIN
      m_pfrmVisual = new TfrmEqInput();
      #endif

      m_vafFilterBorders.resize(2);
      m_vafFilterBorders[0] = 100.0f;
      m_vafFilterBorders[1] = 0.75f*sampleRate/2.0f;

      InitFilter();


      #ifdef EXTPROCS
      sm_peq = this;
      #endif

      // set 'valid flag' (used in _main)
      m_bIsValid = true;
      }
   catch (...)
      {
      DeleteCriticalSection(&csDataSection);
      #ifdef VISUAL_PLUGIN
      TRYDELETENULL(m_pfrmVisual);
      #endif
      }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// destructor. does cleanup
//--------------------------------------------------------------------------
CHtVSTEq::~CHtVSTEq ()
{
   #ifdef EXTPROCS
   sm_peq = NULL;
   #endif
   ExitOLA();

   #ifdef VISUAL_PLUGIN
   TRYDELETENULL(m_pfrmVisual);
   #endif

   DeleteCriticalSection(&csDataSection);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// sets program name: reads passed flag and parses it to a infile-filename 
/// and infile-section
//--------------------------------------------------------------------------
void CHtVSTEq::setProgramName (char *name)
{
   // program name may have format
   //   FILTERSECTION
   // or
   // FILENAME##FILTERSECTION

   m_strProgramName = name;
   // set defaults: full programname is section ...
   AnsiString asSection = m_strProgramName;
   // ... and no file passed
   AnsiString asFilterFile = IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "filters.ini";
   // now check if passed name has two '##'-separated fields
   int n = m_strProgramName.Pos("##");
   if (n > 0)
      {
      asFilterFile = m_strProgramName.SubString(1, n-1);
      asSection = m_strProgramName.SubString(n+2, m_strProgramName.Length());
      }

   LoadFilter(asFilterFile, asSection);

   #ifdef VISUAL_PLUGIN
   m_pfrmVisual->Caption = asSection;
   #endif

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// returns actual program name
//--------------------------------------------------------------------------
void CHtVSTEq::getProgramName (char *name)
{
   if (m_strProgramName.IsEmpty())
      strcpy (name, "(empty)");
   strcpy (name, m_strProgramName.c_str());
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// sets a parameter value
//--------------------------------------------------------------------------
#pragma argsused
void CHtVSTEq::setParameter (VstInt32 index, float value)
{
   switch (index)
      {
      case 0:  if (!m_pOLA)
                  {
                  m_nFFTLen = (unsigned int)(2 << ((int)(7 + (int)floor(8.0f*value))));
                  InitFilter();
                  }
               break;
      case 1:  m_fVisible = value;
               #ifdef VISUAL_PLUGIN
               m_pfrmVisual->Visible = (m_fVisible > 0.5f);
               #endif
               break;
      case 2:  m_fEnabled = value; break;
      }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// returns a parameter value
//--------------------------------------------------------------------------
#pragma argsused
float CHtVSTEq::getParameter (VstInt32 index)
{
   switch (index)
      {
      case 0:  return (Log2((float)m_nFFTLen)-8.0f) / 8.0f;
      case 1:  return m_fVisible;
      case 2:  return m_fEnabled;
      }
   return 0.0f;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
/// returns a parameter name
//--------------------------------------------------------------------------
#pragma argsused
void CHtVSTEq::getParameterName (VstInt32 index, char *text)
{
   switch (index)
      {
      case 0:  strcpy (text, "fftlen");  break;
      case 1:  strcpy (text, "visible");  break;
      case 2:  strcpy (text, "enabled");  break;
      }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// returns a parameter display value
//--------------------------------------------------------------------------
#pragma argsused
void CHtVSTEq::getParameterDisplay (VstInt32 index, char *text)
{
   switch (index)
      {
      case 0:  sprintf(text, "%d", m_nFFTLen); break;
      case 1:  sprintf(text, m_fVisible > 0.5f ? "true" : "false"); break;
      case 2:  sprintf(text, m_fEnabled > 0.5f ? "true" : "false"); break;
      }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// returns a parameter  label (e.g. unit)
//--------------------------------------------------------------------------
#pragma argsused
void CHtVSTEq::getParameterLabel (VstInt32 index, char *label)
{
   strcpy (label, " ");       
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// returns effect name
//--------------------------------------------------------------------------
bool CHtVSTEq::getEffectName (char* name)
{
   strcpy (name, "AS-Equalizer");
   return true;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// returns effect product string
//--------------------------------------------------------------------------
bool CHtVSTEq::getProductString (char* text)
{
   strcpy (text, "HtVSTEqAS");
   return true;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// returns effect vendor string
//--------------------------------------------------------------------------
bool CHtVSTEq::getVendorString (char* text)
{
   strcpy (text, "Uni OL");
   return true;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// inits ola filter
//--------------------------------------------------------------------------
VstInt32 CHtVSTEq::startProcess ()
{
   InitOLA();
   return 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// exits ola buffer
//--------------------------------------------------------------------------
VstInt32 CHtVSTEq::stopProcess ()
{
   ExitOLA();
   return 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// calls startProcess
//--------------------------------------------------------------------------
void CHtVSTEq::resume ()
{
   startProcess();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// calls stopProcess
//--------------------------------------------------------------------------
void CHtVSTEq::suspend ()
{
   stopProcess();
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
/// Processing routine called by process and processReplacing
//--------------------------------------------------------------------------
#pragma argsused
void CHtVSTEq::DoProcess  (float **inputs, float **outputs, VstInt32 sampleFrames, bool bReplace)
{
   unsigned int nChannel;
   if ((m_fEnabled <= 0.5f) || !m_pOLA)
      {
      for (nChannel = 0; nChannel < m_nNumChannels; nChannel++)
         MoveMemory(outputs[nChannel], inputs[nChannel], (size_t)sampleFrames*sizeof(float));
      return;
      }
   else if (m_bMuted)
      {
      for (nChannel = 0; nChannel < m_nNumChannels; nChannel++)
         ZeroMemory(&outputs[nChannel][0], (size_t)sampleFrames*sizeof(float));
      return;
      }

   try
      {
      for (nChannel = 0; nChannel < m_nNumChannels; nChannel++)
         {
         m_vvafBuffer[nChannel] = 0.0f;
         // copy data
         CopyMemory(&m_vvafBuffer[nChannel][0], &inputs[nChannel][0], (size_t)sampleFrames*sizeof(float));
         }


      // call filter
      m_pOLA->DoOLA(m_vvafBuffer);

      for (nChannel = 0; nChannel < m_nNumChannels; nChannel++)
         {
         ZeroMemory(&outputs[nChannel][0], (size_t)sampleFrames*sizeof(float));
         // write data back
         CopyMemory(&outputs[nChannel][0], &m_vvafBuffer[nChannel][0], (size_t)sampleFrames*sizeof(float));
         }
      }
   catch (...)
      {
      OutputDebugString("error");
      }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// accumulating processing routine
//--------------------------------------------------------------------------
void CHtVSTEq::process (float **inputs, float **outputs, VstInt32 sampleFrames)
{
   DoProcess(inputs, outputs, sampleFrames, false);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// replacing processing routine
//--------------------------------------------------------------------------
void CHtVSTEq::processReplacing (float **inputs, float **outputs, VstInt32 sampleFrames)
{
   DoProcess(inputs, outputs, sampleFrames, true);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// initialize data
//--------------------------------------------------------------------------
void CHtVSTEq::InitFilter()
{
   m_nUpdateInterval = 15 - (int)Log2((float)m_nFFTLen);
   if (m_nFFTLen > 4096)
      m_nUpdateInterval = 1;

   m_vafFilter.resize(m_nFFTLen/2);
   // start with identity
   m_vafFilter = 1.0f;



   // alloc buffer for filter values and initialize as real filter with ones
   #ifdef VISUAL_PLUGIN
   m_pfrmVisual->Initialize(m_nFFTLen, sampleRate);

   m_pfrmVisual->Visible = m_fVisible > 0.0f;

   #endif
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// alloc memory and OLA
//--------------------------------------------------------------------------
void CHtVSTEq::InitOLA()
{
   ExitOLA();

   m_vvafBuffer.resize(m_nNumChannels);
   unsigned int nChannel;
   for (nChannel = 0; nChannel < m_nNumChannels; nChannel++)
      m_vvafBuffer[nChannel].resize((unsigned int)blockSize);

   // initialize Ola-Filter
   unsigned int nBufSize  = (unsigned int)blockSize;
   if (nBufSize < m_nFFTLen)
      nBufSize = m_nFFTLen;

   m_pOLA = new CHtOLA(m_nNumChannels, m_nFFTLen, nBufSize, SpecProcessCallback);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// free memory
//--------------------------------------------------------------------------
void CHtVSTEq::ExitOLA()
{
   TRYDELETENULL(m_pOLA);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// spectrum processing callback
//--------------------------------------------------------------------------
#pragma argsused
void CHtVSTEq::SpecProcessCallback(vvac & vvacSpectrum)
{
   static int iNumCallbacks = 0;
   EnterCriticalSection(&csDataSection);
   try
      {
      #ifdef VISUAL_PLUGIN
      DWORD dwBins = (DWORD)vvacSpectrum[0].size();
      // store pre-processing spectrum
      if (m_pfrmVisual->Visible && !(iNumCallbacks % m_nUpdateInterval))
         {
         // NOTE: first bin in spec is DC !!
         for (UINT i = 1; i < dwBins; i++)
            m_pfrmVisual->PreSpecSeriesL->YValues->Value[(int)i-1] = (double)(m_fVisSpecFactor*abs(vvacSpectrum[0][i]));
         m_pfrmVisual->PreSpecSeriesL->Repaint();
         }
      #endif

      //calculate new spectrum
      unsigned int n;
      // NOTE: first bin in spec is DC Set it to 0!!
      vvacSpectrum[0][0] = 0;
      for (n = 1; n < vvacSpectrum[0].size(); n++)
         vvacSpectrum[0][n] *= m_vafFilter[n-1];


      #ifdef VISUAL_PLUGIN
      if (m_pfrmVisual->Visible && !(iNumCallbacks % m_nUpdateInterval))
         {
         // NOTE: first bin in spec is DC !!
         for (UINT i = 1; i < dwBins; i++)
            m_pfrmVisual->PostSpecSeriesL->YValues->Value[(int)i-1] = (double)(m_fVisSpecFactor*abs(vvacSpectrum[0][i]));
         m_pfrmVisual->PostSpecSeriesL->Repaint();
         }
      #endif
      }
   __finally
      {
      LeaveCriticalSection(&csDataSection);
      iNumCallbacks++;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// LoadFilter only checks validity of values and converts values to a binary
// values for later interpolation but does no 'resampling' from frequencies to bins
//------------------------------------------------------------------------------
bool __fastcall CHtVSTEq::LoadFilter(AnsiString sFileName, AnsiString sSection)
{
   bool bReturn = true;

   // temporary StringList for filter values from INI Filterfile
   TMemIniFile *pIni        = NULL;
   TStringList *pslFilter   = NULL;

   try
      {
      if(!FileExists(sFileName))
         return true;

      pIni = new TMemIniFile(sFileName);

      if (!pIni->SectionExists(sSection))
         return true;

      std::vector<std::valarray<float> > vvafFileFilter;
      vvafFileFilter.resize(2);

      m_bMuted = true;

      pslFilter = new TStringList();

      // Read the filter values to StringList
      pIni->ReadSectionValues(sSection, pslFilter);

      // a filter may contain some non-numeral values in the beginning, e.g.
      // "Lin" or addtional infos about the filter not used by the plugin itself
      // these are ignored, but kept on saving!!
      // thus we start reading until first
      bool bLog = true;
      AnsiString str, strName, strVal;
      while (pslFilter->Count  > 0)
         {
         str = pslFilter->Strings[0];
         strName = Trim(str.SubString(1, str.Pos("=")-1));
         if (!strName.Length())
            continue;
         if (IsDouble(strName))
            break;
         strVal = Trim(str.SubString(str.Pos("=")+1, str.Length()));

         if (LowerCase(strName) == "lin")
            bLog = strVal != "1";
         else if (LowerCase(strName) == "minfreq")
            {
            if (!IsDouble(strVal))
               throw Exception("Invalid MinFreq detected");
            m_vafFilterBorders[0] = (float)StrToDouble(strVal);
            }
         else if (LowerCase(strName) == "maxfreq")
            {
            if (!IsDouble(strVal))
               throw Exception("Invalid MaxFreq detected");
            m_vafFilterBorders[1] = (float)StrToDouble(strVal);
            }
         pslFilter->Delete(0);
         }


      if (pslFilter->Count < 2)
         throw Exception("At least two values needed in 'Filter' section");

      vvafFileFilter[0].resize((unsigned int)pslFilter->Count);
      vvafFileFilter[1].resize((unsigned int)pslFilter->Count);

      // NOTE: we do NOT use ->Values and ->Names for filter because it is SLOOOOOW!!
      float fLastFreq = -1;
      for (unsigned int i = 0; i < (unsigned int)pslFilter->Count; i++)
         {
         try
            {
            str = pslFilter->Strings[(int)i];
            strName = Trim(str.SubString(1, str.Pos("=")-1));
            if (!strName.Length())
               continue;
            strVal = Trim(str.SubString(str.Pos("=")+1, str.Length()));
            if (!IsDouble(strName))
               throw Exception("non-float frequency detected: " + strName);
            vvafFileFilter[0][i] = (float)StrToDouble(strName);
            if (vvafFileFilter[0][i] < 0)
               throw Exception("Negative frequencies not allowed!");
            if (fLastFreq >= vvafFileFilter[0][i])
               throw Exception("frequencies must be sorted ascending (no duplicates)");
            fLastFreq = vvafFileFilter[0][i];

            if (!IsDouble(strVal))
               throw Exception("non-float gain detected: " + strVal);
            vvafFileFilter[1][i] = (float)StrToDouble(strVal);

            if (bLog)
               vvafFileFilter[1][i] = dBToFactor(vvafFileFilter[1][i]);
            }
         catch (Exception &e)
            {
            AnsiString sError;
            sError.sprintf("Filter File: format 'frequency=gain' expected (error: '%s' in line '%s', linenumber %d of section 'Filter')",
               AnsiString(e.Message).c_str(), str.c_str(), i);
            throw Exception(sError);
            }
         }
      ResampleFilter(vvafFileFilter);
      #ifdef VISUAL_PLUGIN
      FilterToChart();
      #endif
      }
   catch (Exception &e)
      {
      AnsiString str = "Error loading filter '" + sSection + "' from file '" + sFileName + "': " + e.Message;
      MessageBox(0, str.c_str(), "Error", MB_ICONERROR);
      bReturn = false;
      }
   TRYDELETENULL(pIni);
   TRYDELETENULL(pslFilter);
   m_bMuted = false;
   return bReturn;

}
//------------------------------------------------------------------------------

#ifdef VISUAL_PLUGIN
//------------------------------------------------------------------------------
/// shows filter in chart series
//------------------------------------------------------------------------------
void __fastcall CHtVSTEq::FilterToChart()
{
   unsigned int nNumValues = (unsigned int)m_vafFilter.size();

   for (unsigned int i = 0; i < nNumValues; i++)
      m_pfrmVisual->RawFilterSeries->YValues->Value[(int)i] = (double)m_vafFilter[i];


   m_pfrmVisual->RawFilterSeries->Repaint();
}
//------------------------------------------------------------------------------
#endif

//------------------------------------------------------------------------------
/// resamples a filter
//------------------------------------------------------------------------------
void __fastcall CHtVSTEq::ResampleFilter(std::vector<std::valarray<float> > &rvvaf)
{
   std::valarray<float >   vaf;
   try
      {
      float f4BinSize = sampleRate / (float)m_nFFTLen;
      DWORD dwSamples = m_nFFTLen/2;
      vaf.resize(dwSamples);


      // get number of filter values specified in passed vector
      DWORD dwNumberOfFileValues = (DWORD)rvvaf[0].size();
      if (dwNumberOfFileValues <  2)
         throw Exception("Filter contains less than 2 values!");


      for (UINT n = 0; n < dwNumberOfFileValues; n++)
         {
         rvvaf[1][n] = FactorTodB(rvvaf[1][n]);
         }



      float f4Frequency;
      float f4FreqQuotient;


      for (UINT i = 0; i < dwSamples; i++)
         {
         f4Frequency = f4BinSize * (float)i;
         // first value is 0 Hz in bin-filter. We set all bins below first appearing
         // frequency in file filter to that first frequencies value
         if (f4Frequency <= rvvaf[0][0])
            {
            vaf[i] = rvvaf[1][0];
            }
         //same on upper border
         else if (f4Frequency >= rvvaf[0][dwNumberOfFileValues-1])
            {
            vaf[i] = rvvaf[1][dwNumberOfFileValues-1];
            }
         else //not below lower or above upper frequency
            {
            // not 'high performance': we search for upper and lower freq for all bins...
            for (UINT j = 0; j < dwNumberOfFileValues; j++)
               {
               // found it exactly? Write value and break;
               #pragma clang diagnostic push
               #pragma clang diagnostic ignored "-Wfloat-equal"
               if (rvvaf[0][j] == f4Frequency)
                  {
                  vaf[i] = rvvaf[1][j];
                  break;
                  }
               #pragma clang diagnostic pop
               // above desired freq? Interpolate
               else if (rvvaf[0][j] > f4Frequency)
                  {
                  // this should never happen
                  if (j == 0)
                     throw Exception("filter interpolation error 1");

                  // (f-f1)/(f2-f1)
                  f4FreqQuotient = (f4Frequency-rvvaf[0][j-1])/(rvvaf[0][j]-rvvaf[0][j-1]);
                  // y = y1 + (y2-y1) * f4FreqQuotient;
                  vaf[i] = rvvaf[1][j-1] + (rvvaf[1][j]-rvvaf[1][j-1]) * f4FreqQuotient;
                  break;


                  }
               // this should never happen: reaching the 'last filefilter' value
               if (j == dwNumberOfFileValues-1)
                  throw Exception("filter interpolation error 2");
               }
            }
         }

      for (UINT i = 0; i < dwSamples; i++)
         vaf[i] = dBToFactor(vaf[i]);


      // copy this generated filter from the chart series to the memorystream!
      EnterCriticalSection(&csDataSection);
      try
         {
         InitFilter();
         m_vafFilter = vaf;
         }
      __finally
         {
         LeaveCriticalSection(&csDataSection);
         }
      }
   catch (Exception &)
      {
      throw;
      }
}
//---------------------------------------------------------------------------

#ifdef EXTPROCS
//---------------------------------------------------------------------------
/// proc called externally to set/get/load/save filter
//---------------------------------------------------------------------------
void CHtVSTEq::ExtFilterFunction(int nFunction, std::valarray<float >* pvaf, AnsiString as)
{
   if (!sm_peq)
      return;
   sm_peq->FilterFunction(nFunction, pvaf, as);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// proc called by ExtFilterFunction to set/get/load/save filter
//---------------------------------------------------------------------------
void CHtVSTEq::FilterFunction(int nFunction, std::valarray<float >* pvaf, AnsiString as)
{
   EnterCriticalSection(&csDataSection);
   try
      {
      switch (nFunction)
         {
         case 0:  m_vafFilter = *pvaf;
                  #ifdef VISUAL_PLUGIN
                  FilterToChart();
                  #endif
                  break;
         case 1: *pvaf = m_vafFilter; break;
         case 2: m_vafFilterBorders = *pvaf; break;
         case 3: *pvaf = m_vafFilterBorders; break;
         case 4: LoadFilter(as, "Filter"); break;
         }
      }
   __finally
      {
      LeaveCriticalSection(&csDataSection);
      }
}
//---------------------------------------------------------------------------

           
//---------------------------------------------------------------------------
/// vendor function for user to retrieve pointer to ExtFilterFunction (used
/// in calibration plugins only)
//---------------------------------------------------------------------------
#pragma argsused
VstIntPtr CHtVSTEq::vendorSpecific (VstInt32 lArg, VstIntPtr lArg2, void* ptrArg, float floatArg)
{
   long nReturn = 0;
   int n;
   switch (lArg)
      {
      case HT_VST_VENDOR_USERCONFIG:

            if (ptrArg)
               #ifndef _WIN64
               sprintf((char*)ptrArg, "%d", (NativeInt)CHtVSTEq::ExtFilterFunction);
               #else
               sprintf((char*)ptrArg, "%lld", (NativeInt)CHtVSTEq::ExtFilterFunction);
               #endif
            nReturn = 1;
            break;
      }
   return nReturn;
}
//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
//             TOOLS
//---------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// checks if passed string is double
//------------------------------------------------------------------------------
bool IsDouble(AnsiString s)
{
   char *endptr = NULL;
   s = Trim(s);
   strtod(s.c_str(), &endptr);
   if (*endptr != NULL || s.Length()==0)
      return false;
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a double to a string using passed format string
//------------------------------------------------------------------------------
AnsiString DoubleToStr(double val, const char* lpcszFormat)
{
   AnsiString s;
   if (!lpcszFormat)
      lpcszFormat = "%lf";
   s.sprintf(lpcszFormat, val);
   return s;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a string to a double
//------------------------------------------------------------------------------
double StrToDouble(AnsiString s)
{
   char *endptr = NULL;
   double value;
   s = Trim(s);
   value = strtod(s.c_str(), &endptr);
   if (*endptr != NULL || s.Length()==0)
      throw Exception("not a double");
   return value;
}
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a dB value to linear fctor
//------------------------------------------------------------------------------
float dBToFactor(const float f4dB)
{
   return (float)pow(10.0,(double)f4dB/20.0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a linear fctor to dB value 
//------------------------------------------------------------------------------
float FactorTodB(const float f4Factor)
{
   if (f4Factor <= 0)
      return -1000.0;
   return 20*log10(f4Factor);
}
//------------------------------------------------------------------------------



