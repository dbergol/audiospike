//------------------------------------------------------------------------------
/// \file frmSelect.cpp
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
#include <vcl.h>
#pragma hdrstop

#include "frmSelect.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor - empty
//------------------------------------------------------------------------------
__fastcall TformSelect::TformSelect(TComponent* Owner)
   : TForm(Owner)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// fills listboxes with passed items and shows form modal 
//------------------------------------------------------------------------------
TModalResult TformSelect::ShowSelection(std::vector<UnicodeString > &rvus, bool bYResponse)
{
   lbX->Clear();
   lbY->Clear();
   unsigned int n;

   for (n = 0; n < rvus.size(); n++)
      {
      lbX->Items->Add(rvus[n]);
      lbY->Items->Add(rvus[n]);
      }

   if (bYResponse)
      lbY->Items->Add("Response");

   return ShowModal();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of listboxes: enables/disables OK button
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSelect::lbClick(TObject *Sender)
{
   btnOk->Enabled =  lbX->ItemIndex > -1 && lbY->ItemIndex > -1
                  && lbX->ItemIndex != lbY->ItemIndex;
}
//------------------------------------------------------------------------------

