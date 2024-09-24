//------------------------------------------------------------------------------
/// \file AudioSpike2MAT.cpp
/// \author Berg
/// \brief Main for command line tool for converting AudioSpike-XMLs to MAT-Structs
///
/// Project AudioSpike
/// Module  AudioSpike2MAT.exe
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
#include <XMLDoc.hpp>
#include "Encddecd.hpp"
#pragma hdrstop

#include <tchar.h>
#include <stdio.h>
#include "SWMAT.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// winmain. Calls XMLFile2MAT with passed arugments
//---------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
   if (argc < 2)
      {
      printf("USAGE: AudioSpike2MAT XMLFILE [MATFILE]\n");
      return 0;
      }   
   UnicodeString usXML = argv[1];
   UnicodeString usMAT = argv[2];
   try
      {                                                                                                       
      XMLFile2MAT(usXML, usMAT);
      }                                                                                             
   catch (Exception &e)
      {
      printf("An error occurred: %ls\n", e.Message.w_str());
      return 1;
      }
   catch (...)
      {
      printf("Unknown error\n");
      return 1;                                                                             
      }

   return 0;
}
//---------------------------------------------------------------------------
