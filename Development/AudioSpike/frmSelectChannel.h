//------------------------------------------------------------------------------
/// \file frmSelectChannel.h
///
/// \author Berg
/// \brief Implementation of a form for selecting one channel/item from
/// a listbox
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
#ifndef frmSelectChannelH
#define frmSelectChannelH
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <System.Classes.hpp>
#include <Vcl.CheckLst.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form for selecting one channel/item from
//------------------------------------------------------------------------------
class TformSelectChannel : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TListBox *lb;
      void __fastcall lbClick(TObject *Sender);
   private:	// Benutzer-Deklarationen
   public:		// Benutzer-Deklarationen
      __fastcall TformSelectChannel(TComponent* Owner);
};
//------------------------------------------------------------------------------
#endif
