//------------------------------------------------------------------------------
/// \file AHtVSTEqAS.h
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
#ifndef AHtVSTEqASH
#define AHtVSTEqASH

#ifdef VISUAL_PLUGIN
   #include <VCLTee.Chart.hpp>
   #include <VCLTee.Series.hpp>
#endif

#include <vcl.h>
#include <inifiles.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#include "audioeffectx.h"
#pragma clang diagnostic pop

#include "HtOLA.h"

#define HT_VST_VENDOR_USERCONFIG    1234

//------------------------------------------------------------------------------
/// definitions of non-member tool functions
float       dBToFactor(const float f4dB);
float       FactorTodB(const float f4Factor);
bool        IsDouble(AnsiString s);
AnsiString  DoubleToStr(double val, const char* lpcszFormat = NULL);
double      StrToDouble(AnsiString s);
//------------------------------------------------------------------------------
class TfrmEqInput;
//------------------------------------------------------------------------------
/// VST plugin  class for spectral equalizer
//------------------------------------------------------------------------------
class CHtVSTEq : public AudioEffectX
{
   public:
      CHtVSTEq (audioMasterCallback audioMaster);
      ~CHtVSTEq ();
      #ifdef EXTPROCS
      static CHtVSTEq* sm_peq;
      #endif
      bool m_bIsValid;
      int  m_nUpdateInterval;
      // Processes
      virtual void process (float **inputs, float **outputs, VstInt32 sampleFrames);
      virtual void processReplacing (float **inputs, float **outputs, VstInt32 sampleFrames);
      virtual void DoProcess (float **inputs, float **outputs, VstInt32 sampleFrames, bool bReplace);

      // Program
      virtual void setProgramName (char *name);
      virtual void getProgramName (char *name);

      // Parameters
      virtual void setParameter (VstInt32 index, float value);
      virtual float getParameter (VstInt32 index);
      virtual void getParameterLabel (VstInt32 index, char *label);
      virtual void getParameterDisplay (VstInt32 index, char *text);
      virtual void getParameterName (VstInt32 index, char *text);

      virtual bool getEffectName (char* name);
      virtual bool getVendorString (char* text);
      virtual bool getProductString (char* text);
      virtual VstInt32 getVendorVersion () { return 1000; }
      virtual VstPlugCategory getPlugCategory () { return kPlugCategEffect; }

      virtual VstInt32 startProcess ();
      virtual VstInt32 stopProcess ();
      virtual void resume ();
      virtual void suspend ();
      void ReadFilterNameFromIni();
      virtual bool __fastcall LoadFilter(AnsiString sFileName, AnsiString sSection);
      float GetSampleRate(){return sampleRate;}

      AnsiString              m_strProgramName;
      unsigned int            m_nFFTLen;
      float                   m_fEnabled;
      float                   m_fVisible;
      float                   m_fEdit;
      float                   m_fVisSpecFactor;
      bool                    m_bMuted;
      std::valarray<float >   m_vafFilter;
      std::valarray<float >   m_vafFilterBorders;

      #ifdef VISUAL_PLUGIN
      TfrmEqInput*            m_pfrmVisual;
      virtual void __fastcall FilterToChart();
      #endif

      unsigned int            m_nNumChannels;
      _RTL_CRITICAL_SECTION   csDataSection;
      vvaf                    m_vvafBuffer;
      CHtOLA*                 m_pOLA;

      void InitFilter();
      void InitOLA();
      void ExitOLA();
      void __fastcall ResampleFilter(std::vector<std::valarray<float> > &rvvaf);
      #ifdef VISUAL_PLUGIN
      void __fastcall FilterSetBorders(TChartSeries *pcs);
      void __fastcall FilterSmooth();
      void __fastcall FilterNormalize(TChartSeries *pcs);
      #endif
      void SpecProcessCallback(vvac & vvacSpectrum);

      #ifdef EXTPROCS
      virtual VstIntPtr vendorSpecific (VstInt32 lArg, VstIntPtr lArg2, void* ptrArg, float floatArg);
      static void      ExtFilterFunction(int nFunction, std::valarray<float >* pvaf, AnsiString as);
      void             FilterFunction(int nFunction, std::valarray<float >* pvaf, AnsiString as);
      #endif
};
//------------------------------------------------------------------------------
#endif // AHtVSTEqASH
