//------------------------------------------------------------------------------
/// \file SpikeWareMain.h
///
/// \author Berg
/// \brief Implementation of main form of AudioSpike owning all subwindows, data ...
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
#ifndef SpikeWareMainH
#define SpikeWareMainH
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <msxmldom.hpp>
#include <ToolWin.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include <System.Classes.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>
#include <Xml.Win.msxmldom.hpp>
#include <Xml.XMLDoc.hpp>
#include <Xml.xmldom.hpp>
#include <Xml.XMLIntf.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <VCL.FileCtrl.hpp>
#include <System.inifiles.hpp>
#include <System.ImageList.hpp>
//------------------------------------------------------------------------------
#include "VCLTee.Chart.hpp"
#include "VCLTee.Series.hpp"
#include "VCLTee.TeEngine.hpp"
#include "VCLTee.TeeProcs.hpp"
#include "VCLTee.TeeHistogram.hpp"
#include "VCLTee.TeeShape.hpp"
#include "VCLTee.BubbleCh.hpp"
#include <vector>
#include <valarray>
#include "SWSpike.h"
#include "SWStim.h"
#include "SWEpoches.h"
#include "frmSetParameters.h"
#include "frmCluster.h"
#include "frmBubblePlot.h"
#include "SWSMP.h"
#include "SWTools.h"
#include "frmSettings.h"
#include "SWFilters.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// global defnitions
//------------------------------------------------------------------------------
#define AS_NAME         UnicodeString("AudioSpike")
#define SHIFTED      0x8000
#define WM_SWCMD     WM_USER+100
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// forward declarations
//------------------------------------------------------------------------------
class TformASUI;
class TformSearchFree;
class TformStimuli;
class TformBatch;
class TformSpikes;
class TformPSTH;
class TformSignalPSTH;
class TformSelect;
class TformEpoches;
class TformEpocheWindow;
class TformVectorStrength;
class TformBubbleData;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for status bar panel indices
//------------------------------------------------------------------------------
enum TSBPanel {
   SB_P_FLOPPY = 0,
   SB_P_CONFIG,
   SB_P_STATUS,
   SB_P_DEMO
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for different epoche load modes
//------------------------------------------------------------------------------
enum TEpocheLoadMode
{
   SWELM_NOSPIKES = 0,           ///< load NO spikes
   SWELM_SPIKES,                 ///< load spies
   SWELM_SPIKES_RESET_THRESHOLD  ///< load spikes and reset threshold (rescanning)
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for XML-file load modes
//------------------------------------------------------------------------------
enum TSWLoadMode
{
   SWLM_TEMPLATE = 0,   ///< load a template (start a new measurement)
   SWLM_RESULT,         ///< load a result
   SWLM_RESUME          ///< load an incomplete result for resuling measurement
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for status flag of GUI
//------------------------------------------------------------------------------
enum TSWGuiStatus
{
   SWGS_UNCHANGED = -1,    ///< unchanged reult loaded
   SWGS_NONE      = 0,     ///< nothing loaded at all
   SWGS_LOADED,            ///< template loaded
   SWGS_RESULTLOADED,      ///< result loaded
   SWGS_FREESEARCHRUN,     ///< free search is running
   SWGS_FREESEARCHSTOP,    ///< free seacrh is active, but stopped
   SWGS_SEARCH,            ///< search is running
   SWGS_RUN,               ///< measurement is running
   SWGS_PAUSE,             ///< measurement is paused
   SWGS_STOP               ///< measurement is stopped
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for result mode (status for one result in batch mode)
//------------------------------------------------------------------------------
enum TSWRunResult
{
   SWRR_ERROR = -1,  ///< meas is in error mode
   SWRR_DONE,        ///< meas is done
   SWRR_PAUSE        ///< meas is paused
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Main form and class of AudioSpike: manages all subclasess and GUI
//------------------------------------------------------------------------------
class TformSpikeWare : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TOpenDialog *od;
      TToolBar *tb;
      TMainMenu *mnuMain;
      TMenuItem *miWindows;
      TMenuItem *miSpikes;
      TScrollBox *scb;
      TStatusBar *sb;
      TMenuItem *miPSTH;
      TXMLDocument *xml;
      TTimer *EpocheTimer;
      TMenuItem *miStimuli;
      TMenuItem *N3;
      TMenuItem *miCluster;
      TMenuItem *miParameter;
      TImageList *il;
      TMenuItem *miFile;
      TToolButton *btnLoadTemplate;
      TToolButton *btnCluster;
      TToolButton *btnParam;
      TToolButton *btnRun;
      TToolButton *btnStop;
      TMenuItem *miEpocheWindow;
      TMenuItem *miSettings;
      TToolButton *btnPause;
      TToolButton *btnLoadResult;
      TToolButton *btnSave;
      TToolButton *btnRescanSpikes;
      TImageList *ild;
      TToolButton *btnSearch;
      TMenuItem *miLoadTemplate;
      TMenuItem *miLoadResult;
      TImage *imFloppy;
      TTimer *StatusTimer;
      TXMLDocument *xmlSave;
      TToolButton *btnFreeSearch;
      TXMLDocument *xmlAppend;
      TToolButton *btnAppend;
      TMenuItem *N6;
      TMenuItem *miAppend;
      TMenuItem *N7;
      TMenuItem *miSave;
      TMenuItem *N8;
      TMenuItem *miHelp;
      TMenuItem *miAbout;
      TMenuItem *N9;
      TSpeedButton *btnInSitu;
      TToolButton *tbsSeparator1;
      TToolButton *tbsSeparator2;
      TToolButton *tbsSeparator3;
      TToolButton *tbsSeparator4;
      TToolButton *tbsSeparator5;
      TMenuItem *miSignalPSTH;
      TMenuItem *miTools;
      TMenuItem *miAdjustSpikeLength;
      TToolButton *btnReloadEpoches;
      TMenuItem *N1;
      TMenuItem *miBatchRun;
      TToolButton *btnBatch;
   TMenuItem *miUpdateCheck;
      void __fastcall btnLoadTemplateClick(TObject *Sender);
      void __fastcall btnParamClick(TObject *Sender);
      void __fastcall btnRunClick(TObject *Sender);
      void __fastcall btnClusterClick(TObject *Sender);
      void __fastcall miFormToggleClick(TObject *Sender);
      void __fastcall btnStopClick(TObject *Sender);
      void __fastcall EpocheTimerTimer(TObject *Sender);
      void __fastcall miSettingsClick(TObject *Sender);
      void __fastcall btnPauseClick(TObject *Sender);
      void __fastcall btnLoadResultClick(TObject *Sender);
      void __fastcall btnSaveClick(TObject *Sender);
      void __fastcall btnRescanSpikesClick(TObject *Sender);
      void __fastcall btnSearchClick(TObject *Sender);
      void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
      void __fastcall sbDrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect);
      void __fastcall StatusTimerTimer(TObject *Sender);
      void __fastcall btnFreeSearchClick(TObject *Sender);
      void __fastcall btnAppendClick(TObject *Sender);
      void __fastcall miHelpClick(TObject *Sender);
      void __fastcall miAboutClick(TObject *Sender);
      void __fastcall btnInSituClick(TObject *Sender);
      void __fastcall miAdjustSpikeLengthClick(TObject *Sender);
      void __fastcall FormShow(TObject *Sender);
      void __fastcall btnReloadEpochesClick(TObject *Sender);
      void __fastcall btnBatchClick(TObject *Sender);
      void __fastcall sbResize(TObject *Sender);
   void __fastcall miUpdateCheckClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
      bool     m_bFormsCreated;      
      void     CreateForms();
      void     CreateClusterWindow(int nX = -1, int nY = -1, unsigned int nChannels = 1);
      bool     ProcessEpoches();
      void     CreateParameterWindow();
      void     CreateParameterWindow(int nX, int nY);
      void     SoundNotify(void);
      bool     UpdateStimulusDisplay();
      void     ConvertIniFile();
      void     ReadSettings();
      void     SetStyle();
   public:		// Benutzer-Deklarationen
      UnicodeString        ParameterWindowName(unsigned int nX, unsigned int nY);
      bool                 FormsCreated();
      static UnicodeString GetSettingsRootPath();
      static UnicodeString GetSettingsPath();
      static UnicodeString ms_usSettingsName;
      UnicodeString     m_usASCaption;
      bool              m_bFreeWindows;
      TSWGuiStatus      m_gs;
      CRITICAL_SECTION  m_cs;
      SWSMP             m_smp;
      // forms
      TformSearchFree*     m_pformSearchFree;
      TformBatch*          m_pformBatch;
      TformBubblePlot*     m_pformSearch;
      TformStimuli*        m_pformStimuli;
      TformSpikes*         m_pformSpikes;
      TformPSTH*           m_pformPSTH;
      TformSignalPSTH*     m_pformSignalPSTH;
      TformVectorStrength* m_pformVectorStrength;
      TformEpocheWindow*   m_pformEpoches;
      TformSetParameters*  m_pformSetParameters;
      TformSelect*         m_pformSelect;
      TformSettings*       m_pformSettings;
      TformBubbleData*     m_pformBubbleData;
      std::vector<TformCluster* >      m_vpformCluster;
      std::vector<TformBubblePlot* >   m_vpformBubblePlots;
      std::vector<TColor >             m_vclSpikeColors;
      TStringList*      m_pslParamStr;
      TIniFile*         m_pIni;
      TIniFile*         m_pCalIni;
      TSWFilters*       m_swfFilters;
      TSWStimuli        m_swsStimuli;
      TSWSpikes         m_swsSpikes;
      TSWEpoches        m_sweEpoches;
      bool              m_bBreak;
      UnicodeString     m_usFixResultPath;
      UnicodeString     m_usTemplatePath;
      UnicodeString     m_usResultPath;
      UnicodeString     m_usResultPathRoot;
      UnicodeString     m_usMsg;
      std::vector<int > m_viStimSequence;
      std::vector<int > m_viRepetitionSequence;
      int               m_SearchModeStimIndex;
      bool              m_bTriggerTestRunning;
      bool              m_bFreeSearchRunning;
      int               m_nStimPlayIndex;
      bool              m_bUpdateStimulusDisplay;
      bool              m_bDataAppended;
      bool              m_bLevelDebug;
      bool              m_bSaveMAT;
      bool              m_bSaveProbeMic;
      bool              m_bStartupInSitu;
      bool              m_bCheckUpdateOnStartup;
      // counter functions for plot subroutines
      bool              EnterPlot();
      void              ExitPlot();
      bool              m_bPlotAllowed;
      int               NumRunningPlotCommands();

      __fastcall     TformSpikeWare(TComponent* Owner);
      __fastcall     ~TformSpikeWare();
      void           Initialize();
      bool           GetWindowVisible(TForm* pfrm);
      void           SetWindowVisible(TForm* pfrm, bool bVisible, bool bSetMDI = true);
      void           EnableAllMainControls(bool bEnable);
      void           ProcessCommandLine(bool bReadFromCommandLine);
      bool           LoadMeasurementTemplate(UnicodeString us);
      bool           LoadMeasurementResult(UnicodeString us);
      bool           LoadMeasurement(UnicodeString us, int nMode);
      bool           AppendMeasurement(UnicodeString us);
      bool           InitFreeSearch();
      int            EpochesXML(bool bDone);
      void           CreateXMLEpoches(std::vector<int >* vn = NULL);
      void           LoadEpoches(TEpocheLoadMode nELM);
      void           SetXMLEpocheDone(int nNode, bool bDone);
      void           SetXMLEpocheThreshold(int nNode, std::vector<double >& rvd);
      void           SetXMLEpocheThreshold(_di_IXMLNode xml, std::vector<double >& rvd);
      void           EnsureXMLEpocheThresholds();
      std::vector<double > GetXMLEpocheThresholds(int nNode);
      std::vector<double > GetXMLEpocheThresholds(_di_IXMLNode xml);
      int            SaveResult(bool bForceNewResult = false);
      void           DeleteFilesAndFolder(UnicodeString us);
      void           SetFormParent(TForm* pfrm);
      void           PlotEpoches(TSWEpoche *pswe);
      void           PlotSpikes();
      void           PlotClusters();
      void           PlotBubblePlots();
      void           SetAutoResultPath(UnicodeString usSubPath);
      TSWRunResult   RunMeasurement(bool bResume);
      void           PlaySearchStimulus(int nIndex, bool bUpdateBubble = false);
      void           RemoveClusterWindow(TformCluster* pfrm);
      void           RemoveParamWindow(TformBubblePlot* pfrm);
      void           StoreFormPos(TForm* pfrm);
      void           RestoreFormPos(TForm* pfrm);
      void           RestoreFormVis(TformASUI* pfrm);
      void           StoreChartAxis(TForm* pfrm, TChart *pchrt, bool bLeft = true, bool bBottom = false);
      void           RestoreChartAxis(TForm* pfrm, TChart *pchrt, bool bLeft = true, bool bBottom = false);
      void           SetEvalWindow(TformEpoches* pfrm);
      void           SetMonitor(TformEpoches* pfrm);
      void           SetSelectionWindow(TformCluster* pfrm);
      void           ScaleClusterPlots(int nChannel);
      void           ScaleClusterPlot(TformCluster* pfrm);
      void           UpdateClusterColors();
      TColor         SpikeGroupToColor(int n);
      void           SetThreshold(unsigned int nChannelIndex, double dThreshold, bool bSave = true);
      double         GetThreshold(unsigned int nChannelIndex);
      std::vector<double >& GetThresholds();
      unsigned int   GetCurrentStimulus(unsigned int nEpochesTotal);
      void           SetGUIStatus(TSWGuiStatus gs = SWGS_UNCHANGED);
      bool           IsRunning();
      // static procedures passed to SMP
      static void    SMPPreVSTProc(vvf &vvfBuffers);
      static void    SMPPostVSTProc(vvf &vvfBuffers);
      static void    SMPPostVSTProcMaxSearch(vvf &vvfBuffers);
      static void    SMPRecPreVSTProc(vvf &vvfBuffers);
      static void    SMPRecPostVSTProc(vvf &vvfBuffers);
      static void    SMPBufferDoneProc(vvf &vvfBuffers);
      static void    SMPNotifyProc(void);
      void __fastcall TriggerTest(void);
      void           Cleanup(void);
      void           SetMeasurementChanged(bool bChanged = true, bool bForce = false);
      int            SaveInquiry();
      void           DeleteCurrentResults(bool bQueryForRawDataAndDir = true);
      void           SWErrorBox(UnicodeString us, HWND hwnd = 0);
      void __fastcall AppMessage(tagMSG &Msg, bool &Handled);
      void           SetStatusMsg(UnicodeString us);
      void           CreateStimulusSequence(std::vector<int > &rvn, unsigned int nSize, int nOffset = 0);
      UnicodeString  GetStatusString();
      void           SetGUIBusy(bool bBusy, TWinControl* pctrt);
      void           DebugSaveXML(int n = 0);
      bool           RMSWarning();
      void           HighPassWarning();
      bool           IsInSitu();
      void           SetInSitu(bool b, bool bForce = false);
      bool           IsMDI();
      bool           IsFreeWindows();
      bool           IsBatchMode();
      void           CleanupBatchMode();
      void           EnableEpocheTimer(bool bEnable);      
};
//------------------------------------------------------------------------------
extern PACKAGE TformSpikeWare *formSpikeWare;
//------------------------------------------------------------------------------
#endif
