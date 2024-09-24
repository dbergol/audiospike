//------------------------------------------------------------------------------
/// \file frmBatch.h
///
/// \author Berg
/// \brief Implementation of a form for batch processing of multiple AudioSpike
/// mesurements
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
#ifndef frmBatchH
#define frmBatchH
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ToolWin.hpp>
#include "frmASUI.h"
#include <Vcl.Dialogs.hpp>
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for different batch actions
//------------------------------------------------------------------------------
enum TBatchActions {
   BA_NONE = 0,
   BA_CONTINUE,
   BA_STOP,
   BA_REPEAT,
   BA_SAVESKIP
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// TForm for handling batch processing actions
//------------------------------------------------------------------------------
class TformBatch : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TPanel *pnl;
      TListView *lv;
      TToolBar *tb;
      TImageList *ild;
      TImageList *il;
      TToolButton *btnAdd;
      TToolButton *btnDelete;
      TToolButton *btnUp;
      TToolButton *btnDown;
      TToolButton *btnSep1;
      TToolButton *btnRun;
      TToolButton *btnStopSaveStep;
      TToolButton *btnPause;
      TToolButton *btnStopRejectRepeat;
      TToolButton *btnSep2;
      TToolButton *btnStopBeforeNext;
      TToolButton *btnInSituCalBeforeNext;
      TToolButton *btnLoad;
      TToolButton *btnSave;
      TToolButton *ToolButton2;
      TSaveDialog *sd;
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall btnAddClick(TObject *Sender);
      void __fastcall btnDeleteClick(TObject *Sender);
      void __fastcall btnUpClick(TObject *Sender);
      void __fastcall btnDownClick(TObject *Sender);
      void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
      void __fastcall lvClick(TObject *Sender);
      void __fastcall lvKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
      void __fastcall lvResize(TObject *Sender);
      void __fastcall lvAdvancedCustomDrawItem(TCustomListView *Sender, TListItem *Item,
             TCustomDrawState State, TCustomDrawStage Stage, bool &DefaultDraw);
      void __fastcall btnLoadClick(TObject *Sender);
      void __fastcall btnSaveClick(TObject *Sender);
      void __fastcall btnRunClick(TObject *Sender);
      void __fastcall btnPauseClick(TObject *Sender);
      void __fastcall btnStopSaveStepClick(TObject *Sender);
      void __fastcall btnStopRejectRepeatClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
      void __fastcall lvWndProc(TMessage& Msg);
      TWndMethod     m_lvOldWndProc;  ///< Stores the old listview's WindowProc
      bool           m_bSaved;
      bool           m_bBatchRunning;
      int            m_nAction;
      UnicodeString  m_usSubPath;
      void           AddItem(UnicodeString us);
      bool           ItemIsPending(int nIndex);
      bool           m_bDoInsituCal;
      std::vector<int > m_viLastOutChannels;
   public:		// Benutzer-Deklarationen
      __fastcall TformBatch(TComponent* Owner);
      __fastcall ~TformBatch();
      void DoBatchRun();
      bool DoInsituCal();
      void MoveItem(TListItem* pli, bool bUp);
      void UpdateGUI(bool bUpdateMainGUI = true);
      void SelectItem(int nIndex);
      bool HasUnfinishedFilesInList();
};
//------------------------------------------------------------------------------
#endif
