//------------------------------------------------------------------------------
/// \file frmSelectChannels.h
///
/// \author Berg
/// \brief Implementation of a form for selecting multiple channels/items from
/// a list with checkboxes
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
#ifndef frmSelectChannelsH
#define frmSelectChannelsH
#include <System.Classes.hpp>
#include <Vcl.CheckLst.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// form for selecting multiple channels/items from a list with checkboxes
//------------------------------------------------------------------------------
class TformSelectChannels : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TButton *btnOk;
      TButton *btnCancel;
      TCheckListBox *clb;
   private:	// Benutzer-Deklarationen
   public:		// Benutzer-Deklarationen
      __fastcall TformSelectChannels(TComponent* Owner);
};
//------------------------------------------------------------------------------
#endif
