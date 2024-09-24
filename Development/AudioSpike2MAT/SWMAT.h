//------------------------------------------------------------------------------
/// \file SWMAT.h
/// \author Berg
/// \brief Implementation tools for converting AudioSpike-XMLs to MAT-Structs
///
/// Project AudioSpike
/// Module  AudioSpikeMATLib.lib
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
#ifndef SWMATH
#define SWMATH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <XMLDoc.hpp>
#include <vector>
#include <valarray>


/// \param[in]       usXMLFile name of XML file to convert
/// \param[in,out]   rusMATFile reference to UnicodeString with optional name of
///                  output MAT file. If empty usXMLFile is used and extension changed
///                  to .mat. In every case it is set with expanded output filename
void XMLFile2MAT(UnicodeString usXMLFile, UnicodeString& rusMATFile);

/// \param[in]       xmlDoc AudioSpike XML-DocumentNode to convert
/// \param[in,out]   rusMATFile reference to UnicodeString with optional name of
///                  output MAT file. If empty usXMLFile is used and extension changed
///                  to .mat. In every case it is set with expanded output filename
void XML2MAT(_di_IXMLNode xmlDoc, UnicodeString& rusMATFile);



#endif
//------------------------------------------------------------------------------
