//------------------------------------------------------------------------------
/// \file frmFFTEdit.h
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
#ifndef frmFFTEditH
#define frmFFTEditH
#include <System.Classes.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Dialogs.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.TeeShape.hpp>
#include "HtFFT3.h"
#include "SWSMP.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form to display and edit spectral filters and for in-situ calibration
//------------------------------------------------------------------------------
class TformFFTEdit : public TForm
{
      friend class TfrmWaitAvg;
   __published:	// IDE-verwaltete Komponenten
      TPanel *pnlEq;
      TPanel *pnl;
      TButton *btnCancel;
      TButton *btnOk;
      TImageList *ildBottom;
      TImageList *ilBottom;
      TPanel *pnlControl;
      TToolBar *tbLower;
      TToolButton *tbtnPlay;
      TToolButton *tbtnStop;
      TTrackBar *tb;
      TImage *img2;
      TImage *img1;
      TImageList *il;
      TImageList *ild;
      TPanel *pnlButtons;
      TLabel *lbSmoothing;
      TLabel *lbSmoothingPar;
      TToolBar *tbUpper;
      TToolButton *tbtnDraw;
      TToolButton *tbtnZoom;
      TToolButton *tbSpace1;
      TToolButton *tbtnClear;
      TToolButton *tbSpace2;
      TToolButton *tbtnCalc;
      TToolButton *btnMinMax;
      TToolButton *tbSpace3;
      TToolButton *tbtnImport;
      TToolButton *tbtnExport;
      TTrackBar *tbSmooth;
      TChart *Chart;
      TCheckBox *cbLog;
      TCheckBox *cbExpertMode;
      TFastLineSeries *RawFilterSeries;
      TFastLineSeries *FilterSeriesL;
      TFastLineSeries *ExtProcSeries;
      TChartShape *MinSeries;
      TChartShape *MaxSeries;
      TLineSeries *DummySeries;
      TOpenDialog *od;
      TSaveDialog *sd;
      TTimer *Timer;
      TShape *shClip;
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall tbChange(TObject *Sender);
      void __fastcall btnOkClick(TObject *Sender);
      void __fastcall FormShow(TObject *Sender);
      void __fastcall tbtnPlayClick(TObject *Sender);
      void __fastcall tbtnStopClick(TObject *Sender);
      void __fastcall ChartBeforeDrawAxes(TObject *Sender);
      void __fastcall ChartMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
      void __fastcall ChartMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall ChartMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall btnMinMaxClick(TObject *Sender);
      void __fastcall tbtnDrawClick(TObject *Sender);
      void __fastcall tbtnZoomClick(TObject *Sender);
      void __fastcall tbtnClearClick(TObject *Sender);
      void __fastcall cbLogClick(TObject *Sender);
      void __fastcall tbSmoothChange(TObject *Sender);
      void __fastcall cbExpertModeClick(TObject *Sender);
      void __fastcall tbtnCalcClick(TObject *Sender);
      void __fastcall tbtnImportClick(TObject *Sender);
      void __fastcall tbtnExportClick(TObject *Sender);
      void __fastcall TimerTimer(TObject *Sender);

   private:	// Benutzer-Deklarationen
      _RTL_CRITICAL_SECTION   csDataSection;
      int                     iOldXPos;
      int                     iCapturedValue;
      bool                    m_bChanged;
      float                   m_f4BinSize;
      bool                    m_bRecord;
      double                  m_dLevel;
      bool                    m_bFilterLoaded;
      UnicodeString           m_usChannel;
      float                   m_fHighPass;
      int                     m_nUpdateInterval;
      unsigned int            m_nFFTLen;
      float                   m_fSampleRate;
      UnicodeString           m_usFilterFile;
      float                   m_fVisSpecFactor;
      bool                    m_bCalibratorCallback;
      bool                    m_bUseCalibrator;
      CHtFFT* m_pFFT;
      vvac m_vacRec;       // buffer for spectrum calculation using CHtFFT
      vvaf m_vafRec;       // buffer for collecting record data
      vvaf m_vafRecAvg;    // buffer for collecting record data (averaging)
      vvaf m_vafRecPaint;  // buffer for collecting record data for painting
      unsigned int   m_nRecBufferPos; // current position within record data buffer
      int            m_nAvgSeconds;
      int            m_nAvgIn;
      int            m_nNumAvg;
      vaf            m_vafFilterRef;
      double         m_dProbeMicRMSAt100dB;
      LPFNFILTER     m_lpfnFilter;
      TCalMode       m_cmCalMode;
      int            m_nChannelOutHardware;
      bool           m_bRecData;
      void SetLevel(bool bForce = false);
      void EnableControls(bool bEnable);
      void MinMaxChange();
      void SetPosCaption(double dX, double dY);
      void AdjustLogAxis(TChartAxis* pca);
      void ResetSmoothing();
      void EnableSmoothing(bool bEnable);
      bool SetMinMaxFreq();
      bool  Average(vaf &rvafFilter, UnicodeString usMsg, int nAvgSecondsDevider = 1);
      bool __fastcall ChartToFilter();
      void __fastcall FilterToChart();
      void __fastcall SpecSetBorders(vac & rvac);
      void __fastcall FilterSetBorders(TChartSeries *pcs);

      void __fastcall FilterNormalize(TChartSeries *pcs);
      void __fastcall FilterSmooth();
      bool __fastcall FilterSave(UnicodeString sFileName,
                                 UnicodeString sSection,
                                 UnicodeString sName = "",
                                 UnicodeString sValue = ""
                                 );
      void __fastcall FilterClear(UnicodeString sSection = "");
      void            Init();
      void            CalProbeMic_Step1();
      void            CalProbeMic_Step2();
      void            InSituCal_Step1();
      void            InSituCal_Step2();

   public:		// Benutzer-Deklarationen
      __fastcall TformFFTEdit(TComponent* Owner);
      __fastcall ~TformFFTEdit();
      TModalResult   Calibrate(int nChannelOutHardware, TCalMode cmMode);
      void           ProcessRecordBuffer(vvaf& rvvaf);

};
//------------------------------------------------------------------------------
extern PACKAGE TformFFTEdit *formFFTEdit;
//------------------------------------------------------------------------------
#endif
