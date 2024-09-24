//------------------------------------------------------------------------------
/// \file formVisual.cpp
/// \author Berg
/// \brief Implementation of class TAboutBox
///
/// Project SoundMexPro
/// Module  SoundDllPro.dll
///
/// ****************************************************************************
/// Copyright 2023 Daniel Berg, Oldenburg, Germany
/// ****************************************************************************
///
/// This file is part of SoundMexPro.
///
///    SoundMexPro is free software: you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation, either version 3 of the License, or
///    (at your option) any later version.
///
///    SoundMexPro is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License
///    along with SoundMexPro.  If not, see <http:///www.gnu.org/licenses/>.
///
//------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "formAbout.h"
#include "SpikeWareMain.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#pragma warn -use

TAboutBox *AboutBox;


//------------------------------------------------------------------------------
/// constructor
//------------------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent* Owner)
   : TForm(Owner)
{
   //Panel1->Font->Color = (TColor)RGB(33,58,143);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow sets version caption
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TAboutBox::FormShow(TObject *Sender)
{
    #ifndef _WIN64
   lbVersionValue->Caption =  GetFileVersion() + " (32bit)";
   #else
   lbVersionValue->Caption =  GetFileVersion() + " (64bit)";
   #endif
   
}
//------------------------------------------------------------------------------



