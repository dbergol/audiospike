//------------------------------------------------------------------------------
/// \file frmStimuli.h
///
/// \author Berg
/// \brief Implementation of a form to display a list of used Stimuli
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
#ifndef frmStimuliH
#define frmStimuliH
#include <System.Classes.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "frmASUI.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form to display a list of used Stimuli
//------------------------------------------------------------------------------
class TformStimuli : public TformASUI
{
   __published:	// IDE-verwaltete Komponenten
      TPanel *pnl;
      TListView *lv;
      void __fastcall FormShow(TObject *Sender);
      void __fastcall lvSelectItem(TObject *Sender, TListItem *Item, bool Selected);
      void __fastcall lvAdvancedCustomDrawItem(TCustomListView *Sender, TListItem *Item,
             TCustomDrawState State, TCustomDrawStage Stage, bool &DefaultDraw);
   private:	// Benutzer-Deklarationen
      void CreateCols();
   public:		// Benutzer-Deklarationen
      __fastcall TformStimuli(TComponent* Owner, TMenuItem* pmi);
      __fastcall ~TformStimuli();
      void Clear();
      void Load();
      void ReadColWidths();
      void WriteColWidths();
      void SelectItem(int nIndex);
};
//------------------------------------------------------------------------------
#endif
