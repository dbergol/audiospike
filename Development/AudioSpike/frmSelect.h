//------------------------------------------------------------------------------
/// \file frmSelect.h
///
/// \author Berg
/// \brief Implementation of a form to select two parameters to be used for
/// x-axis or y-axis of a 2d-plot respectively
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
#ifndef frmSelectH
#define frmSelectH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
//------------------------------------------------------------------------------
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form to select two parameters to be used for x-axis or y-axis of 
/// a 2d-plot respectively
//------------------------------------------------------------------------------
class TformSelect : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TListBox *lbX;
      TListBox *lbY;
      TButton *btnOk;
      TButton *btnCancel;
      TLabel *lblX;
      TLabel *lblY;
      void __fastcall lbClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
   public:		// Benutzer-Deklarationen
      __fastcall TformSelect(TComponent* Owner);
      TModalResult ShowSelection(std::vector<UnicodeString > &rvus, bool bYResponse = false);
};
//------------------------------------------------------------------------------
#endif
