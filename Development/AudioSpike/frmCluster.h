//------------------------------------------------------------------------------
/// \file frmCluster.h
///
/// \author Berg
/// \brief Implementation of a form to show a cluster plot
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
#ifndef frmClusterH
#define frmClusterH
//------------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.ImageList.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.TeeShape.hpp>
#include "SWSpike.h"
#include "frmASUI.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// classs for storing one cluster selection rectangle
//------------------------------------------------------------------------------
class TSWClusterSelection
{
   public:
      TSWClusterSelection();
      void     Clear();
      double   dX0;
      double   dX1;
      double   dY0;
      double   dY1;
      bool     bActive;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form for showing a clsuter plot + cluster selections
//------------------------------------------------------------------------------
class TformCluster : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TChart *chrt;
      TPointSeries *csData;
      TChartShape *csSelection;
      TToolBar *tb;
      TToolButton *tbtnSelActive;
      TToolButton *tbtnSelClear;
      TImageList *il1;
      TImageList *il2;
      TBevel *bvl;
      void __fastcall chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall chrtMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
      void __fastcall chrtMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y);
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall tbtnSelActiveClick(TObject *Sender);
      void __fastcall tbtnSelClearClick(TObject *Sender);
      void __fastcall csSelectionClick(TChartSeries *Sender, int ValueIndex, TMouseButton Button,
             TShiftState Shift, int X, int Y);
      void __fastcall chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis, TMouseButton Button,
             TShiftState Shift, int X, int Y);
   private:	// Benutzer-Deklarationen
      int         m_nCurrentSelectionIndex;
      bool        m_bSelectAllOnNoSelection;
      std::vector<TChartShape* > m_vpcs;
      void        SelectionsToSelectionSeries();
      void        SelectionToSelectionSeries(unsigned int nSelectionIndex);
      void        ResetActiveSpikeGroups();
   public:		// Benutzer-Deklarationen
      TSpikeParam          m_spX;
      TSpikeParam          m_spY;
      std::vector<bool >   m_vbSelActive;
      std::vector<std::vector<TSWClusterSelection > > m_vvSWSelections;
      int                  m_nPlotCounter;
      __fastcall TformCluster(TComponent* Owner, TSpikeParam spX, TSpikeParam spY);
      __fastcall ~TformCluster();
      void EnableSelection(bool bEnable);
      void ResetSelection(bool bInit = false);
      void Clear();
      void Plot(unsigned int nChannelIndex);
      void UpdateColors();
      void SetColors();
      void Initialize(unsigned int nChannels);
};
//------------------------------------------------------------------------------
#endif
