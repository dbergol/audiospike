//------------------------------------------------------------------------------
/// \file SpikeWareMain.cpp
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
#include <vcl.h>
#include <math.h>
#include <algorithm>
#include <limits.h>
#include <FileCtrl.hpp>
#include <except.h>
#include <processthreadsapi.h>

#pragma hdrstop

#include "SpikeWareMain.h"
#include "frmASUI.h"
#include "formAbout.h"
#include "frmEpoche.h"
#include "frmSpikes.h"
#include "frmPSTH.h"
#include "frmSelect.h"
#include "frmEpocheWindow.h"
#include "frmVectorStrength.h"
#include "frmBatch.h"
#include "frmStimuli.h"
#include "frmSelectChannels.h"
#include "Encddecd.hpp"
#include "frmWait.h"
#include "frmSearchFree.h"
#include "frmFFTEdit.h"
#include "frmSignalPSTH.h"
#include "frmBubbleData.h"
#include "windowsx.h"
#include "frmCalibrationCalibrator.h"
#include "VersionCheck.h"
#include "frmVersionCheck.h"
#include <System.DateUtils.hpp>



#pragma package(smart_init)
#pragma resource "*.dfm"

#pragma warn -aus
TformSpikeWare *formSpikeWare;

//------------------------------------------------------------------------------
/// initialize static member 'settings name'
//------------------------------------------------------------------------------
 UnicodeString TformSpikeWare::ms_usSettingsName = "default";
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// static function returning current settings root path
//------------------------------------------------------------------------------
UnicodeString TformSpikeWare::GetSettingsRootPath()
{

   return IncludeTrailingBackslash(ExpandFileName(ExtractFilePath(Application->ExeName) + "\\..\\settings"));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// static function returning current settings path
//------------------------------------------------------------------------------
UnicodeString TformSpikeWare::GetSettingsPath()
{
   return IncludeTrailingBackslash(GetSettingsRootPath() + ms_usSettingsName);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. initializing EVERYTHING
//------------------------------------------------------------------------------
__fastcall TformSpikeWare::TformSpikeWare(TComponent* Owner)
   :  TForm(Owner),
      m_gs(SWGS_NONE),
      m_pformSearchFree(NULL),
      m_pformBatch(NULL),
      m_pformSearch(NULL),
      m_pformStimuli(NULL),
      m_pformSpikes(NULL),
      m_pformPSTH(NULL),
      m_pformSignalPSTH(NULL),
      m_pformVectorStrength(NULL),
      m_pformEpoches(NULL),
      m_pformSetParameters(NULL),
      m_pformSelect(NULL),
      m_pformSettings(NULL),
      m_pformBubbleData(NULL),
      m_pIni(NULL),
      m_pCalIni(NULL),
      m_swfFilters(NULL),
      m_bBreak(false),
      m_SearchModeStimIndex(-1),
      m_bTriggerTestRunning(false),
      m_bFreeSearchRunning(false),
      m_nStimPlayIndex(-1),
      m_bUpdateStimulusDisplay(false),
      m_bDataAppended(false),
      m_bSaveMAT(false),
      m_bSaveProbeMic(true),
      m_bStartupInSitu(false),
      m_bCheckUpdateOnStartup(true),
      m_bPlotAllowed(true)
{
   EnableAllMainControls(false);
   randomize();
   #ifndef _WIN64
   m_usASCaption = AS_NAME + " (32bit)";
   #else
   m_usASCaption = AS_NAME + " (64bit)";
   #endif
   
   Caption = m_usASCaption;
   m_pslParamStr = new TStringList();
   InitializeCriticalSection(&m_cs);
   Application->OnMessage = AppMessage;

   m_vclSpikeColors.push_back(clLime);
   m_vclSpikeColors.push_back(clFuchsia);
   m_vclSpikeColors.push_back(clAqua);
   m_vclSpikeColors.push_back(clGreen);
   m_vclSpikeColors.push_back(clRed);
   m_vclSpikeColors.push_back(clBlue);

   m_pIni         = new TIniFile(GetSettingsPath() + "AudioSpike.ini");
   m_pCalIni      = new TIniFile(GetSettingsPath() + "calibration.ini");
   m_swfFilters   = new TSWFilters(GetSettingsPath() +   "filters.ini");

   ConvertIniFile();

   FormStyle      = m_pIni->ReadBool("Settings", "MDI", true) ? Vcl::Forms::fsMDIForm : Vcl::Forms::fsNormal;
   m_bFreeWindows = m_pIni->ReadBool("Settings", "FreeWindows", false);
   scb->Visible   = FormStyle == Vcl::Forms::fsNormal;

   imFloppy->Height     = sb->Height - 6;
   imFloppy->Width      = imFloppy->Height;

   SetStyle();

   m_bLevelDebug  = m_pIni->ReadBool("Debug", "LevelDebug", false);
   btnInSitu->Caption = btnInSitu->Down ? "In-Situ-Mode" : "Standard-Mode";
   sb->Panels->Items[SB_P_CONFIG]->Text = ms_usSettingsName;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, cleans up everything
//------------------------------------------------------------------------------
__fastcall TformSpikeWare::~TformSpikeWare()
{
   EnterCriticalSection(&m_cs);
   try
      {
      EnableEpocheTimer(false);
      m_smp.ExitLibrary();
      StoreFormPos(this);

      m_bFormsCreated = false;


      // cleanup standard child forms
      TRYDELETENULL(m_pformSearchFree);
      TRYDELETENULL(m_pformBatch);
      TRYDELETENULL(m_pformStimuli);
      TRYDELETENULL(m_pformSpikes);
      TRYDELETENULL(m_pformPSTH);
      TRYDELETENULL(m_pformSignalPSTH);
      TRYDELETENULL(m_pformVectorStrength);
      TRYDELETENULL(m_pformBubbleData);

      TRYDELETENULL(m_pformEpoches);

      // cleanup standard tool forms
      TRYDELETENULL(m_pformSelect);
      TRYDELETENULL(m_pformSettings);
      TRYDELETENULL(m_pformSearch);
      TRYDELETENULL(m_pformSetParameters);

      while (m_vpformCluster.size())
        RemoveClusterWindow(m_vpformCluster[0]);

      while (m_vpformBubblePlots.size())
        RemoveParamWindow(m_vpformBubblePlots[0]);


      TRYDELETENULL(m_pIni);
      TRYDELETENULL(m_pCalIni);
      TRYDELETENULL(m_swfFilters);
      TRYDELETENULL(m_pslParamStr);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      DeleteCriticalSection(&m_cs);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Called by sub-forms to set parent
//------------------------------------------------------------------------------
void TformSpikeWare::SetFormParent(TForm* pfrm)
{
   if (!IsMDI() && !m_bFreeWindows)
      pfrm->Parent = scb;
   TBorderIcons tbi = TBorderIcons() << biSystemMenu;
   if (m_bFreeWindows)
      tbi << biMinimize << biMaximize;
   pfrm->BorderIcons = tbi;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns true if form is MDI
//------------------------------------------------------------------------------
bool TformSpikeWare::IsMDI()
{
   return FormStyle == Vcl::Forms::fsMDIForm;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns true if 'free windows' is true (subwindows have / should have no parent)
//------------------------------------------------------------------------------
bool TformSpikeWare::IsFreeWindows()
{
   return m_bFreeWindows;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns true if a form is visible / not minimized
//------------------------------------------------------------------------------
bool TformSpikeWare::GetWindowVisible(TForm* pfrm)
{
   if (IsFreeWindows())
      return IsWindowVisible(pfrm->Handle);
   else
      return pfrm->WindowState != wsMinimized;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets visibility and MDI status of a form
//------------------------------------------------------------------------------
void TformSpikeWare::SetWindowVisible(TForm* pfrm, bool bVisible, bool bSetMDI)
{
   try
      {
      if (IsWindowVisible(pfrm->Handle) == bVisible)
         return;
      if (!IsMDI() || IsFreeWindows())
         bSetMDI = false;
      if (!bVisible)
         {
         if (bSetMDI)
            {
            pfrm->WindowState = wsMinimized;
            pfrm->FormStyle = Vcl::Forms::fsNormal;
            }
         pfrm->Visible = false;
         }
      else
         {
         if (bSetMDI)
            {

            pfrm->FormStyle = Vcl::Forms::fsMDIChild; // sets visible as well!
            pfrm->WindowState = wsNormal;
            }
         else
            pfrm->Visible = true;
         }
      }
   catch (Exception &e)
      {
      OutputDebugStringW(e.Message.w_str());
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enables/disabled all main menu items iand toolbar buttons
//------------------------------------------------------------------------------
void TformSpikeWare::EnableAllMainControls(bool bEnable)
{
   int n;
   for (n = 0; n < mnuMain->Items->Count; n++)
      mnuMain->Items->Items[n]->Enabled = bEnable;
   for (n = 0; n < tb->ButtonCount; n++)
      tb->Buttons[n]->Enabled = bEnable;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns internal flag, if subfroms were already created
//------------------------------------------------------------------------------
bool TformSpikeWare::FormsCreated()
{
   bool bReturn = false;
   EnterCriticalSection(&m_cs);
   try
      {
      bReturn = m_bFormsCreated;
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Crates all subforms
//------------------------------------------------------------------------------
void TformSpikeWare::CreateForms()
{
   if (m_bFormsCreated)
      return;

   // tool windows
   m_pformSetParameters = new TformSetParameters(NULL);
   m_pformSelect        = new TformSelect(NULL);
   m_pformSettings      = new TformSettings(NULL);

   // now the 'standard forms' with visibility managed by menu/ini
   m_pformSignalPSTH  = new TformSignalPSTH(NULL, miSignalPSTH);
   m_pformPSTH        = new TformPSTH(NULL, miPSTH);
   m_pformSpikes      = new TformSpikes(NULL, miSpikes);
   m_pformStimuli     = new TformStimuli(NULL, miStimuli);
   m_pformEpoches     = new TformEpocheWindow(NULL, miEpocheWindow);

   // now the forms that are ALWAYS invisible on start
   m_pformSearchFree    = new TformSearchFree(NULL);
   m_pformBubbleData = new TformBubbleData(NULL);
   m_pformVectorStrength = new TformVectorStrength(NULL);

   m_pformPSTH->Initialize(1);
   m_pformSignalPSTH->Initialize();


   m_pformSpikes->Initialize();

   m_bFormsCreated    = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback: call Initialize on Show only ONCE (store in Tag, if already done)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::FormShow(TObject *Sender)
{
   if (!Tag)
      {
      Tag = 1;
      Initialize();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// cleans up suforms
//------------------------------------------------------------------------------
void TformSpikeWare::Cleanup(void)
{
   // NOTE: param and cluster wnidows to be removed first: might try to access
   // spikes!
   while (m_vpformBubblePlots.size())
      RemoveParamWindow(m_vpformBubblePlots[0]);

   while (m_vpformCluster.size())
      RemoveClusterWindow(m_vpformCluster[0]);

   m_swsSpikes.Clear();
   m_sweEpoches.Clear();
   if (FormsCreated())
      {
      m_pformSpikes->Clear();
      m_pformPSTH->Clear();
      m_pformSignalPSTH->Clear();
      m_pformBubbleData->Clear();
      m_pformBubbleData->Close();
      m_pformVectorStrength->Close();
      }

   m_swsStimuli.Clear();
   if (FormsCreated())
      {
      m_pformStimuli->Clear();
      m_pformEpoches->Clear();
      }

   SetGUIStatus(SWGS_NONE);

   m_viStimSequence.clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnCloseQuery callback. Shows hint for stopping meas and/or daving data
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::FormCloseQuery(TObject *Sender, bool &CanClose)
{
   if (m_smp.Playing())
      {
      CanClose = false;
      SWErrorBox("Please stop current measurement/search before quitting the programm", Handle);
      }

   if (CanClose)
      CanClose = SaveInquiry() != ID_CANCEL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// stores passed form's position and size in INI file
//------------------------------------------------------------------------------
void TformSpikeWare::StoreFormPos(TForm* pfrm)
{
   UnicodeString usSection = GetFormIniSection(pfrm);
   if (pfrm->ClientHeight < 10)
      return;
   if (pfrm == this)
      m_pIni->WriteBool(usSection, "Maximized", pfrm->WindowState == wsMaximized);

   if (pfrm->WindowState == wsNormal)
      {
      int nLeft   = pfrm->Left > 0 ? pfrm->Left : 0;
      int nTop    = pfrm->Top  > 0 ? pfrm->Top  : 0;
      m_pIni->WriteInteger(usSection, "Left", nLeft);
      m_pIni->WriteInteger(usSection, "Top", nTop);
      m_pIni->WriteInteger(usSection, "Height", pfrm->Height);
      m_pIni->WriteInteger(usSection, "Width", pfrm->Width);
      }

   // use menu item if exists: in 'free-window-mode' forms are invisible already
   // in destructor!!
   TformASUI* pfrmASUI = dynamic_cast<TformASUI*>(pfrm);
   if (!!pfrmASUI && !!pfrmASUI->m_pmiMainFormMenu)
      m_pIni->WriteBool(usSection, "Visible", pfrmASUI->m_pmiMainFormMenu->Checked);
   else
      m_pIni->WriteBool(usSection, "Visible", pfrm->Visible);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// restores passed form's position and size from INI file values
//------------------------------------------------------------------------------
void TformSpikeWare::RestoreFormPos(TForm* pfrm)
{
   UnicodeString usSection = GetFormIniSection(pfrm);
   UnicodeString usDefault = usSection;

   int nPos = usSection.Pos("_") - 1;
   if (nPos > 0)
      usDefault = usSection.SubString(1, nPos) ;
   usDefault += ".";

   pfrm->Left   = m_pIni->ReadInteger(usSection, "Left",   m_pIni->ReadInteger("Defaults", usDefault+"Left",   0));
   pfrm->Top    = m_pIni->ReadInteger(usSection, "Top",    m_pIni->ReadInteger("Defaults", usDefault+"Top",   0));
   pfrm->Height = m_pIni->ReadInteger(usSection, "Height", m_pIni->ReadInteger("Defaults", usDefault+"Height",   400));
   pfrm->Width  = m_pIni->ReadInteger(usSection, "Width",  m_pIni->ReadInteger("Defaults", usDefault+"Width",   400));

   if (pfrm == this)
      {
      if (m_pIni->ReadBool(usSection, "Maximized", false))
         WindowState = wsMaximized;
      }
   
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// restores form visibility from INI file values
//------------------------------------------------------------------------------
void TformSpikeWare::RestoreFormVis(TformASUI* pfrm)
{
   UnicodeString usSection = GetFormIniSection(pfrm);
   UnicodeString usDefault = usSection;
   int nPos = usSection.Pos("_") - 1;
   if (nPos > 0)
      usDefault = usSection.SubString(1, nPos) ;
   usDefault += ".";

   bool bVis = m_pIni->ReadBool(usSection, "Visible", m_pIni->ReadInteger("Defaults", usDefault+"Visible",  true));
   SetWindowVisible(pfrm, bVis);

   if (!!pfrm->m_pmiMainFormMenu)
      pfrm->m_pmiMainFormMenu->Checked = bVis;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// stores chart axis properties in INI file
//------------------------------------------------------------------------------
void TformSpikeWare::StoreChartAxis(TForm* pfrm, TChart *pchrt, bool bLeft, bool bBottom)
{
   UnicodeString usSection = GetFormIniSection(pfrm);

   if (bBottom)
      {
      m_pIni->WriteString(usSection, "XMin", DoubleToStr(pchrt->BottomAxis->Minimum));
      m_pIni->WriteString(usSection, "XMax", DoubleToStr(pchrt->BottomAxis->Maximum));
      }
   if (bLeft)
      {
      m_pIni->WriteString(usSection, "YMin", DoubleToStr(pchrt->LeftAxis->Minimum));
      m_pIni->WriteString(usSection, "YMax", DoubleToStr(pchrt->LeftAxis->Maximum));
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// restores chart axis properties from INI file values
//------------------------------------------------------------------------------
void TformSpikeWare::RestoreChartAxis(TForm* pfrm, TChart *pchrt, bool bLeft, bool bBottom)
{
   UnicodeString usSection = GetFormIniSection(pfrm);
   // min AND max must be present
   double dMin, dMax;
   UnicodeString us;
   if (bBottom)
      {
      us = m_pIni->ReadString(usSection, "XMin", "");
      if (TryStrToDouble(us, dMin))
         {
         us = m_pIni->ReadString(usSection, "XMax", "");
         if (TryStrToDouble(us, dMax))
            pchrt->BottomAxis->SetMinMax(dMin, dMax);
         }
      }
   if (bLeft)
      {
      us = m_pIni->ReadString(usSection, "YMin", "");
      if (TryStrToDouble(us, dMin))
         {
         us = m_pIni->ReadString(usSection, "YMax", "");
         if (TryStrToDouble(us, dMax))
            pchrt->LeftAxis->SetMinMax(dMin, dMax);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Main Initialze function: only to be called ONCE. Reads global flags, 
/// initializes SMP, creates subforms
//------------------------------------------------------------------------------
void TformSpikeWare::Initialize()
{
   RestoreFormPos(this);

   ReadSettings();


   if (m_bStartupInSitu)
      SetInSitu(true, true);

   // settings only to be read ONCE
   double dPreThreshold    = IniReadDouble(m_pIni, "Settings", "PreThreshold", 0.0005);
   double dPostThreshold   = IniReadDouble(m_pIni, "Settings", "PostThreshold", 0.0);
   double dSpikeLength     = IniReadDouble(m_pIni, "Settings", "SpikeLength", 0.0025);
   m_swsSpikes.SetSpikeLength(dPreThreshold, dPostThreshold, dSpikeLength);

   m_smp.InitLibrary();

   CreateForms();
   EnableAllMainControls(true);
   SetGUIStatus();

   if (m_bCheckUpdateOnStartup)
      miUpdateCheckClick(NULL);
   
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads global settings from INI file
//------------------------------------------------------------------------------
void TformSpikeWare::ReadSettings()
{
   SetStyle();
   m_usFixResultPath    = m_pIni->ReadString("Settings", "FixResultPath", "");

   m_usResultPathRoot   = m_pIni->ReadString("Settings", "ResultPathRoot", ExpandFileName(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "..\\Results\\"));
   m_usResultPathRoot   = IncludeTrailingBackslash(ExpandFileName(m_usResultPathRoot));
   bool bAutoPath       = m_pIni->ReadBool("Settings", "AutoTemplatePath", false);
   if (bAutoPath)
      m_usTemplatePath     = m_pIni->ReadString("Settings", "TemplatePath", ExpandFileName(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "..\\Templates\\"));
   else
      m_usTemplatePath     = m_pIni->ReadString("Settings", "LastTemplatePath", ExpandFileName(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "..\\Templates\\"));
   m_bSaveMAT           = m_pIni->ReadBool("Settings", "SaveMATFile", false);

   m_bSaveProbeMic      = m_pIni->ReadBool("Settings", "SaveProbeMic", true);
   m_bStartupInSitu     = m_pIni->ReadBool("Settings", "StartupInSitu", false);
   m_bCheckUpdateOnStartup = m_pIni->ReadBool("Settings", "CheckUpdateOnStartup", true);
   if (m_bCheckUpdateOnStartup)
      {
      try
         {
         TDateTime dt = m_pIni->ReadDateTime("Settings", "LastUpdateCheckDate", 0);
         if (DaysBetween(dt, Now()) < 1)
            m_bCheckUpdateOnStartup = false;
         }
      catch (...)
         {
         }
      }
   ;   

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets style from INI
//------------------------------------------------------------------------------
void TformSpikeWare::SetStyle()
{
   UnicodeString us = m_pIni->ReadString("Settings", "Style", "Windows");
   // clear deprectaed style
   if (us == "Hoertech")
      us = "Windows";
   if (!us.IsEmpty())
      TStyleManager::TrySetStyle(us);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// processes commands passed on command line either from 'real' command line
/// or from m_pslParamStr: a call to second instance may create a file with
/// command line parameters and notifies THSI instance to read it to m_pslParamStr
//------------------------------------------------------------------------------
void TformSpikeWare::ProcessCommandLine(bool bReadFromCommandLine)
{
   if (bReadFromCommandLine)
      {
      m_pslParamStr->Clear();
      int n;
      for (n = 0; n < ParamCount(); n++)
         m_pslParamStr->Add(ParamStr(n+1));
      }

   if (!m_pslParamStr->Values["resultpath"].IsEmpty())
      m_usFixResultPath = m_pslParamStr->Values["resultpath"];

   // NOTE: first call 'show' to have everything intialized!!! NEVER REMOVE HERE
   Show();
   UnicodeString usCommand = m_pslParamStr->Values["command"];
   if (usCommand.IsEmpty())
      return;
   if (usCommand == "measure" )
      LoadMeasurementTemplate(m_pslParamStr->Values["file"]);
   else if (usCommand == "append" )
      AppendMeasurement(m_pslParamStr->Values["file"]);
   else if (usCommand == "result" )
      LoadMeasurementResult(m_pslParamStr->Values["file"]);
   else if (usCommand == "result_save" )
      {
      LoadMeasurementResult(m_pslParamStr->Values["file"]);
      SaveResult(true);
      Close();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets threshold with optional saving to INI file
//------------------------------------------------------------------------------
void TformSpikeWare::SetThreshold(unsigned int nChannelIndex, double dThreshold, bool bSave)
{
   m_sweEpoches.SetThreshold(nChannelIndex, dThreshold);
   if (FormsCreated())
      {
      m_pformEpoches->UpdateThreshold(nChannelIndex, dThreshold);
      m_pformSpikes->UpdateThreshold();
      }
   if (bSave)
      {
      m_pIni->WriteString("Settings", "Threshold_" + IntToStr((int)nChannelIndex), DoubleToStr(dThreshold));
      m_pIni->WriteString("Settings", "Threshold_" + IntToStr((int)nChannelIndex), DoubleToStr(dThreshold));
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns threshold for a channel
//------------------------------------------------------------------------------
double TformSpikeWare::GetThreshold(unsigned int nChannelIndex)
{
   return m_sweEpoches.GetThreshold(nChannelIndex);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns reference to vector with thresholds for all channels
//------------------------------------------------------------------------------
std::vector<double >& TformSpikeWare::GetThresholds()
{
   return m_sweEpoches.m_vdThreshold;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///  calls LoadMeasurement with type SWLM_TEMPLATE
//------------------------------------------------------------------------------
bool TformSpikeWare::LoadMeasurementTemplate(UnicodeString us)
{
   if (SaveInquiry() == ID_CANCEL)
      return false;
   m_bDataAppended = false;
   if (!LoadMeasurement(us, SWLM_TEMPLATE))
      return false;


   SetGUIStatus(SWGS_LOADED);
   Caption = m_usASCaption + " - " + xml->FileName;
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///  calls LoadMeasurement with type SWLM_RESULT
//------------------------------------------------------------------------------
bool TformSpikeWare::LoadMeasurementResult(UnicodeString us)
{
   if (SaveInquiry() == ID_CANCEL)
      return false;
   m_bDataAppended = false;


   if (!LoadMeasurement(us, SWLM_RESULT))
      return false;

   EnsureXMLEpocheThresholds();

   m_usResultPath = IncludeTrailingBackslash(ExtractFilePath(xml->FileName));
   Caption = m_usASCaption + " - " + xml->FileName;

   SetGUIStatus(SWGS_RESULTLOADED);

   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads XMLs of diffeent types
//------------------------------------------------------------------------------
bool TformSpikeWare::LoadMeasurement(UnicodeString usFile, int nMode)
{
   if (!FormsCreated())
      return false;
   bool bReturn = false;
   m_pformEpoches->EnableEpocheScrolling(false);
   m_pformEpoches->tbEpoches->Tag      = 0;

   m_pformSpikes->cbPlotEpocheSpikesOnly->Enabled  = false;
   m_pformSpikes->cbPlotEpocheSpikesOnly->Checked  = false;
   m_pformEpoches->cbEpocheThreshold->Enabled  = false;


   TCursor cr = Screen->Cursor;
   bool bAnyThresholdMissing = false;

   try
      {
      try
         {
         Screen->Cursor = crHourGlass;
         formWait->ShowWait("Loading measurement, please wait...");

         Cleanup();

         // settings needed for template AND resume
         if (nMode != SWLM_RESULT)
            {
            // NOTE: ReadSettings shows error itself
            if (!m_smp.ReadSettings())
               return bReturn;
            }

         if (nMode != SWLM_RESUME)
            {
            usFile = ExpandFileName(usFile);
            if (!FileExists(usFile))
               throw Exception("File '" + usFile + "' not found");


            xml->Active = false;
            xml->XML->Text = L"";
            xml->LoadFromFile(usFile);
            xml->Active = true;
            AdjustStimulusFileNames(xml->DocumentElement, IncludeTrailingBackslash(ExtractFilePath(usFile)));
            }


         _di_IXMLNode xmlDoc = xml->DocumentElement;

         if (!xmlDoc || xmlDoc->GetNodeName() != AS_NAME)
            throw Exception("'" + usFile + "' is not a " + AS_NAME + " file");


         _di_IXMLNode xmlResultNode = xmlDoc->ChildNodes->FindNode("Result");
         if (nMode && !xmlResultNode)
            throw Exception(AS_NAME + " file contains no result");

         _di_IXMLNode xmlSettings = xmlDoc->ChildNodes->FindNode("Settings");
         if (!xmlSettings)
            throw Exception("Settings missing");

         // NOTE: if here in-situ mode is changed, then we have to force ReadSettings in SMP!
         bool bInSitu = IsInSitu();
         SetInSitu(GetXMLValue(xmlSettings, "InSitu") == "1");
         if (bInSitu != IsInSitu() && nMode != SWLM_RESULT)
            {
            if (!m_smp.ReadSettings(true, true))
               return bReturn;
            }
         // set SpikeLength and Pre-Stim if present. NOTE: milliseconds!!
         double dPreThreshold, dPostThreshold, dSpikeLength;
         if (TryStrToDouble(GetXMLValue(xmlSettings, "SpikeLength"), dSpikeLength))
            dSpikeLength /= 1000.0;
         else
            dSpikeLength = m_swsSpikes.m_dSpikeLength;

         if (TryStrToDouble(GetXMLValue(xmlSettings, "PreThreshold"), dPreThreshold))
            dPreThreshold /= 1000.0;
         else
            dPreThreshold = m_swsSpikes.m_dPreThreshold;

         if (TryStrToDouble(GetXMLValue(xmlSettings, "PostThreshold"), dPostThreshold))
            dPostThreshold /= 1000.0;
         else
            dPostThreshold = m_swsSpikes.m_dPostThreshold;

         m_swsSpikes.SetSpikeLength(dPreThreshold, dPostThreshold, dSpikeLength);

         double dSampleRate, dSampleRateDevider;

         // TODO : value/range/mandatory check!!
         if (!TryStrToDouble(GetXMLValue(xmlSettings, "SampleRate"), dSampleRate))
            throw Exception("'SampleRate' missing or invalid in 'Settings'");
         if (!TryStrToDouble(GetXMLValue(xmlSettings, "SampleRateDevider"), dSampleRateDevider))
            dSampleRateDevider = (double)m_smp.m_fDefaultSampleRateDevider;



         m_swsSpikes.SetSampleRate(dSampleRate, dSampleRateDevider);
         m_swsStimuli.m_dDeviceSampleRate = dSampleRate;


         m_swsStimuli.m_nNumRepetitions = 1;
         m_swsStimuli.m_nRandom = 0;
         int nVal;
         if (TryStrToInt(GetXMLValue(xmlSettings, "StimulusRepetition"), nVal))
            m_swsStimuli.m_nNumRepetitions = (unsigned int)nVal;
         if (TryStrToInt(GetXMLValue(xmlSettings, "RandomMode"), nVal))
            m_swsStimuli.m_nRandom = (unsigned int)nVal;

         double dEpocheLength, dPreStimulus, dRepetitionPeriod;
         if (!TryStrToDouble(GetXMLValue(xmlSettings, "EpocheLength"), dEpocheLength))
            throw Exception("'EpocheLength' missing or invalid in 'Settings'");
         if (!TryStrToDouble(GetXMLValue(xmlSettings, "PreStimulus"), dPreStimulus))
            throw Exception("'PreStimulus' missing or invalid in 'Settings'");
         if (!TryStrToDouble(GetXMLValue(xmlSettings, "RepetitionPeriod"), dRepetitionPeriod))
            throw Exception("'RepetitionPeriod' missing or invalid in 'Settings'");


         // check for consistancy. check vs. stim-length is done later
         if (dEpocheLength < dPreStimulus)
            throw Exception("'PreStimulus' must not exceed 'EpocheLength'");
         if (dRepetitionPeriod < dEpocheLength)
            throw Exception("'EpocheLength' must not exceed 'RepetitionPeriod'");

         m_sweEpoches.m_dEpocheLength     = dEpocheLength;
         m_sweEpoches.m_dPreStimulus      = dPreStimulus;
         // repetitionperiod needed in samples!!
         m_sweEpoches.m_nRepetitionPeriod = (int)(dRepetitionPeriod * m_swsSpikes.GetSampleRate());

         unsigned int n;

         // read device channels: here we 'translate' the indices from measurement
         // (1-based) to SMP indices (0-based)
         m_smp.m_viMeasChannelsOutUsed.clear();
         UnicodeString us = GetXMLValue(xmlSettings, "OutputChannels");
         if (us == "")
            throw Exception("No 'OutputChannels' specified");
         std::vector<int > vi;

         ParseIntValues(vi, us, "OutputChannels", ' ');

         unsigned int nChannelsOut = (unsigned int)vi.size();
         // last value must not exceed available channels from settings (not checked
         // for results!)
         // needed for template AND resume
         double dCal;
         if (nMode != SWLM_RESULT)
            {
            int nNumOutputs = (int)m_smp.m_swcHWChannels.GetOutputs().size();
            #ifdef CHKCHNLS
            if (m_smp.m_swcHWChannels.GetOutputs().size() != m_smp.m_viChannelsOutSettings.size())
               ShowMessage("error A " + UnicodeString(__FUNC__));
            #endif

            std::sort(vi.begin(), vi.end());
            if (vi.back() > nNumOutputs)
               throw Exception("Invalid 'OutputChannels' specified (maximum available channel index is " + IntToStr(nNumOutputs) + ")");
            for (n = 0; n < vi.size(); n++)
               m_smp.m_viMeasChannelsOutUsed.push_back(vi[n]-1);
            }

         // same for input channels
         m_smp.m_viMeasChannelsInUsed.clear();
         us = GetXMLValue(xmlSettings, "InputChannels");
         if (us == "")
            throw Exception("No 'InputChannels' specified");
         ParseIntValues(vi, us, "InputChannels", ' ');
         unsigned int nChannelsIn = (unsigned int)vi.size();
         // last value must not exceed available channels from settings (not checked
         // for results!)
         // needed for template AND resume
         if (nMode != SWLM_RESULT)
            {
            int nNumInputs = (int)m_smp.m_swcHWChannels.GetElectrodes().size();
            #ifdef CHKCHNLS
            if (m_smp.m_swcHWChannels.GetElectrodes().size() != m_smp.m_viChannelsInSettings.size())
               ShowMessage("error A " + UnicodeString(__FUNC__));
            #endif
            std::sort(vi.begin(), vi.end());
            if (vi.back() > nNumInputs)
               throw Exception("Invalid 'InputChannels' specified (maximum available channel index is " + IntToStr(nNumInputs) + ")");
            for (n = 0; n < nChannelsIn; n++)
               m_smp.m_viMeasChannelsInUsed.push_back(vi[n]-1);
            }

         m_sweEpoches.Initialize(nChannelsIn, (unsigned int)(dEpocheLength * m_swsSpikes.GetSampleRate()));

         m_swsSpikes.SetNumChannels(nChannelsIn);

         m_pformEpoches->Initialize((int)nChannelsIn);


         // setting of thresholds
         // - if 'UseLastThreshold is specified,

         // load thresholds (optional)
         us = GetXMLValue(xmlSettings, "Thresholds");
         if (us != "")
            {
            vved vvedThresholds = ParseMLVector(us, "Thresholds");
            if (vvedThresholds.size() != m_sweEpoches.GetNumChannels())
               throw Exception("invalid number of Thresholds found in settings");
            for (n = 0; n < nChannelsIn; n++)
               {
               if (!vvedThresholds[n].size())
                  throw Exception("invalid number of Thresholds found in settings");
               SetThreshold(n, vvedThresholds[n][0], false);
               }
            }
         else
            {
            for (n = 0; n < nChannelsIn; n++)
               {
               if (m_pIni->ReadString("Settings", "Threshold_" + IntToStr((int)n), "") == "")
                  bAnyThresholdMissing = true;
               SetThreshold(n, IniReadDouble(m_pIni, "Settings", "Threshold_" + IntToStr((int)n), 0.4), false);
               }
            }


         // add stimuli AND stimulus parameters passing nodes and maximum allowed length of the
         // stimulus itself and the number of output channels
         m_swsStimuli.Add( xmlDoc,
                           m_sweEpoches.m_dEpocheLength-m_sweEpoches.m_dPreStimulus,
                           nChannelsOut,
                           // audio data needed for template AND resume
                           nMode
                           );

         // add all stimuli to formStimuli
         m_pformStimuli->Load();
         m_pformStimuli->Caption = "Stimuli";

         // re-initialize windows (all that need an update)
         for (n = 0; n < m_vpformCluster.size(); n++)
            m_vpformCluster[n]->Initialize(nChannelsIn);
         m_pformEpoches->Initialize();
         m_pformSpikes->Initialize();
         m_pformPSTH->Initialize(nChannelsIn);
         m_pformSignalPSTH->Initialize();

         if (!!xmlResultNode)
            {
            // if loading a result read the spikes
            if (nMode)
               {
               // read sequence (doublettes allowed)!
               ParseIntValues(m_viStimSequence, GetXMLValue(xmlResultNode, "StimulusSequence"), "StimulusSequence", ' ', false);
               // NOTE: Stimulus-Sequence was written 1-based!!!
               for (n = 0; n < m_viStimSequence.size(); n++)
                  m_viStimSequence[n] -= 1;

               // LoadSpikes!!
               _di_IXMLNode xmlSpikes = xmlResultNode->ChildNodes->FindNode("Spikes");
               if (!!xmlSpikes)
                  m_swsSpikes.Add(xmlSpikes);
               // AND load UnSelectedSpikes
               xmlSpikes = xmlResultNode->ChildNodes->FindNode("NonSelectedSpikes");
               if (!!xmlSpikes)
                  m_swsSpikes.Add(xmlSpikes);
               }

            double dValue;
            int nCluster, nValue;
            unsigned int nChannel, nSel;
            // load Cluster windows selections
            _di_IXMLNode xmlClusters = xmlResultNode->ChildNodes->FindNode("Clusters");
            if (!!xmlClusters)
               {
               for (nCluster = 0; nCluster < xmlClusters->ChildNodes->Count; nCluster++)
                  {
                  _di_IXMLNode xmlCluster = xmlClusters->ChildNodes->Nodes[nCluster];
                  int nX = m_swsSpikes.m_swspSpikePars.IndexFromID(GetXMLValue(xmlCluster, "X"));
                  int nY = m_swsSpikes.m_swspSpikePars.IndexFromID(GetXMLValue(xmlCluster, "Y"));
                  CreateClusterWindow(nX, nY, nChannelsIn);

                  if (TryStrToDouble(GetXMLValue(xmlCluster, "Xmin"), dValue))
                     m_vpformCluster.back()->chrt->BottomAxis->Minimum = dValue;
                  if (TryStrToDouble(GetXMLValue(xmlCluster, "Xmax"), dValue))
                     m_vpformCluster.back()->chrt->BottomAxis->Maximum = dValue;
                  if (TryStrToDouble(GetXMLValue(xmlCluster, "Ymin"), dValue))
                     m_vpformCluster.back()->chrt->LeftAxis->Minimum = dValue;
                  if (TryStrToDouble(GetXMLValue(xmlCluster, "Ymax"), dValue))
                     m_vpformCluster.back()->chrt->LeftAxis->Maximum = dValue;

                  _di_IXMLNode xmlChannels = xmlCluster->ChildNodes->Nodes["Channels"];
                  for (nChannel = 0; nChannel < (unsigned int)xmlChannels->ChildNodes->Count; nChannel++)
                     {
                     // should not happen ...
                     if (nChannel >= m_vpformCluster.back()->m_vbSelActive.size())
                        break;
                     _di_IXMLNode xmlChannel = xmlChannels->ChildNodes->Nodes[nChannel];
                     if (TryStrToInt(GetXMLValue(xmlChannel, "Active"), nValue))
                        m_vpformCluster.back()->m_vbSelActive[nChannel] = nValue;

                     _di_IXMLNode xmlSelections = xmlChannel->ChildNodes->Nodes["Selections"];
                     for (nSel = 0; nSel < (unsigned int)xmlSelections->ChildNodes->Count; nSel++)
                        {
                        if (nSel >= m_vpformCluster.back()->m_vvSWSelections[nChannel].size())
                           break;

                        _di_IXMLNode xmlSel = xmlSelections->ChildNodes->Nodes[nSel];

                        if (TryStrToDouble(GetXMLValue(xmlSel, "X0"), dValue))
                           m_vpformCluster.back()->m_vvSWSelections[nChannel][nSel].dX0 = dValue;
                        if (TryStrToDouble(GetXMLValue(xmlSel, "X1"), dValue))
                           m_vpformCluster.back()->m_vvSWSelections[nChannel][nSel].dX1 = dValue;
                        if (TryStrToDouble(GetXMLValue(xmlSel, "Y0"), dValue))
                           m_vpformCluster.back()->m_vvSWSelections[nChannel][nSel].dY0 = dValue;
                        if (TryStrToDouble(GetXMLValue(xmlSel, "Y1"), dValue))
                           m_vpformCluster.back()->m_vvSWSelections[nChannel][nSel].dY1 = dValue;
                        if (TryStrToInt(GetXMLValue(xmlSel, "Active"), nValue))
                           m_vpformCluster.back()->m_vvSWSelections[nChannel][nSel].bActive = nValue;
                        }
                     }
                  }
               }

            // load parameter windows
            _di_IXMLNode xmlParameterWindows = xmlResultNode->ChildNodes->Nodes["ParameterWindows"];
            int nWindow, nX, nY;
            for (nWindow = 0; nWindow < xmlParameterWindows->ChildNodes->Count; nWindow++)
               {
               _di_IXMLNode xmlParameterWindow = xmlParameterWindows->ChildNodes->Nodes[nWindow];
               // X/Y-Stim-param combinations
               nX = m_swsStimuli.m_swspStimPars.IndexFromName(GetXMLValue(xmlParameterWindow, "X"));

               UnicodeString usY = GetXMLValue(xmlParameterWindow, "Y");
               if (usY == "Response")
                  nY = (int)m_swsStimuli.m_swspStimPars.m_vusNames.size();
               else
                  nY = m_swsStimuli.m_swspStimPars.IndexFromName(GetXMLValue(xmlParameterWindow, "Y"));
               if (nX >= 0 && nY >= 0)
                  CreateParameterWindow(nX, nY);
               }

            // load PSTH selections
            _di_IXMLNode xmlPSTH = xmlResultNode->ChildNodes->FindNode("PSTH");
            if (!!xmlPSTH)
               {
               _di_IXMLNode xmlChannels = xmlPSTH->ChildNodes->FindNode("Channels");
               if (!!xmlChannels)
                  {
                  for (nChannel = 0; nChannel < nChannelsIn; nChannel++)
                     {
                     // should not happen ...
                     if (  nChannel >= (unsigned int)xmlChannels->ChildNodes->Count
                        || nChannel >= m_pformPSTH->m_vSWSelections.size())
                        break;

                     _di_IXMLNode xmlChannel = xmlChannels->ChildNodes->Nodes[nChannel];

                     if (TryStrToDouble(GetXMLValue(xmlChannel, "Selection_X0"), dValue))
                        m_pformPSTH->m_vSWSelections[nChannel].dX0 = dValue;
                     if (TryStrToDouble(GetXMLValue(xmlChannel, "Selection_X1"), dValue))
                        m_pformPSTH->m_vSWSelections[nChannel].dX1 = dValue;
                     if (TryStrToInt(GetXMLValue(xmlChannel, "Selection_Active"), nValue))
                        m_pformPSTH->m_vSWSelections[nChannel].bActive = nValue;
                     if (TryStrToDouble(GetXMLValue(xmlChannel, "NoiseSelection_X0"), dValue))
                        m_pformPSTH->m_vSWNoiseSelections[nChannel].dX0 = dValue;
                     if (TryStrToDouble(GetXMLValue(xmlChannel, "NoiseSelection_X1"), dValue))
                        m_pformPSTH->m_vSWNoiseSelections[nChannel].dX1 = dValue;
                     if (TryStrToInt(GetXMLValue(xmlChannel, "NoiseSelection_Active"), nValue))
                        m_pformPSTH->m_vSWNoiseSelections[nChannel].bActive = nValue;
                     }
                  }
               }
            }

         // for templates create default cluster window, if none created
         if (nMode == SWLM_TEMPLATE && m_vpformCluster.size() == 0)
            {
            int nX = m_swsSpikes.m_swspSpikePars.IndexFromID(m_pIni->ReadString("Settings", "DefaultClusterX", "Peak1"));
            int nY = m_swsSpikes.m_swspSpikePars.IndexFromID(m_pIni->ReadString("Settings", "DefaultClusterY", "Peak2"));
            CreateClusterWindow(nX, nY, nChannelsIn);
            }


         for (n = (unsigned int)m_pformEpoches->m_vpformEpoches.size(); n > 0; n--)
            SetEvalWindow(m_pformEpoches->m_vpformEpoches[n-1]);

         m_pformPSTH->Plot(0, true);
         m_pformSignalPSTH->Plot(0);

         PlotBubblePlots();
         }
      __finally
         {
         formWait->Hide();
         Screen->Cursor = cr;
         }
      if (bAnyThresholdMissing)
         MessageBoxW(Handle, L"At least one threshold was never specified and set to 0.4 as default", L"Warning", MB_ICONWARNING);

      Application->ProcessMessages();
      SetMeasurementChanged(false);

      if (m_swsStimuli.m_bRMSMissing)
         {
         if (m_pIni->ReadBool("Settings", "ShowRMSWarning", true))
            m_pIni->WriteBool("Settings", "ShowRMSWarning", RMSWarning());
         }

      bReturn = true;
      }
   catch (Exception &e)
      {
      bReturn = false;
      Cleanup();
      SWErrorBox(usFile + ": " + e.Message);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// appends XML to existing XML result
//------------------------------------------------------------------------------
bool TformSpikeWare::AppendMeasurement(UnicodeString usFile)
{
   bool bReturn = false;
   try
      {
      if (!btnAppend->Enabled)
         throw Exception("Appending data currently not possible!");

      if (imFloppy->Visible)
         throw Exception("Please save current measurement before appending data!");

      xml->Active = true;
      _di_IXMLNode xmlDoc        = xml->DocumentElement;
      if (!xmlDoc)
         throw Exception("Current measurement does not contain a valid result. Data cannot be appended.");
      _di_IXMLNode xmlAllStimuli = xmlDoc->ChildNodes->FindNode("AllStimuli");
      _di_IXMLNode xmlParams     = xmlDoc->ChildNodes->FindNode("Parameters");
      _di_IXMLNode xmlResult     = xmlDoc->ChildNodes->FindNode("Result");

      if (!xmlAllStimuli || !xmlParams || !xmlResult)
         throw Exception("Current measurement does not contain a valid result. Data cannot be appended.");

      usFile = ExpandFileName(usFile);

      if (!FileExists(usFile))
         throw Exception("File to append cannot be found: " + usFile);

      Caption = Caption + " + " + usFile;

      xmlAppend->Active = false;
      xmlAppend->XML->LoadFromFile(usFile);
      xmlAppend->Active = true;

      // adjust filenames in 'Stimuli'
      AdjustStimulusFileNames(xmlAppend->DocumentElement, IncludeTrailingBackslash(ExtractFilePath(usFile)));

      _di_IXMLNode xmlAppendDoc  = xmlAppend->DocumentElement;
      _di_IXMLNode xmlSettings   = xmlAppendDoc->ChildNodes->FindNode("Settings");
      _di_IXMLNode xmlStimuli    = xmlAppendDoc->ChildNodes->FindNode("Stimuli");


      // vector for the new repetition indices
      std::vector<int > vnRepetitionIndices;

      bool bNewData = false;
      // do a loop through 'new stimuli' and 'new level'
      int nMode, nNumExistingStimuli;
      for (nMode = 0; nMode < 2; nMode++)
         {
         vnRepetitionIndices.clear();
         // store number of stimuli BEFORE adding new stimuli
         nNumExistingStimuli = xmlAllStimuli->ChildNodes->Count;
         // first add stimuli
         if (nMode == 0)
            {
            // if Stimuli subnode not present or no subnodes, skip it
            _di_IXMLNode xmlTmp = xmlAppendDoc->ChildNodes->FindNode("Stimuli");
            if (!xmlStimuli || !xmlStimuli->ChildNodes->Count)
               continue;

            m_swsStimuli.CreateLevelStimuli(xmlDoc, xmlAppendDoc);

            bNewData = true;
            }
         // then levels
         else
            {
            if (!xmlSettings)
               continue;
            UnicodeString usLevels = GetXMLValue(xmlSettings, "Level");

            if (usLevels.IsEmpty())
               continue;
            bNewData = true;

            vved vvedLevels   = ParseMLVector(usLevels, "Level");
            unsigned int nChannelLevels  = (unsigned int)vvedLevels.size();
            if (!nChannelLevels)
               throw Exception("'Level' does not contain valid lavels in 'Level' parameter");
            // number of channels must be either 1 (then identical level is used
            // for ALL channels) or identical to number of channels
            if (nChannelLevels != 1 && nChannelLevels != m_swsStimuli.m_nNumChannels)
               throw Exception("'Level' must contain either exactly one column or one column for each used output channel");

            // create level string to be appended to XML by removing brackets
            UnicodeString usNewLevel = usLevels;
            RemoveTrailingDelimiter(usNewLevel, L']');
            RemoveLeadingDelimiter(usNewLevel, L'[');
            RemoveTrailingDelimiter(usNewLevel, L' ');

            // find level node
            int nNumPars = xmlParams->ChildNodes->Count;
            int nNode;
            bool bFound = false;
            for (nNode = 0; nNode < nNumPars; nNode++)
               {
               _di_IXMLNode xmlPar = xmlParams->ChildNodes->Nodes[nNode];
               if (xmlPar->GetNodeName() != "Parameter")
                  throw Exception("Invalid subnode name in Parameters");
               if (GetXMLValue(xmlPar, "Name") == "Level")
                  {
                  bFound = true;
                  UnicodeString usLevel = GetXMLValue(xmlPar, "Level");
                  RemoveTrailingDelimiter(usLevel, L']');
                  usLevel += ";" + usNewLevel + "]";
                  xmlPar->ChildValues["Level"] = usLevel;

                  break;
                  }
               }
            if (!bFound)
               throw Exception("Level parameter not found in current measurement");

            // 2. add stimuli to AllStimuli
            m_swsStimuli.AddLevelStimuli(xmlDoc, vvedLevels);
            }

         // Adjust StimulusSequence
         int nNumNewStimuli = xmlAllStimuli->ChildNodes->Count - nNumExistingStimuli;

         // build vector with stimuli sequence for all new stimuli including repetitions
         std::vector<int > vn;
         CreateStimulusSequence(vn, (unsigned int)nNumNewStimuli, nNumExistingStimuli);
         // push it to m_viStimSequence and build string to append in XML
         unsigned int nStim;
         UnicodeString usSequence;
         for (nStim = 0; nStim < vn.size(); nStim++)
            {
            usSequence += IntToStr(vn[nStim]+1) + " ";
            m_viStimSequence.push_back(vn[nStim]);
            // create repetition indices 1-based
            vnRepetitionIndices.push_back(((int)nStim/nNumNewStimuli) + 1);
            }

         // append new sequence to exisiting sequence in XML
         UnicodeString us = GetXMLValue(xmlResult, "StimulusSequence");
         RemoveTrailingDelimiter(us, L']');
         us += " " + Trim(usSequence) + "]";
         xmlResult->ChildValues["StimulusSequence"] = us;

         // create new epoches
         CreateXMLEpoches(&vnRepetitionIndices);
         }

      if (!bNewData)
         {
         SWErrorBox("No new levels or stimuli found to be appended.");
         return true;
         }

      if (LoadMeasurement("", SWLM_RESUME))
         {
         LoadEpoches(SWELM_NOSPIKES);
         m_bDataAppended = true;

         SetGUIStatus(SWGS_RESULTLOADED);
         SetMeasurementChanged(true);
         bReturn = true;
         }
      }
   catch (Exception &e)
      {
      bReturn = false;
      SWErrorBox(e.Message);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback btnFreeSearch: enters free search mode
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnFreeSearchClick(TObject *Sender)
{
   if (InitFreeSearch())
      m_pformSearchFree->ShowFreeSearch();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// initializes free search mode
//------------------------------------------------------------------------------
bool TformSpikeWare::InitFreeSearch()
{
   if (!FormsCreated())
      return false;

   bool bReturn = false;
   try
      {
      Cleanup();
      // NOTE: ReadSettings shows error itself
      if (!m_smp.ReadSettings())
         return bReturn;

      m_swsSpikes.SetSampleRate((double)m_smp.m_fDefaultSampleRate, 1.0);
      m_swsStimuli.m_dDeviceSampleRate = (double)m_smp.m_fDefaultSampleRate;
      m_swsStimuli.m_nNumRepetitions = 1;
      m_swsStimuli.m_nRandom = 0;

      m_sweEpoches.m_dEpocheLength     = (double)m_smp.m_nFreeSearchRepetitionPeriodMs / 1000.0;
      m_sweEpoches.m_dPreStimulus      = (double)m_smp.m_nFreeSearchPreStimLengthMs / 1000.0;

      // repetitionperiod needed in samples!!
      m_sweEpoches.m_nRepetitionPeriod = (int)(m_sweEpoches.m_dEpocheLength * m_swsSpikes.GetSampleRate());

      m_smp.Exit();
      bReturn = m_smp.Init(AS_SMP_INIT_FREESEARCH);


      if (bReturn)
         {
         #ifdef CHKCHNLS
         if (formSpikeWare->m_smp.m_swcHWChannels.GetElectrodes().size() != m_smp.m_viChannelsInSettings.size())
            ShowMessage("error B " + UnicodeString(__FUNC__));
         #endif
         unsigned int nChannelsIn = (unsigned int)formSpikeWare->m_smp.m_swcHWChannels.GetElectrodes().size();
         m_sweEpoches.Initialize(nChannelsIn, (unsigned int)m_sweEpoches.m_nRepetitionPeriod);
         m_swsSpikes.SetNumChannels(nChannelsIn);
         m_pformEpoches->Initialize((int)nChannelsIn);

         if (m_gs < SWGS_LOADED)
            {
            unsigned int n;
            for (n = 0; n < nChannelsIn; n++)
               SetThreshold(n, IniReadDouble(m_pIni, "Settings", "Threshold_" + IntToStr((int)n), 0.4), false);
            }

         // add param (mandatory)
         m_swsStimuli.m_nChannelLevels = 1;
         m_swsStimuli.m_swspStimPars.Add("Level_1", "dB", true, false);
         m_swsStimuli.m_swspStimPars.m_vvdValues.resize(1);
         m_swsStimuli.m_swspStimPars.m_vvdValues[0].resize(1);

         // add dummy stim
         unsigned int nLength = (unsigned int)(m_smp.m_fDefaultSampleRate* m_smp.m_nFreeSearchStimLengthMs / 1000.0f);
         std::vector<double > vdParams(1);
         vdParams[0] = 0;
         std::vector<UnicodeString > vusParams(1);
         vusParams[0] = "Level_1";

         m_swsStimuli.m_swstStimuli.push_back(TSWStimulus("Stim", "dummy", nLength, vdParams, vusParams));
         m_swsStimuli.m_swstStimuli.back().m_nIndex          = 0;
         m_swsStimuli.m_swstStimuli.back().m_nFileStimIndex  = 0;


         m_pformEpoches->Initialize();
         m_pformSpikes->Initialize();
         }

      }
   catch (Exception &e)
      {
      bReturn = false;
      SWErrorBox(e.Message);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns XML epoche count (all or 'done')
//------------------------------------------------------------------------------
int TformSpikeWare::EpochesXML(bool bDone)
{
   if (!xml->Active)
      return 0;

   int nReturn = 0;
   _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
   if (!xmlResultNode)
      return nReturn;

   _di_IXMLNode xmlEpocheNodes = xmlResultNode->ChildNodes->FindNode("Epoches");
   if (!xmlEpocheNodes)
      return nReturn;

   int nNumEpoches = xmlEpocheNodes->ChildNodes->Count;
   if (!bDone)
      return nNumEpoches;

   int n;
   // find first NON-finished epoche
   for (n = 0; n < nNumEpoches; n++)
      {
      _di_IXMLNode xmlEpocheNode  = xmlEpocheNodes->ChildNodes->Nodes[n];
      if (xmlEpocheNode->GetNodeName() != "Epoche")
         throw Exception("Invalid subnode name detected in Epoches");
      if (GetXMLValue(xmlEpocheNode , "Done") == "1")
         nReturn++;
      else
         break;
      }
   return nReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads epoches from current XML in different modes
//------------------------------------------------------------------------------
void TformSpikeWare::LoadEpoches(TEpocheLoadMode nELM)
{
   if (!FormsCreated())
      return;

   EnableEpocheTimer(false);
   // NOTE: when calling 'LoadEpoches' then we DON'T want to use 'ProcessEpoches'
   // function, because we want to keep ALL epoches in memory!
   UnicodeString usEpocheFile = m_usResultPath + "epoches.pcm";

   if (!FileExists(usEpocheFile))
      throw Exception("Epoche file '" + usEpocheFile + "' cannot be found");

   m_pformEpoches->tbEpoches->OnChange = NULL;
   m_pformEpoches->tbEpoches->Max = 0;
   m_pformEpoches->tbEpoches->Position = 0;


   if (!m_sweEpoches.m_vvfEpoche.size())
      return;

   m_sweEpoches.Clear();

   if (nELM > SWELM_NOSPIKES)
      {
      m_pformSpikes->Clear();
      m_swsSpikes.Clear();
      }

   unsigned int nChannelsIn   = (unsigned int)m_sweEpoches.m_vvfEpoche.size();
   unsigned int nSamples      = (unsigned int)m_sweEpoches.m_vvfEpoche[0].size();
   unsigned int nXMLEpoches   = (unsigned int)EpochesXML(true);

   // we need the epoche nodes to re-read the original repetition index!
   _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
   if (!xmlResultNode)
      throw Exception("Invalid Result in XML result");
   _di_IXMLNode xmlEpocheNodes = xmlResultNode->ChildNodes->FindNode("Epoches");
   if (!xmlEpocheNodes)
      throw Exception("Invalid Epoches in XML result");

   vvf vvfData(nChannelsIn, std::valarray<float>(nSamples));

   TFileStream *pfs = NULL;
   TSWEpoche* pswe = NULL;

   formWait->ShowWait("Loading epoches, please wait...");
   try
      {
      pfs = new TFileStream(usEpocheFile, fmOpenRead | fmShareDenyNone);
      unsigned int nEpoches = (unsigned int)pfs->Size / sizeof(float) / nSamples / nChannelsIn;
      if (nXMLEpoches != nEpoches)
         throw Exception("result XML contains " + IntToStr((int)nXMLEpoches) + " epoches, but audio data " + IntToStr((int)nEpoches));
      unsigned int nChannel, nEpoche;
      bool bLast;
      for (nEpoche = 0; nEpoche < nEpoches; nEpoche++)
         {
         for (nChannel = 0; nChannel < nChannelsIn; nChannel++)
            pfs->ReadBuffer(&vvfData[nChannel][0], (NativeInt)(nSamples*sizeof(float)));

         // access epoche to read repetitionindex
         _di_IXMLNode xmlEpocheNode  = xmlEpocheNodes->ChildNodes->Nodes[nEpoche];

         if (nELM == SWELM_SPIKES_RESET_THRESHOLD)
            SetXMLEpocheThreshold(xmlEpocheNode, m_sweEpoches.m_vdThreshold); //m_sweEpoches.m_vdThreshold);

         // - use epoche thresholds or global thresholds (if to be resetted)
         pswe = m_sweEpoches.Push(  vvfData,
                                    GetXMLEpocheThresholds(xmlEpocheNode),
                                    (unsigned int)m_viStimSequence[nEpoche],
                                    // NOTE: XML contains repetitionindex 1-based, thus subtract one here!!
                                    (unsigned int)StrToInt(xmlEpocheNode->ChildValues["RepetitionIndex"] - 1)
                                    );
         if (nELM > SWELM_NOSPIKES)
            m_swsSpikes.Add(pswe);
         pswe->ClearData();
         }

      // plot last epoche
      pswe = m_sweEpoches.Get();
      if (pswe)
         {
         PlotEpoches(pswe);
         m_pformEpoches->tbEpoches->Max = (int)m_sweEpoches.Count()-1;
         m_pformEpoches->tbEpoches->Position = m_pformEpoches->tbEpoches->Max;
         }

      PlotSpikes();
      PlotClusters();
      }
   __finally
      {
      TRYDELETENULL(pfs);
      formWait->Hide();
      m_pformEpoches->tbEpoches->OnChange = m_pformEpoches->tbEpochesChange;

      }

   m_pformEpoches->EnableEpocheScrolling(true);
   m_pformEpoches->tbEpoches->Tag      = 1;
   m_pformSpikes->cbPlotEpocheSpikesOnly->Enabled  = true;
   m_pformEpoches->cbEpocheThreshold->Enabled  = true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// sets an epoche "done/not done" in XML
//------------------------------------------------------------------------------
void TformSpikeWare::SetXMLEpocheDone(int nNode, bool bDone)
{
   if (m_bFreeSearchRunning || m_gs == SWGS_SEARCH)
      return;

   _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
   if (!xmlResultNode)
      {
      OutputDebugString("SetXMLEpocheDone error 1");
      return;
      }
   _di_IXMLNode xmlEpoches = xmlResultNode->ChildNodes->FindNode("Epoches");
   if (!xmlEpoches)
      {
      OutputDebugString("SetXMLEpocheDone error 2");
      return;
      }

   int nNumEpoches = xmlEpoches->ChildNodes->Count;
   if (nNode >= nNumEpoches)
      {
      OutputDebugString("SetXMLEpocheDone error 3");
      return;
      }
   _di_IXMLNode xmlEpoche  = xmlEpoches->ChildNodes->Nodes[nNode];

   // write current thresholds to epoche!!
   SetXMLEpocheThreshold(xmlEpoche, GetThresholds());

   xmlEpoche->ChildValues["Done"] = bDone ? "1" : "0";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets epoches thresholds in XML node by node index
//------------------------------------------------------------------------------
void TformSpikeWare::SetXMLEpocheThreshold(int nNode, std::vector<double >& rvd)
{
   if (m_bFreeSearchRunning || m_gs == SWGS_SEARCH)
      return;

   _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
   if (!xmlResultNode)
      {
      OutputDebugString("SetXMLEpocheThreshold error 1");
      return;
      }
   _di_IXMLNode xmlEpoches = xmlResultNode->ChildNodes->FindNode("Epoches");
   if (!xmlEpoches)
      {
      OutputDebugString("SetXMLEpocheThreshold error 2");
      return;
      }

   int nNumEpoches = xmlEpoches->ChildNodes->Count;
   if (nNode >= nNumEpoches)
      {
      OutputDebugString("SetXMLEpocheThreshold error 3");
      return;
      }

   SetXMLEpocheThreshold(xmlEpoches->ChildNodes->Nodes[nNode], rvd);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets epoches thresholds in passed XML node 
//------------------------------------------------------------------------------
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
void TformSpikeWare::SetXMLEpocheThreshold(_di_IXMLNode xml, std::vector<double >& rvd)
#pragma clang diagnostic pop
{
   if (!xml)
      throw Exception("invalid node passed to " + UnicodeString(__FUNC__));
   // write current thresholds to epoche!!
   UnicodeString usThresholds = "[";
   unsigned int n;
   for (n = 0; n < rvd.size(); n++)
      usThresholds += DoubleToStr(rvd[n]) + " ";
   usThresholds = Trim(usThresholds) + "]";
   xml->ChildValues["Thresholds"] = usThresholds;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns thresholds for an epoche by XML node index
//------------------------------------------------------------------------------
std::vector<double > TformSpikeWare::GetXMLEpocheThresholds(int nNode)
{
   _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
   if (!xmlResultNode)
      throw Exception("GetEpocheThresholds error 1");

   _di_IXMLNode xmlEpoches = xmlResultNode->ChildNodes->FindNode("Epoches");
   if (!xmlEpoches)
      throw Exception("GetEpocheThresholds error 2");

   int nNumEpoches = xmlEpoches->ChildNodes->Count;
   if (nNode >= nNumEpoches)
      throw Exception("GetEpocheThresholds error 3");

   return GetXMLEpocheThresholds(xmlEpoches->ChildNodes->Nodes[nNode]);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns thresholds for epoche in passed XML node 
//------------------------------------------------------------------------------
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
std::vector<double > TformSpikeWare::GetXMLEpocheThresholds(_di_IXMLNode xml)
#pragma clang diagnostic pop
{
   if (!xml)
      throw Exception("invalid node passed to " + UnicodeString(__FUNC__));

   UnicodeString us = Trim(GetXMLValue(xml, "Thresholds"));
   if (us == "")
      throw Exception("epoche threshold missing");


   std::vector<double > vd;
   vved vvedThresholds = ParseMLVector(us, "Thresholds");
   if (vvedThresholds.size() != m_sweEpoches.GetNumChannels())
      throw Exception("invalid number of Thresholds found in epoche");
   unsigned int n;
   for (n = 0; n < vvedThresholds.size(); n++)
      {
      if (!vvedThresholds[n].size())
         throw Exception("invalid number of Thresholds found in epoche");
      vd.push_back(vvedThresholds[n][0]);
      }

   return vd;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// ensures that ALL epoches in current XML have valid thresholds
//------------------------------------------------------------------------------
void TformSpikeWare::EnsureXMLEpocheThresholds()
{
   _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
   if (!xmlResultNode)
      throw Exception("Result node missing");

   _di_IXMLNode xmlEpocheNodes = xmlResultNode->ChildNodes->FindNode("Epoches");
   if (!xmlEpocheNodes)
      throw Exception("Epoches node missing");

   int nNumEpoches = xmlEpocheNodes->ChildNodes->Count;
   int n;
   // find first NON-finished epoche
   for (n = 0; n < nNumEpoches; n++)
      {
      _di_IXMLNode xmlEpocheNode  = xmlEpocheNodes->ChildNodes->Nodes[n];
      if (xmlEpocheNode->GetNodeName() != "Epoche")
         throw Exception("Invalid subnode name detected in Epoches");
      if (GetXMLValue(xmlEpocheNode , "Done") == "1")
         {
         UnicodeString us = Trim(GetXMLValue(xmlEpocheNode, "Thresholds"));
         if (us == "")
            SetXMLEpocheThreshold(xmlEpocheNode, m_sweEpoches.m_vdThreshold);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// create XML epoche from stimuli 
//------------------------------------------------------------------------------
void TformSpikeWare::CreateXMLEpoches(std::vector<int >* pvn)
{
   bool bAppend = !!pvn;

    // create epoche subnodes
   _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
   _di_IXMLNode xmlEpoches = NULL;
   if (!bAppend)
      {
      if (!!xmlResultNode)
         xml->DocumentElement->ChildNodes->Remove(xmlResultNode);

      xmlResultNode = xml->DocumentElement->AddChild("Result");
      xmlEpoches = xmlResultNode->AddChild("Epoches");
      }
   else
      {
      if (!xmlResultNode)
         throw Exception("Result node cannot be found");
      xmlEpoches = xmlResultNode->ChildNodes->FindNode("Epoches");
      }

   if (!xmlEpoches)
      throw Exception("Epoches node cannot be created/found");

   // access source stimuli node
   _di_IXMLNode xmlStimuli = xml->DocumentElement->ChildNodes->Nodes[m_swsStimuli.GetStimuliNodeName()];
   if (!xmlStimuli)
      throw Exception(m_swsStimuli.GetStimuliNodeName() + " node cannot be found");
   int nNumStimuli = xmlStimuli->ChildNodes->Count;


   unsigned int n, m;

   UnicodeString usLevels, usStimulusSequence, usPar, usVal;
   usStimulusSequence = "[";
   int nStartIndex = bAppend ? xmlEpoches->ChildNodes->Count : 0;
   int nNewEpoches = (int)m_viStimSequence.size() - nStartIndex;
   if (!nNewEpoches)
      throw Exception("no new epoches found");

   if (bAppend && nNewEpoches != (int)pvn->size())
      throw Exception("invalid number of repetition indices passed");

   for (n = (unsigned int)nStartIndex; n < m_viStimSequence.size(); n++)
      {
      // NOTE: we write StimulusSequence 1-based!!
      usStimulusSequence += IntToStr(m_viStimSequence[n]+1) + " ";
      _di_IXMLNode xmlEpoche = xmlEpoches->AddChild("Epoche");
      // copy stimulus parameters AND determine concatenated levels
      if (m_viStimSequence[n] >= nNumStimuli)
         throw Exception("invalid stimulus index found in stimulus sequence: " + IntToStr(m_viStimSequence[n]) + "/" + IntToStr(nNumStimuli));
      usLevels = "[";
      _di_IXMLNode xmlStimulus  = xmlStimuli->ChildNodes->Nodes[m_viStimSequence[n]];
      for (m = 0; m < (unsigned int)xmlStimulus->ChildNodes->Count; m++)
         {
         _di_IXMLNode xmlParam  = xmlStimulus->ChildNodes->Nodes[m];
         usPar = xmlParam->GetNodeName();
         usVal = xmlParam->GetText();
         xmlEpoche->ChildValues[usPar] = usVal;
         if (usPar.Pos("Level_") == 1)
            usLevels += usVal + " ";
         }
      usLevels = Trim(usLevels) + "]";
      // write concatenated levels
      xmlEpoche->ChildValues["Level"] = usLevels;

      xmlEpoche->ChildValues["StimIndex"] = IntToStr(m_viStimSequence[n]+1);
      // write repetition
      if (bAppend)
         xmlEpoche->ChildValues["RepetitionIndex"] = IntToStr((*pvn)[n - (unsigned int)nStartIndex]);
      else
         xmlEpoche->ChildValues["RepetitionIndex"] = IntToStr((int)((n / m_swsStimuli.m_swstStimuli.size()) + 1));

      xmlEpoche->ChildValues["Done"] = "0";
      }
   // create COMPLETE repetition sequence
   m_viRepetitionSequence.clear();
   unsigned int nNumEpoches = (unsigned int)xmlEpoches->ChildNodes->Count;
   int nIndex;
   for (n = 0; n < nNumEpoches; n++)
      {
      _di_IXMLNode xmlEpoche = xmlEpoches->ChildNodes->Nodes[n];
      if (!TryStrToInt(GetXMLValue(xmlEpoche, "RepetitionIndex"), nIndex))
         throw Exception("RepetitionIndex unexpectedly not an int");
      // NOTE: -1 to be 0-based again!!
      m_viRepetitionSequence.push_back(StrToInt(nIndex-1));
      }
   if (m_viRepetitionSequence.size() != m_viStimSequence.size())
      throw Exception("RepetitionSequence and StimSequence unexpectedly have different size");
   // write stimulus sequence only new, if NOT bAppend: otherwise it's already written by caller
   if (!bAppend)
      {
      usStimulusSequence = Trim(usStimulusSequence) + "]";
      xmlResultNode->ChildValues["StimulusSequence"] = usStimulusSequence;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows save inquiry for change measurement data
//------------------------------------------------------------------------------
int  TformSpikeWare::SaveInquiry()
{
   if (!imFloppy->Visible)
      return ID_YES;
   int n = MessageBox(  Handle,
                        "The measurement was changed since last saving. Do you want to save changes?",
                        "Question",
                        MB_ICONQUESTION | MB_YESNOCANCEL);
   if (n == ID_YES)
      return SaveResult();
   else if (n == ID_NO)
      DeleteCurrentResults(true);
   return n;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// deletes current (temporary) result files
//------------------------------------------------------------------------------
void TformSpikeWare::DeleteCurrentResults(bool bQueryForRawDataAndDir)
{

   UnicodeString us = IncludeTrailingBackslash(m_usResultPath) + "*.xml";
   WIN32_FIND_DATAW wfd;
   memset(&wfd, 0, sizeof(wfd));
   HANDLE hFind = ::FindFirstFileW(us.w_str(), &wfd);
   if (hFind == INVALID_HANDLE_VALUE)
      {
      ::FindClose(hFind);
      int n = ID_YES;
      if (bQueryForRawDataAndDir)
         {
         n = MessageBox(  Handle,
                           "Delete directory and raw data of unsaved measurement?",
                           "Question",
                           MB_ICONQUESTION | MB_YESNO);
         }

      if (n == ID_YES)
         DeleteFilesAndFolder(m_usResultPath);
      }
   else
      ::FindClose(hFind);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// helper function for deleteing all files in a folder and folder itself
//------------------------------------------------------------------------------
void TformSpikeWare::DeleteFilesAndFolder(UnicodeString us)
{
   if (!DirectoryExists(us))
      return;
   WIN32_FIND_DATAW w32fd;
   memset(&w32fd, 0, sizeof(w32fd));
   UnicodeString usMask = IncludeTrailingBackslash(us) + "*.*";
   HANDLE h = ::FindFirstFileW(usMask.w_str(), &w32fd);
   BOOL bNextFileExists = (h!=INVALID_HANDLE_VALUE);
   for (; bNextFileExists; bNextFileExists=::FindNextFileW(h, &w32fd))
      {
      if ( (w32fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0 )
         continue;
      UnicodeString usFile = IncludeTrailingBackslash(us) + w32fd.cFileName;
      DeleteFile(usFile);
      }
   ::FindClose(h);
   RemoveDir(us);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets measurement status to changed/unchanged
//------------------------------------------------------------------------------
void TformSpikeWare::SetMeasurementChanged(bool bChanged, bool bForce)
{
   // set changed only on particluar states
   if (!bForce)
      {
      if (bChanged
         && (  m_gs < SWGS_RESULTLOADED
            || m_gs == SWGS_FREESEARCHRUN
            || m_gs == SWGS_SEARCH
            )
         )
         return;
      }

   // NOTE: this is a workaround, because hiding this TImage does not work
   // for unknown reason (is hidden, but not repainted. This does not happen under all
   // circumstances, but at least with user-defined dialog color AND manifest).
   // Setting any text in underlying statusbar field helps...
   sb->Panels->Items[SB_P_FLOPPY]->Text = _T(" ");
   imFloppy->Visible = bChanged;
   sb->Panels->Items[SB_P_FLOPPY]->Text = _T("");
   SetGUIStatus();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// saves a result
//------------------------------------------------------------------------------
int TformSpikeWare::SaveResult(bool bForceNewResult)
{
   if (!FormsCreated())
      return ID_CANCEL;

   TCursor cr = Screen->Cursor;
   try
      {
      // write some values to "Settings"
      _di_IXMLNode xmlSettings = xml->DocumentElement->ChildNodes->FindNode("Settings");
      if (!xmlSettings)
         throw Exception("Settings node unexpectedly missing");

      // spike length and pre-threshold
      xmlSettings->ChildValues["SpikeLength"] = DoubleToStr(1000.0 * m_swsSpikes.m_dSpikeLength);
      // epoche length in Samples
      xmlSettings->ChildValues["PreThreshold"] = DoubleToStr(1000.0 * m_swsSpikes.m_dPreThreshold);

      // epoche length in Samples
      xmlSettings->ChildValues["EpocheLengthSamples"] = IntToStr((int)m_sweEpoches.m_vvfEpoche[0].size());
      // spike length in Samples
      xmlSettings->ChildValues["SpikeLengthSamples"]  = IntToStr(m_swsSpikes.m_nSpikeLength);
      // save thresholds
      UnicodeString usThresholds = "[";
      unsigned int n;
      for (n = 0; n < m_sweEpoches.GetNumChannels(); n++)
         usThresholds += DoubleToStr(GetThreshold(n)) + " ";
      usThresholds = Trim(usThresholds) + "]";
      xmlSettings->ChildValues["Thresholds"] = usThresholds;


      _di_IXMLNode xmlResultNode = xml->DocumentElement->ChildNodes->FindNode("Result");
      if (!xmlResultNode)
         throw Exception("Result node unexpectedly missing");

      Screen->Cursor = crHourGlass;
      formWait->ShowWait("Saving result, please wait...");

      _di_IXMLNode xmlEpocheNodes = xmlResultNode->ChildNodes->FindNode("Epoches");
      if (!xmlEpocheNodes)
         throw Exception("Epoche node unexpectedly missing");

      // save current cluster windows and corresponding selections
      _di_IXMLNode xmlClusters = xmlResultNode->ChildNodes->FindNode("Clusters");
      if (!!xmlClusters)
         xmlResultNode->ChildNodes->Remove(xmlClusters);
      xmlClusters = xmlResultNode->AddChild("Clusters");
      unsigned int nWindow, nChannel, nSel;
      for (nWindow = 0; nWindow < m_vpformCluster.size(); nWindow++)
         {
         _di_IXMLNode xmlCluster = xmlClusters->AddChild("Cluster");
         // X/Y-spike-param combinations
         xmlCluster->ChildValues["X"] = m_swsSpikes.m_swspSpikePars.m_vusIDs[m_vpformCluster[nWindow]->m_spX];
         xmlCluster->ChildValues["Y"] = m_swsSpikes.m_swspSpikePars.m_vusIDs[m_vpformCluster[nWindow]->m_spY];
         _di_IXMLNode xmlChannels = xmlCluster->AddChild("Channels");
         for (nChannel = 0; nChannel < m_vpformCluster[nWindow]->m_vvSWSelections.size(); nChannel++)
            {
            _di_IXMLNode xmlChannel = xmlChannels->AddChild("Channel");
            xmlChannel->ChildValues["Active"] = IntToStr((int)m_vpformCluster[nWindow]->m_vbSelActive[nChannel]);
            _di_IXMLNode xmlSelections = xmlChannel->AddChild("Selections");
            for (nSel = 0; nSel < m_vpformCluster[nWindow]->m_vvSWSelections[nChannel].size(); nSel++)
               {
               _di_IXMLNode xmlSel = xmlSelections->AddChild("Selection");
               xmlSel->ChildValues["X0"] = DoubleToStr(m_vpformCluster[nWindow]->m_vvSWSelections[nChannel][nSel].dX0);
               xmlSel->ChildValues["X1"] = DoubleToStr(m_vpformCluster[nWindow]->m_vvSWSelections[nChannel][nSel].dX1);
               xmlSel->ChildValues["Y0"] = DoubleToStr(m_vpformCluster[nWindow]->m_vvSWSelections[nChannel][nSel].dY0);
               xmlSel->ChildValues["Y1"] = DoubleToStr(m_vpformCluster[nWindow]->m_vvSWSelections[nChannel][nSel].dY1);
               xmlSel->ChildValues["Active"] = IntToStr((int)m_vpformCluster[nWindow]->m_vvSWSelections[nChannel][nSel].bActive);
               }
            }
         }

      // save current parameter windows
      _di_IXMLNode xmlParameterWindows = xmlResultNode->ChildNodes->FindNode("ParameterWindows");
      if (!!xmlParameterWindows)
         xmlResultNode->ChildNodes->Remove(xmlParameterWindows);
      xmlParameterWindows = xmlResultNode->AddChild("ParameterWindows");
      for (nWindow = 0; nWindow < m_vpformBubblePlots.size(); nWindow++)
         {
         _di_IXMLNode xmlParameterWindow = xmlParameterWindows->AddChild("ParameterWindow");
         // X/Y-Stim-param combinations
         xmlParameterWindow->ChildValues["X"] = m_swsStimuli.m_swspStimPars.m_vusNames[m_vpformBubblePlots[nWindow]->m_nParamX];
         if (m_vpformBubblePlots[nWindow]->m_bResponseYAxis)
            xmlParameterWindow->ChildValues["Y"] = "Response";
         else
            xmlParameterWindow->ChildValues["Y"] = m_swsStimuli.m_swspStimPars.m_vusNames[m_vpformBubblePlots[nWindow]->m_nParamY];
         }

      _di_IXMLNode xmlPSTH = xmlResultNode->ChildNodes->FindNode("PSTH");
      if (!!xmlPSTH)
         xmlResultNode->ChildNodes->Remove(xmlPSTH);
      xmlPSTH = xmlResultNode->AddChild("PSTH");
      _di_IXMLNode xmlChannels = xmlPSTH->AddChild("Channels");

      // save PSTH selections
      for (nChannel = 0; nChannel < m_pformPSTH->m_vSWSelections.size(); nChannel++)
         {
         _di_IXMLNode xmlChannel = xmlChannels->AddChild("Channel");
         xmlChannel->ChildValues["Selection_X0"] = DoubleToStr(m_pformPSTH->m_vSWSelections[nChannel].dX0);
         xmlChannel->ChildValues["Selection_X1"] = DoubleToStr(m_pformPSTH->m_vSWSelections[nChannel].dX1);
         xmlChannel->ChildValues["Selection_Active"] = IntToStr((int)m_pformPSTH->m_vSWSelections[nChannel].bActive);
         xmlChannel->ChildValues["NoiseSelection_X0"] = DoubleToStr(m_pformPSTH->m_vSWNoiseSelections[nChannel].dX0);
         xmlChannel->ChildValues["NoiseSelection_X1"] = DoubleToStr(m_pformPSTH->m_vSWNoiseSelections[nChannel].dX1);
         xmlChannel->ChildValues["NoiseSelection_Active"] = IntToStr((int)m_pformPSTH->m_vSWNoiseSelections[nChannel].bActive);
         }

      // write Spikes and NonSelectedSpikes to different nodes in XML
      _di_IXMLNode xmlSpikes = xmlResultNode->ChildNodes->FindNode("Spikes");
      if (!!xmlSpikes)
         xmlResultNode->ChildNodes->Remove(xmlSpikes);
      xmlSpikes = xmlResultNode->AddChild("Spikes");

      _di_IXMLNode xmlNonSelectedSpikes = xmlResultNode->ChildNodes->FindNode("NonSelectedSpikes");
      if (!!xmlNonSelectedSpikes)
         xmlResultNode->ChildNodes->Remove(xmlNonSelectedSpikes);
      xmlNonSelectedSpikes = xmlResultNode->AddChild("NonSelectedSpikes");

      unsigned int nPar, nSpikes, nSpike;
      int nSpikeGroup;
      UnicodeString usLevel;
      UnicodeString usProgress = ".";

      for (nChannel = 0; nChannel < m_swsSpikes.m_vvSpikes.size(); nChannel++)
         {
         if ((nChannel % 100) == 0)
            {
            usProgress += ".";
            if (usProgress.Length() > 10)
               usProgress = ".";
            formWait->ShowWait("Saving result, please wait" + usProgress);
            }
         nSpikes = m_swsSpikes.GetNumSpikes(nChannel);
         for (nSpike = 0; nSpike < nSpikes; nSpike++)
            {
            _di_IXMLNode xmlSpike;
            // not selected?
            nSpikeGroup = m_swsSpikes.GetSpikeGroup(nChannel, nSpike);
            if (nSpikeGroup < 0)
               xmlSpike = xmlNonSelectedSpikes->AddChild("Spike");
            else
               {
               xmlSpike = xmlSpikes->AddChild("Spike");
               xmlSpike->ChildValues["SpikeGroup"]  = IntToStr((int)nSpikeGroup+1);
               }

            std::vector<double >& rvdParams =
               m_swsStimuli.m_swstStimuli[m_swsSpikes.GetStimIndex(nChannel, nSpike)].m_vdParams;
            std::vector<UnicodeString >& rvusParams =
               m_swsStimuli.m_swstStimuli[m_swsSpikes.GetStimIndex(nChannel, nSpike)].m_vusParams;


            // NOTE: we write ALL spike parameters 1-based (grace for MATLAB users)
            xmlSpike->ChildValues["SpikeTime"]  = DoubleToStr(m_swsSpikes.GetSpikeTime(nChannel, nSpike));
            xmlSpike->ChildValues["SpikePosition"] = IntToStr((int)m_swsSpikes.GetSpikePosition(nChannel, nSpike)+1);
            xmlSpike->ChildValues["StimIndex"]  = IntToStr((int)m_swsSpikes.GetStimIndex(nChannel, nSpike)+1);
            xmlSpike->ChildValues["EpocheIndex"]= IntToStr((int)m_swsSpikes.GetEpocheIndex(nChannel, nSpike)+1);
            xmlSpike->ChildValues["Channel"]    = IntToStr((int)nChannel+1);
            xmlSpike->ChildValues["RepetitionIndex"] = IntToStr((int)m_swsSpikes.GetRepetitionIndex(nChannel, nSpike)+1);
            xmlSpike->ChildValues["Threshold"]  = DoubleToStr(m_swsSpikes.GetThreshold(nChannel, nSpike));


            // write parameters with special handling of levels
            usLevel = "[";
            for (nPar = 0; nPar < m_swsStimuli.m_swspStimPars.m_vusNames.size(); nPar++)
               {
               UnicodeString us = StringReplace(m_swsStimuli.m_swspStimPars.m_vusNames[nPar], " ", "_", TReplaceFlags() << rfReplaceAll );
               if (us.Pos("Level_") == 1)
                  {
                  usLevel += DoubleToStr(rvdParams[nPar]) + " ";
                  continue;
                  }
               if (m_swsStimuli.m_swspStimPars.m_vbString[nPar])
                  xmlSpike->ChildValues[us] = rvusParams[nPar];
               else
                  xmlSpike->ChildValues[us] = DoubleToStr(rvdParams[nPar]);
               }
            usLevel = Trim(usLevel) + "]";
            xmlSpike->ChildValues["Level"] = usLevel;

            // If not denied from settings, write all spike parameters as well
            if (xmlSettings->ChildValues["SaveSpikeParams"] != "0")
               {
               for (nPar = 0; nPar < m_swsSpikes.m_swspSpikePars.m_vusIDs.size(); nPar++)
                  xmlSpike->ChildValues[m_swsSpikes.m_swspSpikePars.m_vusIDs[nPar]]  = DoubleToStr(m_swsSpikes.GetSpikeParam(nChannel, nSpike, (TSpikeParam)nPar));
               }

            // store raw spike data
            AnsiString as = EncodeBase64(&m_swsSpikes.GetSpike(nChannel, nSpike)[0], (int)(m_swsSpikes.GetSpike(nChannel, nSpike).size()*sizeof(double)));
            xmlSpike->ChildValues["Data"] = as;
            }
         }

      // find file with highest index (10000-based)
      UnicodeString usFileName;
      int nMax = 9999;
      bool bResultExists = false;
      while (nMax-- > 1)
         {
         usFileName.printf(L"%lsresult_%04d.xml", m_usResultPath.w_str(), nMax);
         if (FileExists(usFileName))
            {
            bResultExists = true;
            break;
            }
         }
      if (bResultExists)
         {
         if (bForceNewResult)
            usFileName.printf(L"%lsresult_%04d.xml", m_usResultPath.w_str(), nMax+1);
         else
            {
            int nReturn = MessageBox(  formWait->Handle,
                                 "A result file already exists. Do you want to overwrite it (No creates a result file with a new index",
                                 "",
                                 MB_YESNOCANCEL | MB_ICONQUESTION);
            if (nReturn == ID_CANCEL)
               return nReturn;
            else if (nReturn == ID_NO)
               usFileName.printf(L"%lsresult_%04d.xml", m_usResultPath.w_str(), nMax+1);
            }
         }
      // NOTE: FormatXMLData is very slow, thus we save 'unformatted' by default
      if (m_pIni->ReadBool("Settings", "FormatXML", false))
         {
         xmlSave->Active = false;
         xmlSave->XML->Text = FormatXMLData(xml->XML->Text);
         xmlSave->Active = true;
         xmlSave->SaveToFile(usFileName);
         }
      else
         {
         xml->SaveToFile(usFileName);
         }

      xml->FileName = usFileName;


      if (m_bSaveMAT)
         {
         AnsiString asCommandLine = IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) + "AudioSpike2MAT.exe ";
         asCommandLine +=  "\"" + xml->FileName + "\" \"" + ChangeFileExt(xml->FileName, ".mat") + "\"";


         // create process AudioSpike2MAT.exe
         SECURITY_ATTRIBUTES  sec;
         STARTUPINFO          start;
         PROCESS_INFORMATION  pinfo;
         memset(&start, 0, sizeof(start));
         memset(&pinfo, 0, sizeof(pinfo));
         start.cb = sizeof(start);
         start.wShowWindow = SW_HIDE;
         start.dwFlags = STARTF_USESHOWWINDOW;
         sec.nLength = sizeof(sec);
         sec.lpSecurityDescriptor = NULL;
         sec.bInheritHandle = FALSE;

         // note: CreateProcess uses LPTSTR instead LPCTSTR, which seems to be an error.
         if ( !CreateProcess( NULL,
                              const_cast<LPTSTR>(asCommandLine.c_str()),
                              &sec,
                              &sec,
                              FALSE,
                              0,
                              NULL,
                              NULL,
                              &start,
                              &pinfo)
                              )
               {            
               SWErrorBox("Error calling 'AudioSpike2MAT': MAT file not created");
               }
            else
               {
               WaitForSingleObject( pinfo.hProcess, 10000);
               DWORD dw;
               GetExitCodeProcess(pinfo.hProcess, &dw);
               if (dw != 0)
                  SWErrorBox("An error occurred within 'AudioSpike2MAT': MAT file not valid");
               }
         
         }


      Caption = m_usASCaption + " - " + usFileName;
      SetMeasurementChanged(false);

      // re-load saved file directly!
      LoadMeasurementResult(usFileName);
      }
   __finally
      {
      formWait->Hide();
      Screen->Cursor = cr;
      }
   return ID_YES;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates/sets and automatically generated result path
//------------------------------------------------------------------------------
void TformSpikeWare::SetAutoResultPath(UnicodeString usSubPath)
{
   int n = 0;
   bool bPathExists = false;
   int nMax = 9999;
   while (nMax-- > 1)
      {
      m_usResultPath.printf(L"%ls%ls_%04d", m_usResultPathRoot.w_str(), usSubPath.w_str(), nMax);
      if (DirectoryExists(m_usResultPath))
         {
         m_usResultPath.printf(L"%ls%ls_%04d", m_usResultPathRoot.w_str(), usSubPath.w_str(), nMax+1);
         break;
         }
      }

   Caption = m_usASCaption + " - " + m_usResultPath;

   if (!ForceDirectories(m_usResultPath))
      throw Exception("Cannot create result path '" + m_usResultPath + "'");
   m_usResultPath = IncludeTrailingBackslash(m_usResultPath);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnRun: starts measurement
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnRunClick(TObject *Sender)
{
   if (m_gs == SWGS_FREESEARCHSTOP || m_gs == SWGS_FREESEARCHRUN)
      return;

   // run button may :
   // 1- run new measurement (gs == SWGS_LOADED)
   // 2. resume incomplete stopped measurement (gs == SWGS_STOPPED && unfinished epoches present)
   // 3. resume incomplete loaded measurement (gs == SWGS_SWGS_RESULTLOADED && unfinished epoches present)
   bool bUnfinishedEpoches = EpochesXML(true) < EpochesXML(false);

   if (m_gs == SWGS_LOADED)
      {
      if (!m_usFixResultPath.IsEmpty())
         m_usResultPath = IncludeTrailingBackslash(m_usResultPathRoot + m_usFixResultPath);
      else
         {
         UnicodeString usSubPath;
         usSubPath = m_pslParamStr->Values["subpath"];

         if (usSubPath.IsEmpty())
            usSubPath = m_pIni->ReadString("Settings", "LastSubPath", "");

         if (!InputQuery("Subpath", "Please enter a subdirectory for the results", usSubPath))
            return;
         m_pIni->WriteString("Settings", "LastSubPath", usSubPath);

         SetAutoResultPath(usSubPath);
         }

      RunMeasurement(false);
      }
   else if (m_gs == SWGS_RESULTLOADED && bUnfinishedEpoches)
      {
      // in append-mode next two commands already were called!!
      if (!m_bDataAppended)
         {
         LoadMeasurement("", SWLM_RESUME);
         LoadEpoches(SWELM_NOSPIKES);
         }
      m_swsSpikes.Clear();
      if (FormsCreated())
         m_pformSpikes->Clear();
      m_nStimPlayIndex = EpochesXML(true);
      m_sweEpoches.AppendSave();
      RunMeasurement(true);
      }
   else if (m_gs == SWGS_STOP && bUnfinishedEpoches)
      {
      m_sweEpoches.AppendSave();
      RunMeasurement(true);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnStop: stops measurement
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnStopClick(TObject *Sender)
{
   if (m_gs == SWGS_FREESEARCHSTOP || m_gs == SWGS_FREESEARCHRUN)
      return;

   TRYDELETENULL(m_pformSearch);

   if (m_smp.Playing())
      {
      if (!m_smp.Stop())
         return;
      }

   m_bBreak = true;

   if (m_gs == SWGS_SEARCH)
      {
      m_swsSpikes.Clear();
      m_sweEpoches.Clear();
      SetGUIStatus(SWGS_LOADED);
      }
   else
      {
      if (m_gs == SWGS_PAUSE)
         m_sweEpoches.DoneSave();
      SetGUIStatus(SWGS_STOP);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnPause: toggles pause status of measurement
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnPauseClick(TObject *Sender)
{
   btnPause->Enabled = false;
   btnPause->Tag = !btnPause->Tag;

   if (btnPause->Tag)
      {
      SetGUIStatus(SWGS_PAUSE);
      m_smp.Stop();
      }
   else
      {
      RunMeasurement(true);
      }
   btnPause->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// runs/resumes a measurement
//------------------------------------------------------------------------------
TSWRunResult TformSpikeWare::RunMeasurement(bool bResume)
{
   TSWRunResult swrr = SWRR_ERROR;
   if (!m_smp.Init())
      return swrr;

   try
      {
      try
         {
         SetGUIStatus(SWGS_RUN);

         m_bBreak = false;


         SetMeasurementChanged();

         if (!bResume)
            {
            m_nStimPlayIndex = 0;
            m_SearchModeStimIndex = -1;
            CreateStimulusSequence(m_viStimSequence, (unsigned int)m_swsStimuli.m_swstStimuli.size());
            }
         if (!m_smp.Prepare())
            return swrr;

         if (!bResume)
            {
            formWait->ShowWait("Creating data structures, this may take a while ...");
            CreateXMLEpoches();
            // ... and PCM data saving
            m_sweEpoches.InitSave();
            formWait->Hide();
            }

         UpdateStimulusDisplay();

         EnableEpocheTimer(true);
         m_smp.Start();
         m_smp.Wait();
         m_smp.Exit();


         // if NOT paused, then we're done: stop saving PCM data
         if (m_gs != SWGS_PAUSE)
            {
            m_sweEpoches.DoneSave();
            SetGUIStatus(SWGS_STOP);
            swrr = SWRR_DONE;
            }
         else
            swrr = SWRR_PAUSE;
         // adjust m_nStimPlayIndex to saved (!) epoches
         m_nStimPlayIndex = (int)m_sweEpoches.m_nEpochesTotal;
         EnableEpocheTimer(false);
         }
      __finally
         {
         formWait->Hide();
         }
      }
   catch (...)
      {
      m_sweEpoches.DoneSave();
      throw;
      }
   return swrr;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// runs the trigger test
//------------------------------------------------------------------------------
void __fastcall TformSpikeWare::TriggerTest()
{
   m_sweEpoches.m_nTriggersDetected = 0;
   m_nStimPlayIndex = 0;

   m_swsStimuli.m_dDeviceSampleRate = (double)m_smp.m_fDefaultSampleRate;
   m_swsSpikes.SetSampleRate(m_swsStimuli.m_dDeviceSampleRate, 1.0);

   if (!m_smp.Init(AS_SMP_INIT_TRIGGERTEST))
      throw Exception("error initializing SMP!");

   SetGUIStatus(SWGS_SEARCH);
   if (!m_smp.PrepareTriggerTest())
      throw Exception("error preparing trigger test!");

   m_smp.Start(-1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnSearch: starts search mode (by stimulus list)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnSearchClick(TObject *Sender)
{
   if (!FormsCreated())
      return;

   TRYDELETENULL(m_pformSearch);
   if (mrOk == m_pformSelect->ShowSelection(m_swsStimuli.m_swspStimPars.m_vusNames))
      {
      unsigned int n, nIndex;
      for (n = 0; n < 2; n++)
         {
         nIndex = n ? (unsigned int)m_pformSelect->lbY->ItemIndex : (unsigned int)m_pformSelect->lbX->ItemIndex;
         // selected stimulus parameters must NOT contain doublettes for graphical selection!!
         // (only checked for double values)
         std::vector<double > vd = m_swsStimuli.m_swspStimPars.m_vvdValues[nIndex];
         std::sort(vd.begin(), vd.end());
         #pragma clang diagnostic push
         #pragma clang diagnostic ignored "-Wfloat-equal"
         for (nIndex = 0; nIndex < vd.size() - 1; nIndex++)
            {
            if (vd[nIndex] == vd[nIndex+1])
               throw Exception("Parameter combination cannot be used for search stimulus selection, because a parameter contains doublette values");
            }
         #pragma clang diagnostic pop
         }
      m_pformSearch = new TformBubblePlot(NULL, (unsigned int)m_pformSelect->lbX->ItemIndex, (unsigned int)m_pformSelect->lbY->ItemIndex, true);
      m_pformSearch->Show();
      }
   else
      return;


   if (!m_smp.Init())
      return;
   SetGUIStatus(SWGS_SEARCH);

   m_nStimPlayIndex = -1;
   m_viStimSequence.clear();

   PlaySearchStimulus(0);
   EnableEpocheTimer(true);

   m_pformStimuli->lv->Selected = m_pformStimuli->lv->Items->Item[0];
   m_pformStimuli->lv->Selected->MakeVisible(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// starts playback of a stimulus in seacrh mode by index
//------------------------------------------------------------------------------
void TformSpikeWare::PlaySearchStimulus(int nIndex, bool bUpdateBubble)
{
   if (!FormsCreated())
      return;

   EnableEpocheTimer(false);
   m_pformStimuli->pnl->Enabled = false;
   if (m_pformSearch)
      m_pformSearch->chrt->Enabled = false;

   try
      {
      m_sweEpoches.Clear();
      m_SearchModeStimIndex = nIndex;
      m_pformStimuli->SelectItem(nIndex);
      if (bUpdateBubble)
         m_pformSearch->UpdateBubble(nIndex);
      m_smp.PlayStimulusLooped(m_SearchModeStimIndex);
      }
   __finally
      {
      EnableEpocheTimer(true);
      m_pformStimuli->pnl->Enabled = true;
      if (m_pformSearch)
         m_pformSearch->chrt->Enabled = true;
      }
}
 //------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots passed epoche
//------------------------------------------------------------------------------
void TformSpikeWare::PlotEpoches(TSWEpoche *pswe)
{
   if (FormsCreated())
      m_pformEpoches->Plot(pswe);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates spikes, PSTH and bubble plots
//------------------------------------------------------------------------------
void TformSpikeWare::PlotSpikes()
{
   if (!FormsCreated())
      return;
   m_pformSpikes->Plot(m_pformEpoches->m_nPlotIndex);
   m_pformPSTH->Plot(m_pformEpoches->m_nPlotIndex);
   m_pformSignalPSTH->Plot(m_pformEpoches->m_nPlotIndex);

   PlotBubblePlots();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates bubble plots
//------------------------------------------------------------------------------
void TformSpikeWare::PlotBubblePlots()
{
   unsigned int n;
   for (n = 0; n < m_vpformBubblePlots.size(); n++)
     m_vpformBubblePlots[n]->Plot(m_pformEpoches->m_nPlotIndex);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates bubble cluster plots
//------------------------------------------------------------------------------
void TformSpikeWare::PlotClusters()
{
   unsigned int n;
   for (n = 0; n < m_vpformCluster.size(); n++)
      m_vpformCluster[n]->Plot(m_pformEpoches->m_nPlotIndex);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// toggles visiblity of particular forms
//------------------------------------------------------------------------------
void __fastcall TformSpikeWare::miFormToggleClick(TObject *Sender)
{
   TMenuItem* mi = (TMenuItem*)Sender;
   TForm* pfrm = (TForm*)mi->Tag;

   mi->Checked = !mi->Checked;

   SetWindowVisible(pfrm, mi->Checked);
   if (mi->Checked)
      {
      pfrm->BringToFront();
      if (pfrm == m_pformSpikes)
         PlotSpikes();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates a new cluster plot
//------------------------------------------------------------------------------
void TformSpikeWare::CreateClusterWindow(int nX, int nY, unsigned int nChannels)
{
   if (!FormsCreated())
      return;

   if (nY == -1)
      {
      while (1)
         {
         if (mrOk != m_pformSelect->ShowSelection(m_swsSpikes.m_swspSpikePars.m_vusNames))
            return;

         bool bOk = true;
         unsigned int n;
         for (n = 0; n < m_vpformCluster.size(); n++)
            {
            // parameter names are stored in top and right axis!
            if (  m_vpformCluster[n]->chrt->TopAxis->Title->Caption == m_swsSpikes.m_swspSpikePars.m_vusIDs[(unsigned int)m_pformSelect->lbX->ItemIndex]
               && m_vpformCluster[n]->chrt->RightAxis->Title->Caption == m_swsSpikes.m_swspSpikePars.m_vusIDs[(unsigned int)m_pformSelect->lbY->ItemIndex]
               )
               {
               SWErrorBox("A Cluster window with this parameter combination already exists");
               bOk = false;
               }
            }
         if (bOk)
            {
            nX = m_pformSelect->lbX->ItemIndex;
            nY = m_pformSelect->lbY->ItemIndex;
            break;
            }
         }
      }

   TformCluster* pfrm = new TformCluster(NULL, (TSpikeParam)nX, (TSpikeParam)nY);
   if (!!pfrm)
      {
      m_vpformCluster.push_back(pfrm);
      pfrm->Initialize(nChannels);
      pfrm->Plot(m_pformEpoches->m_nPlotIndex);
      ScaleClusterPlot(pfrm);
      pfrm->Show();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes a cluster plot
//------------------------------------------------------------------------------
void TformSpikeWare::RemoveClusterWindow(TformCluster* pfrm)
{
   unsigned int n;
   for (n = 0; n < m_vpformCluster.size(); n++)
      {
      if (m_vpformCluster[n] == pfrm)
         {
         m_pIni->WriteBool("ClusterWindows", m_vpformCluster[n]->Name, false);
         TRYDELETENULL(m_vpformCluster[n]);
         m_vpformCluster.erase(m_vpformCluster.begin() + (int)n);
         break;
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes a bubble plot
//------------------------------------------------------------------------------
void TformSpikeWare::RemoveParamWindow(TformBubblePlot* pfrm)
{
   unsigned int n;
   for (n = 0; n < m_vpformBubblePlots.size(); n++)
      {
      // delete particular or all forms
      if (m_vpformBubblePlots[n] == pfrm)
         {
         TRYDELETENULL(m_vpformBubblePlots[n]);
         m_vpformBubblePlots.erase(m_vpformBubblePlots.begin() + (int)n);
         break;
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns a name for a BubblePlot by indices
//------------------------------------------------------------------------------
UnicodeString TformSpikeWare::ParameterWindowName(unsigned int nX, unsigned int nY)
{
   // if the index of the Y-axis is one higher (!) than the last parameter, then
   // it is the 'response' ather than a read stimulus parameter!
   bool bResponseYAxis = (nY == m_swsStimuli.m_swspStimPars.m_vusNames.size());
   UnicodeString us     = "Param_"
                  + m_swsStimuli.m_swspStimPars.m_vusNames[nX]
                  + "_";

   if (!bResponseYAxis)
      us = us + m_swsStimuli.m_swspStimPars.m_vusNames[nY];
   else
      us = us + "Response";
   return us;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates a new bubble plot
//------------------------------------------------------------------------------
void TformSpikeWare::CreateParameterWindow()
{
   if (!FormsCreated())
      return;

   while (1)
      {
      // call ShowSelection with parameter 'true' to add 'Response' as possible Y-Axis
      if (mrOk != m_pformSelect->ShowSelection(m_swsStimuli.m_swspStimPars.m_vusNames, true))
         return;

      UnicodeString us = ParameterWindowName((unsigned int)m_pformSelect->lbX->ItemIndex, (unsigned int)m_pformSelect->lbY->ItemIndex);
      bool bOk = true;
      unsigned int n;
      for (n = 0; n < m_vpformBubblePlots.size(); n++)
         {
         if (m_vpformBubblePlots[n]->HelpFile == us)
            {
            SWErrorBox("A Parameter window with this parameter combination already exists");
            bOk = false;
            }
         }
      if (bOk)
         break;
      }
   CreateParameterWindow(m_pformSelect->lbX->ItemIndex, m_pformSelect->lbY->ItemIndex);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates a new bubble plot by parameter names
//------------------------------------------------------------------------------
void TformSpikeWare::CreateParameterWindow(int nX, int nY)
{
   if (!FormsCreated())
      return;

   if (  nX >= (int)m_swsStimuli.m_swspStimPars.m_vusNames.size()
      || nY >  (int)m_swsStimuli.m_swspStimPars.m_vusNames.size()
      )
      throw Exception("invalid param indices passed for creating param window");

   TformBubblePlot* pfrm = new TformBubblePlot(NULL, (unsigned int)nX, (unsigned int)nY, false);

   if (!!pfrm)
      {
      m_vpformBubblePlots.push_back(pfrm);
      Application->ProcessMessages();
      pfrm->Plot(m_pformEpoches->m_nPlotIndex);
      pfrm->Show();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnCluster: calls CreateClusterWindow
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnClusterClick(TObject *Sender)
{
   CreateClusterWindow(-1, -1, (unsigned int)m_swsSpikes.m_vvSpikes.size());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnParam: calls CreateParameterWindow
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnParamClick(TObject *Sender)
{
   CreateParameterWindow();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets current epoche evaluation window to passed one
//------------------------------------------------------------------------------
void TformSpikeWare::SetEvalWindow(TformEpoches* pfrm)
{
   if (!FormsCreated())
      return;

   EnterCriticalSection(&m_cs);
   try
      {
      m_pformEpoches->SetEvalWindow(pfrm);
      }
   __finally
      {
      LeaveCriticalSection(&m_cs);
      }

   PlotClusters();
   PlotSpikes();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets passed cluster window to be active for selection
//------------------------------------------------------------------------------
void TformSpikeWare::SetSelectionWindow(TformCluster* pfrm)
{
   unsigned int n;
   for (n = 0; n < m_vpformCluster.size(); n++)
      m_vpformCluster[n]->EnableSelection(m_vpformCluster[n] == pfrm);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls ScaleClusterPlot foe one or all cluster plot
//------------------------------------------------------------------------------
void TformSpikeWare::ScaleClusterPlots(int nChannel)
{
   unsigned int n;
   for (n = 0; n < m_vpformCluster.size(); n++)
      {
      // check, which one to update
      if (  m_pformEpoches->cbAllAxis->Checked
         || m_vpformCluster[n]->Tag == nChannel
         || nChannel < 0
         )
         {
         ScaleClusterPlot(m_vpformCluster[n]);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// scales (sets chart axis properties) of a cluster plot with respect to
/// used spike parameters
//------------------------------------------------------------------------------
void TformSpikeWare::ScaleClusterPlot(TformCluster* pfrm)
{
   double dMin = m_pformEpoches->m_vpformEpoches[(unsigned int)pfrm->Tag]->chrt->LeftAxis->Minimum;
   double dMax = m_pformEpoches->m_vpformEpoches[(unsigned int)pfrm->Tag]->chrt->LeftAxis->Maximum;


   // check if X axis to be updated at all
   if (  pfrm->m_spX == SP_PEAK1
      || pfrm->m_spX == SP_PEAK2
      || pfrm->m_spX == SP_PEAKPOS
      || pfrm->m_spX == SP_PEAKNEG
      )
      pfrm->chrt->BottomAxis->SetMinMax(dMin, dMax);

   // special for SP_TOTALAMPLITUDE
   if (pfrm->m_spX == SP_TOTALAMPLITUDE)
      {
      double dMax2 = dMax - dMin;
      pfrm->chrt->BottomAxis->SetMinMax(0.0, dMax2);
      }


   // check if Y axis to be updated at all
   if (  pfrm->m_spY == SP_PEAK1
      || pfrm->m_spY == SP_PEAK2
      || pfrm->m_spY == SP_PEAKPOS
      || pfrm->m_spY == SP_PEAKNEG
      )
      pfrm->chrt->LeftAxis->SetMinMax(dMin, dMax);

   // special for SP_TOTALAMPLITUDE
   if (pfrm->m_spY == SP_TOTALAMPLITUDE)
      {
      double dMax2 = dMax - dMin;
      pfrm->chrt->LeftAxis->SetMinMax(0.0, dMax2);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls UpdateColors for all cluster plots and re-plots spikes
//------------------------------------------------------------------------------
void TformSpikeWare::UpdateClusterColors()
{
   unsigned int n;
   for (n = 0; n < m_vpformCluster.size(); n++)
      m_vpformCluster[n]->UpdateColors();
   PlotSpikes();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a spike group index to corresponding color
//------------------------------------------------------------------------------
TColor   TformSpikeWare::SpikeGroupToColor(int n)
{
   if (n < 0)
      return clGray;
   return m_vclSpikeColors[(unsigned int)n % m_vclSpikeColors.size()];
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// SMP processing callback BEFORE VST plugins. Calls particular processing for
/// free seacrh, trigger test or calibration repectively
//------------------------------------------------------------------------------
void TformSpikeWare::SMPPreVSTProc(vvf &vvfBuffers)
{
   try
      {
      // signal generator for free search and calibration
      if (formSpikeWare->m_bFreeSearchRunning)
         formSpikeWare->m_smp.SoundFreeSearchSignalGenerator(vvfBuffers);
      else if (formSpikeWare->m_smp.m_nCalibrate)
         formSpikeWare->m_smp.SoundCalibrationSignalGenerator(vvfBuffers);
      // or playback trigger detecor for triggertest
      else if (formSpikeWare->m_bTriggerTestRunning)
         formSpikeWare->m_sweEpoches.SoundProcTriggerTestPlay(vvfBuffers);
      }
   catch (Exception &e)
      {
      OutputDebugStringW((UnicodeString(__FUNC__) + ": " + e.Message).w_str());
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// SMP processing callback AFTER VST plugins. Calls clip detection 
//------------------------------------------------------------------------------
void TformSpikeWare::SMPPostVSTProc(vvf &vvfBuffers)
{
   try
      {
      // clip detector for
      if (formSpikeWare->m_bFreeSearchRunning || formSpikeWare->m_smp.m_nCalibrate)
         formSpikeWare->m_smp.SoundClipDetector(vvfBuffers);
      }
   catch (Exception &e)
      {
      OutputDebugStringW((UnicodeString(__FUNC__) + ": " + e.Message).w_str());
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// SMP processing callback AFTER VST plugins in maximum search mode. Calls 
/// maximum search processing
//------------------------------------------------------------------------------
void TformSpikeWare::SMPPostVSTProcMaxSearch(vvf &vvfBuffers)
{
   formSpikeWare->m_smp.SoundMaxSearchProc(vvfBuffers);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// SMP processing callback BEFORE recodring (!) VST plugins. Updates recording
/// clip indicators
//------------------------------------------------------------------------------
void TformSpikeWare::SMPRecPreVSTProc(vvf &vvfBuffers)
{
   try
      {
      // create vector with booleans for clipping
      std::vector<bool > vb;

      unsigned int n;
      for (n = 0; n < vvfBuffers.size(); n++)
         vb.push_back(vvfBuffers[n].max() >= 1.0f || vvfBuffers[n].min() <= -1.0f);

      if (formSpikeWare->FormsCreated())
         formSpikeWare->m_pformEpoches->ShowClipping(vb);
      }
   catch (Exception &e)
      {
      OutputDebugStringW((UnicodeString(__FUNC__) + ": " + e.Message).w_str());
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// SMP processing callback AFTER recodring (!) VST plugins. Calls special 
/// processing procedures for calibration and calibrator calibration 
//------------------------------------------------------------------------------
void TformSpikeWare::SMPRecPostVSTProc(vvf &vvfBuffers)
{
   try
      {
      if (formSpikeWare->m_smp.m_nCalibrate == CAL_TYPE_SPEC)
         formFFTEdit->ProcessRecordBuffer(vvfBuffers);
      else
         formCalibrationCalibrator->ProcessRecordBuffer(vvfBuffers);

      }
   catch (Exception &e)
      {
      OutputDebugStringW((UnicodeString(__FUNC__) + ": " + e.Message).w_str());
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// SMP 'done' proc (i.e. 'OnPlay' callback). Calls epoches SoundProc procedure
//------------------------------------------------------------------------------
void TformSpikeWare::SMPBufferDoneProc(vvf &vvfBuffers)
{
   try
      {
      formSpikeWare->m_sweEpoches.SoundProc(vvfBuffers, formSpikeWare->m_bTriggerTestRunning);
      }
   catch (Exception &e)
      {
      OutputDebugStringW((UnicodeString(__FUNC__) + ": " + e.Message).w_str());
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// static procedure: called by SMP on stimulus change (special notfy callback 
/// in SoundMexProAS). Calls SoundNotify of current instance.
//------------------------------------------------------------------------------
void TformSpikeWare::SMPNotifyProc(void)
{
   try
      {
      if (formSpikeWare->m_bFreeSearchRunning)
         return;
      formSpikeWare->SoundNotify();
      }
   catch (Exception &e)
      {
      OutputDebugStringW((UnicodeString(__FUNC__) + ": " + e.Message).w_str());
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Sets flag that stimulus selection display should be updated 
//------------------------------------------------------------------------------
void TformSpikeWare::SoundNotify(void)
{
   m_nStimPlayIndex++;
   if (!m_bTriggerTestRunning)
      {
      if ((int)m_viStimSequence.size() > m_nStimPlayIndex)
         m_bUpdateStimulusDisplay = true;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Updates current stimulus selection in stimulus window AND checks for Xruns
//------------------------------------------------------------------------------
bool TformSpikeWare::UpdateStimulusDisplay()
{
   if (!FormsCreated())
      return false;

   // show current stimulus and repetition
   int nNumStim   = (int)(int)m_swsStimuli.m_swstStimuli.size();
   int nStimIndex =  (m_nStimPlayIndex % nNumStim) + 1;

   UnicodeString us = "Stimuli - Repetition ";
   us += IntToStr(m_viRepetitionSequence[(unsigned int)m_nStimPlayIndex]+1) + "/" + IntToStr((int)m_swsStimuli.m_nNumRepetitions);

   us += ", Stimulus ";
   us += IntToStr(nStimIndex) + "/" + IntToStr(nNumStim);

   m_pformStimuli->Caption = us;
   m_pformStimuli->SelectItem(m_viStimSequence[(unsigned int)m_nStimPlayIndex]);

   // check for xruns
   if (m_smp.Initialized())
      {
      int n = m_smp.GetXRuns();
      if (n > 1)
         {
         btnStopClick(NULL);
         SWErrorBox("A buffer underrun error occurred. The measurement was stopped! Check your hardware!");
         return false;
         }
      }

   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns current stimulus index
//------------------------------------------------------------------------------
unsigned int TformSpikeWare::GetCurrentStimulus(unsigned int nEpochesTotal)
{
   if (m_SearchModeStimIndex >= 0)
      return (unsigned int)m_SearchModeStimIndex;
   else
      return (unsigned int)m_viStimSequence[nEpochesTotal];
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enables/disables epoche timer and stores 'desired' status in it's tag
//------------------------------------------------------------------------------
void TformSpikeWare::EnableEpocheTimer(bool bEnable)
{
   EpocheTimer->Enabled = bEnable;
   EpocheTimer->Tag     = (int)bEnable;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// EpocheTimer callback. Processes pending epoche(s), does trigger/trigger-jitter
/// detection and loads next stimuli
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::EpocheTimerTimer(TObject *Sender)
{
   try
      {
      if (!IsRunning() || !FormsCreated())
         return;

      EpocheTimer->Enabled = false;


      bool b = ProcessEpoches();

      Sleep(1);

      if (m_sweEpoches.m_bTriggerError)
         {
         m_sweEpoches.m_bTriggerError = false;

         // retrieve xruns
         int n = m_smp.GetXRuns();
         if (m_bFreeSearchRunning)
            m_pformSearchFree->btnStopClick(NULL);
         else
            btnStopClick(NULL);
         SWErrorBox("A trigger error occurred (jitter: "
                     + m_sweEpoches.m_usTriggerError
                     + ", xruns: "
                     + IntToStr(n)
                     + "). The measurement was stopped!.");
         return;
         }

      if (m_sweEpoches.m_nFirstTriggerError > 0)
         {
         if (m_bFreeSearchRunning)
            m_pformSearchFree->btnStopClick(NULL);
         else
            btnStopClick(NULL);
         SWErrorBox("The first trigger could not be found (error " + IntToStr(m_sweEpoches.m_nFirstTriggerError) + ") . The measurement was stopped! Please check your hardware!");
         return;
         }

      if (m_bUpdateStimulusDisplay)
         {
         m_bUpdateStimulusDisplay = false;
         if (!UpdateStimulusDisplay())
            return;
         }
      m_smp.LoadStimuli();

      if (EpocheTimer->Tag && IsRunning())
         EpocheTimer->Enabled = true;
      }
   catch (Exception &e)
      {
      OutputDebugStringW(e.Message.w_str());
      throw;
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// processes pending epoches
//------------------------------------------------------------------------------
bool TformSpikeWare::ProcessEpoches()
{
   try
      {
      if (!m_sweEpoches.Count())
         return false;
      TSWEpoche *pswe      = NULL;

      bool bLast = false;
      while (1)
         {
         // get an epoche
         pswe = m_sweEpoches.Pop(bLast);
         if (!pswe)
            break;

         // set epoche done in XML
         SetXMLEpocheDone((int)pswe->m_nIndex, true);
         // add spikes for this epoche
         m_swsSpikes.Add(pswe);

         if (bLast)
            {
            PlotEpoches(pswe);
            TRYDELETENULL(pswe);
            PlotSpikes();
            PlotClusters();
            return true;
            }
         TRYDELETENULL(pswe);

         Application->ProcessMessages();
         if (m_bBreak)
            break;
         }
      return true;
      }
   catch (Exception &e)
      {
      OutputDebugStringW(e.Message.w_str());
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for miSettings: shows settings dialog
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::miSettingsClick(TObject *Sender)
{
   if (!FormsCreated())
      return;

   m_smp.ReadSettings(false, true);
   m_pformSettings->ShowModal();
   ReadSettings();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of miAdjustSpikeLength: updates spike length and - if it 
/// has changed at all - run a rescan
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::miAdjustSpikeLengthClick(TObject *Sender)
{
   if (!FormsCreated())
      return;

   double dValue = m_swsSpikes.m_dSpikeLength*1000.0;
   dValue = (double)StrToFloat(FormatFloat("0.00", (Extended)dValue));

   if (!m_pformSetParameters->SetParameter("Spike-Length", "ms", dValue, this))
      return;

   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wfloat-equal"
   if (dValue != m_swsSpikes.m_dSpikeLength)
      {
      m_swsSpikes.Clear();
      m_swsSpikes.SetSpikeLength(m_swsSpikes.m_dPreThreshold, m_swsSpikes.m_dPostThreshold, dValue/1000.0);
      m_pformSpikes->Initialize();
      if (btnRescanSpikes->Enabled)
         LoadEpoches(SWELM_SPIKES);
      }
   #pragma clang diagnostic pop
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnLoadTemplate: loads a measurement template
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnLoadTemplateClick(TObject *Sender)
{
   od->FileName   = "";
   od->InitialDir = m_usTemplatePath;
   if (od->Execute())
      {
      if (!LoadMeasurementTemplate(od->FileName))
         return;
      SetMeasurementChanged(false);
      m_pIni->WriteString("Settings", "LastTemplatePath", IncludeTrailingBackslash(ExtractFileDir(od->FileName)));
      }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnClick callback of btnLoadResult: loads a measurement result
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnLoadResultClick(TObject *Sender)
{
   od->FileName   = "";
   od->InitialDir = ExpandFileName(m_usResultPathRoot);
   if (od->Execute())
      LoadMeasurementResult(od->FileName);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnAppend: lets user load an XML to be appended to current
/// result
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnAppendClick(TObject *Sender)
{
   od->FileName   = "";
   od->InitialDir = ExpandFileName(m_usTemplatePath);
   if (od->Execute())
      {
      if (!AppendMeasurement(od->FileName))
         return;
      m_pIni->WriteString("Settings", "LastTemplatePath", IncludeTrailingBackslash(ExtractFileDir(od->FileName)));
      }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnClick callback of btnSave: calls SaveResult
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnSaveClick(TObject *Sender)
{
   SaveResult();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Enables/disables monitor channel
//------------------------------------------------------------------------------
void TformSpikeWare::SetMonitor(TformEpoches* pfrm)
{
   if (!m_smp.Initialized())
      {
      pfrm->tbnListen->Down = false;
      return;
      }
   if (!pfrm->tbnListen->Down)
      m_smp.SetMonitor(-1);
   else
      {
      m_smp.SetMonitor((int)pfrm->Tag);
      if (FormsCreated())
         m_pformEpoches->UpdateListenButtons(pfrm);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets GUI status to passed status
//------------------------------------------------------------------------------
void TformSpikeWare::SetGUIStatus(TSWGuiStatus gs)
{
   if (gs != SWGS_UNCHANGED)
      m_gs = gs;

   bool bRunning  =  IsRunning();

   btnLoadTemplate->Enabled   = !bRunning && !IsBatchMode();
   btnLoadResult->Enabled     = btnLoadTemplate->Enabled;
   btnBatch->Enabled          = btnLoadTemplate->Enabled;
   miLoadTemplate->Enabled    = btnLoadTemplate->Enabled;
   miLoadResult->Enabled      = btnLoadTemplate->Enabled;

   btnAppend->Enabled         = btnLoadTemplate->Enabled && m_gs == SWGS_RESULTLOADED;
   miAppend->Enabled          = btnAppend->Enabled;


   btnSave->Enabled     = !bRunning && !IsBatchMode() && xml->Active;
   miSave->Enabled      = btnSave->Enabled;

   btnCluster->Enabled  =  m_gs > SWGS_NONE
                        && m_gs != SWGS_FREESEARCHRUN
                        && m_gs != SWGS_FREESEARCHSTOP;
   miCluster->Enabled   = btnCluster->Enabled;

   btnParam->Enabled    = btnCluster->Enabled;
   miParameter->Enabled = btnParam->Enabled;

   btnSearch->Enabled   = m_gs == SWGS_LOADED && !IsBatchMode();
   btnStop->Enabled     = bRunning
                        && m_gs != SWGS_FREESEARCHRUN
                        && m_gs != SWGS_FREESEARCHSTOP
                        && !IsBatchMode();


   btnPause->Enabled    = !IsBatchMode() && (m_gs == SWGS_RUN || m_gs == SWGS_PAUSE);

   btnFreeSearch->Enabled = !IsBatchMode()
                           && (m_gs == SWGS_NONE
                              || m_gs == SWGS_LOADED
                              || m_gs == SWGS_RESULTLOADED);

   miSettings->Enabled = btnFreeSearch->Enabled && ! bRunning;

   miAdjustSpikeLength->Enabled = m_gs == SWGS_RESULTLOADED;

   btnRescanSpikes->Enabled   = !bRunning && (m_gs == SWGS_RESULTLOADED || m_gs == SWGS_STOP);
   btnReloadEpoches->Enabled  = btnRescanSpikes->Enabled;

   miUpdateCheck->Enabled      = btnLoadTemplate->Enabled;



   btnInSitu->Enabled = m_gs == SWGS_NONE;

   // run button may :
   // - run new measurement (m_gs == SWGS_LOADED)
   // - resume incomplete stopped measurement (m_gs == SWGS_STOPPED && unfinished epoches present)
   // - resume incomplete loaded measurement (m_gs == SWGS_SWGS_RESULTLOADED && unfinished epoches present)

   bool bUnfinishedEpoches = EpochesXML(true) < EpochesXML(false);
   btnRun->Enabled      = !IsBatchMode()
                        && (m_gs == SWGS_LOADED
                           || (m_gs == SWGS_RESULTLOADED && bUnfinishedEpoches)
                           || (m_gs == SWGS_STOP && bUnfinishedEpoches)
                           );

   if (IsBatchMode())
      m_pformBatch->UpdateGUI(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns GUI status string depending on current status
//------------------------------------------------------------------------------
UnicodeString  TformSpikeWare::GetStatusString()
{
   // some enum values not handled by purpose...
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wswitch-enum"
   switch (m_gs)
      {
      case SWGS_NONE:         return "none";
      case SWGS_LOADED:       return "template loaded";
      case SWGS_RESULTLOADED: return "result loaded";
      case SWGS_SEARCH:       return "searching";
      case SWGS_RUN:          return "running";
      case SWGS_PAUSE:        return "paused";
      case SWGS_STOP:         return "stopped";
      default:                return "unknown";
      }
   #pragma clang diagnostic pop
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns true if status is in any of the 'running' or paused states (i.e. 
/// NOT idle)
//------------------------------------------------------------------------------
bool TformSpikeWare::IsRunning()
{
   return            m_gs == SWGS_SEARCH
                  || m_gs == SWGS_RUN
                  || m_gs == SWGS_PAUSE
                  || m_gs == SWGS_FREESEARCHRUN
                  || m_gs == SWGS_FREESEARCHSTOP;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnReloadEpoches: calls LoadEpoches(SWELM_NOSPIKES)
//------------------------------------------------------------------------------
void __fastcall TformSpikeWare::btnReloadEpochesClick(TObject *Sender)
#pragma argsused
{
   LoadEpoches(SWELM_NOSPIKES);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnRescanSpikes: calls LoadEpoches(SWELM_SPIKES_RESET_THRESHOLD)
//------------------------------------------------------------------------------
void __fastcall TformSpikeWare::btnRescanSpikesClick(TObject *Sender)
#pragma argsused
{
   // ask user....
   if (ID_YES != MessageBox(  Handle,
                              "Are you sure, that you want to rescan all spikes with current threshold (all individual epoche thresholds will be cleared)?",
                              "Question",
                              MB_ICONQUESTION | MB_YESNO)
      )
      return;
   // call loadepoches with resetting thresholds
   LoadEpoches(SWELM_SPIKES_RESET_THRESHOLD);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows an error message box
//------------------------------------------------------------------------------
void TformSpikeWare::SWErrorBox(UnicodeString us, HWND hwnd)
{
   if (formWait->Visible)
      formWait->Hide();
   MessageBoxW(hwnd, us.w_str(), L"Error", MB_ICONERROR);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMessage callback of Application object WM_SWCMD message that is sent by
/// a second nstance, when it has written it's command line parameters to a file
//------------------------------------------------------------------------------
void __fastcall TformSpikeWare::AppMessage(tagMSG &Msg, bool &Handled)
{
   if (Msg.message == WM_SWCMD)
      {
      // CHECK IF ANY INTERACTION ALLOWED
      if (  m_gs > SWGS_RESULTLOADED

         || Application->ModalLevel > 0)
         {
         SetStatusMsg("External call to " + m_usASCaption + " detected while not allowed");
         return;
         }

      UnicodeString us;
      if (!!m_pIni)
         {
         int nParamCount = m_pIni->ReadInteger("IPC", "ParamCount", 0);
         if (nParamCount > 0)
            {
            m_pslParamStr->Clear();
            int n;
            for (n = 0; n < nParamCount; n++)
               m_pslParamStr->Add(m_pIni->ReadString("IPC", "Param" + IntToStr(n+1), ""));
            ProcessCommandLine(false);
            }
         }
      Handled = true;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets current status message and enabled status timer
//------------------------------------------------------------------------------
void TformSpikeWare::SetStatusMsg(UnicodeString us)
{
   sb->Panels->Items[SB_P_STATUS]->Text = us;
   StatusTimer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnDrawPanel callback of statusbar: sets red font color for status string
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::sbDrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel,
          const TRect &Rect)
{
   if (Panel->Index == SB_P_STATUS)
      {
      sb->Canvas->Font->Style = TFontStyles() << fsBold;
      sb->Canvas->Font->Color = clRed;
      sb->Canvas->TextOut(Rect.Left, Rect.Top, Panel->Text);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// StausTimer callback: resets status string again
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::StatusTimerTimer(TObject *Sender)
{
   StatusTimer->Enabled = false;
   sb->Panels->Items[SB_P_STATUS]->Text = "";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates stimulus sequence with respect to randomization
//------------------------------------------------------------------------------
void TformSpikeWare::CreateStimulusSequence( std::vector<int > &rvn,
                                             unsigned int nSize,
                                             int nOffset
                                             )
{
   rvn.clear();
   unsigned int nStim, nRep;

   std::vector<int > vnStimIndices;
   for (nStim = 0; nStim < nSize; nStim++)
      vnStimIndices.push_back((int)nStim + nOffset);

   for (nRep = 0; nRep < m_swsStimuli.m_nNumRepetitions; nRep++)
      {
      if (m_swsStimuli.m_nRandom)
         random_shuffle(vnStimIndices.begin(), vnStimIndices.end());
      for (nStim = 0; nStim < nSize; nStim++)
         rvn.push_back(vnStimIndices[nStim]);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets GUI busy/unbusy
//------------------------------------------------------------------------------
void TformSpikeWare::SetGUIBusy(bool bBusy, TWinControl* pctrt)
{
   Cursor = bBusy ? crHourGlass : crDefault;

   if (!pctrt)
      pctrt = this;
   // enable disable all controls
   int n;
   for (n = 0; n < pctrt->ControlCount; n++)
      {
      pctrt->Controls[n]->Enabled = !bBusy;
      TWinControl* p = dynamic_cast<TWinControl*>(pctrt->Controls[n]);
      // recurse call
      if (p)
         SetGUIBusy(bBusy, p);
      }
   // enable/disable all main menu items
   if (pctrt == this)
      {
      for (n = 0; n < mnuMain->Items->Count; n++)
         {
         mnuMain->Items->Items[n]->Enabled = !bBusy;
         }
      }
   Application->ProcessMessages();
   if (!bBusy)
      SetGUIStatus();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Debug function for saving current XML as it is
//------------------------------------------------------------------------------
void TformSpikeWare::DebugSaveXML(int n)
{
   xmlSave->Active = false;
   xmlSave->XML->Text = FormatXMLData(formSpikeWare->xml->XML->Text);
   xmlSave->Active = true;
   xmlSave->SaveToFile(xml->FileName + "." + IntToStr(n) +  ".xml");
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// INI file conversion function for converting existing INIfiles, if format has
/// changed for newer AudioSpike versions.
/// Up to now ONE conversion available: old INI files do NOT contain a version 
/// info, current version is fix '1'. Conversion moves calibration values to 
/// separate INI file
//------------------------------------------------------------------------------
void TformSpikeWare::ConvertIniFile()
{
   int nCurrentIniVersion = 1;
   int nIniVersion = m_pIni->ReadInteger("Version", "Ini", 0);
   if (nIniVersion == nCurrentIniVersion)
      return;

   // move calibrations to separate file
   if (nIniVersion < 1)
      {
      UnicodeString usSection, usSectionNew;
      TStringList *pslSections  = new TStringList();
      TStringList *pslSection  = new TStringList();
      try
         {
         m_pIni->ReadSections(pslSections);
         int n, m;
         for (n = 0; n < pslSections->Count; n++)
            {
            usSection = pslSections->Strings[n];
            if (usSection.Pos("Calibration") != 1)
               continue;

            m_pIni->ReadSectionValues(usSection, pslSection);

            if (usSection == "Calibration_")
               usSectionNew = "IR_EMPTY";
            else if (usSection.Pos(".wav") != 0)
               usSectionNew = L"IR" + usSection.SubString(12, usSection.Length());
            else
               usSectionNew = L"FFT";

            for (m = 0; m < pslSection->Count; m++)
               {
               m_pCalIni->WriteString(usSectionNew, pslSection->Names[m], pslSection->Values[pslSection->Names[m]]);
               }
            m_pIni->EraseSection(usSection);
            }
         }
      __finally
         {
         TRYDELETENULL(pslSections);
         TRYDELETENULL(pslSection);
         }
      }
   // store, that it's already converted
   m_pIni->WriteInteger("Version", "Ini", nCurrentIniVersion);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for miHelp: shows help PDF
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::miHelpClick(TObject *Sender)
{
   UnicodeString us = ExpandFileName(IncludeTrailingBackslash(ExtractFilePath(Application->ExeName)) +  "..\\manual\\Manual.AudioSpike.pdf");
   ShellExecuteW(Handle, NULL, us.w_str(), NULL,  NULL, SW_SHOWNORMAL);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// OnClick callback for miAbout: shows about box
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::miAboutClick(TObject *Sender)
{
   AboutBox->ShowModal();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for UpdateCheck: checks for updates
//------------------------------------------------------------------------------
void __fastcall TformSpikeWare::miUpdateCheckClick(TObject *Sender)
{
   // store date, when update check was done last
   m_pIni->WriteDateTime("Settings", "LastUpdateCheckDate", Now());

   //
   TVersionChecker   vch;
   if (!vch.ReadVersionHistoryURL("https://www.audiospike.de/downloads/history.txt"))
      {
      // show an error only if a sender was passed (i.e. invoked from menu)
      if (Sender != NULL)
         SWErrorBox("Unkown error checking for updates", Handle);
      return;      
      }

   UnicodeString usVersion = GetFileVersion();
   // for testing this feature: read a fake current version number from ini
   UnicodeString usTestVersion = m_pIni->ReadString("Debug", "TestUpdateVersion", "");
   if (usTestVersion.Length())
      usVersion = usTestVersion;
      
   if (vch.VersionIsLatest(usVersion))
      {
      // show feedback on "no update available" only if a sender was passed (i.e. invoked from menu)
      if (Sender != NULL)
         MessageBoxW(Handle, L"You are running the latest version of AudioSpike", L"Information", MB_ICONINFORMATION);
      }
   else
      {
      TformVersionCheck* pfrm = new TformVersionCheck(this);
      pfrm->DoShowModal(vch, "AudioSpike", usVersion, "https://www.audiospike.de/download");
      TRYDELETENULL(pfrm);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Shows a special warning about RMS values and influence on final level output
//------------------------------------------------------------------------------
bool TformSpikeWare::RMSWarning()
{
   UnicodeString usCaption    = "Warning";
   UnicodeString usIntruction = "RMS values missing";
   UnicodeString usText       = "You have not specified an RMS value for one or more signals. "
                                "The missing RMS values are calculated automatically. This may lead to unexpected levels.";
   UnicodeString usCheckText  = "Don't show this again";

   bool bShowAgain            = true;

   TTaskDialog *ptdlg = NULL;
   try
      {
      ptdlg                   = new TTaskDialog(formSpikeWare);
      ptdlg->Caption          = usCaption;
      ptdlg->Title            = usIntruction;
      ptdlg->Text             = usText;
      ptdlg->VerificationText = usCheckText;
      ptdlg->MainIcon         = tdiWarning;
      ptdlg->CommonButtons    = TTaskDialogCommonButtons() << tcbOk;

      ptdlg->Execute(NULL);

      bShowAgain              = !ptdlg->Flags.Contains(tfVerificationFlagChecked);
      }
   // fallback for XP (does not now TTaskDialog)
   catch (...)
      {
      MessageBoxW(Handle, usText.w_str(), usCaption.w_str(), MB_ICONWARNING);
      }

   TRYDELETENULL(ptdlg);

   return bShowAgain;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Shows special warning if a Hi-Pass is active and that user should check used
/// stimulus frequencies
//------------------------------------------------------------------------------
void TformSpikeWare::HighPassWarning()
{
   if (!m_pIni->ReadBool("Settings", "ShowHiPassWarning", true))
      return;


   UnicodeString usCaption    = "Warning";
   UnicodeString usIntruction = "Hi-Pass in operation";
   UnicodeString usText       = "You have entered Hi-Pass frequencies for one or more output channel. "
                                "Be sure that you don't use lower frequencies in your measurement!";
   UnicodeString usCheckText  = "Don't show this again";

   bool bShowAgain            = true;

   TTaskDialog *ptdlg = NULL;
   try
      {
      ptdlg                   = new TTaskDialog(formSpikeWare);
      ptdlg->Caption          = usCaption;
      ptdlg->Title            = usIntruction;
      ptdlg->Text             = usText;
      ptdlg->VerificationText = usCheckText;
      ptdlg->MainIcon         = tdiWarning;
      ptdlg->CommonButtons    = TTaskDialogCommonButtons() << tcbOk;


      ptdlg->Execute(NULL);

      bShowAgain              = !ptdlg->Flags.Contains(tfVerificationFlagChecked);
      }
   // fallback for XP (does not now TTaskDialog)
   catch (...)
      {
      MessageBoxW(Handle, usText.w_str(), usCaption.w_str(), MB_ICONWARNING);
      }

   TRYDELETENULL(ptdlg);

   if (!bShowAgain)
      m_pIni->WriteBool("Settings", "ShowHiPassWarning", false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnInSitu: toggles InSitu mode
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnInSituClick(TObject *Sender)
{
   SetInSitu(btnInSitu->Down, true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns, if AudioSpike is in InSitu mode
//------------------------------------------------------------------------------
bool TformSpikeWare::IsInSitu()
{
   return btnInSitu->Down;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets InSitue mode on/off
//------------------------------------------------------------------------------
void TformSpikeWare::SetInSitu(bool b, bool bForce)
{
   // nothing to change?
   if (!bForce && btnInSitu->Down == b)
      return;
   btnInSitu->Down = b;
   btnInSitu->Caption = btnInSitu->Down ? "In-Situ-Mode" : "Standard-Mode";
   m_smp.m_bSettingsRead = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns flag, if AusioSpike is in batch mode
//------------------------------------------------------------------------------
bool TformSpikeWare::IsBatchMode()
{
   return !!m_pformBatch;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnBatch: starts batch mode
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::btnBatchClick(TObject *Sender)
{
   if (IsBatchMode())
      return;
   if (SaveInquiry() == ID_CANCEL)
      return;
   Cleanup();
   SetMeasurementChanged(false);
   m_pformBatch = new TformBatch(NULL);
   SetGUIStatus();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// cleans up batch mode
//------------------------------------------------------------------------------
void TformSpikeWare::CleanupBatchMode()
{
   TRYDELETENULL(m_pformBatch);
   SetGUIStatus();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnResize callback of status bar for adjusting panel widths
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSpikeWare::sbResize(TObject *Sender)
{
   sb->Panels->Items[SB_P_STATUS]->Width = sb->Width
      - sb->Panels->Items[SB_P_FLOPPY]->Width
      - sb->Panels->Items[SB_P_CONFIG]->Width
      - sb->Panels->Items[SB_P_DEMO]->Width;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns total number of running plot commands. Not sed, for debugging only
//------------------------------------------------------------------------------
int TformSpikeWare::NumRunningPlotCommands()
{
   int nReturn = 0;

   if (!!m_pformBubbleData)
      {
      nReturn += m_pformBubbleData->framePSTH->m_nPlotCounter;
      nReturn += m_pformBubbleData->framePeriodHist->m_nPlotCounter;
      }

   if (!!m_pformSearch)
      nReturn += m_pformSearch->m_nPlotCounter;

   unsigned int n;
   for (n = 0; n < m_vpformBubblePlots.size(); n++)
      nReturn += m_vpformBubblePlots[n]->m_nPlotCounter;

   for (n = 0; n < m_vpformCluster.size(); n++)
      nReturn += m_vpformCluster[n]->m_nPlotCounter;

   if (!!m_pformPSTH)
      nReturn += m_pformPSTH->m_nPlotCounter;

   if (!!m_pformSignalPSTH)
      nReturn += m_pformSignalPSTH->m_nPlotCounter;

   if (!!m_pformSpikes)
      nReturn += m_pformSpikes->m_nPlotCounter;

   if (!!m_pformVectorStrength)
      nReturn += m_pformVectorStrength->m_nPlotCounter;

   if (!!m_pformEpoches)
      {
      for (n = 0; n < m_pformEpoches->m_vpformEpoches.size(); n++)
         nReturn += m_pformEpoches->m_vpformEpoches[n]->m_nPlotCounter;
      }

   return nReturn;
}
//------------------------------------------------------------------------------




