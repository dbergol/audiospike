//------------------------------------------------------------------------------
/// \file frmFreeSearch.h
///
/// \author Berg
/// \brief Implementation of a form for free search, where stimulus parameters
/// can be selected in a 2d-plot
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
#ifndef frmSearchFreeH
#define frmSearchFreeH
//------------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
//------------------------------------------------------------------------------
#include "SWSpike.h"
#include "frmASUI.h"

class TformSelectChannels;

//------------------------------------------------------------------------------
/// form for free search, where stimulus parameters can be selected  in a 2d-plot
//------------------------------------------------------------------------------
class TformSearchFree : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TChart *chrt;
      TPointSeries *cs;
      TToolBar *tb;
      TToolButton *tbtnChannels;
      TImageList *il;
      TImageList *ild;
      TToolButton *tbtnSine;
      TToolButton *tbtnNoise;
      TToolButton *tbtnLog;
      TTimer *ClipTimer;
      TToolButton *btnStart;
      TToolButton *btnStop;
      TShape *shClip;
      TBevel *bvl;
      void __fastcall chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall tbtnChannelsClick(TObject *Sender);
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall tbtnSignalClick(TObject *Sender);
      void __fastcall tbtnLogClick(TObject *Sender);
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall chrtGetAxisLabel(TChartAxis *Sender, TChartSeries *Series, int ValueIndex,
             UnicodeString &LabelText);
      void __fastcall ClipTimerTimer(TObject *Sender);
      void __fastcall btnStartClick(TObject *Sender);
      void __fastcall btnStopClick(TObject *Sender);
   void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

   private:	// Benutzer-Deklarationen
      // overloaded base methods
      void ApplyMute();
      TformSelectChannels* m_pfrmSelectChannels;
      std::valarray<float >         m_vfSchroeder;
      float                         m_fSchroederRMSdB;
      float                         m_fSchroederLowFreq;
      float                         m_fSchroederHiFreq;
      bool                          m_bSchroederCached;
      int                           m_nClearCounter;
      void                          ClearData();
      void                          CreateFreeSearchSchroederPhaseToneComplex();

   public:		// Benutzer-Deklarationen
      __fastcall TformSearchFree(TComponent* Owner);
      __fastcall ~TformSearchFree();
      void     __fastcall ShowFreeSearch();
      bool     SetValues();
      float    m_fGain;
      float    m_fFrequency;
      void     SetSchroederPhaseToneComplex( float fLowFreq,
                                             float fHiFreq,
                                             bool bRandom = true,
                                             int nSamples = 0);
};
//------------------------------------------------------------------------------
#endif
