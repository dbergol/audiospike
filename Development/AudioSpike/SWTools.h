//------------------------------------------------------------------------------
/// \file SWTools.h
/// \author Berg
/// \brief Implementation of Tools for AudioSpike
///
/// Project AudioSpike
/// Module  AudioSpike.exe
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
#ifndef SWToolsH
#define SWToolsH
//------------------------------------------------------------------------------
#include <vcl.h>                                      
#include <System.inifiles.hpp>
#include <vector>
#include <valarray>
#include <Xml.Win.msxmldom.hpp>
#include <XML.XMLDoc.hpp>
#include <XML.xmldom.hpp>
#include <XML.XMLIntf.hpp>
#include "SWTools_Shared.h"


//------------------------------------------------------------------------------
// global tool functions called in AudioSpike.cpp before really initializing APP
//------------------------------------------------------------------------------
HWND ShowRunningAppWindow();

void EnsureNewIniLocation();
UnicodeString ReadSettingsName();
bool MulipleInstanceAllowed();
void WriteParamStrIni();


void ODSElapsed(DWORD &dw, UnicodeString us = "");
int            MsToSamples(double dMs, double dSampleRate);

UnicodeString  GetFormIniSection(TForm* pfrm);
void           AdjustStimulusFileNames(_di_IXMLNode xml, UnicodeString usPath);
double         dBToFactor(const double ddB);
double         FactorTodB(const double dFactor);
float          RMS(std::valarray<float > &rvaf);
double         RoundToTWoDecimalPlaces(double d);
AnsiString     GetStringValueFromSMPReturn(AnsiString as, AnsiString asField);

double         IniReadDouble(TIniFile* pIni, UnicodeString usSection, UnicodeString usPar, double dDefault);
AnsiString     DoubleToDisplayStr(double val, const char* lpcszFormat = NULL);
void           WriteToLogFile(AnsiString str, AnsiString strFile);
AnsiString     GetLastWindowsError();
AnsiString     GetFileVersion();
void           CreateSchroederPhaseToneComplexVector( std::valarray<float >   &rvf,
                                                      unsigned int            nSamples,
                                                      double                  dSampleRate,
                                                      double                  dFreqLo,
                                                      double                  dFreqHi,
                                                      bool                    bRandomPhase
                                                      );
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

