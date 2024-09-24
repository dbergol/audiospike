//------------------------------------------------------------------------------
/// \file frmWait.h
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
#ifndef frmWaitH
#define frmWaitH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form to display a wait message
//------------------------------------------------------------------------------
class TformWait : public TForm
{
__published:	// IDE-verwaltete Komponenten
   TLabel *lb;
   void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FormKeyPress(TObject *Sender, System::WideChar &Key);
private:	// Benutzer-Deklarationen
public:		// Benutzer-Deklarationen
   __fastcall TformWait(TComponent* Owner);
   bool m_bCanClose;
   bool m_bCancel;
   void ShowWait(UnicodeString us);
};
//------------------------------------------------------------------------------
extern PACKAGE TformWait *formWait;
//------------------------------------------------------------------------------
#endif
