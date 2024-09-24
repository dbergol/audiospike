//------------------------------------------------------------------------------
/// \file frmWait.cpp
///
/// \author Berg
/// \brief Implementation of a form to display a wait message
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

#include "frmWait.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TformWait *formWait;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor - empty
//------------------------------------------------------------------------------
__fastcall TformWait::TformWait(TComponent* Owner)
   : TForm(Owner)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnCloseQuery callback. Allows closing only, if corresponding member is true
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformWait::FormCloseQuery(TObject *Sender, bool &CanClose)
{
   CanClose    = m_bCanClose;
   m_bCancel   = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows wait dialog with passed message
//------------------------------------------------------------------------------
void TformWait::ShowWait(UnicodeString us)
{
   m_bCanClose = false;
   if (us == "")
      us = "Please wait ...";
   lb->Caption = us;
   Show();
   Application->ProcessMessages();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback: sets initial values
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformWait::FormShow(TObject *Sender)
{
   m_bCanClose = false;
   m_bCancel = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnKeyPress dialog set cancel member through ESCAPE
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformWait::FormKeyPress(TObject *Sender, System::WideChar &Key)
{
   if (Key == VK_ESCAPE)
      m_bCancel = true;
}
//------------------------------------------------------------------------------

