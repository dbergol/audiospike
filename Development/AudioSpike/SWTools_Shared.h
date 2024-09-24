//------------------------------------------------------------------------------
/// \file SWTools_Shared.h
/// \author Berg
/// \brief Implementation of shared Tools for AudioSpike (used in AudioSpikeMATLib 
/// as well)
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
#ifndef SWTools_SharedH
#define SWTools_SharedH
//------------------------------------------------------------------------------
#include <vcl.h>                                      
#include <System.inifiles.hpp>
#include <vector>
#include <valarray>
#include <Xml.Win.msxmldom.hpp>
#include <XML.XMLDoc.hpp>
#include <XML.xmldom.hpp>
#include <XML.XMLIntf.hpp>


typedef std::vector<std::valarray<float> >   vvf;
typedef std::vector<std::valarray<double> >  vvd;
typedef std::vector<std::vector<double> >    vved;

//------------------------------------------------------------------------------
/// \define TRYDELETENULL
/// \brief small helper function for calling delete on a pointer with
/// try...catch and setting pointer to NULL
//------------------------------------------------------------------------------
#define TRYDELETENULL(p) {if (p!=NULL) { try {delete p;} catch (...){;} p = NULL;}}


void           RemoveBrackets(UnicodeString &rus);
void           RemoveTrailingDelimiter(UnicodeString &us, wchar_t szDelimiter = L',');
void           RemoveLeadingDelimiter(UnicodeString &us, wchar_t szDelimiter);
void           ParseValues(TStrings *psl, const char* lpcsz, char szDelimiter = ',');
void           ParseValues(TStrings *psl, UnicodeString us, char szDelimiter = ',');
bool           TryParseMLVector(UnicodeString us, vved &rvved);
vved           ParseMLVector(UnicodeString us, UnicodeString usName);
void           ParseIntValues(   std::vector<int >& rvi,
                                 UnicodeString us,
                                 UnicodeString usName,
                                 char szDelimiter = ',',
                                 bool bUnique = true,
                                 bool bPositive = true);

UnicodeString  GetXMLValue(_di_IXMLNode &rNode, UnicodeString usName);

bool           IsDouble(AnsiString s);
AnsiString     DoubleToStr(double val);
double         StrToDouble(AnsiString s);
bool           TryStrToDouble(AnsiString s, double &d);

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

