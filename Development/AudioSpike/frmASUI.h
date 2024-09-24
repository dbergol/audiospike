//------------------------------------------------------------------------------
/// \file frmASUI.h
///
/// \author Berg
/// \brief Implementation of a TForm TformASUI: base class for all sub-forms of
/// AudioSpike
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
#ifndef frmASUIH
#define frmASUIH
//------------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.StdCtrls.hpp>
#include <System.ImageList.hpp>
#include <VCLTee.BubbleCh.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// base class for all AudioSpike (sub-)forms
//------------------------------------------------------------------------------
class TformASUI : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
      void __fastcall FormMouseActivate(TObject *Sender, TMouseButton Button, TShiftState Shift,
             int X, int Y, int HitTest, TMouseActivate &MouseActivate);
      void __fastcall FormShow(TObject *Sender);
   private:	// Benutzer-Deklarationen
   public:		// Benutzer-Deklarationen
      TMenuItem* m_pmiMainFormMenu;
      __fastcall TformASUI(TComponent* Owner, TMenuItem* pmi = NULL);
      virtual __fastcall ~TformASUI();
};
//------------------------------------------------------------------------------
#endif
