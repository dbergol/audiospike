//------------------------------------------------------------------------------
/// \file SWSMP.h
///
/// \author Berg
/// \brief Implementation of a class SWSMP, the interface to SoundMexProAS dll
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
#ifndef SWSMPH
#define SWSMPH
//------------------------------------------------------------------------------

#include <vcl.h>
#include <limits.h>
#include <vector>
#include <valarray>
#include "SWSMPChannels.h"
#include "SWTools.h"

#define FFTLEN_DEFAULT     2048

//------------------------------------------------------------------------------
///
//------------------------------------------------------------------------------
#define SOUNDDLL_COMMANDNAME           "SoundDllProCommand"
#define RETSTRMAXLEN 2*SHRT_MAX
//------------------------------------------------------------------------------

float GetHanningValue(unsigned int uWindowPos, unsigned int uWindowLen);

//------------------------------------------------------------------------------
/// filter plugin callback prototype
typedef void  (*LPFNFILTER)(int nFunction, std::valarray<float >* pvaf, AnsiString as);
/// SMP command function prototype
typedef int    (cdecl *LPFNSOUNDDLLPROCOMMAND)(const char*, char*, int);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for first arg of LPFNFILTER: what-to-do!
//------------------------------------------------------------------------------
enum TFilterFcn
{
   FILTER_FCN_SETFILTER = 0,
   FILTER_FCN_GETFILTER,
   FILTER_FCN_SETBORDERS,
   FILTER_FCN_GETBORDERS,
   FILTER_FCN_LOADFILE
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// positions of plugins
//------------------------------------------------------------------------------
enum TCalPluginPos {
   PLUGIN_POS_CUT = 1,
   PLUGIN_POS_EQ = 3
};

//------------------------------------------------------------------------------
/// calibration types (level calibation or spectral calibration
//------------------------------------------------------------------------------
enum TCalType {
   CAL_TYPE_LEVEL   = 1,
   CAL_TYPE_SPEC
};

//------------------------------------------------------------------------------
/// calibration modes
//------------------------------------------------------------------------------
enum TCalMode {
   CAL_MODE_SPEAKER   = 0,
   CAL_MODE_MIC_REF,
   CAL_MODE_MIC_PROBE,
   CAL_MODE_INSITU,
   CAL_MODE_INSITU_LEVEL
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Init-modes for SMP
//------------------------------------------------------------------------------
enum {
   AS_SMP_INIT_MEASURE = 0,
   AS_SMP_INIT_TRIGGERTEST,
   AS_SMP_INIT_FREESEARCH
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// equalisation methods
//------------------------------------------------------------------------------
enum {
   AW_SMP_EQ_IR = 0,
   AW_SMP_EQ_FFT,
};
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
/// enum for SoundMexPro return values. Not really used yet...
//---------------------------------------------------------------------------------
enum  {
      SOUNDDLL_RETURN_WARNING = -3,
      SOUNDDLL_RETURN_ERROR,
      SOUNDDLL_RETURN_MEXERROR,
      SOUNDDLL_RETURN_BUSY,
      SOUNDDLL_RETURN_OK
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// interface class to SoundMexProAS dll
//------------------------------------------------------------------------------
class SWSMP
{
   public:
      #ifdef CHKCHNLS
      TStringList*      m_pslChannelsIn;
      TStringList*      m_pslChannelsOut;
      // hardware device channel indices
      int               m_nTriggerChannelIn;
      int               m_nTriggerChannelOut;
      int               m_nMonitorChannelOut;
      int               m_nMicChannelIn;
      // channels selected in settings (i.e. available)
      // NOTE: these are hardware indices in ascending order as selected in the
      // settings dialog. They contain electrode channels ...
      std::vector<int >             m_viChannelsInSettings;
      // ... or playback channels respectivey
      std::vector<int >             m_viChannelsOutSettings;

      // used hardware (!) indices
      std::vector<int >       m_viHardwareChannelsOutUsed;
      std::vector<int >       m_viOutTrackIndices;


      // soundmexpro channel indices (after init!)
      int               m_nTriggerChannelOutIndex;
      int               m_nMonitorChannelOutIndex;
      int               m_nTriggerChannelInIndex;
      #endif

      bool              m_bAsyncError;
      SWSMPHWChannels   m_swcHWChannels;
      SWSMPHWChannels   m_swcUsedChannels;
      UnicodeString     m_usLastError;
      TStringList*      m_pslDrivers;
      UnicodeString     m_usDriver;

      // channels selected in measurement: NOTE these are zero based indices
      // relative to m_viChannelsOutSettings. Only on Init() they are translated
      // to hardware indices again!
      // m_viMeasChannelsOutUsed additionally is only used to calculate the number
      // of currently used outputs!!
      std::vector<int >             m_viMeasChannelsOutUsed;
      std::vector<int >             m_viMeasChannelsInUsed;
      // m_viProbeMicOutChannels contains the 'mapping' of probemic channels to outputs
      // This is only used for saving probemic raw data in TSWEpoches::SoundProc
      std::vector<int >             m_viProbeMicOutChannels;

      int                           m_nCalibrate;
      int                           m_nEqualisationLength;
      int                           m_nEqualisationMethod;
      int                           m_nEqFFTLen;
      bool                          m_bAllowEqDiffLengths;
      bool                          m_bShowFFTPlugins;


      // calibration members
      unsigned int                  m_nCalSignalPos;
      std::valarray<float >         m_vfSchroeder;
      float                         m_fSchroederRMSdB;
      void                          CreateSchroederPhaseToneComplex( float fLowFreq,
                                                                     float fHiFreq,
                                                                     bool bRandom = true,
                                                                     int nSamples = 0);


      UnicodeString     m_usLog;
      bool              m_bShowTracks;
      bool              m_bShowMixer;
      bool              m_bSettingsRead;
      bool              m_bSaveProbeMics;
      int               m_nTriggerLength;
      int               m_nTriggerOffset;
      float             m_fDefaultSampleRate;
      float             m_fDefaultSampleRateDevider;
      double            m_dTriggerLatency;
      // values for free search
      bool              m_bFreeSearchContinuous;
      vvd               m_vvdFreeSearchBuffer;
      unsigned int      m_nFreeSearchSamplesPlayed;
      int               m_nFreeSearchStimLengthMs;
      int               m_nFreeSearchPreStimLengthMs;
      int               m_nFreeSearchRepetitionPeriodMs;
      int               m_nFreeSearchRepetitionPeriodSamples;
      int               m_nFreeSearchRampLengthMs;
      int               m_nFreeSearchWindowPos;
      unsigned int      m_nFreeSearchSchroederPos;
      int               m_nFreeSearchTriggerPos;
      float             m_fSineFreqPending;
      std::valarray<float > m_vafFreeSearchWindow;
      // values for maximum search and calibration
      std::valarray<double > m_vadMaxSearch;
      std::valarray<double > m_vadMaxLevelsAvailable;
      float                   m_fTriggerValue;


   public:
      SWSMP();
      ~SWSMP();
      void  ExitLibrary();
      void  InitLibrary();
      bool  Command(AnsiString asCmd, AnsiString asArgs = "", bool bShowError = true, AnsiString *asReturn = NULL);

      bool  Playing();
      bool  Stop();
      bool  Exit();
      bool  Wait();
      bool  Initialized();
      int   GetXRuns();
      bool  Init(int nMode = AS_SMP_INIT_MEASURE);
      bool  Prepare(int nStimIndex = -1);
      bool  PrepareTriggerTest();
      bool  PlayStimulusLooped(int nStimIndex);
      bool  ReadSettings(bool bShowError = true, bool bForce = false);
      bool  SetMonitor(int nChannel);
      void  WriteSettings();
      bool  InitializeChannels();

      UnicodeString GetDriver();
      void  SetDriver(UnicodeString usDriver);
      bool  GetChannels(UnicodeString usDriver, TStrings *pslIn, TStrings *pslOut);
      bool  VSTLoad( int nChannel,
                     UnicodeString usEqualisation,
                     int nPos,
                     bool bInput = false,
                     UnicodeString usPlugin = "");
      #ifdef CHKCHNLS
      void  InitFreeSearch(std::vector<int >& rviOutputTracks);
      #else
      void  InitFreeSearch();
      #endif
      bool  Start(int nEpocheSize = 0);
      void  SoundFreeSearchSignalGenerator(vvf &vvfBuffers);
      void  SoundClipDetector(vvf &vvfBuffers);
      void  SoundCalibrationSignalGenerator(vvf &vvfBuffers);

      bool  MaxSearch();
      void  SoundMaxSearchProc(vvf &vvfBuffers);

      void           SetCalibrationValue(unsigned int nOutChannel, double dCalValue);
      double         GetCalibrationValueN(unsigned int nOutChannel);
      double         GetCalibrationValue(UnicodeString usChannel);
      void           SetEqualisation(unsigned int nOutChannel, UnicodeString usEqualisation);
      UnicodeString  GetEqualisationN(unsigned int nOutChannel);
      UnicodeString  GetEqualisation(UnicodeString usChannel);
      UnicodeString  GetCalibrationSection(UnicodeString usChannel);
      bool           InitCalibration(int nOutChannel, bool bUseRefMic = false);
      bool           LoadStimuli();

      // functions and variables used for FFTEdit (equalisation)
      LPFNFILTER     InitSpectralCalibration(int nOutChannel, TCalMode cmMode = CAL_MODE_SPEAKER);

      // insitu channel functions
      UnicodeString  GetInSituInput(UnicodeString usOutChannel);
      int            GetInSituInputChannel(UnicodeString usOutChannel);
      UnicodeString  GetInSituInputFilterSection(UnicodeString usOutChannel);
      void           SetInSituInput(UnicodeString usOutChannel, UnicodeString usInChannel);

   private:
      CRITICAL_SECTION        m_cs;
      bool                    m_bStopping;
      char                    m_lpszReturn[RETSTRMAXLEN];
      HINSTANCE               m_hLib;
      LPFNSOUNDDLLPROCOMMAND  m_lpfnSoundDllProCommand;

      UnicodeString           m_usIniSection;
      unsigned int            m_nBufferSize;
      float                   m_fSineFreq;

      int                     m_nFakeTotalRecOffset;

      unsigned int            m_nNumLoadedStimuli;
      std::valarray<double >  m_vadTrigger;
      std::valarray<double >  m_vadStimulus;
      std::vector<int >       m_viGainIndices;
      std::vector<int >       m_viTrackLoad;
      void LoadStim(int nStimInd, bool bFirstStim = false, int nLoopCount = 1);
      void LoadFreeSearchStim();
};
//------------------------------------------------------------------------------
#endif
