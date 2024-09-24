//------------------------------------------------------------------------------
/// \file frmFFTEdit.cpp
///
/// \author Berg
/// \brief Implementation of a form to display and edit spectral filters and for
/// in-situ calibration
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
#pragma hdrstop

#include "frmFFTEdit.h"
#include "SpikeWareMain.h"
#include "SWTools.h"
#include "frmWait.h"
#include "frmCalibration.h"
#include "frmCalibrationCalibrator.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TformFFTEdit *formFFTEdit;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor, initializes members
//------------------------------------------------------------------------------
__fastcall TformFFTEdit::TformFFTEdit(TComponent* Owner)
   : TForm(Owner), m_bCalibratorCallback(false), m_pFFT(NULL), m_lpfnFilter(NULL)
{
   InitializeCriticalSection(&csDataSection);

   FilterSeriesL->XValues->Order    = loAscending;
   RawFilterSeries->XValues->Order   = loAscending;
   ExtProcSeries->XValues->Order   = loAscending;


   m_usFilterFile = formSpikeWare->m_swfFilters->m_pFilterIni->FileName;

   // factor for painting spectrum reasonable with respect to fullscale
   m_fVisSpecFactor = (float)dBToFactor(15.0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor: does cleanup
//------------------------------------------------------------------------------
__fastcall TformFFTEdit::~TformFFTEdit()
{
   formSpikeWare->StoreFormPos(this);
   TRYDELETENULL(m_pFFT);
   DeleteCriticalSection(&csDataSection);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback. reads ini and starts timer for initialization (calling "Init()"
/// directly in OnShow led to problems - not really clear why...)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::FormShow(TObject *Sender)
{
   m_bUseCalibrator = formSpikeWare->m_pIni->ReadBool("Settings", "UseCalibrator", false);
   formSpikeWare->RestoreFormPos(this);

   // enable for painting spectra and overdrive LED
   m_bCalibratorCallback = false;
   m_bRecData = false;
   Timer->Interval = 50;
   Timer->Tag = 0;
   Timer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Timer callback. If never called (see also FormShow) it calls Init() otherwise
/// ExtProcSeries is updated and repainted
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::TimerTimer(TObject *Sender)
{
   static int nTimerCount = 0;
   Timer->Enabled = false;
   if (!Timer->Tag)
      {
      Timer->Tag = 1;
      Init();
      }
   else if (m_bRecData)
      {
      m_bRecData = false;
      // copy last stored wave data: block csDataSection as short as possible!
      EnterCriticalSection(&csDataSection);
      try
         {
         m_vafRecPaint[0] = m_vafRec[0];
         }
      __finally
         {
         LeaveCriticalSection(&csDataSection);
         }
      // do the FFT and plot it
      m_pFFT->Wave2Spec(m_vafRecPaint, m_vacRec, false);
      // NOTE: first bin in spec is DC !!
      unsigned int nSize = (unsigned int)m_vacRec[0].size();
      for (unsigned int n = 1; n < nSize; n++)
         ExtProcSeries->YValues->Value[(int)n-1] = (double)(m_fVisSpecFactor*abs(m_vacRec[0][n]));
      ExtProcSeries->Repaint();

      shClip->Brush->Color = (m_vafRec[0].max() >= 1.0f || m_vafRec[0].min() <= -1.0f) ? clRed : clLime;
      }

   // if trackbar was changed, apply change only, if left mouse button is up again!
   if ((nTimerCount % 10) == 0)
      {
      if (tb->Tag && GetAsyncKeyState(VK_LBUTTON) == 0)
         {
         tb->Tag = 0;
         SetLevel();
         }
      }
   nTimerCount++;
   Timer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets calibration mode and channels and calls ShowModal
//------------------------------------------------------------------------------
TModalResult TformFFTEdit::Calibrate(int nChannelOutHardware, TCalMode cmMode)
{
   m_cmCalMode    = cmMode;
   m_nChannelOutHardware  = nChannelOutHardware;
   m_usChannel    = formSpikeWare->m_smp.m_swcHWChannels.m_vvswcChannels[SWSMPHWCDIR_OUT][(unsigned int)m_nChannelOutHardware].m_usName;
   return ShowModal();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes everything for current calmode (FFT, series, chart .....)
//------------------------------------------------------------------------------
void TformFFTEdit::Init()
{
   try
      {
      FilterSeriesL->Clear();
      RawFilterSeries->Clear();
      ExtProcSeries->Clear();
      double dMinFreq = 0.0;
      double dMaxFreq = 0.0;
      if (m_cmCalMode == CAL_MODE_MIC_PROBE)
         {
         UnicodeString usSection = formSpikeWare->m_smp.GetInSituInputFilterSection(m_usChannel);
         dMinFreq = StrToDouble(formSpikeWare->m_swfFilters->m_pFilterIni->ReadString(usSection, "MinFreq", "100"));
         dMaxFreq = StrToDouble(formSpikeWare->m_swfFilters->m_pFilterIni->ReadString(usSection, "MaxFreq", "16000"));
         }


      // init SMP for specral calibration. Function returns pointer to filter function
      // in filter plugin
      m_lpfnFilter   = formSpikeWare->m_smp.InitSpectralCalibration(m_nChannelOutHardware, m_cmCalMode);
      if (!m_lpfnFilter)
         {
         ModalResult = mrCancel;
         return;
         }

      tbtnDraw->Enabled       = m_cmCalMode == CAL_MODE_SPEAKER;
      tbtnZoom->Enabled       = tbtnDraw->Enabled;
      tbtnClear->Enabled      = tbtnDraw->Enabled;
      tbtnCalc->Enabled       = tbtnDraw->Enabled;
      tbtnImport->Enabled     = tbtnDraw->Enabled;
      tbtnExport->Enabled     = tbtnDraw->Enabled;
      btnMinMax->Enabled      = tbtnDraw->Enabled;
      Chart->AllowZoom        = tbtnZoom->Enabled && tbtnZoom->Down;
      tbtnPlay->Visible       = m_cmCalMode != CAL_MODE_INSITU;
      tbtnStop->Visible       = m_cmCalMode != CAL_MODE_INSITU;
      if (!tbtnDraw->Enabled)
         tbtnZoom->Down       = true;

      pnlControl->Visible     = m_cmCalMode == CAL_MODE_SPEAKER || m_cmCalMode == CAL_MODE_INSITU;

      m_dProbeMicRMSAt100dB = 0.0;
      Caption        = "Equalisation - " + m_usChannel;

      m_bRecord      = formSpikeWare->m_smp.m_swcHWChannels.GetRefMic() >= 0;

      #ifdef CHKCHNLS
      if (formSpikeWare->m_smp.m_nMicChannelIn != formSpikeWare->m_smp.m_swcHWChannels.GetRefMic())
         ShowMessage("error in " + UnicodeString(__FUNC__));
      #endif

      iOldXPos       = -1;
      iCapturedValue = -1;
      m_bChanged     = false;
      m_nAvgIn       = -1;
      MinSeries->X1  = 100;
      MaxSeries->X0  = 16000;

      SetPosCaption(0, 1);

      formSpikeWare->m_swfFilters->GetHiPass(m_usChannel, m_fHighPass);
      m_dLevel       = -40.0;

      shClip->Visible      = m_bRecord;
      shClip->Brush->Color = clLime;

      tbtnCalc->Enabled       = false;
      ExtProcSeries->Active   = m_bRecord;

      m_nFFTLen     = (unsigned int)formSpikeWare->m_smp.m_nEqFFTLen;
      m_fSampleRate = (float)formSpikeWare->m_swsStimuli.m_dDeviceSampleRate;

      m_f4BinSize  = m_fSampleRate / (float)m_nFFTLen;

      double dX = 0.0;
      unsigned int i;
      for (i = 0; i < m_nFFTLen/2; i++)
         {
         dX += (double)m_f4BinSize;
         FilterSeriesL->AddXY(dX, 1, "", clTeeColor);
         RawFilterSeries->AddXY(dX, 1, "", clTeeColor);
         ExtProcSeries->AddXY(dX, 1, "", clTeeColor);
         }

      tbSmooth->Max = m_nFFTLen/10;
      // store this value in tag additionally!
      tbSmooth->Tag = tbSmooth->Max;

      Chart->BottomAxis->Maximum = (double)m_fSampleRate/2.0;
      Chart->BottomAxis->Minimum = (double)m_f4BinSize;


      MinSeries->Y0 = Chart->LeftAxis->Minimum;
      MinSeries->Y1 = Chart->LeftAxis->Maximum;
      MaxSeries->Y0 = Chart->LeftAxis->Minimum;
      MaxSeries->Y1 = Chart->LeftAxis->Maximum;

      MinSeries->X0 = -1000;
      MaxSeries->X1 = 2*Chart->BottomAxis->Maximum;

      ModalResult = mrNone;
      tb->Position = -400;


      TRYDELETENULL(m_pFFT);
      m_pFFT = new CHtFFT(m_nFFTLen);

      m_nRecBufferPos = 0;

      m_vafRec.resize(1);
      m_vafRecAvg.resize(1);
      m_vafRecPaint.resize(1);
      m_vacRec.resize(1);     // rec spectrum


      m_vafRec[0].resize(m_nFFTLen);
      m_vafRec[0] = 0.0f;
      m_vafRecAvg[0].resize(m_nFFTLen);
      m_vafRecAvg[0] = 0.0f;
      m_vafRecPaint[0].resize(m_nFFTLen);
      m_vafRecPaint[0] = 0.0f;
      m_vacRec[0].resize(m_nFFTLen/2+1);


      // now apply highpass
      if ((double)m_fHighPass > 0.0 && MinSeries->X1 < (double)m_fHighPass)
         MinSeries->X1 = (double)m_fHighPass;

      tbtnPlay->Enabled       = !formSpikeWare->m_smp.Playing();
      m_nAvgSeconds           = formSpikeWare->m_pIni->ReadInteger("Settings", "FFTAvgSeconds", 10);
      if (m_cmCalMode != CAL_MODE_INSITU)
         cbExpertMode->Checked   = formSpikeWare->m_pIni->ReadBool("Settings",   "ExpertMode", false);
      else
         cbExpertMode->Enabled = false;
      cbLog->Checked          = formSpikeWare->m_pIni->ReadBool("Settings",   "LogFreq", true);
      cbLog->Checked          = formSpikeWare->m_pIni->ReadBool("Debug",      "FFTPluginLogChecked", cbLog->Checked);

      cbLogClick(NULL);
      cbExpertModeClick(NULL);

      SetLevel(true);
      FilterToChart();

      // for in-situ we need to use Min/Max and calibration value from INUT filter of probe mic!!
      if (m_cmCalMode == CAL_MODE_INSITU)
         {
         try
            {
            UnicodeString usSection = formSpikeWare->m_smp.GetInSituInputFilterSection(m_usChannel);
            MinSeries->X1 = StrToDouble(formSpikeWare->m_swfFilters->m_pFilterIni->ReadString(usSection, "MinFreq", ""));
            MaxSeries->X0 = StrToDouble(formSpikeWare->m_swfFilters->m_pFilterIni->ReadString(usSection, "MaxFreq", ""));
            m_dProbeMicRMSAt100dB = StrToDouble(formSpikeWare->m_swfFilters->m_pFilterIni->ReadString(usSection, "RMS@100dB", ""));
            }
         catch (...)
            {
            throw Exception("Invalid probe microphone filter. Please repeat calibration of probe microphone");
            }
         }

      if (m_cmCalMode == CAL_MODE_MIC_PROBE)
         {
         MinSeries->X1 = dMinFreq;
         MaxSeries->X0 = dMaxFreq;
         }

      // call MinMaxChange to generate the tone complex
      MinMaxChange();

      if (m_cmCalMode == CAL_MODE_MIC_REF)
         {
         if (!SetMinMaxFreq())
            {
            ModalResult = mrCancel;
            return;
            }
         UnicodeString usSection = formSpikeWare->m_smp.GetInSituInputFilterSection(m_usChannel);
         formSpikeWare->m_swfFilters->m_pFilterIni->WriteInteger(usSection, "MinFreq", (int)MinSeries->X1);
         formSpikeWare->m_swfFilters->m_pFilterIni->WriteInteger(usSection, "MaxFreq", (int)MaxSeries->X0);

         // NOTE: with calibrator the instruction to place the microphone is not necessary: done
         // in Calibrator-dialog itself
         if (!m_bUseCalibrator)
            MessageBox(Handle, "Please connect/place the reference microphone now and click 'Ok'", "Info", MB_OK | MB_ICONINFORMATION);

         CalProbeMic_Step1();
         }
      else if (m_cmCalMode == CAL_MODE_INSITU)
         {
         if (!formSpikeWare->m_smp.Command("start", "length=0"))
            throw Exception("error running SMP for calibration");
         btnOk->Caption = "Start";
         Timer->Enabled = true;
         MessageBox(Handle, "Please connect/place the probe microphone now, check the signal, adjust the volume and click 'Start'", "Info", MB_OK | MB_ICONINFORMATION);
         }
      }
   catch (Exception &e)
      {
      UnicodeString us = "Error running calibration: " + e.Message;
      MessageBoxW(Handle, us.w_str(), L"Info", MB_OK | MB_ICONERROR);
      ModalResult = mrCancel;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnCloe callback. Exits calibration and stores settings
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::FormClose(TObject *Sender, TCloseAction &Action)
{
   Timer->Tag = 0;
   Timer->Enabled = false;
   formSpikeWare->m_smp.Exit();
   formSpikeWare->StoreFormPos(this);
   formSpikeWare->m_pIni->WriteBool("Settings", "LogFreq", cbLog->Checked);
   formSpikeWare->m_pIni->WriteBool("Settings", "ExpertMode", cbExpertMode->Checked);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of trackbar: sets level
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbChange(TObject *Sender)
{
   // store, that trackbar was changed (with mouse): change is applied in
   // TimerTimer
   tb->Tag = 1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets level through SMP
//------------------------------------------------------------------------------
void TformFFTEdit::SetLevel(bool bForce)
{

   EnableControls(false);
   try
      {
      double dLevel = (double)tb->Position / 10.0;
      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wfloat-equal"
      if (bForce || (GetAsyncKeyState(VK_LBUTTON) == 0 && dLevel != m_dLevel))
         {
         m_dLevel = dLevel;
         formSpikeWare->m_smp.Command("volume", "value=" + DoubleToStr(dBToFactor(dLevel)));
         }
      #pragma clang diagnostic pop
      }
   __finally
      {
      EnableControls(true);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enables/disables controls
//------------------------------------------------------------------------------
void TformFFTEdit::EnableControls(bool bEnable)
{
   // have to set tb->Enabled twice: no idea why....
   tb->Enabled       = !bEnable;
   tb->Enabled       = bEnable;
   tbtnStop->Enabled = bEnable;
   tbtnPlay->Enabled = bEnable && !formSpikeWare->m_smp.Playing();
   btnOk->Enabled    = bEnable;
   btnCancel->Enabled = bEnable;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of OK button. Saves filter or enters next in-situ step
/// respectively
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::btnOkClick(TObject *Sender)
{
   // in-situ....
   if (m_cmCalMode == CAL_MODE_INSITU)
      InSituCal_Step1();
   else if (m_cmCalMode == CAL_MODE_SPEAKER)
      {
      FilterSave(m_usFilterFile, m_usChannel);
      ModalResult = mrOk;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of play button. Starts playback.
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnPlayClick(TObject *Sender)
{
   if (formSpikeWare->m_smp.Playing())
      return;

   if (formSpikeWare->m_pIni->ReadBool("Debug", "ShowMixer", false))
      formSpikeWare->m_smp.Command("showmixer");


   if (!formSpikeWare->m_smp.Command("start", "length=0"))
      throw Exception("error running SMP for calibration");

   tbtnCalc->Enabled = m_bRecord;
   tbtnPlay->Enabled = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of stop button. Stops playback.
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnStopClick(TObject *Sender)
{
   tbtnCalc->Enabled = false;
   formSpikeWare->m_smp.Stop();
   tbtnPlay->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///  changes frequency borders and re-creates schroeder tone complex.
//------------------------------------------------------------------------------
void TformFFTEdit::MinMaxChange()
{
   // stop, if currently running
   bool bPlaying = formSpikeWare->m_smp.Playing();

   if (bPlaying)
      formSpikeWare->m_smp.Stop();

   double dMin = MinSeries->X1;
   if (dMin < Chart->BottomAxis->Minimum)
      dMin = Chart->BottomAxis->Minimum;
   double dMax = MaxSeries->X0;
   if (dMax > Chart->BottomAxis->Maximum)
      dMax = Chart->BottomAxis->Maximum;
   formSpikeWare->m_smp.CreateSchroederPhaseToneComplex((float)dMin, (float)dMax);

   // restart, if we played on function entry
   if (bPlaying)
      {
      if (!formSpikeWare->m_smp.Command("start", "length=0"))
         throw Exception("error running SMP for calibration");
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///      BELOW CHART FUNCTIONS ORIGINALLY FROM PLUGIN !
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Writes current mouse position to footer of chart
//------------------------------------------------------------------------------
void  TformFFTEdit::SetPosCaption(double dX, double dY)
{
   int n = (int)floor(dX);
   if (n < 0)
      n = 0;
   UnicodeString us;
   us.printf(L"Position:     %d Hz  %d dB", n, (int)floor(FactorTodB(dY)));
   Chart->Foot->Text->Text = us;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnBeforeDrawAxes of chart: paints chart rect black
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::ChartBeforeDrawAxes(TObject *Sender)
{
   Chart->Canvas->Brush->Color   = clBlack;
   Chart->Canvas->Pen->Color     = clBlack;
   Chart->Canvas->FillRect(Chart->ChartRect);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseMove callback of chart: paints new filter to chart (if enabled at all!)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::ChartMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
   if (tbtnZoom->Down || !cbExpertMode->Checked)
      return;
   // painting filter
   if ( PtInRect((RECT*)&Chart->ChartRect, Point(X-Chart->Width3D,Y+Chart->Height3D)))
      {
      double tmpX,tmpY;
      // set label text
      FilterSeriesL->GetCursorValues(tmpX,tmpY);  // <-- get values under mouse cursor
      SetPosCaption(tmpX, tmpY);


      if (Shift.Contains(ssRight) || Shift.Contains(ssLeft) )
         {
         TFastLineSeries *tl = RawFilterSeries;

         int iPos = -1;
         for (int i = 0; i < tl->XValues->Count; i++)
            {
            if (tmpX < tl->XValues->Value[i])
               {
               iPos = tl->XValues->Locate(tl->XValues->Value[i]);
               break;
               }
            }

         if (iPos != -1)
            {
            m_bChanged = true;
            if (iOldXPos == iPos || iOldXPos == -1)
               tl->YValues->Value[iPos] = tmpY;
            else
               {
               int iTmp = tl->XValues->Locate(tl->XValues->Value[iOldXPos]);
               double dStep   = (tl->YValues->Value[iTmp] - tmpY) / double(iPos - iOldXPos);
               double dVal    = tl->YValues->Value[iTmp];

               if (iOldXPos < iPos)
                  {
                  for (int i = iOldXPos; i <= iPos; i++)
                     {
                     tl->YValues->Value[i] = dVal;
                     dVal -= dStep;
                     }
                  }
               else
                  {
                  for (int i = iOldXPos; i >= iPos; i--)
                     {
                     tl->YValues->Value[i] = dVal;
                     dVal += dStep;
                     }
                  }

               }
            iOldXPos = iPos;
            tl->Repaint();
            }
         }
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseUp callback of chart: stops painting new filter and applies it and 
/// resets smoothing
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::ChartMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
   iOldXPos = -1;
   iCapturedValue = -1;
   if (m_bChanged)
      {
      ChartToFilter();
      ResetSmoothing();
      }
   m_bChanged = false;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseDown callback of chart: does un-zoom on right mouse button
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::ChartMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
   if (Button == mbRight)
      Chart->UndoZoom();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets reasonable borders and labels for log X-axis
//------------------------------------------------------------------------------
#pragma argsused
void TformFFTEdit::AdjustLogAxis(TChartAxis* pca)
{
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wundefined-func-template"
   pca->Items->Clear();
   #pragma clang diagnostic pop
   pca->Items->Automatic = true;
   pca->AxisValuesFormat = "";
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wfloat-equal"
   if (!pca->Logarithmic || pca->Maximum == pca->Minimum)
      return;
   #pragma clang diagnostic pop
   pca->Items->Automatic = false;

   Application->ProcessMessages();
   int n = 10;
   int m, q;
   pca->Items->Add(n,n);
   while (1)
      {
      m = (int)pow(10, floor(log10((float)n)));
      n += m;
      q = n/m;
      UnicodeString us;
      if (q == 2 || q == 5 || q == 10)
         pca->Items->Add(n,n);
      else
         pca->Items->Add(n,"");

      if (n >pca->Maximum)
         break;
      }

   Chart->Repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnMinMax: calls SetMinMaxFreq
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::btnMinMaxClick(TObject *Sender)
{
   SetMinMaxFreq();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows dialog to adjust min and max frequency for filter
//------------------------------------------------------------------------------
bool TformFFTEdit::SetMinMaxFreq()
{
   bool bReturn = false;
   TformSetParameters* pfrm = new TformSetParameters(this);
   try
      {
      double dMin = MinSeries->X1;
      double dMax = MaxSeries->X0;
      double dHighPass = (double)m_fHighPass;
      bReturn = pfrm->SetParameters("Min. Frequency", "", dMin, "Max. Frequency", "", dMax, this);
      if (bReturn)
         {
         if (dHighPass != 0.0 && dMin < dHighPass)
            dMin = dHighPass;
         MinSeries->X1 = dMin;
         MaxSeries->X0 = dMax;
         MinMaxChange();
         }
     }
   __finally
      {
      TRYDELETENULL(pfrm)
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Enables drawing (to be sure re-enables zoom) 
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnDrawClick(TObject *Sender)
{
   Chart->AllowZoom = tbtnZoom->Down;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of zoom button. Allows/disallows zooming of chart
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnZoomClick(TObject *Sender)
{
   Chart->AllowZoom = tbtnZoom->Down;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of clear button: resets filter to 0 dB
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnClearClick(TObject *Sender)
{
   ResetSmoothing();
   for (int i = 0; i < FilterSeriesL->XValues->Count; i++)
      {
      FilterSeriesL->YValues->Value[i] = 1;
      RawFilterSeries->YValues->Value[i] = 1;
      }

   FilterSeriesL->Repaint();
   RawFilterSeries->Repaint();
   ChartToFilter();
   FilterClear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of tbSmooth: cahnges filter smoothing properties and applies 
/// it to filter
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbSmoothChange(TObject *Sender)
{
   if (!cbLog->Checked || tbSmooth->Position == tbSmooth->Min)
      {
      int nSize = 2*tbSmooth->Position - 1;
      lbSmoothingPar->Caption =  "Bins: " + IntToStr(nSize) +
                                 " (" + DoubleToDisplayStr((double)(nSize*m_f4BinSize), "%.2lf") + "Hz)";
      }
   else
      {
      lbSmoothingPar->Caption =  "1/" + IntToStr(tbSmooth->Max - tbSmooth->Position + 1) +
                                 " Octave";
      }

   FilterSmooth();
   ChartToFilter();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enables/disables smoothing
//------------------------------------------------------------------------------ 
void TformFFTEdit::EnableSmoothing(bool bEnable)
{
   lbSmoothing->Enabled = bEnable;
   lbSmoothingPar->Enabled = bEnable;
   tbSmooth->Enabled = bEnable;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resets smooting to 'none'
//------------------------------------------------------------------------------
void TformFFTEdit::ResetSmoothing()
{
   // check if position is already "resetted"
   if (tbSmooth->Position == 1)
      tbSmoothChange(NULL);
   else
      tbSmooth->Position = 1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of cbLog: toggles X-Axis lin/log
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::cbLogClick(TObject *Sender)
{
   Chart->BottomAxis->Logarithmic = cbLog->Checked;
   tbSmooth->Max = cbLog->Checked ? 13 : (int)tbSmooth->Tag;
   AdjustLogAxis(Chart->BottomAxis);
   if (!!Sender)
      ResetSmoothing();

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of cbLog: toggles 'expert' mode (i.e. enabels/disables 
/// smooting)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::cbExpertModeClick(TObject *Sender)
{
   if (!cbExpertMode->Checked && !!Sender)
      ResetSmoothing();
   tbSmooth->Enabled = cbExpertMode->Checked;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// proc called by static external callback from TformSpikeWare::SMPRecPreVSTProc
/// NOTE: currently we access first channel HARD: always only one recording channel
/// currently during spectral calibration!!
//------------------------------------------------------------------------------
#pragma argsused
void TformFFTEdit::ProcessRecordBuffer(vvaf& rvvaf)
{
   if (m_bCalibratorCallback)
      {
      formCalibrationCalibrator->ProcessRecordBuffer(rvvaf);
      return;
      }

   if (!m_pFFT)
         return;

   try
      {
      int nSourcePos = 0;
      int nSamples = (int)rvvaf[0].size();

      // fill wave buffer (multiple times if necessary)
      while (nSamples > 0)
         {
         // available space in buffer
         int nSpace = (int)(m_vafRec[0].size() - m_nRecBufferPos);

         // samples to copy
         int nCopy = nSamples <= nSpace ? nSamples : nSpace;

         // copy data: block csDataSection as short as possible!
         EnterCriticalSection(&csDataSection);
         try
            {
            CopyMemory(&m_vafRec[0][(unsigned int)m_nRecBufferPos], &rvvaf[0][(unsigned int)nSourcePos], sizeof(float)*(unsigned int)nCopy);
            nSourcePos += nCopy;

            m_nRecBufferPos += (unsigned int)nCopy;
            nSamples -= nCopy;

            // if buffer is full do the FFT!!
            if (m_nRecBufferPos == m_vafRec[0].size())
               {
               if (  m_nAvgIn >= 0
                  && m_nAvgIn < m_nNumAvg
                  )
                  {
                  m_vafRecAvg[0] += m_vafRec[0];
                  m_nAvgIn++;
                  }

               m_nRecBufferPos = 0;
               }
            }
         __finally
            {
            LeaveCriticalSection(&csDataSection);
            }
         m_bRecData = true;
         }
       }
   catch (Exception &e)
      {
      UnicodeString us = "error in " + UnicodeString(__FUNC__) + ": " + e.Message;
      OutputDebugStringW(us.w_str());
      }
   catch (...)
      {
      UnicodeString us = "unknown error in " + UnicodeString(__FUNC__);
      OutputDebugStringW(us.w_str());
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ************      Filter save/resample ....
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Saves filter to ini-file
//------------------------------------------------------------------------------
bool __fastcall TformFFTEdit::FilterSave( UnicodeString sFileName,
                                          UnicodeString sSection,
                                          UnicodeString sName,
                                          UnicodeString sValue
                                          )
{
   // set border values
   FilterSetBorders(FilterSeriesL);
   // scale it to 1
   FilterNormalize(FilterSeriesL);

   sFileName = ExpandFileName(sFileName);
   TStringList *psl = new TStringList();
   try
      {
      FilterClear(sSection);

      if (FileExists(sFileName))
         psl->LoadFromFile(sFileName);
      // create section "by hand" (faster than INI-API)
      psl->Add("");
      psl->Add("[" + sSection + "]");
      psl->Add("Lin=1");
      psl->Add("MinFreq=" +  DoubleToStr(MinSeries->X1));
      psl->Add("MaxFreq=" + DoubleToStr(MaxSeries->X0));
      if (sName != "")
         psl->Add(sName + "=" + sValue);



      // go through filter
      float fFreq = 0.0f;
      for (int i = 0; i < FilterSeriesL->YValues->Count; i++)
         {
         sValue = DoubleToStr(FilterSeriesL->YValues->Value[i]);
         psl->Add(DoubleToStr((double)fFreq) + "=" + sValue);
         fFreq += m_f4BinSize;
         }
      psl->SaveToFile(sFileName);
      }
   catch (Exception &e)
      {
      TRYDELETENULL(psl);
      AnsiString str = "Error saving filter file '" + sFileName + "': " + e.Message;
      MessageBox(Handle, str.c_str(), "Error", MB_ICONERROR);
      return false;
      }
   TRYDELETENULL(psl);
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears filter from ini-file
//------------------------------------------------------------------------------
void __fastcall TformFFTEdit::FilterClear(UnicodeString sSection)
{
   if (sSection == "")
      sSection = m_usChannel;
   // read existing filter to keep additional 'non-numerical' values
   formSpikeWare->m_swfFilters->m_pFilterIni->EraseSection(sSection);
   formSpikeWare->m_swfFilters->m_pFilterIni->UpdateFile();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// pass filter from chart to plugin via special callback
//------------------------------------------------------------------------------
bool __fastcall TformFFTEdit::ChartToFilter()
{
   if (!m_lpfnFilter)
      return false;
   unsigned int nNumValues = (unsigned int)FilterSeriesL->YValues->Count;

   std::valarray<float >   vafTmp(nNumValues);

   for (unsigned int i = 0; i < nNumValues; i++)
      vafTmp[i] = (float)FilterSeriesL->YValues->Value[(int)i];

   m_lpfnFilter(FILTER_FCN_SETFILTER, &vafTmp, "");
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// copies filter to chart series
//------------------------------------------------------------------------------
void __fastcall TformFFTEdit::FilterToChart()
{
   if (!m_lpfnFilter)
      return;
   std::valarray<float >   vafTmp;

   // retrieve filter itself
   m_lpfnFilter(FILTER_FCN_GETFILTER, &vafTmp, "");
   unsigned int nNumValues = (unsigned int)vafTmp.size();

   for (unsigned int i = 0; i < nNumValues; i++)
      {
      FilterSeriesL->YValues->Value[(int)i]    = (double)vafTmp[i];
      RawFilterSeries->YValues->Value[(int)i]  = (double)vafTmp[i];
      }

   // retrieve borders
   m_lpfnFilter(FILTER_FCN_GETBORDERS, &vafTmp, "");
   MinSeries->X1 = (double)vafTmp[0];
   MaxSeries->X0 = (double)vafTmp[1];

   // now apply highpass
   double dHighPass = (double)m_fHighPass;
   if (dHighPass > 0.0 && MinSeries->X1 < dHighPass)
      MinSeries->X1 = dHighPass;

   FilterSeriesL->Repaint();
   RawFilterSeries->Repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets spectral filter borders in BINS
//------------------------------------------------------------------------------
void __fastcall TformFFTEdit::SpecSetBorders(vac & rvac)
{
   // calculate bins for upper and lower frequency
   unsigned int nMin = (unsigned int)ceil(MinSeries->X1 / (double)m_f4BinSize);
   unsigned int nMax = (unsigned int)floor(MaxSeries->X0 / (double)m_f4BinSize);
   unsigned int n;
   for (n = 0; n < nMin; n++)
      rvac[n] = rvac[nMin];
   for (n = (unsigned int)rvac.size(); n > nMax; n--)
      rvac[n-1] = rvac[nMax];
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets spectral filter borders in Hz
//------------------------------------------------------------------------------
void __fastcall TformFFTEdit::FilterSetBorders(TChartSeries *pcs)
{
   // calculate bins for upper and lower frequency
   int nMin = (int)ceil(MinSeries->X1 / (double)m_f4BinSize);
   int nMax = (int)floor(MaxSeries->X0 / (double)m_f4BinSize);
   int n;
   for (n = 0; n < nMin; n++)
      pcs->YValues->Value[n] = pcs->YValues->Value[nMin];
   for (n = pcs->YValues->Count; n > nMax; n--)
      pcs->YValues->Value[n-1] = pcs->YValues->Value[nMax];
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// normalizes a filter to 0 sB
//------------------------------------------------------------------------------
void __fastcall TformFFTEdit::FilterNormalize(TChartSeries *pcs)
{
   // NOTE: YValues->MaxValue seems to be buggy!!
   double dMax = -1000000.0;
   for (int i = 0; i < pcs->YValues->Count; i++)
      {
      if (pcs->YValues->Value[i] > dMax)
         dMax = pcs->YValues->Value[i];
      }
   for (int i = 0; i < pcs->YValues->Count; i++)
      pcs->YValues->Value[i] /= dMax;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// does filter smoothing
//------------------------------------------------------------------------------
void __fastcall TformFFTEdit::FilterSmooth()
{
   int nSize = 2*tbSmooth->Position - 1;
   if (nSize == 1)
      {
      for (int i = 0; i < FilterSeriesL->YValues->Count; i++)
         FilterSeriesL->YValues->Value[i] = RawFilterSeries->YValues->Value[i];
      FilterSeriesL->Repaint();
      return;
      }

   // calculate bins for upper and lower frequency
   int nMin = (int)floor(MinSeries->X1 / (double)m_f4BinSize);
   int nMax = (int)ceil(MaxSeries->X0 / (double)m_f4BinSize);
   int n,m;
   bool bBreak;

   double dValue, dStartFreq, dStopFreq;
   int nNumBins, nStartBin, nStopBin;
   nSize = (nSize-1)/2;

   double dnthOctave = tbSmooth->Max - tbSmooth->Position + 1;

   for (n = nMin; n <= nMax; n++)
      {
      dValue   = 0.0;
      nNumBins = 0;

      // do nth-ocatve smoothing ....
      if (cbLog->Checked)
         {
         // center frequency
         double dCenter = (double)(n * m_f4BinSize);
         // caclulate half of octave-width up and down:
         //
         double dWidth = dCenter / 2.0 / dnthOctave;

         nStartBin = (int)ceil((dCenter - dWidth) / (double)m_f4BinSize);
         if (nStartBin < 0)
            nStartBin = 0;
         nStopBin = (int)floor((dCenter + dWidth) / (double)m_f4BinSize);
         if (nStopBin >= FilterSeriesL->YValues->Count)
            nStopBin = FilterSeriesL->YValues->Count;
         }
      // ... or linear bin-averaging
      else
         {
         nStartBin = n-nSize;
         if (nStartBin < 0)
            nStartBin = 0;
         nStopBin = n+nSize;
         if (nStopBin >= FilterSeriesL->YValues->Count)
            nStopBin = FilterSeriesL->YValues->Count;
         }

      if (nStartBin == nStopBin)
         {
         FilterSeriesL->YValues->Value[n] = RawFilterSeries->YValues->Value[nStartBin];
         }
      else
         {
         for (m = nStartBin; m < nStopBin; m++)
            {
            dValue += RawFilterSeries->YValues->Value[m];
            nNumBins++;
            }
         FilterSeriesL->YValues->Value[n] = dValue/(double)nNumBins;
         }
      }
   FilterSeriesL->Repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnCalc: starts avaeraging ob multiple recordings
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnCalcClick(TObject *Sender)
{
   tbtnClearClick(NULL);

   // reference microphone recording .....
   vaf vafFilter;
   if (!Average(vafFilter, "Averaging, please wait ..."))
      return;

   // copy filter to series
   unsigned int n;
   for (n = 0; n < m_nFFTLen/2;n++)
      RawFilterSeries->YValues->Value[(int)n] = (double)vafFilter[n];

   // set border values
   FilterSetBorders(RawFilterSeries);

   // scale it to 1
   FilterNormalize(RawFilterSeries);

   // apply smoothing
   tbSmoothChange(NULL);
   ChartToFilter();
   FilterSeriesL->Repaint();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Starts averaging of 10 seconds of recording
//------------------------------------------------------------------------------
bool  TformFFTEdit::Average(vaf &rvafFilter, UnicodeString usMsg, int nAvgSecondsDevider)
{
   rvafFilter.resize(m_nFFTLen/2);

   // average output and inputs and divide them
   m_vafRecAvg[0] = 0.0f;

   // average 10 seconds: we need 3*FFTLen per 'real samples' (overlap + buffering
   m_nNumAvg = (int)((m_nAvgSeconds/nAvgSecondsDevider) *m_fSampleRate / m_nFFTLen);
   if (m_nNumAvg < 40)
      m_nNumAvg = 40;

   // create own wait dialog (for centering)
   if (usMsg == "")
      usMsg = "Averaging, please wait ...";
   bool bBreak = false;
   TformWait *pfrmWait = new TformWait(this);
   pfrmWait->ShowWait(usMsg);
   try
      {
      Enabled = false;
      m_nAvgIn    = 0;
      int nLastAvgIn    = 0;
      DWORD dwIn  = GetTickCount();
      while (m_nAvgIn < m_nNumAvg)
         {
         Sleep(10);
         Application->ProcessMessages();
         if (m_nAvgIn != nLastAvgIn)
            {
            nLastAvgIn = m_nAvgIn;
            dwIn  = GetTickCount();
            }
         else if (GetTickCount() - dwIn > 3000)
            throw Exception("Unexpected averaging error 1");
         bBreak = pfrmWait->m_bCancel;
         if (bBreak)
            break;
         }
      }
   __finally
      {
      Enabled = true;
      TRYDELETENULL(pfrmWait);
      }

   if (bBreak)
      return false;

   m_vafRecAvg[0] /= (float)m_nNumAvg;

   //calculate ffts and amplitudes
   float fAmp;
   CHtFFT fft(m_nFFTLen);
   vvac vac(1);    // buffer for spectrum calculation using CHtFFT
   vac[0].resize(m_nFFTLen/2+1);

   fft.Wave2Spec(m_vafRecAvg, vac, false);

   // adjust borders NOW in spectrum: otherwise we might find zeroes....
   SpecSetBorders(vac[0]);

   unsigned int n;
   for (n = 0; n < m_nFFTLen/2;n++)
      {
      // NOTE: first bin in spec is DC !!
      fAmp = abs(vac[0][n+1]);
      if (fAmp == 0.0f)
         throw Exception("unexpected zeroes in recording buffer");
      rvafFilter[n] = 1.0f/fAmp;
      }
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnImport. Imports filter from file
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnImportClick(TObject *Sender)
{
   if (!m_lpfnFilter)
      return;

   Chart->Enabled = false;
   try
      {

      if (od->Execute())
         {
         m_lpfnFilter(FILTER_FCN_LOADFILE, NULL, od->FileName);
         FilterToChart();
         }
      }
   __finally
      {
      Application->ProcessMessages();
      Chart->Enabled = true;
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of tbtnExport. Exports filter to file
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFFTEdit::tbtnExportClick(TObject *Sender)
{
   Chart->Enabled = false;
   try
      {
      if (sd->Execute())
         FilterSave(sd->FileName, "Filter");
      }
   __finally
      {
      Application->ProcessMessages();
      Chart->Enabled = true;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Calibration step 1 for probe microphone calibration
//------------------------------------------------------------------------------
void TformFFTEdit::CalProbeMic_Step1()
{
   btnMinMax->Enabled = false;
   try
      {
      tbtnClearClick(NULL);

      if (!formSpikeWare->m_smp.Command("start", "length=0"))
         throw Exception("error running SMP for calibration");

      TModalResult mr;
      double dCalValue, dLevel;
      if (m_bUseCalibrator)
         {
         m_bCalibratorCallback = true;

         mr = formCalibrationCalibrator->Calibrate(this, false);
         m_bCalibratorCallback = false;
         dCalValue = formCalibrationCalibrator->m_dCalValue;
         dLevel = formCalibrationCalibrator->m_dLevel;
         }
      else
         {
         mr = formCalibration->Calibrate(this, false);
         dCalValue = formCalibration->m_dCalValue;
         dLevel = formCalibration->m_dLevel;
         }


      if (mr != mrOk || dCalValue == 0.0)
         {
         ModalResult = mrCancel;
         return;
         }


      // reference microphone recording .....
      if (!Average(m_vafFilterRef, "Averaging reference microphone signal, please wait ..."))
         {
         ModalResult = mrCancel;
         return;
         }


      formSpikeWare->m_smp.Stop();

      m_cmCalMode = CAL_MODE_MIC_PROBE;
      Init();

      if (formSpikeWare->m_pIni->ReadBool("Debug", "CalInsituMicTest", false))
         {
         UnicodeString us = "programname=test.ini##DUMMY2;";
         us += "input=" + IntToStr(0) + ";";
         us += "position=" + IntToStr(0) + ";";
         us += "type=input;";
         // return
         if (!formSpikeWare->m_smp.Command("vstprogramname", us))
            throw Exception("no");
         }


      // set same volume as on "Ok" in calibration dialog!!
      formSpikeWare->m_smp.Command("volume", "value=" + DoubleToStr(dBToFactor(dLevel)));

      if (!formSpikeWare->m_smp.Command("start", "length=0"))
         throw Exception("error running SMP for calibration");

      Timer->Enabled = true;
      MessageBox(Handle, "Please connect/place probe microphone now and click 'Ok'", "Info", MB_OK | MB_ICONINFORMATION);

      // automatically proceed with step 2!
      CalProbeMic_Step2();
      }
   catch(Exception &)
      {
      m_bCalibratorCallback = false;
      ModalResult = mrCancel;
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Calibration step 2 for probe microphone calibration
//------------------------------------------------------------------------------
void TformFFTEdit::CalProbeMic_Step2()
{
   try
      {
      vaf vafFilterProbe;
      if (!Average(vafFilterProbe, "Averaging probe microphone signal, please wait ..."))
         {
         ModalResult = mrCancel;
         return;
         }

      vafFilterProbe /= m_vafFilterRef;
      // copy filter to series
      unsigned int n;
      for (n = 0; n < m_nFFTLen/2;n++)
         RawFilterSeries->YValues->Value[(int)n] = (double)vafFilterProbe[n];

      // set border values
      FilterSetBorders(RawFilterSeries);

      // scale it to 1
      FilterNormalize(RawFilterSeries);

      // apply smoothing
      tbSmoothChange(NULL);
      ChartToFilter();
      FilterSeriesL->Repaint();

      vafFilterProbe = 0.0f;

      // now apply filter and calculate RMS in probe mic. To have a reasonable RMS
      // we do it twice: in first loop we look at min and max
      if (!Average(vafFilterProbe, "Calculating calibration for probe microphone, please wait ...", 2))
         {
         ModalResult = mrCancel;
         return;
         }

      // now vafFilterProbe contains inverted played spectrum...
      vafFilterProbe = 1.0f / vafFilterProbe;
      // determin min and max in spctrum
      double ddBMin = FactorTodB((double)vafFilterProbe.min());
      double ddBMax = FactorTodB((double)vafFilterProbe.max());

      // try to shift it towards min of -80 dB, with the follwing constraints:
      // - A: max in spectrum should not exceed -30
      // - B: absolute playback level should not exceed 90 dB
      double dGain = 0.0;
      if (ddBMin < -80.0)
         {
         dGain = -80.0 - ddBMin;
         // constraint A
         double dMaxGain = -30.0 - ddBMax;
         // constraint B
         if (formCalibration->m_dTargetLevel + dMaxGain > 90.0)
            dMaxGain = 90.0 - formCalibration->m_dTargetLevel;
         // apply contraint
         if (dGain > dMaxGain)
            dGain = dMaxGain;
         // any gain required: apply it and do averaging again....
         if (dGain > 0.0)
            {
            // set same volume as on "Ok" in calibration dialog!!
            formSpikeWare->m_smp.Command("volume", "value=" + DoubleToStr(dBToFactor(formCalibration->m_dLevel + dGain)));
            // now apply filter and calculate RMS in probe mic. To have a reasonable RMS
            // we do it twice: in first loop we look at min and max
            if (!Average(vafFilterProbe, "Calculating calibration for probe microphone, please wait ...", 2))
               {
               ModalResult = mrCancel;
               return;
               }
            }
         // now gain required: set it 0.0 for calculation below
         else
            dGain = 0.0;
         }



      // now we now THIS RMS corresponds to m_dTargetLevel dB;
      double dProbeRMS = (double)RMS(m_vafRecAvg[0]);
      // calculate RMS that would be measured @ 100 dB
      double dRMSAt100dB = dProbeRMS + 100.0 - formCalibration->m_dTargetLevel - dGain;
      // save filter passing the additional value!
      FilterSave( m_usFilterFile,
                  formSpikeWare->m_smp.GetInSituInputFilterSection(m_usChannel),
                  "RMS@100dB",
                  DoubleToStr(dRMSAt100dB)
                  );
      formSpikeWare->m_smp.Stop();

      MessageBox(Handle, "Calibration was completed successfully.", "Info", MB_OK | MB_ICONINFORMATION);

      ModalResult = mrOk;
      }
   catch(Exception &)
      {
      ModalResult = mrCancel;
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Calibration step 1 for in-situ calibration
//------------------------------------------------------------------------------
void TformFFTEdit::InSituCal_Step1()
{
   try
      {
      vaf vafFilter;
      // reference microphone recording .....
      if (!Average(vafFilter, "Averaging probe microphone signal, please wait ..."))
         {
         ModalResult = mrCancel;
         return;
         }
      // copy filter to series
      unsigned int n;
      for (n = 0; n < m_nFFTLen/2;n++)
         RawFilterSeries->YValues->Value[(int)n] = (double)vafFilter[n];

      // set border values
      FilterSetBorders(RawFilterSeries);

      // scale it to 1
      FilterNormalize(RawFilterSeries);

      // apply smoothing
      tbSmoothChange(NULL);
      ChartToFilter();
      FilterSeriesL->Repaint();
      m_cmCalMode = CAL_MODE_INSITU_LEVEL;

      // automatically proceed with step 2!
      InSituCal_Step2();
      }
   catch(Exception &)
      {
      ModalResult = mrCancel;
      throw;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Calibration step 2 for in-situ calibration
//------------------------------------------------------------------------------
void TformFFTEdit::InSituCal_Step2()
{
   try
      {
      FilterNormalize(RawFilterSeries);
      ChartToFilter();
      FilterSeriesL->Repaint();


      vaf vafFilter;
      // probe microphone recording .....
       if (!Average(vafFilter, "Calculating calibration for probe microphone, please wait ...", 2))
         {
         ModalResult = mrCancel;
         return;
         }
      double dProbeRMS =(double)RMS(m_vafRecAvg[0]);
      // this corresponds to level in dB:
      double dProbeLevel = 100.0 + (dProbeRMS - m_dProbeMicRMSAt100dB);
      // calculate the real calvalue from this value current SMP volume and RMS of tone complex
      double dCalValue = dProbeLevel - m_dLevel - (double)formSpikeWare->m_smp.m_fSchroederRMSdB;

      formSpikeWare->m_smp.SetCalibrationValue((unsigned int)m_nChannelOutHardware, dCalValue);
     // write filter and calvalue
      FilterSave(m_usFilterFile, m_usChannel + " - INSITU");

      ModalResult = mrOk;
      }
   catch(Exception &)
      {
      ModalResult = mrCancel;
      throw;
      }
}
//------------------------------------------------------------------------------


