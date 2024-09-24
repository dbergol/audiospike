//------------------------------------------------------------------------------
/// \file frmEpocheWindow.cpp
///
/// \author Berg
/// \brief Implementation of a container form to show multiple TformEpoches forms
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

#include "frmEpocheWindow.h"
#include "frmSelectChannels.h"
#include "frmSpikes.h"
#include "SpikeWareMain.h"
#include "frmStimuli.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor: adjusts form height
//------------------------------------------------------------------------------
__fastcall TformEpocheWindow::TformEpocheWindow(TComponent* Owner, TMenuItem* pmi)
   : TformASUI(Owner, pmi), m_nPlotIndex(0)
{
   m_nHeight = formSpikeWare->m_pIni->ReadInteger("formEpocheWindow", "ChannelHeight", 200);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor. Stores channel height and calls Clear
//------------------------------------------------------------------------------
__fastcall TformEpocheWindow::~TformEpocheWindow()
{
   formSpikeWare->m_pIni->WriteInteger("formEpocheWindow", "ChannelHeight", m_nHeight);
   Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls base class and ensures that top panel stays on top
//------------------------------------------------------------------------------
void __fastcall TformEpocheWindow::FormShow(TObject *Sender)
{
   TformASUI::FormShow(Sender);   
   pnlTop->Top = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// does cleanup of TformEpoches children
//------------------------------------------------------------------------------
void TformEpocheWindow::Clear()
{
   unsigned int n;
   for (n = 0; n < m_vpformEpoches.size(); n++)
      {
      TRYDELETENULL(m_vpformEpoches[n]);
      }
   m_vpformEpoches.clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Creates TformEpoches children or only call their Initialize function if 
/// nChannels < 0
//------------------------------------------------------------------------------
void TformEpocheWindow::Initialize(int nChannels)
{
   
   if (nChannels >= 0)
      {
      Clear();
      int n;
      for (n = 0; n < nChannels; n++)
         {
         m_vpformEpoches.push_back(new TformEpoches(NULL, n));
         m_vpformEpoches.back()->Initialize();
         m_vpformEpoches.back()->Height = m_nHeight;
         if (n == 0)
            m_vpformEpoches.back()->tbtnEvalActive->Down = true;
         m_vpformEpoches.back()->Top = pnlTop->Height + n*m_nHeight + 10;

         m_vpformEpoches.back()->Show();
         }
      }
   else
      {
      unsigned int n;
      for (n = 0; n < m_vpformEpoches.size(); n++)
         m_vpformEpoches[n]->Initialize();
      }
   pnlTop->Top = 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots epoche by calling Plot of all children and clears passed data
//------------------------------------------------------------------------------
void TformEpocheWindow::Plot(TSWEpoche *pswe)
{
   unsigned int n;
   for (n = 0; n < m_vpformEpoches.size(); n++)
     m_vpformEpoches[n]->Plot(pswe);
   if (pswe)
      pswe->ClearData();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots search data by calling SetData of children
//------------------------------------------------------------------------------
void TformEpocheWindow::PlotSearchData(vvd &rvvdData)
{
   unsigned int n;
   // push data
   for (n = 0; n < m_vpformEpoches.size(); n++)
     m_vpformEpoches[n]->SetData(rvvdData);
   // then call plotting asynchroneously
   Timer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Updates "listen"-button status of all children
//------------------------------------------------------------------------------
void TformEpocheWindow::UpdateListenButtons(TformEpoches* pfrm)
{
   unsigned int n;
   for (n = 0; n < m_vpformEpoches.size(); n++)
      {
      if (m_vpformEpoches[n] != pfrm)
         m_vpformEpoches[n]->tbnListen->Down = false;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates threshold in all children
//------------------------------------------------------------------------------
void TformEpocheWindow::UpdateThreshold(unsigned int nChannel, double dThreshold)
{
   if (nChannel < m_vpformEpoches.size())
      m_vpformEpoches[nChannel]->UpdateThreshold(dThreshold);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates evaluation status of all children
//------------------------------------------------------------------------------
void TformEpocheWindow::SetEvalWindow(TformEpoches* pfrm)
{
      unsigned int n;

      for (n = 0; n < m_vpformEpoches.size(); n++)
         {
         if (m_vpformEpoches[n]==pfrm)
            {
            m_nPlotIndex = n;
            m_vpformEpoches[n]->tbtnEvalActive->Down = true;
            }
         else
            m_vpformEpoches[n]->tbtnEvalActive->Down = false;
         m_vpformEpoches[n]->chrt->Invalidate();
         }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates chart axis of all children corresponding to passed form
//------------------------------------------------------------------------------
void TformEpocheWindow::SetAllAxis(TformEpoches* pfrm, bool bLeft, bool bForce)
{
   if (!cbAllAxis->Checked && !bForce)
      return;
   unsigned int n;
   for (n = 0; n < m_vpformEpoches.size(); n++)
      {
      if (m_vpformEpoches[n] == pfrm)
         continue;
      if (!bLeft)
         {
         m_vpformEpoches[n]->chrt->TopAxis->SetMinMax(pfrm->chrt->TopAxis->Minimum, pfrm->chrt->TopAxis->Maximum);
         m_vpformEpoches[n]->chrt->BottomAxis->SetMinMax(pfrm->chrt->BottomAxis->Minimum, pfrm->chrt->BottomAxis->Maximum);
         }
      else
         {
         m_vpformEpoches[n]->chrt->LeftAxis->SetMinMax(pfrm->chrt->LeftAxis->Minimum, pfrm->chrt->LeftAxis->Maximum);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnChannels: shows a from for selecting channels to display
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::btnChannelsClick(TObject *Sender)
{
   //
   TformSelectChannels* pfrm = new TformSelectChannels(NULL);
   try
      {
      unsigned int n;
      for (n = 0; n < m_vpformEpoches.size(); n++)
         {
         pfrm->clb->Items->Add("Channel " + IntToStr((int)n+1));
         pfrm->clb->Checked[(int)n] = m_vpformEpoches[n]->Visible;
         }

      if (pfrm->ShowModal() == mrOk)
         {
         for (n = 0; n < m_vpformEpoches.size(); n++)
            m_vpformEpoches[n]->Align = alNone;
         for (n = 0; n < m_vpformEpoches.size(); n++)
            {
            m_vpformEpoches[n]->Top = (int)m_vpformEpoches[n]->Tag*m_vpformEpoches[n]->Height +1;
            m_vpformEpoches[n]->Visible = pfrm->clb->Checked[(int)n];
            }
         for (n = 0; n < m_vpformEpoches.size(); n++)
            m_vpformEpoches[n]->Align = alTop;
         }
      }
   __finally
      {
      TRYDELETENULL(pfrm);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for zoom buttons: adjusts height of children
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::tbtnZoomClick(TObject *Sender)
{
   if (!m_vpformEpoches.size())
      return;
   unsigned int n;
   if (Sender == tbtnZoomOut)
      {
      m_nHeight = m_nHeight *10 / 11;
      if (m_nHeight < 100)
         m_nHeight = 100;
      }
   else
      m_nHeight = m_nHeight*11/10;
   for (n = 0; n < m_vpformEpoches.size(); n++)
      m_vpformEpoches[n]->Height = m_nHeight;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseWheel callback: scrolls children
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled)
{
   if (WheelDelta > 0)
      scb->VertScrollBar->Position -= scb->VertScrollBar->Increment;
   else
      scb->VertScrollBar->Position += scb->VertScrollBar->Increment;
   Handled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Timer function calling PlotData of children
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::TimerTimer(TObject *Sender)
{
   static bool bPlotting = false;
   Timer->Enabled = false;
   if (bPlotting)
      return;
   bPlotting = true;
   try
      {
      unsigned int n;
      for (n = 0; n < m_vpformEpoches.size(); n++)
        m_vpformEpoches[n]->PlotData();
      }
   __finally
      {
      bPlotting = false;
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clipping timer callback: updates clip indicators on children
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::ClipTimerTimer(TObject *Sender)
{
   unsigned int n;
   bool b;
   for (n = 0; n < m_vpformEpoches.size(); n++)
      {
      b = false;
      if (n < m_vbClip.size())
         b = m_vbClip[n];
      m_vpformEpoches[n]->shClip->Brush->Color = b ? clRed : clLime;
      }

   ClipTimer->Enabled = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resets clipping indicators on children
//------------------------------------------------------------------------------
void TformEpocheWindow::ResetClipping()
{
   unsigned int n;
   for (n = 0; n < m_vpformEpoches.size(); n++)
      m_vpformEpoches[n]->shClip->Brush->Color = clLime;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// External "ShowClipping" funtion: copies clipping data and enables clipping 
/// timer
//------------------------------------------------------------------------------
void TformEpocheWindow::ShowClipping(std::vector<bool > &rvb)
{
   if (ClipTimer->Enabled)
      return;

   m_vbClip = rvb;
   ClipTimer->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Enables/disables controls for scrolling through epoches
//------------------------------------------------------------------------------
void TformEpocheWindow::EnableEpocheScrolling(bool bEnable)
{
   tbEpoches->Enabled         = bEnable;
   sbtnScrollLeft->Enabled    = tbEpoches->Enabled && (tbEpoches->Position > 0);
   sbtnScrollRight->Enabled   = tbEpoches->Enabled && (tbEpoches->Position < tbEpoches->Max);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback of epoche trackbar for scrolling through single epoches
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::tbEpochesChange(TObject *Sender)
{
   // store, that trackbar was changed (with mouse) in tag of sbtnScrollLeft: 
   // change is applied in TimerTimer
   sbtnScrollLeft->Tag = 1;
   sbtnScrollLeft->Enabled    = tbEpoches->Enabled && (tbEpoches->Position > 0);
   sbtnScrollRight->Enabled   = tbEpoches->Enabled && (tbEpoches->Position < tbEpoches->Max);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnEnter callback of tbEpoches: adjusts it's maximum
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::tbEpochesEnter(TObject *Sender)
{
   tbEpoches->Max = (int)formSpikeWare->m_sweEpoches.Count() - 1;   
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for sbtnScrollLeft and sbtnScrollRight: increment/decrement
/// position of epoche trackbar
//------------------------------------------------------------------------------
void __fastcall TformEpocheWindow::sbtnScrollClick(TObject *Sender)
{
   if (Sender == sbtnScrollLeft)
      {
      if (tbEpoches->Position >= 0)
         tbEpoches->Position = tbEpoches->Position - 1;
      sbtnScrollLeft->Enabled    = tbEpoches->Enabled && (tbEpoches->Position > 0);
      }
   else
      {
      if (tbEpoches->Position < tbEpoches->Max)
         tbEpoches->Position = tbEpoches->Position + 1;
      sbtnScrollRight->Enabled   = tbEpoches->Enabled && (tbEpoches->Position < tbEpoches->Max);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Timer for applying change og tbEpcoches (is only done when mouse is released
/// again)
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformEpocheWindow::EpocheScrollTimerTimer(TObject *Sender)
{
   if (!tbEpoches->Enabled)
      return;
                     
   if (sbtnScrollLeft->Tag && GetAsyncKeyState(VK_LBUTTON) == 0)
      {


      if (!formSpikeWare->FormsCreated() || !formSpikeWare->m_sweEpoches.Count())
         return;

      EnableEpocheScrolling(false);
      sbtnScrollLeft->Tag = 0;
      tbEpoches->Max = (int)formSpikeWare->m_sweEpoches.Count() - 1;

      TSWEpoche* pswe = formSpikeWare->m_sweEpoches.Get(tbEpoches->Position);
      if (pswe)
         {
         formSpikeWare->PlotEpoches(pswe);
         formSpikeWare->m_pformStimuli->SelectItem((int)pswe->m_nStimIndex);
         Caption = "Epoches - Index " + IntToStr(tbEpoches->Position + 1);

         if (formSpikeWare->m_pformSpikes->cbPlotEpocheSpikesOnly->Checked)
            formSpikeWare->m_pformSpikes->Plot((unsigned int)formSpikeWare->m_pformSpikes->Tag);
         }
      EnableEpocheScrolling(true);
      tbEpoches->SetFocus();
      }
      
   
}
//------------------------------------------------------------------------------



