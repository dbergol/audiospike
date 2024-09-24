//------------------------------------------------------------------------------
/// \file frmFileSel.h
///
/// \author Berg
/// \brief Implementation of a form to select a filter from a list
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
#ifndef frmFileSelH
#define frmFileSelH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.FileCtrl.hpp>
#include <Vcl.StdCtrls.hpp>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form to select a filter from a list
//------------------------------------------------------------------------------
class TformFileSel : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TFileListBox *flb;
      TButton *btnOk;
      TButton *btnCancel;
      TLabel *lblSelEq;
      void __fastcall flbClick(TObject *Sender);
      void __fastcall flbDblClick(TObject *Sender);
      void __fastcall btnOkClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
   public:		// Benutzer-Deklarationen
      __fastcall TformFileSel(TComponent* Owner);
      double m_dSampleRate;
};
//------------------------------------------------------------------------------
extern PACKAGE TformFileSel *formFileSel;
//------------------------------------------------------------------------------
#endif
