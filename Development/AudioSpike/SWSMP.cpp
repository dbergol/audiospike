//------------------------------------------------------------------------------
/// \file SWSMP.cpp
///
/// \author Berg
/// \brief Implementation of a class SWSMP, the interface to SoundMexProAS DLL
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

#include "SWSMP.h"
#include "SpikeWareMain.h"
#include "frmEpocheWindow.h"
#include "frmWait.h"
#include "frmSearchFree.h"
#include "frmCalibration.h"
#include "frmCalibrationCalibrator.h"
#include "frmFFTEdit.h"
#include "frmBatch.h"
#include "formAbout.h"
#include <math.h>
#include <string>
#include <algorithm>
//------------------------------------------------------------------------------

#pragma package(smart_init)
#pragma warn -aus
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// local tool function for calculating a hanning window value
//------------------------------------------------------------------------------
float GetHanningValue(unsigned int uWindowPos, unsigned int uWindowLen)
{
   if (!uWindowLen)
      throw Exception("invalid window lenght passed to " + UnicodeString(__FUNC__));
   return (float)(0.5 - 0.5*cos(M_PI*(double)(uWindowPos)/(double)uWindowLen));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initializes members
//------------------------------------------------------------------------------
SWSMP::SWSMP()
 :  m_hLib(NULL), m_lpfnSoundDllProCommand(NULL)
{
   #ifdef CHKCHNLS
   m_pslChannelsIn   = new TStringList();
   m_pslChannelsOut  = new TStringList();
   m_nTriggerChannelIn  = -1;
   m_nTriggerChannelOut = -1;
   m_nMonitorChannelOut = -1;
   m_nMicChannelIn      = -1;

   m_nTriggerChannelInIndex   = -1;
   m_nTriggerChannelOutIndex  = -1;
   m_nMonitorChannelOutIndex  = -1;
   #endif

   InitializeCriticalSection(&m_cs);
   m_pslDrivers      = new TStringList();
   m_usIniSection = "SoundSettings";
   m_bSettingsRead = false;
   m_fSineFreq = 1000.f;
   m_fSineFreqPending = m_fSineFreq;
   m_bAllowEqDiffLengths = false;
   m_nEqualisationLength = -1;
   m_nEqualisationMethod = AW_SMP_EQ_FFT;
   m_bShowFFTPlugins = false;

   m_bStopping    = false;
   m_nCalibrate   = 0;

   m_nFreeSearchStimLengthMs        = 0;
   m_nFreeSearchPreStimLengthMs     = 0;
   m_nFreeSearchRepetitionPeriodMs        = 0;
   m_nFreeSearchRepetitionPeriodSamples   = 0;
   m_nFreeSearchRampLengthMs        = 0;
   m_nFreeSearchSamplesPlayed       = 0;
   m_bFreeSearchContinuous          = false;
   m_fSchroederRMSdB                = 0.0f;
   m_fTriggerValue                  = 1.0f;
   m_nFakeTotalRecOffset            = 0;
   m_bSaveProbeMics                 = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destrctor, does cleanup
//------------------------------------------------------------------------------
SWSMP::~SWSMP()
{
   ExitLibrary();
   TRYDELETENULL(m_pslDrivers);
   #ifdef CHKCHNLS
   TRYDELETENULL(m_pslChannelsIn);
   TRYDELETENULL(m_pslChannelsOut);
   #endif
   DeleteCriticalSection(&m_cs);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads SoundMexPro (SMP) special DLL and gets pointer to exported command 
/// interface function
//------------------------------------------------------------------------------
void SWSMP::InitLibrary()
{
   ExitLibrary();
   UnicodeString us = IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "SoundDllPro.dll";

   m_hLib = LoadLibraryW(us.w_str());
   if (!m_hLib)
      {
      AnsiString as = GetLastWindowsError();
      throw Exception("error loading library '" + us + "': " + as);
      }

   m_lpfnSoundDllProCommand = (LPFNSOUNDDLLPROCOMMAND)GetProcAddress(m_hLib, "_" SOUNDDLL_COMMANDNAME);
   if (!m_lpfnSoundDllProCommand)
      m_lpfnSoundDllProCommand = (LPFNSOUNDDLLPROCOMMAND)GetProcAddress(m_hLib, SOUNDDLL_COMMANDNAME);
   if (!m_lpfnSoundDllProCommand)
      throw Exception("Cannot load function " SOUNDDLL_COMMANDNAME);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// exits SMP library by calling exit and unloading DLL
//------------------------------------------------------------------------------
void SWSMP::ExitLibrary()
{
   EnterCriticalSection(&m_cs);
   m_bStopping = true;
   try
      {
      if (m_lpfnSoundDllProCommand)
         {
         try
            {
            m_lpfnSoundDllProCommand("command=exit", m_lpszReturn, RETSTRMAXLEN);
            }
         catch (...)
            {
            }
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      formSpikeWare->m_bTriggerTestRunning   = false;
      formSpikeWare->m_bFreeSearchRunning    = false;
      }

   if (m_hLib)
      FreeLibrary(m_hLib);
   m_hLib = NULL;
   m_lpfnSoundDllProCommand = NULL;

   m_swcUsedChannels.Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns tue if SMP is initialized, false else
//------------------------------------------------------------------------------
bool SWSMP::Initialized()
{
   if (!m_hLib)
      return false;
   AnsiString asReturn;
   if (!Command("initialized", "", false, &asReturn))
      return false;

   bool bInitialized = asReturn.Pos("initialized=1") > 0;

   return bInitialized;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns number of processing xruns occured in SMP
//------------------------------------------------------------------------------
int  SWSMP::GetXRuns()
{
   int n = 0;

   AnsiString asReturn;
   if (!Command("xrun", "", true, &asReturn))
      return 0;
   if (TryStrToInt(GetStringValueFromSMPReturn(asReturn, "xrunproc"), n))
      return n;
   return 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// does some init-checking, sets starup values and calls "start" in SMP
//------------------------------------------------------------------------------
bool SWSMP::Start(int nEpocheSize)
{
   if (!nEpocheSize)
      {
      if (!formSpikeWare->m_sweEpoches.m_vvfEpoche.size())
         throw Exception("epoches not initialized");
      nEpocheSize = (int)formSpikeWare->m_sweEpoches.m_vvfEpoche[0].size();
      }
   if (nEpocheSize < 0)
      nEpocheSize = (int)m_nBufferSize;
   else if (nEpocheSize < (int)m_nBufferSize)
      throw Exception("epoche size must not be smaller than ASIO buffer size");

   m_nFreeSearchSamplesPlayed    = 0;
   m_nFreeSearchWindowPos        = -1;
   m_nFreeSearchSchroederPos     = 0;
   m_nFreeSearchTriggerPos       = -1;

   formSpikeWare->m_sweEpoches.Start();
   m_bStopping = false;
   return Command("start");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls "stop" in SMP
//------------------------------------------------------------------------------
bool SWSMP::Stop()
{
   if (!Initialized())
      return true;

   bool bReturn = false;
   EnterCriticalSection(&m_cs);
   try
      {
      m_bStopping = true;
      bReturn = Command("stop", "", false);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls "exit" in SMP
//------------------------------------------------------------------------------
bool SWSMP::Exit()
{
   bool bReturn = false;
   EnterCriticalSection(&m_cs);
   try
      {
      m_nCalibrate = 0;
      Stop();
      bReturn = Command("exit", "", false);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
   return bReturn;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls "wait" in SMP
//------------------------------------------------------------------------------
bool SWSMP::Wait()
{
   return Command("wait");
}
//------------------------------------------------------------------------------
/// calls "started" in SMP
//------------------------------------------------------------------------------
bool SWSMP::Playing()
{
   if (!Initialized())
      return false;

   AnsiString asReturn;
   if (!Command("started", "", true, &asReturn))
      return false;

   return asReturn.Pos("value=1") > 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Sends a command to SMP, stores return values in m_lpszReturn and handles
/// success/failure
//------------------------------------------------------------------------------
bool SWSMP::Command(AnsiString asCmd, AnsiString asArgs, bool bShowError,  AnsiString *asReturn)
{
   if (!m_hLib)
      throw Exception("SMP not initialized", -1);

   if (asCmd == "start")
      m_bStopping = false;

   asCmd = "command=" + asCmd;
   if (asArgs.Length())
      asCmd = asCmd + ";" + asArgs;

   int nReturn = m_lpfnSoundDllProCommand(asCmd.c_str(), m_lpszReturn, RETSTRMAXLEN);
   if (!!asReturn)
      *asReturn = m_lpszReturn;

   Application->ProcessMessages();

   if (nReturn != SOUNDDLL_RETURN_OK)
      {
      m_usLastError = GetStringValueFromSMPReturn(m_lpszReturn, "error");

      if (bShowError)
         {
         // special for 'insufficient license')
         UnicodeString us = "Error in sound command '" + asCmd + "': " + m_usLastError;
         formSpikeWare->SWErrorBox(us);
         }
      return false;
      }
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns currently used driver
//------------------------------------------------------------------------------
UnicodeString SWSMP::GetDriver()
{
   return m_usDriver;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets current driver abd calls InitializeChannels
//------------------------------------------------------------------------------
void  SWSMP::SetDriver(UnicodeString usDriver)
{
   if (m_pslDrivers->IndexOf(usDriver) < 0)
      throw Exception("driver not found in system: " + usDriver);
   m_usDriver = usDriver;
   if (!InitializeChannels())
      throw Exception("error initializing audio channels");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// initializes own channel lists after query to SMP for channels
//------------------------------------------------------------------------------
bool SWSMP::InitializeChannels()
{
   m_swcHWChannels.Clear();
   TStringList *pslOut = new TStringList();
   TStringList *pslIn  = new TStringList();
   try
      {
      if (!GetChannels(m_usDriver, pslIn, pslOut))
         return false;
      int n;

      for (n = 0; n < pslIn->Count; n++)
         m_swcHWChannels.Add(pslIn->Strings[n], SWSMPHWCDIR_IN);
      for (n = 0; n < pslOut->Count; n++)
         m_swcHWChannels.Add(pslOut->Strings[n], SWSMPHWCDIR_OUT);
      }
   __finally
      {
      TRYDELETENULL(pslIn);
      TRYDELETENULL(pslOut);
      }
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls SMP "getchannels" and parses return into passed string lists
//------------------------------------------------------------------------------
bool SWSMP::GetChannels(UnicodeString usDriver, TStrings *pslIn, TStrings *pslOut)
{
   UnicodeString usArgs;
   if (!Initialized())
      {
      if (m_pslDrivers->IndexOf(usDriver) < 0)
         {
         UnicodeString us = "Driver '" + usDriver + "' not found in the system";
         formSpikeWare->SWErrorBox(us);
         return false;
         }
      usArgs = "driver=" + usDriver;
      }

   AnsiString asReturn;
   // call command without showing error
   if (!Command("getchannels", usArgs, false, &asReturn))
      return false;

   TStringList* psl = new TStringList();
   try
      {
      ParseValues(psl, asReturn, ';');
      if (!!pslOut)
         ParseValues(pslOut, psl->Values["output"]);

      if (!!pslIn)
         ParseValues(pslIn, psl->Values["input"]);
      return true;
      }
   __finally
      {
      TRYDELETENULL(psl);
      }
   return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns name of section in calibration ini file to be used
//------------------------------------------------------------------------------
UnicodeString SWSMP::GetCalibrationSection(UnicodeString usChannel)
{
   UnicodeString us = formSpikeWare->IsInSitu() ? "FFT - INSITU" : "FFT";
   if (m_nEqualisationMethod != AW_SMP_EQ_FFT)
      {
      us = Trim(GetEqualisation(usChannel));
      if (us.IsEmpty())
         us = "EMPTY";
      us = "IR_" + us;
      }
   return us;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// writes a calibration value calibration ini file 
//------------------------------------------------------------------------------
void SWSMP::SetCalibrationValue(unsigned int nOutChannel, double dCalValue)
{
   #ifdef CHKCHNLS
   if ((int)m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT) != m_pslChannelsOut->Count)
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   if (m_swcHWChannels.GetChannelName(nOutChannel, SWSMPHWCDIR_OUT) != m_pslChannelsOut->Strings[(int)nOutChannel])
      ShowMessage("error B "+ UnicodeString( __FUNC__));
   #endif

   if (nOutChannel >= m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT))
      throw Exception("Cannot write calibration value: index exceeds available channels");
   UnicodeString usChannel = m_swcHWChannels.GetChannelName(nOutChannel, SWSMPHWCDIR_OUT);
   formSpikeWare->m_pCalIni->WriteString(GetCalibrationSection(usChannel), usChannel, DoubleToStr(dCalValue));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns a calibration value from calibration ini file for particular channel
/// by index
//------------------------------------------------------------------------------
double SWSMP::GetCalibrationValueN(unsigned int nOutChannel)
{
   std::vector<int > vi = m_swcUsedChannels.GetOutputs();

   #ifdef CHKCHNLS
   if ((int)m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT) != m_pslChannelsOut->Count)
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   if (m_swcHWChannels.GetChannelName((unsigned int)m_viHardwareChannelsOutUsed[nOutChannel], SWSMPHWCDIR_OUT) != m_pslChannelsOut->Strings[m_viHardwareChannelsOutUsed[nOutChannel]])
      ShowMessage("error B "+ UnicodeString( __FUNC__));
   if (  m_swcHWChannels.GetChannelName((unsigned int)m_viHardwareChannelsOutUsed[nOutChannel], SWSMPHWCDIR_OUT)
      != m_swcUsedChannels.GetChannelName((unsigned int)vi[nOutChannel], SWSMPHWCDIR_OUT)
      )
      ShowMessage("error D "+ UnicodeString( __FUNC__));

   #endif

   if (nOutChannel >= vi.size())
      return 0.0;

   return GetCalibrationValue(m_swcUsedChannels.GetChannelName((unsigned int)vi[nOutChannel], SWSMPHWCDIR_OUT));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns a calibration value from calibration ini file for particular channel
/// by naeme
//------------------------------------------------------------------------------
double SWSMP::GetCalibrationValue(UnicodeString usChannel)
{
   return IniReadDouble(formSpikeWare->m_pCalIni, GetCalibrationSection(usChannel), usChannel, 0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets equalisation for a channel by index
//------------------------------------------------------------------------------
void SWSMP::SetEqualisation(unsigned int nOutChannel, UnicodeString usEqualisation)
{

   #ifdef CHKCHNLS
   if ((int)m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT) != m_pslChannelsOut->Count)
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   if (m_swcHWChannels.GetChannelName(nOutChannel, SWSMPHWCDIR_OUT) != m_pslChannelsOut->Strings[(int)nOutChannel])
      ShowMessage("error B "+ UnicodeString( __FUNC__));
   #endif

  if (nOutChannel >= m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT))
      throw Exception("Cannot write equalisation: index exceeds available channels");
   UnicodeString usChannel = m_swcHWChannels.GetChannelName(nOutChannel, SWSMPHWCDIR_OUT);
   formSpikeWare->m_pIni->WriteString("Equalisation", usChannel, usEqualisation);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// returns a equalisation for particular channel by index
//------------------------------------------------------------------------------
UnicodeString SWSMP::GetEqualisationN(unsigned int nOutChannel)
{
   std::vector<int > vi = m_swcUsedChannels.GetOutputs();

   #ifdef CHKCHNLS
   if ((int)m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT) != m_pslChannelsOut->Count)
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   if (m_swcHWChannels.GetChannelName((unsigned int)m_viHardwareChannelsOutUsed[nOutChannel], SWSMPHWCDIR_OUT) != m_pslChannelsOut->Strings[m_viHardwareChannelsOutUsed[nOutChannel]])
      ShowMessage("error B "+ UnicodeString( __FUNC__));
   if (  m_swcHWChannels.GetChannelName((unsigned int)m_viHardwareChannelsOutUsed[nOutChannel], SWSMPHWCDIR_OUT)
      != m_swcUsedChannels.GetChannelName((unsigned int)vi[nOutChannel], SWSMPHWCDIR_OUT)
      )
      ShowMessage("error D "+ UnicodeString( __FUNC__));
   #endif

   if (nOutChannel >= vi.size())
      return "";

   return GetEqualisation(m_swcUsedChannels.GetChannelName((unsigned int)vi[nOutChannel], SWSMPHWCDIR_OUT));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns a equalisation for particular channel by name
//------------------------------------------------------------------------------
UnicodeString SWSMP::GetEqualisation(UnicodeString usChannel)
{
   if (m_nEqualisationMethod == AW_SMP_EQ_FFT)
      {
      if (formSpikeWare->IsInSitu())
         return usChannel + " - INSITU";
      else
         return usChannel;
      }
   else
      return formSpikeWare->m_pIni->ReadString("Equalisation", usChannel, "");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns in-situ input channel name for an output channel
//------------------------------------------------------------------------------
UnicodeString SWSMP::GetInSituInput(UnicodeString usOutChannel)
{
   UnicodeString us = formSpikeWare->m_pIni->ReadString("SoundSettingsInSitu", usOutChannel, "");

   #ifdef CHKCHNLS
   if (m_swcHWChannels.GetChannelIndex(us, SWSMPHWCDIR_IN) != m_pslChannelsIn->IndexOf(us))
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   #endif
   if (m_swcHWChannels.GetChannelIndex(us, SWSMPHWCDIR_IN) >= -1)
      return us;
   return "";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns in-situ input channel index for an output channel
//------------------------------------------------------------------------------
int SWSMP::GetInSituInputChannel(UnicodeString usOutChannel)
{
   int nReturn = -1;
   UnicodeString us = GetInSituInput(usOutChannel);
   if (us.Length())
      nReturn = m_swcHWChannels.GetChannelIndex(us, SWSMPHWCDIR_IN);

   #ifdef CHKCHNLS
   if (m_swcHWChannels.GetChannelIndex(us, SWSMPHWCDIR_IN) != m_pslChannelsIn->IndexOf(us))
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   #endif

   return nReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns name of filter section for an in-situ input
//------------------------------------------------------------------------------
UnicodeString SWSMP::GetInSituInputFilterSection(UnicodeString usOutChannel)
{
   UnicodeString us = GetInSituInput(usOutChannel);
   if (!us.IsEmpty())
      us += " - PROBEMIC";
   return us;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets/clears an in-situ input for an output channel 
//------------------------------------------------------------------------------
void SWSMP::SetInSituInput(UnicodeString usOutChannel, UnicodeString usInChannel)
{
   if (Trim(usInChannel) == "")
      formSpikeWare->m_pIni->DeleteKey("SoundSettingsInSitu", usOutChannel);
   else
      formSpikeWare->m_pIni->WriteString("SoundSettingsInSitu", usOutChannel, usInChannel);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// main init procedure for different modes "measure", "trigger test", "free search"
//------------------------------------------------------------------------------
bool SWSMP::Init(int nMode)
{
   if (!ReadSettings())
      return false;
      
   try
      {
      if (formSpikeWare->m_swsStimuli.m_dDeviceSampleRate == 0.0)
         formSpikeWare->m_swsStimuli.m_dDeviceSampleRate = (double)m_fDefaultSampleRate;
      float fSampleRate = (float)formSpikeWare->m_swsStimuli.m_dDeviceSampleRate;

      formSpikeWare->m_bTriggerTestRunning   = nMode == AS_SMP_INIT_TRIGGERTEST;
      formSpikeWare->m_bFreeSearchRunning    = false;

      m_nTriggerLength  = (int)formSpikeWare->m_swsSpikes.m_dSampleRateDevider*4;

      if (Initialized())
         Exit();

      m_swcUsedChannels.Clear();

      unsigned int n;
      std::vector<int > vi;
      std::vector<int > viChannelsOutSettings = m_swcHWChannels.GetOutputs();

      #ifdef CHKCHNLS
      if (viChannelsOutSettings.size() != m_viChannelsOutSettings.size())
         ShowMessage("error A "+ UnicodeString( __FUNC__));
      else
         {
         for (unsigned int x = 0; x < viChannelsOutSettings.size();x++)
            {
            if (viChannelsOutSettings[x] !=  m_viChannelsOutSettings[x])
               ShowMessage("error B "+ UnicodeString( __FUNC__));
            }
         }
      #endif


      if (nMode == AS_SMP_INIT_FREESEARCH)
         {
         m_nNumLoadedStimuli = 0;
         vi = viChannelsOutSettings;
         // maybe for future use: allow raw output in free search
         /*
         for (unsigned int i = 0;i  < viTmp.size(); i++)
            {
            ShowMessage();
            if (!m_swcHWChannels.IsOutputRaw(viChannelsOutSettings[i]))
               {

               vi.push_back(viChannelsOutSettings[i]);
               }
            }
         */
         }
      else if (nMode == AS_SMP_INIT_MEASURE)
         {
         float f;
         bool bHiPassUsed = false;
         // translate m_viMeasChannelsOutUsed to hardware channels relative to viChannelsOutSettings
         // NOTE: m_viMeasChannelsOutUsed is sorted ascending
         if (m_viMeasChannelsOutUsed.back() >= (int)viChannelsOutSettings.size())
            throw Exception("Invalid 'OutputChannels' specified (maximum available channel index is " + IntToStr((int)viChannelsOutSettings.size()) + ")");
         for (n = 0; n < m_viMeasChannelsOutUsed.size(); n++)
            {
            vi.push_back(viChannelsOutSettings[(unsigned int)m_viMeasChannelsOutUsed[n]]);
            #ifdef CHKCHNLS
            if (m_swcHWChannels.GetChannelName((unsigned int)vi.back(), SWSMPHWCDIR_OUT) != m_pslChannelsOut->Strings[vi.back()])
               ShowMessage("error X1 "+ UnicodeString( __FUNC__));
            #endif
            formSpikeWare->m_swfFilters->GetHiPass(m_swcHWChannels.GetChannelName((unsigned int)vi.back(), SWSMPHWCDIR_OUT), f);
            if (f > 0.0f)
               bHiPassUsed = true;
            }


         if (bHiPassUsed)
            formSpikeWare->HighPassWarning();
         }

      #ifdef CHKCHNLS
      m_viHardwareChannelsOutUsed = vi;
      #endif

      // start to fill used channels
      int nTriggerOutHWIndex = m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT);


      // - outputs
      for (n = 0; n < vi.size(); n++)
         {
         m_swcUsedChannels.AddChannel(m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][(unsigned int)vi[n]], SWSMPHWCDIR_OUT);
         }
      // - trigger
      m_swcUsedChannels.AddChannel(m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][(unsigned int)nTriggerOutHWIndex], SWSMPHWCDIR_OUT);
      // - optionally monitor
      if (m_swcHWChannels.GetMonitor() >= 0)
         m_swcUsedChannels.AddChannel(m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][(unsigned int)m_swcHWChannels.GetMonitor()], SWSMPHWCDIR_OUT);

      // sort them ascending by hardware index
      m_swcUsedChannels.SortByHWIndex(SWSMPHWCDIR_OUT);

      #ifdef CHKCHNLS
      std::vector<int > viTmp = m_swcUsedChannels.GetOutputIndices();
      if (viTmp.size() != vi.size())
         ShowMessage("error V1 "+ UnicodeString( __FUNC__));
      if (viTmp.size() == vi.size())
         {
         for (n = 0; n < vi.size(); n++)
            {
            if (vi[n] != viTmp[n])
               ShowMessage("error V2 "+ UnicodeString( __FUNC__));
            }
         }
      #endif

      if (nMode != AS_SMP_INIT_TRIGGERTEST)
         {
         if (!vi.size())
            throw Exception("no output channels configured");
         // HIER war
         //    if (false && formSpikeWare->IsInSitu())
         // WARUM???
         if (formSpikeWare->IsInSitu())
            {
            // first check existance of probe-mic filter of all used channels before
            // running calibration....
            for (n = 0; n < vi.size(); n++)
               {
               // skip raw output channels: they don't have a probe mic
               if (m_swcHWChannels.IsOutputRaw((unsigned int)vi[n]))
                  continue;
               //
               #ifdef CHKCHNLS
               if (m_swcHWChannels.GetChannelName((unsigned int)vi[n], SWSMPHWCDIR_OUT) != m_pslChannelsOut->Strings[vi[n]])
                  ShowMessage("error X1 "+ UnicodeString( __FUNC__));
               #endif

               UnicodeString us = GetInSituInputFilterSection(m_swcHWChannels.GetChannelName((unsigned int)vi[n], SWSMPHWCDIR_OUT));

               if (us.IsEmpty() || !formSpikeWare->m_swfFilters->m_pFilterIni->SectionExists(us))
                  throw Exception("probe microphone is not calibrated on one or more used channels");
               }

            if (!formSpikeWare->IsBatchMode() || formSpikeWare->m_pformBatch->DoInsituCal())
               {
               for (n = 0; n < vi.size(); n++)
                  {
                  // skip raw output channels: they don't have a probe mic
                  if (m_swcHWChannels.IsOutputRaw((unsigned int)vi[n]))
                     continue;
                  if (mrOk != formFFTEdit->Calibrate(vi[n], CAL_MODE_INSITU))
                     return false;
                  }
               }
            }
         }

      formSpikeWare->m_bFreeSearchRunning    = nMode == AS_SMP_INIT_FREESEARCH;

      if (nMode == AS_SMP_INIT_MEASURE)
         {
         if (!formSpikeWare->m_pIni->ReadBool("Debug", "NoMaxSearch", false))
            {
            if (!MaxSearch())
               throw Exception("Error determining maximum levels");
            Application->ProcessMessages();

            if (vi.size() != m_vadMaxLevelsAvailable.size())
               throw Exception("unexpected maxlevel sizing error");
            int nIndex, nChannelIndex;
            std::vector<double > vd;
            for (n = 0; n < m_vadMaxLevelsAvailable.size(); n++)
               {
               nChannelIndex = (int)n+1;
               // get maximum requested level for that channel
               //& - first get index to correct level parameter
               if (formSpikeWare->m_swsStimuli.m_nChannelLevels == 1)
                  nIndex = formSpikeWare->m_swsStimuli.m_swspStimPars.IndexFromName("Level_1");
               else
                  nIndex = formSpikeWare->m_swsStimuli.m_swspStimPars.IndexFromName("Level_" + IntToStr(nChannelIndex));
               if (nIndex < 0)
                  throw Exception("fatal error: 'Level_" + IntToStr(nChannelIndex) + "' missing in parameters");
               // copy ALL values to temporary vector
               vd = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vvdValues[(unsigned int)nIndex];
               // sort them ascending
               std::sort(vd.begin(), vd.end());
               // check it vs. available level
               if (vd.back() > m_vadMaxLevelsAvailable[n])
                  {
                  UnicodeString us;
                  us.printf(L"A presentation level above the maximum available level is requested on channel %d (requested: %lf dB, available: %lf dB",
                     n+1, vd.back(), m_vadMaxLevelsAvailable[n]);
                  throw Exception(us);
                  }
               }
            }
         else if (formSpikeWare->m_pIni->ReadBool("Debug", "ShowNoMaxSearch", true))
            MessageBox(0, "MaxSearch disabled", "Warning", MB_ICONWARNING);
         }

      #ifdef OLDBOUBLETTECHECK
      // OLD DOUBLETTE CHECK
      // NOTE: channels ushed to vi only for checking, vi not used below for outputs any more!
      int nTriggerOutChannel = m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT);
      vi.push_back(nTriggerOutChannel);
      int nMonitorOutChannel = m_swcHWChannels.GetMonitor();
      if (nMonitorOutChannel >= 0)
         vi.push_back(nMonitorOutChannel);
      if (m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT) == m_swcHWChannels.GetMonitor())
         throw Exception("Monitor out channel unexpectedly identical to trigger out channel");

      if (nMonitorOutChannel != m_nMonitorChannelOut)
         ShowMessage("error M1 "+ UnicodeString( __FUNC__));
      if (nTriggerOutChannel != m_nTriggerChannelOut)
         ShowMessage("error T1 "+ UnicodeString( __FUNC__));


      std::sort(vi.begin(), vi.end());
      // check for doublettes
      for (n = 0; n < vi.size()-1; n++)
         {
         if (vi[n] == vi[n+1])
            {
            if (vi[n] == nMonitorOutChannel)
               throw Exception("Monitor out channel unexpectedly used as output channel as well");
            else if (vi[n] == nTriggerOutChannel)
               throw Exception("Trigger out channel unexpectedly used as output channel as well");
            else
               throw Exception("Unexpected output channel doublette detected");
            }
         }
      #else
      // NEW DOUBLETTE CHECK!!
      std::vector<int > viCheck;
      for (n = 0; n < m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_OUT].size(); n++)
         viCheck.push_back((int)m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][n].m_nHWIndex);
      std::sort(viCheck.begin(), viCheck.end());
      for (n = 0; n < viCheck.size()-1; n++)
         {
         if (viCheck[n] == viCheck[n+1])
            throw Exception("Unexpectedly an output channel is used twice: " + m_swcHWChannels.GetChannelName(n, SWSMPHWCDIR_OUT));
         }
      #endif


      std::vector<int > viOutputTracks = m_swcUsedChannels.GetOutputs();
      UnicodeString usOut;
      for (n = 0; n < m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_OUT].size(); n++)
         usOut += IntToStr((int)m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][n].m_nHWIndex) + ",";
      RemoveTrailingDelimiter(usOut, L',');


      #ifdef CHKCHNLS
      m_nTriggerChannelOutIndex = (int)std::distance(viCheck.begin(), std::find(viCheck.begin(), viCheck.end(), m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT)));
      if (m_swcHWChannels.GetMonitor() >= 0)
         m_nMonitorChannelOutIndex  = (int)std::distance(viCheck.begin(), std::find(viCheck.begin(), viCheck.end(), m_swcHWChannels.GetMonitor()));
      else
         m_nMonitorChannelOutIndex = -1;



      if (m_nTriggerChannelOutIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
         ShowMessage("error TO1 " + UnicodeString(__FUNC__) +  ": " + IntToStr(m_nTriggerChannelOutIndex) + " | " + IntToStr(m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT)));
      if (m_nMonitorChannelOutIndex != m_swcUsedChannels.GetMonitor())
         ShowMessage("error TM1 " + UnicodeString(__FUNC__) +  ": " + IntToStr(m_nMonitorChannelOutIndex) + " | " + IntToStr(m_swcUsedChannels.GetMonitor()));
      #endif



      std::vector<int > viChannelsInSettings = m_swcHWChannels.GetElectrodes();
      #ifdef CHKCHNLS
      if (viChannelsInSettings.size() != m_viChannelsInSettings.size())
         ShowMessage("error C "+ UnicodeString( __FUNC__));
      else
         {
         for (unsigned int x = 0; x < viChannelsInSettings.size();x++)
            {
            if (viChannelsInSettings[x] !=  m_viChannelsInSettings[x])
               ShowMessage("error D "+ UnicodeString( __FUNC__));
            }
         }
      #endif
// HIER
      vi.clear();
      if (nMode == AS_SMP_INIT_FREESEARCH)
         vi = viChannelsInSettings;
      else if (nMode == AS_SMP_INIT_MEASURE)
         {
         // translate m_viMeasChannelsInUsed to hardware channels relative to viChannelsInSettings
         // NOTE: m_viMeasChannelsInUsed is sorted ascending
         if (m_viMeasChannelsInUsed.back() >= (int)viChannelsInSettings.size())
            throw Exception("Invalid 'InputChannels' specified (maximum available channel index is " + IntToStr((int)viChannelsInSettings.size()) + ")");
         for (n = 0; n < m_viMeasChannelsInUsed.size(); n++)
            vi.push_back(viChannelsInSettings[(unsigned int)m_viMeasChannelsInUsed[n]]);
         }

      if (nMode != AS_SMP_INIT_TRIGGERTEST && !vi.size())
         throw Exception("no input channels configured");


      // start to fill used channels
      // - inputs
      for (n = 0; n < vi.size(); n++)
         m_swcUsedChannels.AddChannel(m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)vi[n]], SWSMPHWCDIR_IN);

      // - insitu mics
      std::vector<SWChannelSort > vswcs;
      if (formSpikeWare->IsInSitu() && m_bSaveProbeMics)
         {
         // NOTE: here we setup m_viProbeMicOutChannels that will contain the order for saving
         // probemic audio data
         m_viProbeMicOutChannels.clear();
         unsigned int nInsituIndex = 0;
         for (n = 0; n < m_swcUsedChannels.GetNumChannels(SWSMPHWCDIR_OUT); n++)
            {
            // skip raw output channels: they don't have a probe mic
            if (!m_swcUsedChannels.IsOutput(n) || m_swcUsedChannels.IsOutputRaw(n))
               continue;
            int nMicChannel = GetInSituInputChannel(m_swcUsedChannels.GetChannelName(n, SWSMPHWCDIR_OUT));
            if (nMicChannel < 0)
               throw Exception("In-situ input channel for output '" + m_swcUsedChannels.GetChannelName(n, SWSMPHWCDIR_OUT) + "' not configured.");

            // setup vector with structs hardware indices AND index of all insitu mics
            SWChannelSort swcs;
            swcs.m_nAbsolutIndex = (unsigned int)nMicChannel;
            swcs.m_nIndex = nInsituIndex++;
            swcs.m_usName = m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)nMicChannel].m_usName;
            vswcs.push_back(swcs);

            m_swcUsedChannels.AddChannel(m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)nMicChannel], SWSMPHWCDIR_IN);
            vi.push_back((int)m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)nMicChannel].m_nHWIndex);
            }

         // sort vector by hardware index
         std::sort(vswcs.begin(), vswcs.end(), by_AbsoluteIndex());
         // fill new ordered probemic indices
         for (n = 0; n < vswcs.size(); n++)\
            m_viProbeMicOutChannels.push_back((int)vswcs[n].m_nIndex);

         }

      // - trigger
      int nTriggerInIndex = m_swcHWChannels.GetTrigger(SWSMPHWCDIR_IN);
      m_swcUsedChannels.AddChannel(m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)nTriggerInIndex], SWSMPHWCDIR_IN);
      // sort them ascending by hardware index
      m_swcUsedChannels.SortByHWIndex(SWSMPHWCDIR_IN);

      #ifdef CHKCHNLS
      viTmp = m_swcUsedChannels.GetElectrodeIndices();
      std::vector<int > viTmp2;
      if (formSpikeWare->IsInSitu() && m_bSaveProbeMics)
         {
         viTmp2 = m_swcUsedChannels.GetProbeMicIndices();
         for (n = 0; n < viTmp2.size(); n++)
            viTmp.push_back(viTmp2[n]);
         std::sort(viTmp.begin(), viTmp.end());
         std::sort(vi.begin(), vi.end());
         }

      if (viTmp.size() != vi.size())
         ShowMessage("error VI1 "+ UnicodeString( __FUNC__));
      if (viTmp.size() == vi.size())
         {
         for (n = 0; n < vi.size(); n++)
            {
            if (vi[n] != viTmp[n])
               ShowMessage("error VI2 "+ UnicodeString( __FUNC__));
            }
         }
      #endif


      // NOTE: input trigger channel ALWAYS added here: used below to load plugin for correct latency handling
      // below in VSTLoad loop on vi!!!
      int nTriggerInChannel = m_swcHWChannels.GetTrigger(SWSMPHWCDIR_IN);
      vi.push_back(nTriggerInChannel);

      #ifdef CHKCHNLS

      if (nTriggerInChannel != m_nTriggerChannelIn)
         ShowMessage("error T2 "+ UnicodeString( __FUNC__));
      #endif


      #ifdef OLDBOUBLETTECHECK
      // OLD DOUBLETTE CHECK
      std::sort(vi.begin(), vi.end());

      // check for doublettes
      for (n = 0; n < vi.size()-1; n++)
         {
         if (vi[n] == vi[n+1])
            {
            if (vi[n] == nTriggerInChannel)
               throw Exception("Trigger in channel unexpectedly used as input channel as well");
            else
               throw Exception("Unexpected input channel doublette detected");
            }
         }

      #else
      // NEW DOUBLETTE CHECK!!
      viCheck.clear();
      for (n = 0; n < m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_IN].size(); n++)
         viCheck.push_back((int)m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_IN][n].m_nHWIndex);
      std::sort(viCheck.begin(), viCheck.end());
      for (n = 0; n < viCheck.size()-1; n++)
         {
         if (viCheck[n] == viCheck[n+1])
            throw Exception("Unexpectedly an input channel is used twice : " + m_swcHWChannels.GetChannelName(n, SWSMPHWCDIR_IN));
         }

      #endif

      UnicodeString usIn;
      for (n = 0; n < m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_IN].size(); n++)
         usIn += IntToStr((int)m_swcUsedChannels.m_vvswcChannels[SWSMPHWCDIR_IN][n].m_nHWIndex) + ",";
      RemoveTrailingDelimiter(usIn, L',');


      #ifdef CHKCHNLS
      std::sort(vi.begin(), vi.end());
      UnicodeString usIn2;
      for (n = 0; n < vi.size(); n++)
         usIn2 += IntToStr(vi[n]) + ",";
      RemoveTrailingDelimiter(usIn2, L',');
      if (usIn != usIn2)
         ShowMessage("error I1C "+ UnicodeString( __FUNC__));

      m_nTriggerChannelInIndex = (int)std::distance(vi.begin(), std::find(vi.begin(), vi.end(), nTriggerInChannel));

      if (m_nTriggerChannelInIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
         ShowMessage("error TI1 "+ UnicodeString( __FUNC__));
      #endif

      int nNotifyChannel = m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT);
      UnicodeString us = "driver=" + m_usDriver + ";";
      us += "output="   + usOut + ";";
      us += "input="    + usIn + ";";
      us += "ramplen=0;";
      us += "autocleardata=1;";
      us += "samplerate=" + DoubleToStr((double)fSampleRate) + ";";
      us += "recdownsamplefactor=" + IntToStr((int)floor(formSpikeWare->m_swsSpikes.m_dSampleRateDevider)) + ";";
      // attach pre-vst processing for signal generator in free search mode
      us += "extprevstproc="  + IntToStr((NativeInt)&TformSpikeWare::SMPPreVSTProc) + ";";
      // attach post-vst processing for clip detection in free search mode
      us += "extpostvstproc="  + IntToStr((NativeInt)&TformSpikeWare::SMPPostVSTProc) + ";";
      // attach pre-vst processing for recoring channels (clip detector)
      us += "extrecprevstproc="  + IntToStr((NativeInt)&TformSpikeWare::SMPRecPreVSTProc) + ";";
      // attach recording callback for trigger test and measurement
      us += "extdoneproc="  + IntToStr((NativeInt)&TformSpikeWare::SMPBufferDoneProc) + ";";
      // attach notify callback: SMP tells us, when a new stimulus is started
      us += "extdatanotify=" + IntToStr((NativeInt)&TformSpikeWare::SMPNotifyProc) + ";";
      us += "datanotifytrack=" + IntToStr(nNotifyChannel) + ";";
      if (!formSpikeWare->m_pIni->ReadBool("Debug", "RecSave", false))
         us += "recfiledisable=1;";
      if (formSpikeWare->m_pIni->ReadBool(formSpikeWare->m_pIni->ReadString("Debug", "Fake", "Fake"), "SoundCopyOutToIn", false))
         {
         MessageBox(0, "SoundCopyOutToIn ACTIVE", "Warning", MB_ICONWARNING);
         us += "copyout2in=1;";
         }
      us += "force=1;";
      us += "recprocesseddata=1;";
      if (!m_usLog.IsEmpty())
         us += "logfile=" + m_usLog + ";";

      bool bReturn = Command("init", us);

      // on success retrieve buffer size
      if (bReturn)
         {
         #ifdef CHKCHNLS
         if (m_nTriggerChannelInIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
            ShowMessage("error TI2 "+ UnicodeString( __FUNC__));
         if (m_nTriggerChannelInIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
            ShowMessage("error TI2 "+ UnicodeString( __FUNC__));
         #endif


         #ifdef CHKCHNLS
         formSpikeWare->m_sweEpoches.SetTriggerChannel((unsigned int)m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN));
         #endif

         AnsiString asReturn;
         bReturn = Command("getproperties", "", true, &asReturn);

         if (bReturn)
            m_nBufferSize = (unsigned int)StrToInt(GetStringValueFromSMPReturn(asReturn, "bufsize"));

         if (m_bShowMixer)
            Command("showmixer;topmost=1");

         if (formSpikeWare->m_pIni->ReadBool("Debug", "DebugSave", false))
            Command("debugsave;value=1;");


         // load equalisations
         UnicodeString usEqualisation;
         m_nEqualisationLength = -1;
         for (n = 0; n < viOutputTracks.size(); n++)
            {
            // load equalisation
            if (!VSTLoad(viOutputTracks[n], GetEqualisationN(n), PLUGIN_POS_EQ))
               return false;
            }

         // load bandpass for inputs. NOTE: vi contains all inputs including trigger and insitu.
         UnicodeString usFilter;
         for (n = 0; n < vi.size(); n++)
            {
            // NOTE: for trigger wo load plugin as well to keep channels aligned but
            // always use identity filter!
            if (vi[n] == nTriggerInChannel)
               usFilter = "";
            else
               {
               #ifdef CHKCHNLS
               if (m_swcHWChannels.GetChannelName((unsigned int)vi[n], SWSMPHWCDIR_IN) != m_pslChannelsIn->Strings[vi[n]])
                  ShowMessage("error F1 "+ UnicodeString( __FUNC__));
               #endif
               usFilter = m_swcHWChannels.GetChannelName((unsigned int)vi[n], SWSMPHWCDIR_IN) + " - BANDPASS";
               }
            // load bandpass
            if (!VSTLoad((int)n, usFilter, PLUGIN_POS_CUT, true))
               return false;
            }

         }

      if (nMode == AS_SMP_INIT_FREESEARCH)
         {
         #ifdef CHKCHNLS
         InitFreeSearch(viOutputTracks);
         #else
         InitFreeSearch();
         #endif
         }

      return bReturn;
      }
   catch(Exception &e)
      {
      formSpikeWare->SWErrorBox("Error initializing SMP for " + AS_NAME + ": " + e.Message);
      return false;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// initializes free search
//------------------------------------------------------------------------------
#ifdef CHKCHNLS
void SWSMP::InitFreeSearch(std::vector<int >& rviOutputTracks)
#else
void SWSMP::InitFreeSearch()
#endif
{
   m_nFreeSearchSamplesPlayed       = 0;

   float fSampleRate       = (float)formSpikeWare->m_swsStimuli.m_dDeviceSampleRate;
   m_fSineFreq             = 1000.0f;
   m_fSineFreqPending      = m_fSineFreq;
   m_nFreeSearchWindowPos  = -1;
   m_nFreeSearchSchroederPos = 0;
   m_nFreeSearchTriggerPos = -1;

   int nPreStimulus  = (int)(m_nFreeSearchPreStimLengthMs * fSampleRate / 1000.0f);
   int nSignalLength = (int)(m_nFreeSearchStimLengthMs * fSampleRate / 1000.0f);
   int nRampLength   = (int)(m_nFreeSearchRampLengthMs * fSampleRate / 1000.0f);
   if (nRampLength > nSignalLength/2)
      throw Exception("Free search ramp length must not exceed half of free search signal length");
   nSignalLength += nPreStimulus;
   m_nFreeSearchRepetitionPeriodSamples = MsToSamples(m_nFreeSearchRepetitionPeriodMs, formSpikeWare->m_swsStimuli.m_dDeviceSampleRate);

   if (nSignalLength > m_nFreeSearchRepetitionPeriodSamples)
      throw Exception("Free search stimulus length + pre-stimulus length must not exceed repetition period");

   m_vafFreeSearchWindow.resize((unsigned int)nSignalLength);
   m_vafFreeSearchWindow = 1.0f;

   float f;
   unsigned int n;
   for (n = 0; n < (unsigned int)nPreStimulus; n++)
      m_vafFreeSearchWindow[n] = 0.0f;
   for (n = 0; n < (unsigned int)nRampLength; n++)
      {
      f = GetHanningValue(n, (unsigned int)nRampLength);
      m_vafFreeSearchWindow[n+(unsigned int)nPreStimulus] = f;
      m_vafFreeSearchWindow[(unsigned int)nSignalLength-n] = f;
      }


   std::vector<int > viChannelsOutSettings = m_swcHWChannels.GetOutputs();

   #ifdef CHKCHNLS
   if (rviOutputTracks.size() != viChannelsOutSettings.size())
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   else
      {
      for (n = 0; n < viChannelsOutSettings.size(); n++)
         {
         if (m_viHardwareChannelsOutUsed[n] != viChannelsOutSettings[n])
            ShowMessage("error A2 "+ UnicodeString( __FUNC__));
         }
      }
   #endif

   // create Schroeder phase tone complex: here we use the highest LoFreq
   // of all used channels and the lowest HiFreq and nSingalLenth
   float fLo = -1.0f;
   float fHi = (float)(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate / 2.0);
   float fLoTmp, fHiTmp;
   for (n = 0; n < viChannelsOutSettings.size(); n++)
      {
      #ifdef CHKCHNLS
      if (m_swcHWChannels.GetChannelName((unsigned int)m_viHardwareChannelsOutUsed[n], SWSMPHWCDIR_OUT) != m_pslChannelsOut->Strings[m_viHardwareChannelsOutUsed[n]])
         ShowMessage("error B "+ UnicodeString( __FUNC__));
      if (m_swcHWChannels.GetChannelName((unsigned int)m_viHardwareChannelsOutUsed[n], SWSMPHWCDIR_OUT) != m_swcHWChannels.GetChannelName((unsigned int)viChannelsOutSettings[n], SWSMPHWCDIR_OUT))
         ShowMessage("error B2 "+ UnicodeString( __FUNC__));
      #endif

      formSpikeWare->m_swfFilters->GetChannelFreqs(
               m_swcHWChannels.GetChannelName((unsigned int)viChannelsOutSettings[n], SWSMPHWCDIR_OUT),
               fLoTmp,
               fHiTmp,
               (float)(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/2.0),
               formSpikeWare->IsInSitu()
               );
      if (fLo < fLoTmp)
         fLo = fLoTmp;
      if (fHi > fHiTmp)
         fHi = fHiTmp;
      }

   // pass schroeder phase
   if (formSpikeWare->FormsCreated())
      formSpikeWare->m_pformSearchFree->SetSchroederPhaseToneComplex(fLo, fHi, true, nSignalLength);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads an equalizer plugin visual for debugging) or non-visual 
//------------------------------------------------------------------------------
bool  SWSMP::VSTLoad(int nChannel, UnicodeString usEqualisation, int nPos, bool bInput, UnicodeString usPlugin)
{
   UnicodeString usEqName = m_bShowFFTPlugins ? "HtVstEqVisAS.dll" : "HtVstEqAS.dll";

   // if no plugin name passed used default FFT-Plugin
   if (usPlugin.IsEmpty())
      usPlugin = usEqName;

   // for output plugin: force loading the Hi-Pass-Filter
   if (!bInput && nPos == PLUGIN_POS_EQ)
      {
      if (!VSTLoad(nChannel, usEqualisation + " - HIPASS", PLUGIN_POS_CUT))
         throw Exception("Error loading Hi-Pass filter");

      // for debugging: load a "final" plugin, always as visual plugin
      /*
      UnicodeString us = "filename=" + IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "HtVstEqVisAS.dll;";
      us += "input=" + IntToStr(nChannel) + ";";
      us += "output=" + IntToStr(nChannel) + ";";
      us += "position=" + IntToStr(0) + ";";
      us += "type=final;";

      if (!Command("vstload", us))
         return false;

         us = "parameter=fftlen;";
         us += "input=" + IntToStr(nChannel) + ";";
         us += "value=" + DoubleToStr((Log2((double)m_nEqFFTLen)-8.0) / 8.0) + ";";
         us += "position=" + IntToStr(0) + ";";
         us += "type=final;";
         if (!Command("vstparam", us))
            return false;
      */
      }

   bool bFFT = (m_nEqualisationMethod == AW_SMP_EQ_FFT) || nPos == PLUGIN_POS_CUT;
   if (!bFFT)
      {
      if (usEqualisation.IsEmpty())
         return true;

      usPlugin = "HtVSTConv.dll";

      unsigned int nChannels, nSamples;
      double dSampleRate;
      usEqualisation = ExpandFileName(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "..\\equalisations\\" + usEqualisation);

      TSWStimuli::AudioFileProperties(usEqualisation, nChannels, nSamples, dSampleRate);
      if (nChannels != 1)
         throw Exception("Equalisation '" + usEqualisation + "' invalid: must have one audio channel");
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wfloat-equal"
      if (dSampleRate != formSpikeWare->m_swsStimuli.m_dDeviceSampleRate)
         throw Exception("Equalisation '" + usEqualisation + "' cannot be used with current samplerate (device: "
                        + DoubleToStr(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate) + ", equalisation: "
                        + DoubleToStr(dSampleRate) + ")");
      #pragma clang diagnostic pop
      if (m_nEqualisationLength < 0)
         m_nEqualisationLength = (int)nSamples;
      else if (!m_bAllowEqDiffLengths && m_nEqualisationLength != (int)nSamples)
         throw Exception("Equalisations with different lenghts must not be specified for simulatneously used output channels");
      }

   UnicodeString us = "filename=" + IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + usPlugin + ";";
   us += "input=" + IntToStr(nChannel) + ";";
   us += "output=" + IntToStr(nChannel) + ";";
   us += "position=" + IntToStr(nPos) + ";";
   if (bInput)
      us += "type=input;";

   if (!Command("vstload", us))
      return false;

   if (bFFT)
      {
      us = "parameter=fftlen;";
      us += "input=" + IntToStr(nChannel) + ";";
      us += "value=" + DoubleToStr((Log2((double)m_nEqFFTLen)-8.0) / 8.0) + ";";
      us += "position=" + IntToStr(nPos) + ";";
      if (bInput)
         us += "type=input;";
      if (!Command("vstparam", us))
         return false;
      }

   if (!usEqualisation.IsEmpty())
      {
      us = "programname=" + TformSpikeWare::GetSettingsPath() + "filters.ini##" + usEqualisation + ";";
      us += "input=" + IntToStr(nChannel) + ";";
      us += "position=" + IntToStr(nPos) + ";";

      if (bInput)
         us += "type=input;";
      // return
      if (!Command("vstprogramname", us))
         return false;
      }

   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads a stimulus (condition) to SMP using "loadmem"
//------------------------------------------------------------------------------
void SWSMP::LoadStim(int nStimInd, bool bFirstStim, int nLoopCount)
{
      #ifdef CHKCHNLS
      if (m_nTriggerChannelOutIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
         ShowMessage("error TO1 "+ UnicodeString( __FUNC__));
      #endif


   // prepend 2 seconds of silence (only applied for FirstStim - see below)
   int nStartOffset  = (int)floor(2.0*formSpikeWare->m_swsStimuli.m_dDeviceSampleRate);
   // if endless loop, we are in manual search mode: then only prepend 200 ms
   if (!nLoopCount)
      nStartOffset  = (int)floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate / 5.0);
   int nPreStimulus  = MsToSamples(1000.0*formSpikeWare->m_sweEpoches.m_dPreStimulus, formSpikeWare->m_swsStimuli.m_dDeviceSampleRate);
   // first load trigger.
   if (!Command(  "loadmem",
                  "track="       + IntToStr(m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
               +  ";offset="     + IntToStr(bFirstStim ? nStartOffset + m_nTriggerOffset : 0)
               +  ";data="       + IntToStr((NativeInt)&m_vadTrigger[0])
               +  ";loopcount="  + IntToStr(nLoopCount)
               +  ";samples="    + IntToStr((int)m_vadTrigger.size())
               +  ";channels=1"
               ))
      throw Exception("error loading trigger");


   // get reference to stimulus
   TSWStimulus &rstim = formSpikeWare->m_swsStimuli.m_swstStimuli[(unsigned int)nStimInd];

   // get pointer to audio data
   TSWAudioData* pAudioData = formSpikeWare->m_swsStimuli.GetAudioData(rstim.m_usFileName);
   if (!pAudioData)
      throw Exception("fatal error: audio data missing (1)");


   std::vector<int > viOutTrackIndices = m_swcUsedChannels.GetOutputs();
   #ifdef CHKCHNLS
   if (m_viOutTrackIndices.size() != viOutTrackIndices.size())
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   else
      {
      unsigned int x;
      for (x = 0; x < viOutTrackIndices.size(); x++)
         {
         if (m_viOutTrackIndices[x] != viOutTrackIndices[x])
            ShowMessage("error V "+ UnicodeString( __FUNC__));
         }
      }
   #endif

   vvd& rvvdData = pAudioData->m_vvdData;
   unsigned int nAudioDataChannels = (unsigned int)rvvdData.size();
   if (nAudioDataChannels != 1 && (unsigned int)nAudioDataChannels != viOutTrackIndices.size())
      throw Exception("fatal error: audio data channel number size error");

   // loop through output channels
   unsigned int n;
   double dRMS, dCal, dGain;

   // Apply debugging total offset if set in debug flags for stim only to
   // generate a latency between trigger and signal
   nStartOffset += m_nFakeTotalRecOffset;

   for (n = 0; n < viOutTrackIndices.size(); n++)
      {
      m_vadStimulus = 0.0;
      void* pData = nAudioDataChannels == 1 ? &rvvdData[0][0] : &rvvdData[n][0];
      dRMS = nAudioDataChannels == 1 ? pAudioData->m_vadRMS[0] : pAudioData->m_vadRMS[n];
      CopyMemory(&m_vadStimulus[(unsigned int)nPreStimulus], pData, rstim.m_nLength*sizeof(double));
      // Set gain for current level
      // - CalLevel is level if signal had 0 dB RMS AND if 0 dB (fullscale)
      // -> Gain = Level - CalLevel - RMS

      if (m_swcUsedChannels.IsOutputRaw(n))
         dGain = 1.0;
      else
         {
         dCal = GetCalibrationValueN(n);
         if (dCal == 0.0)
            throw Exception("Calibration value(s) missing, check settings (error 1)");
         dGain = dBToFactor(rstim.m_vdParams[(unsigned int)m_viGainIndices[n]] - dCal - dRMS);
         }
      if (formSpikeWare->m_bLevelDebug)
         {
         UnicodeString us;
         us.printf(L"PREPARE: %lf, %lf, %lf", rstim.m_vdParams[(unsigned int)m_viGainIndices[n]], FactorTodB(dGain), dRMS);
         OutputDebugStringW(us.w_str());
         }
      if (!Command( "loadmem",
                    "track="       + IntToStr(viOutTrackIndices[n])
                  + ";gain="       + DoubleToStr(dGain)
                  + ";offset="      + IntToStr(bFirstStim ? nStartOffset : 0)
                  + ";data="        + IntToStr((NativeInt)&m_vadStimulus[0])
                  + ";loopcount="   + IntToStr(nLoopCount)
                  + ";samples="     + IntToStr((int)m_vadStimulus.size())
                  + ";channels=1"
                  ))
         throw Exception("error loading signal");
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads a free search stimulus to SMP using "loadmem"
//------------------------------------------------------------------------------
void SWSMP::LoadFreeSearchStim()
{
   int nStartOffset        = (int)floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate );

   std::vector<int > viOutTrackIndices = m_swcUsedChannels.GetOutputs();
   #ifdef CHKCHNLS
   if (viOutTrackIndices.size() != m_viOutTrackIndices.size())
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   else
      {
      unsigned int x;
      for (x = 0; x < viOutTrackIndices.size(); x++)
         {
         if (viOutTrackIndices[x] != m_viOutTrackIndices[x])
            ShowMessage("error V "+ UnicodeString( __FUNC__));
         }
      }


   #endif

   // here we only load empty dummy stimulus, no trigger: trigger is written
   // in signal generator!!
   m_vadStimulus = 0.0;
   unsigned int n;
   for (n = 0; n < viOutTrackIndices.size(); n++)
      {
      if (!Command( "loadmem",
                    "track="       + IntToStr(viOutTrackIndices[n])
                  + ";offset="      + IntToStr(nStartOffset)
                  + ";data="        + IntToStr((NativeInt)&m_vadStimulus[0])
                  + ";loopcount="   + IntToStr(0)
                  + ";samples="     + IntToStr((int)m_vadStimulus.size())
                  + ";channels=1"
                  ))
         throw Exception("error loading signal");
      }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// loads ten more stimuli (if necessary)
//------------------------------------------------------------------------------
bool SWSMP::LoadStimuli()
{
   if (  !Initialized()
      || m_nNumLoadedStimuli == formSpikeWare->m_viStimSequence.size()
      )
      return true;

      #ifdef CHKCHNLS
      if (m_nTriggerChannelOutIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
         ShowMessage("error TO1 "+ UnicodeString( __FUNC__));
      if (m_nMonitorChannelOutIndex != m_swcUsedChannels.GetMonitor())
         ShowMessage("error TM1 "+ UnicodeString( __FUNC__));
      #endif

   // get current track load
   AnsiString asReturn;
   Command("trackload", "", true, &asReturn);
   ParseIntValues(m_viTrackLoad, GetStringValueFromSMPReturn(asReturn, "value"), "trackload", ',', false, false);
   if (m_viTrackLoad[(unsigned int)m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT)] > 9)
      return true;

   int n;
   for (n = 0; n < 10; n++)
      {
      if (formSpikeWare->m_viStimSequence.size() <= m_nNumLoadedStimuli)
         throw Exception("internal stimulus index error: " + IntToStr((int)formSpikeWare->m_viStimSequence.size()) + ", " + IntToStr((int)m_nNumLoadedStimuli));
      LoadStim(formSpikeWare->m_viStimSequence[m_nNumLoadedStimuli]);
      m_nNumLoadedStimuli++;
      if (m_nNumLoadedStimuli == formSpikeWare->m_viStimSequence.size())
         {
         // finally add a "zero trigger" with offset to be sure that wait does not return too early
         // (before we have recorded everything)!!
         m_vadStimulus = 0.0;
         if (!Command(  "loadmem",
                        "track="    + IntToStr(m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
                     +  ";offset="  + IntToStr((int)(2*m_vadStimulus.size()))
                     +  ";data="    + IntToStr((NativeInt)&m_vadStimulus[0])
                     +  ";samples=" + IntToStr((int)m_vadStimulus.size())
                     +  ";channels=1"
                     ))
            throw Exception("error loading final dummy trigger");

        return true;
        }
      }
   return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// prepares and loads a stimulus
//------------------------------------------------------------------------------
bool SWSMP::Prepare(int nStimIndex)
{
   try
      {
      bool bSearch      = nStimIndex != -1;
      bool bFreeSearch  = nStimIndex == -2;
      if (bFreeSearch)
         nStimIndex = 0;
      // calculate some values in samples
      int nRepetitionPeriod   = formSpikeWare->m_sweEpoches.m_nRepetitionPeriod * (int)formSpikeWare->m_swsSpikes.m_dSampleRateDevider;
      m_nTriggerOffset        = MsToSamples(1000.0*m_dTriggerLatency, formSpikeWare->m_swsStimuli.m_dDeviceSampleRate);

      // additional trigger offset for hipass: ALWAYS RUNNING
      m_nTriggerOffset += m_nEqFFTLen;
      // additional trigger offset, if FFT equalisation used as well
      if (m_nEqualisationMethod == AW_SMP_EQ_FFT)
         m_nTriggerOffset += m_nEqFFTLen;
                                                                                           
      int n;
      // create the trigger
      m_vadTrigger.resize((unsigned int)nRepetitionPeriod);
      m_vadTrigger = 0.0;

      // write trigger
      for (n = 0; n < m_nTriggerLength; n++)
         {
         m_vadTrigger[(unsigned int)n] = (double)m_fTriggerValue;
         // the first trigger is a special 'double-trigger'. Below
         // second pulse is removed again! Write only half size: we don't want
         // second pulse to be detected as the first one!
         m_vadTrigger[(unsigned int)(n+4*m_nTriggerLength)] = (double)m_fTriggerValue / 2.0;
         }

      // set correct size for stimulus buffer
      m_vadStimulus.resize((unsigned int)nRepetitionPeriod);


      #ifdef CHKCHNLS
      if (m_nTriggerChannelOutIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
         ShowMessage("error TO1 "+ UnicodeString( __FUNC__));
      if (m_nMonitorChannelOutIndex != m_swcUsedChannels.GetMonitor())
         ShowMessage("error TM1 "+ UnicodeString( __FUNC__));
      #endif


      int nNumOutCh = (int)m_swcUsedChannels.GetOutputs().size();
      #ifdef CHKCHNLS
      // create vector with all track indices used for signal output (not trigger)
      m_viOutTrackIndices.resize(0);

      // - determine total number of used output channels
      nNumOutCh = (int)m_viMeasChannelsOutUsed.size() + 1; // + 1 for trigger channel!
      if (m_nMonitorChannelOutIndex >= 0)
         nNumOutCh++;
      for (n = 0; n < nNumOutCh; n++)
         {
         if (n == m_nMonitorChannelOutIndex || n == m_nTriggerChannelOutIndex)
            continue;
         m_viOutTrackIndices.push_back(n);
         }
      nNumOutCh = (int)m_viOutTrackIndices.size();
      // build a vector with indices to ALL needed gains (might be the same index
      // for all channels...)
      if (  formSpikeWare->m_swsStimuli.m_nChannelLevels != 1
         && formSpikeWare->m_swsStimuli.m_nChannelLevels != m_viOutTrackIndices.size()
         )
         throw Exception("fatal error: internal gain/output channel sizing error");

      #endif


   #ifdef CHKCHNLS
   std::vector<int > vi = m_swcUsedChannels.GetOutputs();

   // NOTE: this is a bugfix: for freesearch m_viOutTrackIndices was calculated wrong!
   m_viOutTrackIndices = vi;
   if (m_viOutTrackIndices.size() != vi.size())
      ShowMessage("error A "+ UnicodeString( __FUNC__));
   else
      {
      unsigned int x;
      for (x = 0; x < vi.size(); x++)
         {
         if (m_viOutTrackIndices[x] != vi[x])
            ShowMessage("error V "+ UnicodeString( __FUNC__));
         }
      }
   #endif

      m_viGainIndices.resize(0);
      int nIndex;
      for (n = 0; n < nNumOutCh; n++)
         {
         if (formSpikeWare->m_swsStimuli.m_nChannelLevels == 1)
            nIndex = formSpikeWare->m_swsStimuli.m_swspStimPars.IndexFromName("Level_1");
         else
            nIndex = formSpikeWare->m_swsStimuli.m_swspStimPars.IndexFromName("Level_" + IntToStr(n+1));
         if (nIndex < 0)
            throw Exception("fatal error: 'Level_" + IntToStr(n+1) + "' missing in parameters");
         m_viGainIndices.push_back(nIndex);
         }

      // in free search we only have to load ONE dummy stimulus in endless loop
      if (bFreeSearch)
         {
         LoadFreeSearchStim();
         }
      // In (regular) search mode, load one stimulus in endless loop
      else if (bSearch)
         {
         LoadStim(nStimIndex, true, 0);
         }
      // regular measurement!
      else
         {
         m_nNumLoadedStimuli = (unsigned int)formSpikeWare->m_nStimPlayIndex;

         // load very first stimulus
         LoadStim(formSpikeWare->m_viStimSequence[m_nNumLoadedStimuli], true);
         m_nNumLoadedStimuli++;

         // remove second pulse (to be used for the very first trigger only)!!
         for (n = 0; n < m_nTriggerLength; n++)
            m_vadTrigger[(unsigned int)(n+4*m_nTriggerLength)] = 0.0;

         // Load Stimuli ONCE to pre-load 10 stimuli
         LoadStimuli();
         }
      if (m_bShowTracks)
         Command("showtracks");
      }
   catch (Exception &e)
      {
      Exit();
      formSpikeWare->SWErrorBox(e.Message);
      return false;
      }
   return true;
}
//------------------------------------------------------------------------------
              
//------------------------------------------------------------------------------
/// prepares trigger test loadeing a trigger to SmP using "loadmem"
//------------------------------------------------------------------------------
bool SWSMP::PrepareTriggerTest()
{
   try
      {

      // create the trigger
      m_vadTrigger.resize((unsigned int)floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate));
      m_vadTrigger = 0.0;
      m_nTriggerLength = (int)formSpikeWare->m_swsSpikes.m_dSampleRateDevider*4;


      unsigned int n;
      for (n = 0; n < (unsigned int)m_nTriggerLength; n++)
         m_vadTrigger[n] = (double)m_fTriggerValue;

      #ifdef CHKCHNLS
      if (m_nTriggerChannelOutIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
         ShowMessage("error TO1 "+ UnicodeString( __FUNC__));
      #endif

      // write trigger
      // load endless triggers
      if (!Command(  "loadmem",
                     "track="       + IntToStr(m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
                  +  ";offset="     + IntToStr(2*(int)m_vadTrigger.size())
                  +  ";data="       + IntToStr((NativeInt)&m_vadTrigger[0])
                  +  ";samples="    + IntToStr((int)m_vadTrigger.size())
                  +  ";loopcount=0"
                  +  ";channels=1"
                  ))
         throw Exception("error loading trigger for trigger test");

      if (m_bShowTracks)
         Command("showtracks");
      }
   catch (Exception &)
      {
      Exit();
      return false;
      }
   return true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// plays a stimulus looped
//------------------------------------------------------------------------------
bool SWSMP::PlayStimulusLooped(int nStimIndex)
{
   bool bPlaying = Playing();

   if (bPlaying)
      {
      if (!Command("pause", "value=1"))
         return false;
      Sleep(10);
      if (!Command("cleardata"))
         return false;
      formSpikeWare->m_sweEpoches.Reset();
      }

   if (!Prepare(nStimIndex))
      return false;

   bool b = false;
   if (bPlaying)
      b = Command("pause", "value=0");
   else
      b = Start();

   return b;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets monitor channel
//------------------------------------------------------------------------------
bool SWSMP::SetMonitor(int nChannel)
{
   try
      {
      // build vector for first resetting iostatus for ALL inputs
      UnicodeString us;
      // first reset iostatus for ALL inputs
      if (!Command("iostatus", "track=-1"))
         throw Exception("Error clearing iostatus: " + m_usLastError);

      // only switch off? then return
      if (nChannel < 0)
         return true;

      #ifdef CHKCHNLS
      if (m_nTriggerChannelInIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
         ShowMessage("error TI1 "+ UnicodeString( __FUNC__));
      if (m_nMonitorChannelOutIndex != m_swcUsedChannels.GetMonitor())
         ShowMessage("error M1 "+ UnicodeString( __FUNC__));
      #endif


      if (nChannel >= m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_IN))
         nChannel++;
      if (!Command("iostatus", "input=" + IntToStr(nChannel) + ";track=" + IntToStr((int)m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))))
         throw Exception("Error setting iostatus: " + m_usLastError);
      return true;
      }
   catch(Exception &e)
      {
      formSpikeWare->SWErrorBox(e.Message);
      return false;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads audio settings from inifile
//------------------------------------------------------------------------------
bool SWSMP::ReadSettings(bool bShowError, bool bForce)
{

   if (!bForce && m_bSettingsRead)
      return true;

   bool bSettingsValid = true;
   AnsiString asReturn;
   if (!Command("getdrivers", "", true, &asReturn))
      return false;
   try
      {
      m_usLog                 = formSpikeWare->m_pIni->ReadString("Debug", "Logfile", "");
      m_bShowFFTPlugins       = formSpikeWare->m_pIni->ReadBool("Debug", "ShowFFTPlugins", false);
      m_bFreeSearchContinuous = formSpikeWare->m_pIni->ReadBool("Debug", "FreeSearchContinuous", false);
      m_bShowTracks = formSpikeWare->m_pIni->ReadBool("Debug", "ShowTracks", false);
      m_bShowMixer  = formSpikeWare->m_pIni->ReadBool("Debug", "ShowMixer", false);
      m_usIniSection = formSpikeWare->m_pIni->ReadString(formSpikeWare->m_pIni->ReadString("Debug", "Fake", "Fake"), "SoundSettings", "SoundSettings");
      m_nFakeTotalRecOffset   = formSpikeWare->m_pIni->ReadInteger(formSpikeWare->m_pIni->ReadString("Debug", "Fake", "Fake"), "TotalRecOffset", 0);

      m_bSaveProbeMics                 = formSpikeWare->m_pIni->ReadBool("Settings", "SaveProbeMic", false);
      m_fDefaultSampleRate             = formSpikeWare->m_pIni->ReadInteger("Settings", "SampleRateDefault", 44100);
      m_fDefaultSampleRateDevider      = formSpikeWare->m_pIni->ReadInteger("Settings", "SampleRateDeviderDefault", 1.0);
      m_nFreeSearchStimLengthMs        = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchStimLengthMs", 150);
      m_nFreeSearchPreStimLengthMs     = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchPreStimLengthMs", 20);
      m_nFreeSearchRepetitionPeriodMs  = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchRepetitionPeriodMs", 350);
      m_nFreeSearchRampLengthMs        = formSpikeWare->m_pIni->ReadInteger("Settings", "FreeSearchRampLengthMs", 5);
      if (  m_nFreeSearchStimLengthMs        <= 0
         || m_nFreeSearchPreStimLengthMs     <  0
         || m_nFreeSearchRepetitionPeriodMs  <= 0
         || m_nFreeSearchRampLengthMs        <  0
         )
         throw Exception("Free search parameters invalid (<= 0)");

      m_fTriggerValue                  = 1.0f;
      int nTriggerAtt                  = formSpikeWare->m_pIni->ReadInteger("Settings", "TriggerAttenuation", 0);
      if (nTriggerAtt < 0)
         m_fTriggerValue               = (float)dBToFactor(nTriggerAtt);

      m_bAllowEqDiffLengths            = formSpikeWare->m_pIni->ReadBool("Settings", "AllowEqDiffLengths", false);
      if (formSpikeWare->IsInSitu())
         m_nEqualisationMethod         = AW_SMP_EQ_FFT;
      else
         m_nEqualisationMethod         = formSpikeWare->m_pIni->ReadInteger("Settings", "EqualisationMethod", AW_SMP_EQ_FFT);

      m_nEqFFTLen                      = formSpikeWare->m_pIni->ReadInteger("Settings", "FFTLen", FFTLEN_DEFAULT);

      m_dTriggerLatency = IniReadDouble(formSpikeWare->m_pIni, "Settings", "TriggerLatency", 0.0);


      ParseValues(m_pslDrivers, GetStringValueFromSMPReturn(asReturn, "driver"));

      if (!m_pslDrivers->Count)
         throw Exception("No ASIO sound drivers found in the system");

      UnicodeString usDriver = formSpikeWare->m_pIni->ReadString(m_usIniSection, "Driver", "");
      if (usDriver == "")
         throw Exception("No sound driver selected in settings");
      if (m_pslDrivers->IndexOf(usDriver) < 0)
         throw Exception("Selected sound driver not found in the system");

      

      // note: SetDriver reinitializes ALL Channels thus resets all channel types as well
      SetDriver(usDriver);

      #ifdef CHKCHNLS
      if (!GetChannels(m_usDriver, m_pslChannelsIn, m_pslChannelsOut))
         return false;
      #endif

      if (!m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT) || !m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_IN))
         throw Exception("Selected sound driver has no audio channels. Maybe device is not connected or switched off.");



      // from here on we want to (try to) continue if NO error to be displayed: used
      // when settings are invoked: we want to read as much as possible, even if some settings
      // are invalid. To be sure we return false in this case....

      #define THROWCOND(us) \
         { \
         bSettingsValid = false;\
         if (bShowError) throw Exception(us);\
         }

      // read out channels:
      std::vector<int > viChannelsOutSettings;
      UnicodeString usChannelsOutField = formSpikeWare->IsInSitu() ? "ChannelsOutInSitu" : "ChannelsOut";
      ParseIntValues(viChannelsOutSettings, formSpikeWare->m_pIni->ReadString(m_usIniSection, usChannelsOutField, ""), usChannelsOutField);
      if (!viChannelsOutSettings.size())
         THROWCOND("No output channels set in settings");
      m_swcHWChannels.SetOutputs(viChannelsOutSettings);

      std::vector<int > viChannelsOutRawSettings;
      UnicodeString usChannelsOutRawField = formSpikeWare->IsInSitu() ? "ChannelsOutRawInSitu" : "ChannelsOutRaw";
      ParseIntValues(viChannelsOutRawSettings, formSpikeWare->m_pIni->ReadString(m_usIniSection, usChannelsOutRawField, ""), usChannelsOutRawField);
      m_swcHWChannels.SetOutputsRaw(viChannelsOutRawSettings);


      std::vector<int > viChannelsInSettings;
      ParseIntValues(viChannelsInSettings, formSpikeWare->m_pIni->ReadString(m_usIniSection, "ChannelsIn", ""), "ChannelsIn");
      if (!viChannelsInSettings.size())
         THROWCOND("No input channels set in settings");
      m_swcHWChannels.SetElectrodes(viChannelsInSettings);

      int nTriggerChannelOut = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "TriggerOut", -1);
      if (nTriggerChannelOut < 0)
         THROWCOND("No trigger out channel set in settings");
      if (m_swcHWChannels.GetChannelType((unsigned int)nTriggerChannelOut, SWSMPHWCDIR_OUT) != AS_SMP_NONE)
         THROWCOND("Invalid trigger out channel set in settings: channel is used as output channel as well");
      m_swcHWChannels.SetTrigger(nTriggerChannelOut, SWSMPHWCDIR_OUT);



      int nMonitorChannelOut = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "MonitorOut", -1);
      if (nMonitorChannelOut >= 0 &&m_swcHWChannels.GetChannelType((unsigned int)nMonitorChannelOut, SWSMPHWCDIR_OUT) != AS_SMP_NONE)
         THROWCOND("Invalid monitor channel set in settings: channel is used as output channel or trigger output as well");
      m_swcHWChannels.SetMonitor(nMonitorChannelOut);


      int nTriggerChannelIn  = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "TriggerIn", -1);
      if (nTriggerChannelIn < 0)
         THROWCOND("No trigger in channel set in settings");
      if (m_swcHWChannels.GetChannelType((unsigned int)nTriggerChannelIn, SWSMPHWCDIR_IN) != AS_SMP_NONE)
         THROWCOND("Invalid trigger in channel set in settings: channel is used as electrode channel as well");
      m_swcHWChannels.SetTrigger(nTriggerChannelIn, SWSMPHWCDIR_IN);

      int nMicChannelIn      = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "MicIn", -1);
      if (nMicChannelIn >= 0 && m_swcHWChannels.GetChannelType((unsigned int)nMicChannelIn, SWSMPHWCDIR_IN) != AS_SMP_NONE)
         THROWCOND("Invalid reference microphone channel set in settings: channel is used as electrode channel or trigger in as well");
      m_swcHWChannels.SetRefMic(nMicChannelIn);





      #ifdef CHKCHNLS
      // Compare AAAALLLL channels old vs new
      m_nTriggerChannelIn  = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "TriggerIn", -1);
      m_nTriggerChannelOut = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "TriggerOut", -1);
      m_nMonitorChannelOut = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "MonitorOut", -1);
      m_nMicChannelIn      = formSpikeWare->m_pIni->ReadInteger(m_usIniSection, "MicIn", -1);

      ParseIntValues(m_viChannelsOutSettings, formSpikeWare->m_pIni->ReadString(m_usIniSection, usChannelsOutField, ""), usChannelsOutField);
      ParseIntValues(m_viChannelsInSettings, formSpikeWare->m_pIni->ReadString(m_usIniSection, "ChannelsIn", ""), "ChannelsIn");

      if (m_swcHWChannels.GetTrigger(SWSMPHWCDIR_IN) != m_nTriggerChannelIn)
         ShowMessage("error A "+ UnicodeString( __FUNC__));
      if (m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT) != m_nTriggerChannelOut)
         ShowMessage("error B "+ UnicodeString( __FUNC__));
      if (m_swcHWChannels.GetMonitor() != m_nMonitorChannelOut)
         ShowMessage("error C "+ UnicodeString( __FUNC__));
      if (m_swcHWChannels.GetRefMic() != m_nMicChannelIn)
         ShowMessage("error D "+ UnicodeString( __FUNC__));

      std::vector<int > viChannelsOutSettings2 = m_swcHWChannels.GetOutputs();
      if (viChannelsOutSettings2.size() != m_viChannelsOutSettings.size())
         ShowMessage("error O1 "+ UnicodeString( __FUNC__));
      else
         {
         for (unsigned int x = 0; x < viChannelsOutSettings2.size();x++)
            {
            if (viChannelsOutSettings2[x] !=  m_viChannelsOutSettings[x])
               ShowMessage("error O2 "+ UnicodeString( __FUNC__));
            }
         }
      std::vector<int > viChannelsInSettings2 = m_swcHWChannels.GetElectrodes();
      if (viChannelsInSettings2.size() != m_viChannelsInSettings.size())
         ShowMessage("error I1 "+ UnicodeString( __FUNC__));
      else
         {
         for (unsigned int x = 0; x < viChannelsInSettings2.size();x++)
            {
            if (viChannelsInSettings2[x] !=  m_viChannelsInSettings[x])
               ShowMessage("error I2 "+ UnicodeString( __FUNC__));
            }
         }

      #endif


      // for insitu: check, that an insitu channel is available for every selected output!
      if (formSpikeWare->IsInSitu() && viChannelsOutSettings.size())
         {

         
         std::vector<int > vi;
         unsigned int n, m, nChannel;
         int nChannelInSitu;
         for (n = 0; n < viChannelsOutSettings.size(); n++)
            {
            // for raw output channel no probe mic is required
            if (m_swcHWChannels.IsOutputRaw(n))
               continue;

            

            nChannelInSitu = GetInSituInputChannel(m_swcHWChannels.GetChannelName((unsigned int)viChannelsOutSettings[n], SWSMPHWCDIR_OUT));
            if (nChannelInSitu == -1)
               THROWCOND("No input channel set in settings for at least one in-situ output channel!");
            nChannel = m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_IN][(unsigned int)nChannelInSitu].m_nHWIndex;

            if ( std::find(vi.begin(), vi.end(), nChannel) != vi.end())
               THROWCOND("Identical in-situ input channel used for multiple outputs in settings");

            #ifdef CHKCHNLS
//            if (bShowError && m_swcHWChannels.GetChannelName((unsigned int)viChannelsOutSettings[n], SWSMPHWCDIR_OUT) != m_pslChannelsIn->Strings[m_viChannelsOutSettings[n]])
//               ShowMessage("error insitu "+ UnicodeString( __FUNC__));
            #endif

            #ifdef CHKCHNLS
            /*
            ShowMessage(GetInSituInputChannel(m_swcHWChannels.GetChannelName((unsigned int)viChannelsOutSettings[n], SWSMPHWCDIR_OUT)));
            ShowMessage(m_pslChannelsIn->Strings[m_viChannelsOutSettings[n]]);
            if (GetInSituInputChannel(m_swcHWChannels.GetChannelName((unsigned int)viChannelsOutSettings[n], SWSMPHWCDIR_OUT)) != m_pslChannelsIn->Strings[m_viChannelsOutSettings[n]])
               {
               UnicodeString usError;
//               usError.printf(L"error insitu %hs\n%s\n%s", __FUNC__,
//                  GetInSituInputChannel(m_swcHWChannels.GetChannelName((unsigned int)viChannelsOutSettings[n], SWSMPHWCDIR_OUT)).w_str(),
//                  m_pslChannelsIn->Strings[m_viChannelsOutSettings[n]].w_str());
                                    
               ShowMessage("error a");
               }
            */
            #endif

            vi.push_back((int)nChannel);
            }
         m_swcHWChannels.SetProbeMics(vi);
         }


      if  (!bShowError)
         return bSettingsValid;
      }
   catch (Exception &e)
      {
      if (bShowError)
         formSpikeWare->SWErrorBox(e.Message);
      else
         OutputDebugStringW(e.Message.w_str());
      return false;
      }
   m_bSettingsRead = true;
   return true;

   #undef THROWCOND
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// writes audio settings to inifile
//------------------------------------------------------------------------------
void  SWSMP::WriteSettings()
{
   formSpikeWare->m_pIni->WriteString(m_usIniSection, "Driver", m_usDriver);

   UnicodeString us, usRaw;
   unsigned int n;
   for (n = 0; n < m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_OUT); n++)
      {
      if (m_swcHWChannels.IsOutput(n))
         {
         us += IntToStr((int)n) + ",";
         if (m_swcHWChannels.IsOutputRaw(n))
            usRaw += IntToStr((int)n) + ",";
         }
      }
   RemoveTrailingDelimiter(us, L',');
   RemoveTrailingDelimiter(usRaw, L',');

   #ifdef CHKCHNLS
   UnicodeString us2;

   for (n = 0; n < m_viChannelsOutSettings.size(); n++)
      us2 += IntToStr(m_viChannelsOutSettings[n]) + ",";
   RemoveTrailingDelimiter(us2, L',');

   if (us != us2)
      {
      ShowMessage("error A "+ UnicodeString( __FUNC__));
      }
   #endif


   UnicodeString usChannelsOutField = formSpikeWare->IsInSitu() ? "ChannelsOutInSitu" : "ChannelsOut";
   formSpikeWare->m_pIni->WriteString(m_usIniSection, usChannelsOutField, us);
   usChannelsOutField = formSpikeWare->IsInSitu() ? "ChannelsOutRawInSitu" : "ChannelsOutRaw";
   formSpikeWare->m_pIni->WriteString(m_usIniSection, usChannelsOutField, usRaw);

   us = "";
   for (n = 0; n < m_swcHWChannels.GetNumChannels(SWSMPHWCDIR_IN); n++)
      {
      if (m_swcHWChannels.IsElectrode(n))
         us += IntToStr((int)n) + ",";
      }

   RemoveTrailingDelimiter(us, L',');


   formSpikeWare->m_pIni->WriteString(m_usIniSection, "ChannelsIn", us);

   formSpikeWare->m_pIni->WriteInteger(m_usIniSection, "TriggerIn", m_swcHWChannels.GetTrigger(SWSMPHWCDIR_IN));
   formSpikeWare->m_pIni->WriteInteger(m_usIniSection, "TriggerOut", m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT));
   formSpikeWare->m_pIni->WriteInteger(m_usIniSection, "MonitorOut", m_swcHWChannels.GetMonitor());
   formSpikeWare->m_pIni->WriteInteger(m_usIniSection, "MicIn", m_swcHWChannels.GetRefMic());


   #ifdef CHKCHNLS
   us2 = "";
   for (n = 0; n < m_viChannelsInSettings.size(); n++)
      us2 += IntToStr(m_viChannelsInSettings[n]) + ",";

   RemoveTrailingDelimiter(us2, L',');

   if (m_swcHWChannels.GetTrigger(SWSMPHWCDIR_IN) != m_nTriggerChannelIn)
      ShowMessage("error B "+ UnicodeString( __FUNC__));

   if (m_swcHWChannels.GetTrigger(SWSMPHWCDIR_OUT) != m_nTriggerChannelOut)
      ShowMessage("error C "+ UnicodeString( __FUNC__));
   if (us != us2)
      ShowMessage("error D "+ UnicodeString( __FUNC__));
   if (m_swcHWChannels.GetMonitor() != m_nMonitorChannelOut)
      ShowMessage("error E "+ UnicodeString( __FUNC__));
   if (m_swcHWChannels.GetRefMic() != m_nMicChannelIn)
      ShowMessage("error F "+ UnicodeString( __FUNC__));
   #endif
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// free search signal generator: realtime callback wriing a sine to output
/// buffers
//------------------------------------------------------------------------------
void SWSMP::SoundFreeSearchSignalGenerator(vvf &vvfBuffers)
{
   if (m_bStopping)
      return;
   EnterCriticalSection(&m_cs);
   unsigned int nBufSize = 0;
   try
      {
      unsigned int nCh = (unsigned int)vvfBuffers.size();
      if (!nCh)
         return;


      #ifdef CHKCHNLS
      if (m_nTriggerChannelOutIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
         ShowMessage("error TO1 "+ UnicodeString( __FUNC__));
      if (m_nMonitorChannelOutIndex != m_swcUsedChannels.GetMonitor())
         ShowMessage("error TM1 "+ UnicodeString( __FUNC__));

      UnicodeString us1, us2;
      int nChannelTmp;
      
      for (nChannelTmp = 0; nChannelTmp < (int)nCh; nChannelTmp++)
         {
         if (m_swcUsedChannels.IsOutput((unsigned int)nChannelTmp))
            us1 += IntToStr(nChannelTmp) + ",";
         if (nChannelTmp == m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT) || nChannelTmp == m_swcUsedChannels.GetMonitor())
            continue;
         us2 += IntToStr(nChannelTmp) + ",";
         }

      if (us1 != us2)
         ShowMessage("error CH1 "+ UnicodeString( __FUNC__));
      #endif

      unsigned int nChannel;
      // clear channels
      for (nChannel = 0; nChannel < nCh; nChannel++)
         {
         if (m_swcUsedChannels.IsOutput(nChannel))
            vvfBuffers[nChannel] = 0.0f;
         }

      nBufSize = (unsigned int)vvfBuffers[0].size();
      // do nothing within first second
      if (m_nFreeSearchSamplesPlayed < floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate))
         return;

      unsigned int nPosition = m_nFreeSearchSamplesPlayed;
      unsigned int n;
      unsigned int nSignalStartPos = 0;
      unsigned int nTriggerStartPos = 0;

      for (n = 0; n < nBufSize; n++)
         {
         // check, if to set position info for signal (only if not continuous debug-playback)
         if (!m_bFreeSearchContinuous)
            {
            if (  m_nFreeSearchWindowPos == -1
               && ((int)nPosition % m_nFreeSearchRepetitionPeriodSamples ) == 0
               )
               {
               m_nFreeSearchWindowPos = 0;
               m_fSineFreq = m_fSineFreqPending;
               nSignalStartPos = n;
               }
            }
         // check, if to set position info for trigger
         if (  m_nFreeSearchTriggerPos == -1
            && (int)nPosition > m_nTriggerOffset
            && (((int)nPosition - m_nTriggerOffset) % m_nFreeSearchRepetitionPeriodSamples ) == 0
            )
            {
            m_nFreeSearchTriggerPos = 0;
            nTriggerStartPos = n;
            }
         nPosition++;
         }

      // write trigger
      if (m_nFreeSearchTriggerPos >= 0)
         {
         for (n = nTriggerStartPos; n < nBufSize; n++)
            {
            vvfBuffers[(unsigned int)m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT)][n] = 1.0;
            m_nFreeSearchTriggerPos++;
            if (m_nFreeSearchTriggerPos >= m_nTriggerLength)
               {
               m_nFreeSearchTriggerPos = -1;
               break;
               }
            }
         }


      // write signal
      if (m_nFreeSearchWindowPos >= 0 || m_bFreeSearchContinuous)
         {
         // set fix continuous debug-playback values
         if (m_bFreeSearchContinuous)
            {
            nSignalStartPos   = 0;
            m_fSineFreq       = m_fSineFreqPending;
            }

         float fFreq = m_fSineFreq;
         float fValue      = 0.0f;
         float fFactor     = 1.0f;
         float fPhaseInc   = 1.0f;
         if (fFreq > 0.0f)
            fPhaseInc = fFreq / (float)formSpikeWare->m_swsStimuli.m_dDeviceSampleRate;

         for (n = nSignalStartPos; n < nBufSize; n++)
            {
            // get window factor if not continous debug-playback
            if (!m_bFreeSearchContinuous)
               fFactor = m_vafFreeSearchWindow[(unsigned int)m_nFreeSearchWindowPos];

            // calculate same sine value for ALL channels
            if ( fFreq > 0.0f)
               fValue = (float)(sin(2*M_PI*(double)(m_nFreeSearchWindowPos*fPhaseInc)));

            for (nChannel = 0; nChannel < nCh; nChannel++)
               {
               if (!m_swcUsedChannels.IsOutput(nChannel))
                  continue;

               // calculate different noise value for each channel (decorrelate)
               if (fFreq <= 0.0f)
                  // vvfBuffers[nChannel][n] = fFactor * (float)(random(USHRT_MAX) + SHRT_MIN) / (float)(-SHRT_MIN);
                  vvfBuffers[nChannel][n] = fFactor * m_vfSchroeder[m_nFreeSearchSchroederPos];
               else
                  vvfBuffers[nChannel][n] = fFactor * fValue;
               }

            m_nFreeSearchWindowPos++;
            m_nFreeSearchSchroederPos++;
            if (m_nFreeSearchSchroederPos >= m_vfSchroeder.size())
               m_nFreeSearchSchroederPos = 0;

            // reset window if not continous debug-playback
            if (!m_bFreeSearchContinuous && m_nFreeSearchWindowPos >= (int)m_vafFreeSearchWindow.size())
               {
               m_nFreeSearchWindowPos = -1;
               break;
               }
            }
         }

      }
   __finally
      {
      m_nFreeSearchSamplesPlayed += nBufSize;
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// realtime callback as clip-detector
//------------------------------------------------------------------------------
void SWSMP::SoundClipDetector(vvf &vvfBuffers)
{
   if (m_bStopping)
      return;
   EnterCriticalSection(&m_cs);
   try
      {
      #ifdef CHKCHNLS
      if (formSpikeWare->m_bFreeSearchRunning)
         {
         try
            {

            if (m_nTriggerChannelOutIndex != m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT))
               throw Exception("error TO1 "+ UnicodeString( __FUNC__));
            if (m_nMonitorChannelOutIndex != m_swcUsedChannels.GetMonitor())
               throw Exception("error TM1 "+ UnicodeString( __FUNC__));

            UnicodeString us1, us2;
            int nChannel;
            for (nChannel = 0; nChannel < (int)vvfBuffers.size(); nChannel++)
               {
               if (m_swcUsedChannels.IsOutput((unsigned int)nChannel))
                  us1 += IntToStr(nChannel) + ",";
               if (nChannel == m_swcUsedChannels.GetTrigger(SWSMPHWCDIR_OUT) || nChannel == m_swcUsedChannels.GetMonitor())
                  continue;
               us2 += IntToStr(nChannel) + ",";
               }

            if (us1 != us2)
               throw Exception("error CH1 "+ UnicodeString( __FUNC__));
            }
         catch (Exception &e)
            {
            OutputDebugStringW(e.Message.w_str());
            return;
            }
         }
      #endif

      unsigned int nNumChannels = (unsigned int)vvfBuffers.size();
      unsigned int n;
      float fMax;
      for (n = 0; n < nNumChannels; n++)
         {
         if (formSpikeWare->m_bFreeSearchRunning)
            {
            if (!m_swcUsedChannels.IsOutput(n))
               continue;
            }
         fMax = fabs(vvfBuffers[n].max());
         if (fMax < 1.0f)
            fMax = fabs(vvfBuffers[n].min());
         if (fMax > 1.0f)
            {
            // send message to free search or calibration window: will indicate clipping!
            if (formSpikeWare->m_bFreeSearchRunning && formSpikeWare->FormsCreated())
               formSpikeWare->m_pformSearchFree->ClipTimer->Tag = 1;
            else
               formCalibration->ClipTimer->Tag = 1;
            break;
            }
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Processing callback in maximum search mode: search maximum in each channel
/// and store it if > total maximum in channel up to now
//------------------------------------------------------------------------------
void SWSMP::SoundMaxSearchProc(vvf &vvfBuffers)
{
   if (m_bStopping)
      return;
   unsigned int nNumChannels = (unsigned int)vvfBuffers.size();
   if (nNumChannels != m_vadMaxSearch.size())
      throw Exception("unexpected sizing error in MaxSearch");
   EnterCriticalSection(&m_cs);
   try
      {
      unsigned int n;
      double dMin, dMax;
      for (n = 0; n < nNumChannels; n++)
         {
         dMin = (double)fabs(vvfBuffers[n].min());
         dMax = (double)fabs(vvfBuffers[n].max());
         if (dMin > dMax)
            dMax = dMin;
         if (dMax > m_vadMaxSearch[n])
            m_vadMaxSearch[n] = dMax;
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Runs maximum search
//------------------------------------------------------------------------------
bool SWSMP::MaxSearch()
{
   bool bReturn = false;
   if (!ReadSettings())
      return false;

   try
      {
      try
         {
         if (!Exit())
            return false;

         formWait->ShowWait("Calculating available levels, this may take a while ...");
         unsigned int nNumOutCh = (unsigned int)m_viMeasChannelsOutUsed.size();
         m_vadMaxSearch.resize(nNumOutCh);


         m_vadMaxSearch = 0.0;
         m_vadMaxLevelsAvailable.resize(nNumOutCh);
         m_nBufferSize = 1024;


         UnicodeString us = "driver=" + m_usDriver + ";";
         us += "output=" + IntToStr((int)nNumOutCh) + ";";
         us += "ramplen=0;";
         us += "samplerate=" + DoubleToStr(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate) + ";";
         us += "file2file=1;";
         us += "f2fbufsize=" + IntToStr((int)m_nBufferSize) + ";";
         // attach post-vst processing (not used here, but required to check 'compatibility' of special SMP)
         us += "extprevstproc="  + IntToStr((int)&TformSpikeWare::SMPPreVSTProc) + ";";
         // attach post-vst processing callback: we want to check peak AFTER equalisation
         us += "extpostvstproc="  + IntToStr((int)&TformSpikeWare::SMPPostVSTProcMaxSearch) + ";";
         us += "force=1;";
         us += "recprocesseddata=1;";
         us += "autocleardata=1;";
         if (!m_usLog.IsEmpty())
            us += "logfile=" + m_usLog + ";";

         if (!Command("init", us))
            return false;


         unsigned int nChannel;
         UnicodeString usEqualisation;
         m_nEqualisationLength = -1;
         for (nChannel = 0; nChannel < nNumOutCh; nChannel++)
            {
            if (!VSTLoad((int)nChannel, GetEqualisationN(nChannel), PLUGIN_POS_EQ))
               return false;
            }

         if (!Command("debugsave;value=0;"))
            return false;

         unsigned int nNumAudio = (unsigned int)formSpikeWare->m_swsStimuli.m_vSWAudioData.size();
         unsigned int n, nAudio, nAudioDataChannels, nAudioIndex;
         double dGain;
         for (nAudio = 0; nAudio < nNumAudio; nAudio++)
            {
            TSWAudioData &rstim = formSpikeWare->m_swsStimuli.m_vSWAudioData[nAudio];
            nAudioDataChannels = (unsigned int)rstim.m_vvdData.size();
            if (nAudioDataChannels != 1 && nAudioDataChannels != nNumOutCh)
               throw Exception("fatal error: audio data channel number size error");
            // loop through output channels
            for (n = 0; n < nNumOutCh; n++)
               {
               nAudioIndex = nAudioDataChannels == 1 ? 0 : n;

               // set gain: ALL signals are scale by their RMS. This way ALL
               // signals are scaled identical with respect to 'level'
               dGain = 1.0 / dBToFactor(rstim.m_vadRMS[nAudioIndex]);
               if (!Command( "loadmem",
                             "track="       + IntToStr((int)n)
                           + ";gain="       + DoubleToStr(dGain)
                           + ";offset=1000"
                           + ";data="        + IntToStr((NativeInt)&rstim.m_vvdData[nAudioIndex][0])
                           + ";samples="     + IntToStr((int)rstim.m_vvdData[nAudioIndex].size())
                           + ";channels=1"
                           ))
                  throw Exception("error loading signal");
               }
            }

            // append one zero on first track with 1 seconds offset to be sure
            // everything is procssed
            double d = 0.0;
            if (!Command( "loadmem",
                        "data="        + IntToStr((NativeInt)&d)
                        +  ";track=0"
                        + ";offset=" + IntToStr((int)floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate))
                        + ";samples=1"
                        + ";channels=1"
                        ))
               throw Exception("error loading ZERO signal");

         if (!Command("start"))
            throw Exception("error running SMP for calculating peaks");

         // now we know by how many dB's a channel would clip at 0dB gain, if a signal had an RMS of 0dB RMS
         // so we can calculate the total maximum available level from calvalue
         double dCal;
         for (n = 0; n < nNumOutCh; n++)
            {
            if (m_swcUsedChannels.IsOutputRaw(n))
               {
               m_vadMaxLevelsAvailable[n] = 1000.0;
               }
            else
               {
               dCal = GetCalibrationValueN(n);
               if (dCal == 0.0)
                  throw Exception("Calibration value(s) missing, check settings settings (error 2)");
               // 0 dB gain would be exactly calvalue, so we subtract the peak from max-search
               m_vadMaxLevelsAvailable[n] = dCal - FactorTodB(m_vadMaxSearch[n]);
               if (formSpikeWare->m_bLevelDebug)
                  {
                  us.printf(L"MaxLevel: %d: %lf", n, m_vadMaxLevelsAvailable[n]);
                  OutputDebugStringW(us.w_str());
                  }
               }
            }

         bReturn = true;
         }
      __finally
         {
         formWait->Hide();
         Exit();
         }
      }
   catch(Exception &e)
      {
      formSpikeWare->SWErrorBox("Error initializing MaxSearch SMP for " + AS_NAME + ": " + e.Message);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// real time callback for calibration writing schroeder tone complex buffer by
/// buffer to outut channels
//------------------------------------------------------------------------------
void SWSMP::SoundCalibrationSignalGenerator(vvf &vvfBuffers)
{
   if (m_bStopping || !vvfBuffers.size())
      return;
   EnterCriticalSection(&m_cs);
   try
      {
      // write schroeder tone complex
      unsigned int n;
      for (n = 0; n < vvfBuffers[0].size(); n++)
         {
         vvfBuffers[0][n] = m_vfSchroeder[m_nCalSignalPos++];
         if (m_nCalSignalPos >= m_vfSchroeder.size())
            m_nCalSignalPos = 0;
         }
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// initializes calibratuion procedure
//------------------------------------------------------------------------------
bool SWSMP::InitCalibration(int nOutChannel, bool bUseRefMic)
{

   if (!ReadSettings())
      return false;

   try
      {
      if (!Exit())
         return false;

      UnicodeString usOutChannel = m_swcHWChannels.GetChannelName((unsigned int)nOutChannel, SWSMPHWCDIR_OUT);

      m_nCalSignalPos = 0;

      m_nCalibrate = CAL_TYPE_LEVEL;

      if (formSpikeWare->m_swsStimuli.m_dDeviceSampleRate == 0.0)
         formSpikeWare->m_swsStimuli.m_dDeviceSampleRate = (double)m_fDefaultSampleRate;

      #ifdef CHKCHNLS
      if (usOutChannel != m_pslChannelsOut->Strings[nOutChannel])
         ShowMessage("error B "+ UnicodeString( __FUNC__));
      #endif

      // create schroeder phase complex
      float fLo, fHi;
      formSpikeWare->m_swfFilters->GetChannelFreqs(
         usOutChannel,
         fLo,
         fHi,
         (float)(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/2.0),
         formSpikeWare->IsInSitu()
         );
      CreateSchroederPhaseToneComplex(fLo, fHi);


      UnicodeString us = "driver=" + m_usDriver + ";";
      us += "output=" + IntToStr(nOutChannel) + ";";
      if (bUseRefMic)
         {
         int nMicChannel = m_swcHWChannels.GetRefMic();
         if (nMicChannel < 0)
            throw Exception("reference microphone not configured");
         us += "input=" + IntToStr(nMicChannel) + ";";
         if (!formSpikeWare->m_pIni->ReadBool("Debug", "RecSave", false))
            us += "recfiledisable=1;";
         // attach post-vst processing for recording channels: used on formFFTEdit to retrieve
         // recording data on the fly
         us += "extrecpostvstproc="  + IntToStr((int)&TformSpikeWare::SMPRecPostVSTProc) + ";";
         }
      us += "ramplen=" + IntToStr((int)floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/20.0)) + ";";
      us += "samplerate=" + DoubleToStr(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate) + ";";
      // attach pre-vst processing for signal generator in calibration mode
      us += "extprevstproc="  + IntToStr((int)&TformSpikeWare::SMPPreVSTProc) + ";";
      // attach post-vst processing for clip detection
      us += "extpostvstproc="  + IntToStr((int)&TformSpikeWare::SMPPostVSTProc) + ";";
      us += "force=1;";
      us += "recprocesseddata=1;";
      us += "autocleardata=1;";
      if (!m_usLog.IsEmpty())
         us += "logfile=" + m_usLog + ";";

      if (!Command("init", us))
         return false;

      #ifdef CHKCHNLS
      if (usOutChannel != m_pslChannelsOut->Strings[nOutChannel])
         ShowMessage("error B "+ UnicodeString( __FUNC__));
      #endif

      if (!VSTLoad(0, GetEqualisation(usOutChannel), PLUGIN_POS_EQ))
         return false;

      if (m_bShowMixer)
         Command("showmixer;topmost=1");

      if (m_bShowTracks)
         Command("showtracks");

      if (!Command("volume", "value=0"))
         throw Exception("error setting volume for calibration");
      if (!Command("start", "length=0"))
         throw Exception("error running SMP for calibration");

      return true;
      }
   catch(Exception &e)
      {
      Exit();
      formSpikeWare->SWErrorBox("Error initializing SMP calibration for " + AS_NAME + ": " + e.Message);
      }
   return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes spectral calibration procedure
//------------------------------------------------------------------------------
LPFNFILTER SWSMP::InitSpectralCalibration(int nOutChannel, TCalMode cmMode)
{
   if (!ReadSettings())
      return NULL;

   try
      {
      if (!Exit())
         throw Exception("cannot Exit SMP");

      #ifdef CHKCHNLS
      if (m_swcHWChannels.GetRefMic() != m_nMicChannelIn)
         ShowMessage("error A "+ UnicodeString( __FUNC__));
      #endif

      int nMicChannelIn = m_swcHWChannels.GetRefMic();
      bool bUseMicIn = nMicChannelIn >= 0;
      UnicodeString usOutChannel = m_swcHWChannels.GetChannelName((unsigned int)nOutChannel, SWSMPHWCDIR_OUT);

      m_nCalibrate = CAL_TYPE_SPEC;
      if (formSpikeWare->m_swsStimuli.m_dDeviceSampleRate == 0.0)
         formSpikeWare->m_swsStimuli.m_dDeviceSampleRate = (double)m_fDefaultSampleRate;

      // NOTE: creation of  schroeder phase complex done on formFFTEdit (TformFFTEdit::Run)
      UnicodeString us = "driver=" + m_usDriver + ";";
      us += "output=" + IntToStr(nOutChannel) + ";";

      // initialize input cahnnel depending on mode
      if (cmMode == CAL_MODE_SPEAKER)
         {
         if (bUseMicIn)
            us += "input=" + IntToStr(nMicChannelIn) + ";";
         }
      else
         {
         int nMicChannel = -1;
         // use channel of reference microphone for input, or ...
         if (cmMode == CAL_MODE_MIC_REF)
            {
            nMicChannel = nMicChannelIn;
            if (nMicChannel < 0)
               throw Exception("Reference microphone input channel not configured.");
            }
         // ... use probe microphone channel.
         else
            {
            #ifdef CHKCHNLS
            if (usOutChannel != m_pslChannelsOut->Strings[nOutChannel])
               ShowMessage("error B "+ UnicodeString( __FUNC__));
            #endif

            nMicChannel = GetInSituInputChannel(usOutChannel);
            if (nMicChannel < 0)
               throw Exception("In-situ input channel for output '" + usOutChannel + "' not configured.");
            }
         us += "input=" + IntToStr(nMicChannel) + ";";
         }

      if (!formSpikeWare->m_pIni->ReadBool("Debug", "RecSave", false))
         us += "recfiledisable=1;";
      us += "ramplen=" + IntToStr((int)floor(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate/20.0)) + ";";
      us += "samplerate=" + DoubleToStr(formSpikeWare->m_swsStimuli.m_dDeviceSampleRate) + ";";
      // attach pre-vst processing for signal generator in calibration mode
      us += "extprevstproc="  + IntToStr((int)&TformSpikeWare::SMPPreVSTProc) + ";";
      // playback clip detector
      us += "extpostvstproc="  + IntToStr((int)&TformSpikeWare::SMPPostVSTProc) + ";";
      // attach post-vst processing for recording channels: used on formFFTEdit to retrieve
      // recording data on the fly
      us += "extrecpostvstproc="  + IntToStr((int)&TformSpikeWare::SMPRecPostVSTProc) + ";";
      us += "force=1;";
      us += "recprocesseddata=1;";
      us += "autocleardata=1;";

      if (!m_usLog.IsEmpty())
         us += "logfile=" + m_usLog + ";";
      if (!Command("init", us))
         throw Exception("'init' failed");

      if (m_bShowMixer)
         Command("showmixer;topmost=1");

      if (formSpikeWare->m_pIni->ReadBool("Debug", "DebugSave", false))
         Command("debugsave;value=1;");

      AnsiString asReturn;
      // use special "cal-plugin" supporting user callback "FilterFunction"
      UnicodeString usPluginName = m_bShowFFTPlugins ? "HtVstCalVisAS.dll" : "HtVstCalAS.dll";
      // NOTE: channel is always 0 in the following because we run only
      // one channel (see "output=" and "input=" arguments on init above
      if (cmMode == CAL_MODE_SPEAKER)
         {
         #ifdef CHKCHNLS
         if (usOutChannel != m_pslChannelsOut->Strings[nOutChannel])
            ShowMessage("error A "+ UnicodeString( __FUNC__));
         #endif
         // load special plugin to output.
         if (!VSTLoad(0, GetEqualisation(usOutChannel), PLUGIN_POS_EQ, false, usPluginName))
            throw Exception("error loading output cal-plugin for filtering");
         // retrieve "filter-function" callback pointer.
         if (!Command("vstprogram", "userconfig=1;program=1;position=" + IntToStr(PLUGIN_POS_EQ), false, &asReturn))
            throw Exception("error retrieving vstprogram/userconfig of output cal-plugin");
         }
      else if (cmMode == CAL_MODE_INSITU)
         {
         #ifdef CHKCHNLS
         if (usOutChannel != m_pslChannelsOut->Strings[nOutChannel])
            ShowMessage("error B "+ UnicodeString( __FUNC__));
         #endif
         // load input plugin with probe-mic filter. We check beforehand, if filter for probe mic
         // is available at all!!
         us = GetInSituInputFilterSection(usOutChannel);
         if (us.IsEmpty() || !formSpikeWare->m_swfFilters->m_pFilterIni->SectionExists(us))
            throw Exception("probe microphone is not calibrated");

         if (!VSTLoad(0, us, PLUGIN_POS_EQ, true))
            throw Exception("error loading input cal-plugin for filtering");

         // load empty special pugin to output
         if (!VSTLoad(0, "", PLUGIN_POS_EQ, false, usPluginName))
            throw Exception("error loading output plugin");
         // retrieve "filter-function" callback pointer.
         if (!Command("vstprogram", "userconfig=1;program=1;position=" + IntToStr(PLUGIN_POS_EQ), false, &asReturn))
            throw Exception("error retrieving vstprogram/userconfig of output cal-plugin");
         }
      // CAL_MODE_MIC_REF and CAL_MODE_MIC_PROBE
      else
         {
         // for testing load more plugins....
         if (formSpikeWare->m_pIni->ReadBool("Debug", "CalInsituMicTest", false))
            {
            if (!VSTLoad(0, "test.ini##DUMMY1", PLUGIN_POS_EQ))
               throw Exception("error loading output plugin");

            // for testing load additional input plugin to simulate a nonlinear microphone
            if (!VSTLoad(0, "", 0, true, usPluginName))
               throw Exception("error loading input cal-plugin for filtering");
            }
         // REGULAR MODE!
         else
            {
            // NOTE: for microphone calibration we load an EMPTY standard EQ plugin to output
            if (!VSTLoad(0, "", PLUGIN_POS_EQ))
               throw Exception("error loading output plugin");
            }

         // load special plugin to input
         if (!VSTLoad(0, "", PLUGIN_POS_EQ, true, usPluginName))
            throw Exception("error loading input  cal-plugin for filtering");

         // retrieve "filter-function" callback pointer.
         if (!Command("vstprogram", "type=input;userconfig=1;program=1;position=" + IntToStr(PLUGIN_POS_EQ), false, &asReturn))
            throw Exception("error retrieving vstprogram/userconfig of input cal-plugin");
         }
      //
      // retrieve filter function pointer from return values and return it
      TStringList* psl = new TStringList();
      try
         {
         ParseValues(psl, asReturn, ';');
         
         int64_t nlpfn = 0;
         AnsiString as = psl->Values["program"];
         if (!as.Length())
            throw Exception("error retrieving FilterFunction callback from eq-plugin (1)");
         if (as[1] == '"')
            as = as.SubString(2, as.Length());

         if (!TryStrToInt64(as, nlpfn)|| !nlpfn)
            throw Exception("error retrieving FilterFunction callback from eq-plugin (2)");
         

         return (LPFNFILTER)nlpfn;
         }
      __finally
         {
         TRYDELETENULL(psl);
         }

      }
   catch(Exception &e)
      {
      Exit();
      formSpikeWare->SWErrorBox("Error initializing SMP spectral calibration for " + AS_NAME + ": " + e.Message);
      }
   return NULL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates a schroeder tone complex for calibration
//------------------------------------------------------------------------------
void  SWSMP::CreateSchroederPhaseToneComplex(float fLowFreq,
                                             float fHiFreq,
                                             bool bRandom,
                                             int nSamples
                                             )
{
   // default length is FFT-Length
   if (nSamples == 0)
      nSamples = m_nEqFFTLen;

   CreateSchroederPhaseToneComplexVector( m_vfSchroeder,
                                          (unsigned int)nSamples,
                                          formSpikeWare->m_swsStimuli.m_dDeviceSampleRate,
                                          (double)fLowFreq,
                                          (double)fHiFreq,
                                          bRandom);
   m_fSchroederRMSdB = RMS(m_vfSchroeder);
}
//------------------------------------------------------------------------------

