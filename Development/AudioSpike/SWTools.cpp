//------------------------------------------------------------------------------
/// \file SWTools.cpp
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
#pragma hdrstop

#include "SWTools.h"
#include "SpikeWareMain.h"
#include <math.h>
#include <algorithm>

#pragma package(smart_init)


//------------------------------------------------------------------------------
/// global tool functions called in AudioSpike.cpp before really initializing APP
//------------------------------------------------------------------------------
HWND ShowRunningAppWindow()
{
   HWND hwnd = FindWindow("TformSpikeWare", NULL);
   if (!!hwnd)
      {
      HWND hwndPopup = GetLastActivePopup(hwnd);
      BringWindowToTop(hwnd);
      SetForegroundWindow(hwnd);
      if (IsIconic(hwndPopup))
         ShowWindow(hwndPopup, SW_RESTORE);
      else
         BringWindowToTop(hwndPopup);
      SetActiveWindow(hwndPopup);
      }

   return hwnd;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Moves existing inifiles from "old" default directory (application's 
/// directory) to new 'default' directory
//------------------------------------------------------------------------------
void EnsureNewIniLocation()
{
   // first of all: move 'old' inifiles to new location
   UnicodeString usOld = IncludeTrailingBackslash(ExtractFilePath(Application->ExeName));
   UnicodeString usNew = TformSpikeWare::GetSettingsRootPath()  + "default\\";
   if (!DirectoryExists(usNew))
      {
      ForceDirectories(usNew);
      if (CopyFileW(UnicodeString(usOld + L"AudioSpike.ini").w_str(),    UnicodeString(usNew + L"AudioSpike.ini").w_str(), FALSE))
         RenameFile(usOld + L"AudioSpike.ini", usOld + L"AudioSpike.ini.obsolete");
      if (CopyFileW(UnicodeString(usOld + L"calibration.ini").w_str(),   UnicodeString(usNew + L"calibration.ini").w_str(), FALSE))
         RenameFile(usOld + L"calibration.ini", usOld + L"calibration.ini.obsolete");
      if (CopyFileW(UnicodeString(usOld + L"filters.ini").w_str(),       UnicodeString(usNew + L"filters.ini").w_str(), FALSE))
         RenameFile(usOld + L"filters.ini", usOld + L"filters.ini.obsolete");
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads current/last settings name from global Settings.ini OR command line
/// (command line wins)
//------------------------------------------------------------------------------
UnicodeString ReadSettingsName()
{
   UnicodeString usPath;
   // determine the inifile path in the following order
   // - read it from command line
   // - read it from INI in applications path   UnicodeString usPath;
   TStringList *psl = new TStringList();
   TIniFile* pIni = NULL;
   try
      {
      int n;
      for (n = 0; n < ParamCount(); n++)
         psl->Add(ParamStr(n+1));
      usPath = psl->Values["settings"];
      if (usPath.IsEmpty())
         {
         pIni = new TIniFile(ChangeFileExt(Application->ExeName, ".Settings.ini"));
         usPath = pIni->ReadString("Global", "Settings", "default");
         }
      if (!DirectoryExists(TformSpikeWare::GetSettingsRootPath() + usPath))
         usPath = "default";
      }
   __finally
      {
      TRYDELETENULL(psl);
      TRYDELETENULL(pIni);
      }
   return usPath;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns MulipleInstanceAllowed flag from current inifile
//------------------------------------------------------------------------------
bool MulipleInstanceAllowed()
{
   bool b = false;
   TIniFile* pIni = new TIniFile(TformSpikeWare::GetSettingsPath() + "AudioSpike.ini");
   try
      {
      b = pIni->ReadBool("Settings", "MulipleInstanceAllowed", false);
      }
   __finally
      {
      TRYDELETENULL(pIni);
      }
   return b;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// writes passed command line parameters to current AudioSpiki.ini
//------------------------------------------------------------------------------
void WriteParamStrIni()
{
   TIniFile* pIni = new TIniFile(TformSpikeWare::GetSettingsPath() + "AudioSpike.ini");
   try
      {
      pIni->WriteInteger("IPC", "ParamCount", ParamCount());
      int n;
      for (n = 0; n < ParamCount(); n++)
         pIni->WriteString("IPC", "Param"+IntToStr(n+1), ParamStr(n+1));
      }
   __finally
      {
      TRYDELETENULL(pIni);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// debugging function: writes passed string to OutputDebugString with timing
/// info and writes crrent tick count to passed DWORD
//------------------------------------------------------------------------------
void ODSElapsed(DWORD &dw, UnicodeString us)
{
   OutputDebugStringW((us + ": " + IntToStr((int)(GetTickCount() - dw))).w_str());

   dw = GetTickCount();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// converts milliseconds to number of samples for passed samplerate
//------------------------------------------------------------------------------
int MsToSamples(double dMs, double dSampleRate)
{
   return (int)ceil(dMs * dSampleRate / 1000.0);
}
//------------------------------------------------------------------------------

/*
//------------------------------------------------------------------------------
/// tool function triming an std::string in place
//------------------------------------------------------------------------------
void trim(std::string& str, char cTrim)
{
  std::string::size_type pos1 = str.find_first_not_of(cTrim);
  std::string::size_type pos2 = str.find_last_not_of(cTrim);
  str = str.substr(pos1 == std::string::npos ? 0 : pos1,
         pos2 == std::string::npos ? str.length() - 1 : pos2 - pos1 + 1);
}
//--------------------------------------------------------------------------------
*/

//------------------------------------------------------------------------------
/// reads a section name within an INI for properties of a form from forms
/// HelpFile (property is 'abused' here), or if empty from forms name
//------------------------------------------------------------------------------
UnicodeString  GetFormIniSection(TForm* pfrm)
{
   UnicodeString usSection = pfrm->HelpFile;
   if (usSection.IsEmpty())
      usSection = pfrm->Name;
   return usSection;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts dB to linear factor
//------------------------------------------------------------------------------
double dBToFactor(const double ddB)
{
   if (ddB > 740.0)
      return 1E37;
   else if (ddB <= -740.0)
      return 0.0;
   return (double)pow(10.0, (double)ddB/20.0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts linear factor to dB
//------------------------------------------------------------------------------
double FactorTodB(const double dFactor)
{
   if (dFactor < 1E-37)
      return -740.0;
   else if (dFactor > 1E37)
      return 740.0;
   return 20.0*log10(dFactor);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns RMS of passed signal in dB
//------------------------------------------------------------------------------
float RMS(std::valarray<float > &rvaf)
{
   float fRMS   = 0.0f;
   unsigned int n;
   for (n = 0; n < rvaf.size(); n++)
      fRMS += rvaf[n]*rvaf[n];
   fRMS = sqrt(fRMS/(float)rvaf.size());
   return (float)FactorTodB((double)fRMS);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// floors a double to two decimal places
//------------------------------------------------------------------------------
double RoundToTWoDecimalPlaces(double d)
{
   int n100 = (int)floor(100.0*d);
   return (double)n100 / 100.0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads an double value from ini returning a default value on conversio errors
//------------------------------------------------------------------------------
double IniReadDouble(TIniFile* pIni, UnicodeString usSection, UnicodeString usPar, double dDefault)
{
   double d;
   if (TryStrToDouble(pIni->ReadString(usSection, usPar, ""), d))
      return d;
   return dDefault;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// Returns a value from a "SoundMexPro"-like return string, i.e. 
///      "field1=value1;field2=value2"
//------------------------------------------------------------------------------
AnsiString GetStringValueFromSMPReturn(AnsiString as, AnsiString asField)
{
   TStringList *psl = new TStringList();
   try
      {
      ParseValues(psl, as, ';');
      return psl->Values[asField];
      }
   __finally
      {
      TRYDELETENULL(psl);
      }
   return "";
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// converts a double to a "displayable" string using passed format string.
/// default is "%lf"
//------------------------------------------------------------------------------
AnsiString DoubleToDisplayStr(double val, const char* lpcszFormat)
{
   AnsiString s;
   if (!lpcszFormat)
      lpcszFormat = "%lf";
   s.sprintf(lpcszFormat, val);
   return s;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// prepends path to all stimulus filenames in a subnode if not already a full path
//------------------------------------------------------------------------------
void  AdjustStimulusFileNames(_di_IXMLNode  xml, UnicodeString usPath)
{
   if (!xml)
      return;
   _di_IXMLNode xmlStimuli    = xml->ChildNodes->FindNode("Stimuli");
   if (!xmlStimuli)
      return;

   // number of stimuli
   int nNumStim = xmlStimuli->ChildNodes->Count;
   if (!nNumStim)
      return;
   UnicodeString usFileName;
   int nNode;

   for (nNode = 0; nNode < nNumStim; nNode++)
      {
      _di_IXMLNode xmlStim = xmlStimuli->ChildNodes->Nodes[nNode];
      if (xmlStim->GetNodeName() != "Stimulus")
         throw Exception("Invalid subnode name in Stimuli: " + xmlStim->GetNodeName());

      usFileName = GetXMLValue(xmlStim, "FileName");
      if (usFileName.Length() < 2)
         continue;
      // check if NOT starting with \ or :
      if (usFileName[1] != '\\' && usFileName[2] != ':')
         usFileName = usPath + usFileName;
      xmlStim->ChildValues["FileName"] = usFileName;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns versioninfo string as MAJOR.MINOR.REVISION.BUILD
//------------------------------------------------------------------------------
AnsiString GetFileVersion()
{
   AnsiString as;
   char cFilename[2*MAX_PATH];
   ZeroMemory(cFilename, 2*MAX_PATH);
   if (!GetModuleFileName(GetModuleHandle(NULL), cFilename, 2*MAX_PATH-1))
      return as;

   DWORD dwHandle;
   DWORD sz = GetFileVersionInfoSizeA( cFilename, & dwHandle );
   if ( 0 == sz )
      return as;

   char *buf = new char[sz];
   if ( !GetFileVersionInfoA( cFilename, dwHandle, sz, & buf[ 0 ] ) )
      {
      delete[] buf;
      return as;
      }
      
   VS_FIXEDFILEINFO * pvi;
   sz = sizeof( VS_FIXEDFILEINFO );
   if ( !VerQueryValueA( & buf[ 0 ], "\\", (LPVOID*)&pvi, (unsigned int*)&sz ) )
      {
      delete[] buf;
      return as;
      }

   as.sprintf( "%d.%d.%d.%d",
               pvi->dwProductVersionMS >> 16,
               pvi->dwFileVersionMS & 0xFFFF,
               pvi->dwFileVersionLS >> 16,
               pvi->dwFileVersionLS & 0xFFFF
            );

   delete[] buf;
   return as;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns last windows error message as AnsiString
//------------------------------------------------------------------------------
AnsiString GetLastWindowsError()
{
   DWORD dw = ::GetLastError();
   LPVOID lpMsgBuf;
   FormatMessage(
       FORMAT_MESSAGE_ALLOCATE_BUFFER |
       FORMAT_MESSAGE_FROM_SYSTEM |
       FORMAT_MESSAGE_IGNORE_INSERTS,
       NULL,
       dw,
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
       (LPTSTR) &lpMsgBuf,
       0,
       NULL
   );

   AnsiString strError = (LPCSTR)lpMsgBuf;
   // Free the buffer.
   LocalFree( lpMsgBuf );

   return strError;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// appends passed string to file
//------------------------------------------------------------------------------
void WriteToLogFile(AnsiString str, AnsiString strFile)
{
   if (strFile.IsEmpty() || strFile.Length() == 0)
      return;
   AnsiString strError;
   FILE *file = fopen(strFile.c_str(), "a");
   try
      {
      try
         {
         if (file)
            fprintf(file, "%s\n", str.c_str());
         }
      __finally
         {
         if (file)
            fclose(file);
         }
      }
   catch (Exception &e)
      {
      strError = e.Message;
      }
   catch (...)
      {
      strError = "unknown error";
      }
   if (!strError.IsEmpty())
      throw Exception("error writing to logfile '" + ExpandFileName(strFile) + "': " + strError);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Creates a Schroeder-phase tone complex
//------------------------------------------------------------------------------
void CreateSchroederPhaseToneComplexVector(  std::valarray<float >   &rvf,
                                             unsigned int            nSamples,
                                             double                  dSampleRate,
                                             double                  dFreqLo,
                                             double                  dFreqHi,
                                             bool                    bRandomPhase
                                             )
{
   double df0 = dSampleRate / (double)nSamples;

   unsigned int nLo = (unsigned int)floor(dFreqLo/df0);
   unsigned int nHi = (unsigned int)ceil(dFreqHi/df0);


   rvf.resize(nSamples);
   rvf = 0.0f;
   std::valarray<float > vfTmp(nSamples);
   std::valarray<float > vfTheta(nHi-nLo+1);

   unsigned int n;
   for (n = 0; n < vfTheta.size(); n++)
      {
      if (bRandomPhase)
         vfTheta[n] = (float)(2.0 * M_PI * (double)random((int)vfTheta.size()) / (double)vfTheta.size());
      else
         vfTheta[n] = (float)(M_PI * (double)(n+1) * (double)(n+2) / (double)vfTheta.size());
      }

   unsigned int m;
   for (n = nLo; n <= nHi; n++)
      {
      for (m = 0; m < nSamples; m++)
         vfTmp[m] = (float)cos(2*M_PI*(double)n*df0 * ((double)m/dSampleRate) + (double)vfTheta[n-nLo]);
      rvf += vfTmp;
      }

   float fMax = rvf.max();
   float fMin = fabs(rvf.min());
   if (fMin > fMax)
      fMax = fMin;


   rvf /= fMax;
}
//------------------------------------------------------------------------------

