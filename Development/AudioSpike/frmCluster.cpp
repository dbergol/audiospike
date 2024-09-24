//------------------------------------------------------------------------------
/// \file frmCluster.cpp
///
/// \author Berg
/// \brief Implementation of a form to show a cluster plot
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
#include <math.h>
#pragma hdrstop

#include "frmCluster.h"
#include "SpikeWareMain.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor, calls Clear to initialize members
//------------------------------------------------------------------------------
TSWClusterSelection::TSWClusterSelection()
{
   Clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resets all members
//------------------------------------------------------------------------------
void TSWClusterSelection::Clear()
{
   dX0      = 0.0;
   dX1      = 0.0;
   dY0      = 0.0;
   dY1      = 0.0;
   bActive  = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// constructor. Initializes members and creates ChartShapes for cluster selections
//------------------------------------------------------------------------------
__fastcall TformCluster::TformCluster(TComponent* Owner, TSpikeParam spX, TSpikeParam spY)
   : TformASUI(Owner), m_spX(spX), m_spY(spY), m_nPlotCounter(0)
{
   Name        = "Cluster_"
               + formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusIDs[m_spX] + "_"
               + formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusIDs[m_spY];

   // create the selection series
   TChartShape* pcs;
   unsigned int n;
   for (n = 0; n < formSpikeWare->m_vclSpikeColors.size(); n++)
      {
      pcs = new TChartShape(this);
      pcs->Selected->Hover->Visible = false;
      pcs->ParentChart = chrt;
      pcs->Assign(csSelection);
      pcs->SeriesColor = formSpikeWare->m_vclSpikeColors[n];
      pcs->Active = false;
      pcs->OnClick = csSelectionClick;
      pcs->Tag = (NativeInt)n;
      m_vpcs.push_back(pcs);
      }


   // call RestoreFormPos again due to 'Name' that corresponds to section name!
   formSpikeWare->RestoreFormPos(this);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor stores X/Y properties for this plot in ini
//------------------------------------------------------------------------------
__fastcall TformCluster::~TformCluster()
{
   formSpikeWare->m_pIni->WriteString(Name, "X", formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusIDs[m_spX]);
   formSpikeWare->m_pIni->WriteString(Name, "Y", formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusIDs[m_spY]);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClose callback. resets spike groups and removes clusetr window from main 
//------------------------------------------------------------------------------
void __fastcall TformCluster::FormClose(TObject *Sender, TCloseAction &Action)
{
   TformASUI::FormClose(Sender, Action);
   ResetActiveSpikeGroups();
   formSpikeWare->RemoveClusterWindow(this);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes plot/chart and creates TSWClusterSelection instances for each
/// corresponding TChartShape for each channel
//------------------------------------------------------------------------------
void TformCluster::Initialize(unsigned int nChannels)
{
   m_bSelectAllOnNoSelection = formSpikeWare->m_pIni->ReadBool("Settings", "SelectAllOnNoSelection", false);

   csData->Clear();
   csData->Pointer->Size = formSpikeWare->m_pIni->ReadInteger("Settings", "ClusterPointSize", 2);

   chrt->LeftAxis->Title->Caption   = formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusNames[m_spY]
                                      + " [" + formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusUnits[m_spY] + "]";
   chrt->LeftAxis->SetMinMax(formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vdMin[m_spY], formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vdMax[m_spY]);


   chrt->BottomAxis->Title->Caption = formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusNames[m_spX]
                                      + " [" + formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusUnits[m_spX] + "]";
   chrt->BottomAxis->SetMinMax(formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vdMin[m_spX], formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vdMax[m_spX]);

   chrt->RightAxis->Title->Caption = formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusIDs[m_spY];
   chrt->TopAxis->Title->Caption = formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vusIDs[m_spX];

   m_vbSelActive.clear();
   m_vvSWSelections.clear();
   m_vvSWSelections.resize(nChannels);
   unsigned int n, m;
   for (n = 0; n < nChannels; n++)
      {
      m_vbSelActive.push_back(formSpikeWare->m_vpformCluster[0] == this);
      for (m = 0; m < m_vpcs.size(); m++)
         {
         m_vvSWSelections[n].push_back(TSWClusterSelection());
         }
      }

   ResetSelection(true);

   if (formSpikeWare->m_vpformCluster[0] == this)
      EnableSelection(true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resets all selections (ChartShapes and TSWClusterSelections)
//------------------------------------------------------------------------------
void TformCluster::ResetSelection(bool bInit)
{
   EnterCriticalSection(&formSpikeWare->m_cs);
   try
      {
      unsigned int n;
      for (n = 0; n < m_vpcs.size(); n++)
         {
         m_vpcs[n]->X0 = 0.0;
         m_vpcs[n]->X1 = 0.0;
         m_vpcs[n]->Y0 = 0.0;
         m_vpcs[n]->Y1 = 0.0;
         m_vpcs[n]->Active = false;
         if (Tag < (int)m_vvSWSelections.size())
            m_vvSWSelections[(unsigned int)Tag][n].Clear();
         }
      if (!bInit)
         formSpikeWare->m_swsSpikes.SpikeGroupReset((unsigned int)Tag);
      SetColors();
      }
   __finally
      {
      LeaveCriticalSection(&formSpikeWare->m_cs);
      }
   formSpikeWare->SetMeasurementChanged();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resets currently active spike groups
//------------------------------------------------------------------------------
void TformCluster::ResetActiveSpikeGroups()
{
   EnterCriticalSection(&formSpikeWare->m_cs);
   try
      {
      unsigned int n;
      for (n = 0; n < m_vvSWSelections.size(); n++)
         {
         if (m_vbSelActive[n])
            formSpikeWare->m_swsSpikes.SpikeGroupReset(n);
         }
      formSpikeWare->UpdateClusterColors();
      }
   __finally
      {
      LeaveCriticalSection(&formSpikeWare->m_cs);
      }
   formSpikeWare->SetMeasurementChanged();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// copies all selections to correponding SelectionSeries
//------------------------------------------------------------------------------
void TformCluster::SelectionsToSelectionSeries()
{
   unsigned int n;

   for (n = 0; n < m_vpcs.size(); n++)
      SelectionToSelectionSeries(n);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// copies on selection to correponding SelectionSeries
//------------------------------------------------------------------------------
void TformCluster::SelectionToSelectionSeries(unsigned int nSelectionIndex)
{
   m_vpcs[nSelectionIndex]->X0     = m_vvSWSelections[(unsigned int)Tag][nSelectionIndex].dX0;
   m_vpcs[nSelectionIndex]->X1     = m_vvSWSelections[(unsigned int)Tag][nSelectionIndex].dX1;
   m_vpcs[nSelectionIndex]->Y0     = m_vvSWSelections[(unsigned int)Tag][nSelectionIndex].dY0;
   m_vpcs[nSelectionIndex]->Y1     = m_vvSWSelections[(unsigned int)Tag][nSelectionIndex].dY1;
   m_vpcs[nSelectionIndex]->Active = m_vvSWSelections[(unsigned int)Tag][nSelectionIndex].bActive && tbtnSelActive->Down;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// plots spike data in cluster plot
//------------------------------------------------------------------------------
void TformCluster::Plot(unsigned int nChannelIndex)
{
   if (m_nPlotCounter || !formSpikeWare->m_bPlotAllowed)
      return;
   try
      {
      m_nPlotCounter++;
      if ((int)nChannelIndex != Tag)
         {
         Tag = (NativeInt)nChannelIndex;
         SelectionsToSelectionSeries();
         EnableSelection(m_vbSelActive[(unsigned int)Tag]);
         formSpikeWare->ScaleClusterPlot(this);
         }

      Caption = "Clusterplot - Channel " + IntToStr(Tag+1);

      unsigned int nNum = formSpikeWare->m_swsSpikes.GetNumSpikes((unsigned int)Tag);
      if (!nNum)
         {
         csData->Clear();
         return;
         }

      csData->BeginUpdate();

      csData->Clear();
      // NOTE: it is MUCH faster to 'pre-allocate' memory with FillSampleValues
      // and afterwards set values, than calling AddXY in a loop (example found in
      // the web -  and tested with success!!)!!
      csData->FillSampleValues((int)nNum);
      unsigned int n;
      for (n = 0; n < nNum; n++)
         {
         csData->XValues->Value[(int)n]  = formSpikeWare->m_swsSpikes.GetSpikeParam((unsigned int)Tag, n, m_spX);
         csData->YValues->Value[(int)n]  = formSpikeWare->m_swsSpikes.GetSpikeParam((unsigned int)Tag, n, m_spY);
         csData->ValueColor[(int)n]      = formSpikeWare->SpikeGroupToColor(formSpikeWare->m_swsSpikes.GetSpikeGroup((unsigned int)Tag, n));
         }


      csData->EndUpdate();

      // update colors
      if (tbtnSelActive->Down)
         EnableSelection(true);
      }
   __finally
      {
      m_nPlotCounter--;
      }

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Updates colors of single points
//------------------------------------------------------------------------------
void TformCluster::UpdateColors()
{
   if (!formSpikeWare->m_bPlotAllowed)
      return;

   try
      {
      m_nPlotCounter++;
      unsigned int nNumSpikes = formSpikeWare->m_swsSpikes.GetNumSpikes((unsigned int)Tag);
      unsigned int nNum = nNumSpikes;
      if ((unsigned int)csData->YValues->Count < nNum)
         nNum = (unsigned int)csData->YValues->Count;
      int nSelected = 0;
      int nGroup;
      unsigned int n;
      for (n = 0; n < nNum; n++)
         {
         nGroup = formSpikeWare->m_swsSpikes.GetSpikeGroup((unsigned int)Tag, n);
         if (nGroup >= 0)
            nSelected++;
         csData->ValueColor[(int)n] = formSpikeWare->SpikeGroupToColor(nGroup);
         }

      csData->Repaint();

      if (tbtnSelActive->Down)
         {
         UnicodeString us;
         us.printf(L"Clusterplot - Channel %d (%d/%lu)", Tag+1, nSelected, nNumSpikes);
         Caption = us;
         }

      }
   __finally
      {
      m_nPlotCounter--;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets colors of single points depending on selection
//------------------------------------------------------------------------------
void TformCluster::SetColors()
{
   if (!formSpikeWare->m_bPlotAllowed)
      return;
   try
      {
      m_nPlotCounter++;
      unsigned int nSpike, n;
      bool bAnyActiveSel = true;


      if (m_bSelectAllOnNoSelection)
         {
         bAnyActiveSel = false;
         for (n = 0; n < m_vpcs.size(); n++)
            {
            if (m_vpcs[n]->Active)
               {
               bAnyActiveSel = true;
               break;
               }
            }
         }

      unsigned int nNum = formSpikeWare->m_swsSpikes.GetNumSpikes((unsigned int)Tag);
      if ((unsigned int)csData->YValues->Count < nNum)
         nNum = (unsigned int)csData->YValues->Count;
      int nGroup;
      double dX, dY, dXMin, dXMax, dYMin, dYMax;
      for (nSpike = 0; nSpike < nNum; nSpike++)
         {
         nGroup =  bAnyActiveSel ? -1 : 0;
         dX = csData->XValues->Value[(int)nSpike];
         dY = csData->YValues->Value[(int)nSpike];
         for (n = 0; n < m_vpcs.size(); n++)
            {
            if (!m_vpcs[n]->Active)
               continue;
            dXMin = m_vpcs[n]->X0 < m_vpcs[n]->X1 ? m_vpcs[n]->X0 : m_vpcs[n]->X1;
            dXMax = m_vpcs[n]->X0 < m_vpcs[n]->X1 ? m_vpcs[n]->X1 : m_vpcs[n]->X0;
            dYMin = m_vpcs[n]->Y0 < m_vpcs[n]->Y1 ? m_vpcs[n]->Y0 : m_vpcs[n]->Y1;
            dYMax = m_vpcs[n]->Y0 < m_vpcs[n]->Y1 ? m_vpcs[n]->Y1 : m_vpcs[n]->Y0;
            if (dX >= dXMin && dX <= dXMax && dY >= dYMin && dY <= dYMax)
               {
               nGroup = (int)n;
               break;
               }
            }
         formSpikeWare->m_swsSpikes.SetSpikeGroup((unsigned int)Tag, nSpike, nGroup);
         }
      }
   __finally
      {
      m_nPlotCounter--;
      }
   formSpikeWare->UpdateClusterColors();
   formSpikeWare->SetMeasurementChanged();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// clears data and resets selctions
//------------------------------------------------------------------------------
void TformCluster::Clear()
{
   csData->Clear();
   ResetSelection();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseDown callback for chart: sets "starting" point for dragging up a selection
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCluster::chrtMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
   unsigned int nTag = (unsigned int)Tag;
   if (  !tbtnSelActive->Down || Button != mbLeft
      || !m_vpcs.size() || !PtInRect(chrt->ChartRect, TPoint(X,Y))
      )
      return;

   // find an unused selection series
   bool bFound = false;
   int n;
   int nNum =  (int)m_vvSWSelections[nTag].size()-1;
   for (n = nNum; n >= 0; n--)
      {
      if (!m_vvSWSelections[nTag][(unsigned int)n].bActive)
         {
         bFound = true;
         m_nCurrentSelectionIndex = n;
         break;
         }
      }

   if (!bFound)
      return;

   double dX, dY;
   m_vpcs[0]->GetCursorValues(dX, dY);
   m_vvSWSelections[nTag][(unsigned int)m_nCurrentSelectionIndex].dX0 = dX;
   m_vvSWSelections[nTag][(unsigned int)m_nCurrentSelectionIndex].dX1 = dX;
   m_vvSWSelections[nTag][(unsigned int)m_nCurrentSelectionIndex].dY0 = dY;
   m_vvSWSelections[nTag][(unsigned int)m_nCurrentSelectionIndex].dY1 = dY;
   m_vvSWSelections[nTag][(unsigned int)m_nCurrentSelectionIndex].bActive  = true;
   SelectionToSelectionSeries((unsigned int)m_nCurrentSelectionIndex);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseMove callback for chart: sets "end" point for dragging up a selection
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCluster::chrtMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
   if (  !tbtnSelActive->Down || !Shift.Contains(ssLeft)
      || m_nCurrentSelectionIndex < 0  || !PtInRect(chrt->ChartRect, TPoint(X,Y))
      )
      return;

   TChartShape* pcs = m_vpcs[(unsigned int)m_nCurrentSelectionIndex];

   double dX, dY;
   pcs->GetCursorValues(dX, dY);
   pcs->X1 = dX;
   pcs->Y1 = dY;
   m_vvSWSelections[(unsigned int)Tag][(unsigned int)m_nCurrentSelectionIndex].dX1 = dX;
   m_vvSWSelections[(unsigned int)Tag][(unsigned int)m_nCurrentSelectionIndex].dY1 = dY;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnMouseUp callback for chart: ends dragging up a selection
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCluster::chrtMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
   if (!tbtnSelActive->Down || m_nCurrentSelectionIndex == -1)
      return;

   TChartShape* pcs = m_vpcs[(unsigned int)m_nCurrentSelectionIndex];

   // don't use a selection that is too small
   if (  fabs(pcs->X0-pcs->X1) < (chrt->BottomAxis->Maximum - chrt->BottomAxis->Minimum) / 50.0
      || fabs(pcs->Y0-pcs->Y1) < (chrt->LeftAxis->Maximum - chrt->LeftAxis->Minimum) / 50.0
      )
      {
      pcs->Active = false;
      m_vvSWSelections[(unsigned int)Tag][(unsigned int)m_nCurrentSelectionIndex].bActive = false;
      }
   else
      SetColors();

   m_nCurrentSelectionIndex = -1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for tbtnSelActive: toggles SelectionActive status
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCluster::tbtnSelActiveClick(TObject *Sender)
{
   if (!tbtnSelActive->Down)
      tbtnSelActive->Down = true;
   else
      formSpikeWare->SetSelectionWindow(this);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enables/disables selections
//------------------------------------------------------------------------------
void TformCluster::EnableSelection(bool bEnable)
{
   tbtnSelActive->Down  = bEnable;
   m_vbSelActive[(unsigned int)Tag]   = bEnable;
   if (bEnable)
      {
      SelectionsToSelectionSeries();
      SetColors();
      }
   else
      {
      unsigned int n;
      for (n = 0; n < m_vpcs.size(); n++)
         m_vpcs[n]->Active = false;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for tbtnSelClear: resets selections
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCluster::tbtnSelClearClick(TObject *Sender)
{
   ResetSelection();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback for selection ChartShapes: if right mouse button click, the
/// corresponding selection is deactivated
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCluster::csSelectionClick(TChartSeries *Sender, int ValueIndex,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if (Button == mbRight)
      {
      Sender->Active = false;
      m_vvSWSelections[(unsigned int)Tag][(unsigned int)Sender->Tag].bActive = false;
      Application->ProcessMessages();
      SetColors();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// axis onclick callback: shows dialog for adjusting axis min/max
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformCluster::chrtClickAxis(TCustomChart *Sender, TChartAxis *Axis,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   int n = Axis == chrt->BottomAxis ? m_spX : m_spY;
   formSpikeWare->m_pformSetParameters->SetAxisMinMax(Axis,
                                          formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vdMin[(unsigned int)n],
                                          formSpikeWare->m_swsSpikes.m_swspSpikePars.m_vdMax[(unsigned int)n],
                                          this
                                          );
}
//------------------------------------------------------------------------------






