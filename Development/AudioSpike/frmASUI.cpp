//------------------------------------------------------------------------------
/// \file frmASUI.cpp
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
#include <vcl.h>
#pragma hdrstop

#include "frmASUI.h"
#include "SpikeWareMain.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// contructor implementing basic actions (owner, size, position ...)
//------------------------------------------------------------------------------
__fastcall TformASUI::TformASUI(TComponent* Owner, TMenuItem* pmi)
   : TForm(Owner), m_pmiMainFormMenu(pmi)
{
   formSpikeWare->SetFormParent(this);
   formSpikeWare->RestoreFormPos(this);

   // NOTE Formstyle is managed by TformSpikeWare::SetWindowVisible...

   // if a menu item was passed, then the form's visibility is controlled by
   // main form and/or INI file entry. Then we call RestoreFormVis....
   if (!!m_pmiMainFormMenu)
      {
      m_pmiMainFormMenu->Tag = (int)this;
      formSpikeWare->RestoreFormVis(this);
      }
   // or we set intial state if either is visible or 'magic' (== abused) HelpContext is > 0
   else if (Visible || HelpContext)
      {
      if (formSpikeWare->IsMDI() && !formSpikeWare->IsFreeWindows())
         FormStyle = fsMDIChild;
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor. stores position
//------------------------------------------------------------------------------
__fastcall TformASUI::~TformASUI()
{
   formSpikeWare->StoreFormPos(this);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback. restores position
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformASUI::FormShow(TObject *Sender)
{
   formSpikeWare->RestoreFormPos(this);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClose callback. stores position, sets visibility ...
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformASUI::FormClose(TObject *Sender, TCloseAction &Action)
{
   if (!!m_pmiMainFormMenu)
      m_pmiMainFormMenu->Checked = false;
   formSpikeWare->StoreFormPos(this);
   formSpikeWare->SetWindowVisible(this, false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseActivate callback: calls BringToFront
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformASUI::FormMouseActivate(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y, int HitTest, TMouseActivate &MouseActivate)
{
   BringToFront();
}
//------------------------------------------------------------------------------

