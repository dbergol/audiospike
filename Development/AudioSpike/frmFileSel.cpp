//------------------------------------------------------------------------------
/// \file frmFileSel.cpp
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
#include <vcl.h>
#pragma hdrstop

#include "frmFileSel.h"
#include "SpikeWareMain.h"
#include "SWStim.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TformFileSel *formFileSel;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor - empty
//------------------------------------------------------------------------------
#pragma argsused
__fastcall TformFileSel::TformFileSel(TComponent* Owner)
   : TForm(Owner)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for list: enables/disables OK button
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFileSel::flbClick(TObject *Sender)
{
   btnOk->Enabled = flb->ItemIndex != -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnDblClick callback for list: calls BtnOkClick, if any filter is selected
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFileSel::flbDblClick(TObject *Sender)
{
   if (flb->ItemIndex != -1)
      btnOkClick(NULL);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// checks selected filter and checks if samplerate is OK
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformFileSel::btnOkClick(TObject *Sender)
{
   if (flb->ItemIndex == -1)
      return;
   unsigned int nChannels, nSamples;
   double dSampleRate;
   UnicodeString us;

   TSWStimuli::AudioFileProperties(flb->FileName, nChannels, nSamples, dSampleRate);
   if (nChannels != 1)
      {
      us = "Equalisation '" + flb->FileName + "' invalid: must have one audio channel";
      MessageBoxW(Handle, us.w_str(), L"Error", MB_ICONERROR);
      return;
      }
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wfloat-equal"
   if (dSampleRate != m_dSampleRate)
      {
      us = "Equalisation '" + flb->FileName + "' has a different samplerate than device (device: "
         + DoubleToStr(m_dSampleRate) + ", equalisation: "
         + DoubleToStr(dSampleRate) + "). With current samplerate it will not be usable. Do you want to select it anyway?";

      if (ID_YES != MessageBoxW(Handle, us.w_str(), L"Error", MB_ICONWARNING | MB_YESNO))
         return;
      }
   #pragma clang diagnostic pop
   ModalResult = mrOk;
}
//------------------------------------------------------------------------------

