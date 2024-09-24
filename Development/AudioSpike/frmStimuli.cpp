//------------------------------------------------------------------------------
/// \file frmStimuli.cpp
///
/// \author Berg
/// \brief Implementation of a form to display a list of used Stimuli
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

#include "frmStimuli.h"
#include "SpikeWareMain.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor - empty
//------------------------------------------------------------------------------
__fastcall TformStimuli::TformStimuli(TComponent* Owner, TMenuItem* pmi)
   : TformASUI(Owner, pmi)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor: saves column widths
//------------------------------------------------------------------------------
__fastcall TformStimuli::~TformStimuli()
{
   WriteColWidths();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback. Calls base class and reads column widths
//------------------------------------------------------------------------------
void __fastcall TformStimuli::FormShow(TObject *Sender)
{
   TformASUI::FormShow(Sender);
   ReadColWidths();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears all listvew items except fix ones
//------------------------------------------------------------------------------
void TformStimuli::Clear()
{
   lv->Items->Clear();
   while (lv->Columns->Count > 2)
      lv->Columns->Delete(lv->Columns->Count-1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates list items for all stimuli and parameters
//------------------------------------------------------------------------------
void TformStimuli::Load()
{
   Clear();
   CreateCols();

   TSWStimuli &rswStimuli = formSpikeWare->m_swsStimuli;

   unsigned int nStim, nPar;
   for (nStim = 0; nStim < rswStimuli.m_swstStimuli.size(); nStim++)
      {
      TListItem* pli = lv->Items->Add();
      pli->Caption = IntToStr(lv->Items->Count);
      pli->SubItems->Add(rswStimuli.m_swstStimuli[nStim].m_usName);

      for (nPar = 0; nPar < rswStimuli.m_swspStimPars.m_vusNames.size(); nPar++)
         {
         // write param as double ...
         if (!rswStimuli.m_swspStimPars.m_vbString[nPar])
            pli->SubItems->Add(DoubleToStr(rswStimuli.m_swstStimuli[nStim].m_vdParams[nPar]));
         // or retrieve string: the double value is the index within m_swspStimPars.m_vvusValues!
         else
            pli->SubItems->Add(rswStimuli.m_swstStimuli[nStim].m_vusParams[nPar]);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// creates listview columns
//------------------------------------------------------------------------------
void TformStimuli::CreateCols()
{
   WriteColWidths();
   unsigned int n;
   while (lv->Columns->Count > 2)
      lv->Columns->Delete(lv->Columns->Count-1);

   for (n = 0; n < formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusNames.size(); n++)
      {
      TListColumn *plc = lv->Columns->Add();
      plc->Caption = formSpikeWare->m_swsStimuli.m_swspStimPars.m_vusNames[n];
      }
   ReadColWidths();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads column widths from INI
//------------------------------------------------------------------------------
void TformStimuli::ReadColWidths()
{
   // restore column widths
   int n;
   for (n = 0; n < lv->Columns->Count; n++)
      lv->Columns->Items[n]->Width = formSpikeWare->m_pIni->ReadInteger(Name, "Col_" + lv->Columns->Items[n]->Caption, 50);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// writes column widths to INI
//------------------------------------------------------------------------------
void TformStimuli::WriteColWidths()
{
   // store column widths
   int n;
   for (n = 0; n < lv->Columns->Count; n++)
      formSpikeWare->m_pIni->WriteInteger(Name, "Col_" + lv->Columns->Items[n]->Caption, lv->Columns->Items[n]->Width);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnSelcectItem callback of listview: calls formSpikeWare->PlaySearchStimulus
/// (only if search mode is running at all)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformStimuli::lvSelectItem(TObject *Sender, TListItem *Item, bool Selected)
{
   if (formSpikeWare->m_gs != SWGS_SEARCH)
      return;
   if (lv->Selected)
      formSpikeWare->PlaySearchStimulus(lv->Selected->Index, true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Selects an item and scrolls it into view
//------------------------------------------------------------------------------
void TformStimuli::SelectItem(int nIndex)
{
   if (lv->ItemIndex == nIndex || nIndex < 0 || nIndex >= lv->Items->Count)
      return;
   lv->ItemIndex = nIndex;
   lv->Selected->MakeVisible(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnAdvancedDrawtem callback of listview: shows current selection if any style
/// is active other than "WIndows"
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformStimuli::lvAdvancedCustomDrawItem(TCustomListView *Sender, TListItem *Item,
          TCustomDrawState State, TCustomDrawStage Stage, bool &DefaultDraw)
{
   if (TStyleManager::ActiveStyle->Name == "Windows")
      return;

   if (Item->Selected)
      {
      lv->Canvas->Font->Color = clHighlightText;
      lv->Canvas->Brush->Color =clHighlight;
      }

}
//------------------------------------------------------------------------------

