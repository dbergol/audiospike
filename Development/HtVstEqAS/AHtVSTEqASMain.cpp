//------------------------------------------------------------------------------
/// \file AHtVSTEqASMain.cpp
/// \author Berg
/// \brief Implementation of main function of VST plugin CHtVSTEqAS
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
#include <windows.h>
#include <limits.h>
#include <float.h>

#include "AHtVSTEqAS.h"

//--------------------------------------------------------------------------
/// Prototype of the export function main
//--------------------------------------------------------------------------
extern "C" {
   AEffect *VSTPluginMain (audioMasterCallback audioMaster);
};
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// FTTW-Dll is loaded through import library using delayed loading. To be
/// able to show a human readable error to the user, we load the DLL here 
/// 'by hand' and show an error if it's missing. On success we unload it 
/// again and let delayed loading succeed on effect creation
//--------------------------------------------------------------------------
static void EnsureFFTW()
{
   HINSTANCE hLib = NULL;
   try
      {
      hLib = LoadLibrary("libfftw3f-3.dll");
      if (!hLib)
         MessageBox(0, "Cannot load mandatory FFTW library 'libfftw3f-3.dll'", "Error", MB_ICONERROR);
      }
   __finally
      {
      if (hLib)
         FreeLibrary(hLib);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// main function creating effect
//------------------------------------------------------------------------------
AEffect *VSTPluginMain (audioMasterCallback audioMaster)
{
   EnsureFFTW();

   // Get VST Version
   if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
      return 0;  // old version

   randomize();

   // Create the AudioEffect
   CHtVSTEq* effect = new CHtVSTEq (audioMaster);


   if (!effect)
      return 0;

   // Check if no problem in constructor of CHtVSTEqualizer
   if (!effect->m_bIsValid)
      {
      delete effect;
      return 0;
      }
   return effect->getAeffect();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// Dll-entry
//------------------------------------------------------------------------------
#pragma argsused
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
   #ifndef _WIN64
   if (dwReason == DLL_PROCESS_ATTACH)
      _control87(PC_64|MCW_EM,MCW_PC|MCW_EM);
   #endif
   return 1;
}
//------------------------------------------------------------------------------
