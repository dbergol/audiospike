//------------------------------------------------------------------------------
/// \file frmBatch.cpp
///
/// \author Berg
/// \brief Implementation of a form for batch processing of multiple AudioSpike
/// mesurements
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
#include <limits.h>
#pragma hdrstop

#include "frmBatch.h"
#include "SpikeWareMain.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "frmASUI"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
/// constructor. Initializes members and ListView
//------------------------------------------------------------------------------
__fastcall TformBatch::TformBatch(TComponent* Owner)
   : TformASUI(Owner)
{
   m_lvOldWndProc = lv->WindowProc;
   lv->WindowProc = lvWndProc;
   DragAcceptFiles(lv->Handle, TRUE);
   m_bSaved = true;
   m_bDoInsituCal = true;
   m_bBatchRunning = false;
   btnInSituCalBeforeNext->Tag = 1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor. Restores default ListView procs
//------------------------------------------------------------------------------
__fastcall TformBatch::~TformBatch()
{
   if (lv->HandleAllocated())
      DragAcceptFiles(lv->Handle, FALSE);
   lv->WindowProc = m_lvOldWndProc;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnCloseQuery callback. Disallows closing if a batch is running and shows
/// 'saving question' otherwise
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::FormCloseQuery(TObject *Sender, bool &CanClose)
{
   if (formSpikeWare->IsRunning())
      {
      CanClose = false;
      return;
      }
   else if (!m_bSaved)
      {
      int n = MessageBox(  Handle,
                           "Do you want to quit batch mode without saving the batch?",
                           "Question",
                           MB_ICONQUESTION | MB_YESNO);
      CanClose = (n == ID_YES);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClose callback. Calls base class and cleans up batch mode
//------------------------------------------------------------------------------
void __fastcall TformBatch::FormClose(TObject *Sender, TCloseAction &Action)
{
   TformASUI::FormClose(Sender, Action);
   formSpikeWare->CleanupBatchMode();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// checks if passed item is pending 
//------------------------------------------------------------------------------
bool TformBatch::ItemIsPending(int nIndex)
{
   if (nIndex >= lv->Items->Count)
      throw Exception("Invalid index passed to " + UnicodeString(__FUNC__));
   return lv->Items->Item[nIndex]->SubItems->Strings[0] == "pending";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds an item as "pending"
//------------------------------------------------------------------------------
void TformBatch::AddItem(UnicodeString us)
{
   TListItem* pli = lv->Items->Add();
   pli->SubItems->Add("pending");
   pli->SubItems->Add(us);
   m_bSaved = false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// loads a bach from file
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnLoadClick(TObject *Sender)
{
   if (!m_bSaved)
      {
      if (ID_YES != MessageBox(  Handle,
                           "Current batch list was not saved. Do you want to load a new list and discard changes?",
                           "Question",
                           MB_ICONQUESTION | MB_YESNO)
         )
         return;
      }

   formSpikeWare->od->FileName   = "";
   formSpikeWare->od->InitialDir = ExpandFileName(formSpikeWare->m_usTemplatePath);
   TStringList *psl = new TStringList();
   try
      {
      formSpikeWare->od->Filter = "Audiospike Batchfiles|*.asb";
      if (formSpikeWare->od->Execute())
         {
         m_usSubPath = "";
         btnInSituCalBeforeNext->Tag = 1;
         m_viLastOutChannels.clear();
         lv->Clear();
         psl->LoadFromFile(formSpikeWare->od->FileName);
         int n;
         for (n = 0; n < psl->Count; n++)
            AddItem(psl->Strings[n]);
         m_bSaved = true;
         UpdateGUI();
         }
      }
   __finally
      {
      TRYDELETENULL(psl);
      formSpikeWare->od->Filter = "XML-file|*.xml";
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// saves a batch to file
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnSaveClick(TObject *Sender)
{
   if (!lv->Items->Count)
      return;
   //
   if (sd->Execute())
      {
      TStringList *psl = new TStringList();
      try
         {
         int n;
         for (n = 0; n < lv->Items->Count; n++)
            psl->Add(lv->Items->Item[n]->SubItems->Strings[1]);
         psl->SaveToFile(ChangeFileExt(sd->FileName, ".asb"));
         m_bSaved = true;
         }
      __finally
         {
         TRYDELETENULL(psl);
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// calls DoBatchRun
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnRunClick(TObject *Sender)
{
   DoBatchRun();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns flag if InsituCal to be done
//------------------------------------------------------------------------------
bool TformBatch::DoInsituCal()
{
   return m_bDoInsituCal;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// does the real batch processing
//------------------------------------------------------------------------------
void TformBatch::DoBatchRun()
{
   try
      {
      m_bBatchRunning = true;
      btnStopBeforeNext->Down = false;
      // check all files for validity before running measurement by loading each
      formSpikeWare->m_pslParamStr->Values["batchsubpath"] = "";
      int n;
      for (n = 0; n < lv->Items->Count; n++)
         {
         // only load pending meas
         if (!ItemIsPending(n))
            continue;
         // break on errors. LoadMeasurementTemplate has displayed errors
         if (!formSpikeWare->LoadMeasurementTemplate(lv->Items->Item[n]->SubItems->Strings[1]))
            return;
         }

      // ask for subpath
      // until m_pslParamStr->Values["batchsubpath"] is filled
      if (m_usSubPath.IsEmpty())
         {
         if (!InputQuery("Subpath", "Please enter a subdirectory for the results", m_usSubPath))
            return;
         }

      std::vector<int > vi = formSpikeWare->m_smp.m_viMeasChannelsOutUsed;
      TSWRunResult swrr;
      m_nAction = BA_NONE;
      n = 0;

      while (n < lv->Items->Count)
         {
         lv->MultiSelect = false;
         // only run pending meas
         if (!ItemIsPending(n))
            {
            n++;
            continue;
            }

         // break on errors. LoadMeasurementTemplate has displayed errors
         if (!formSpikeWare->LoadMeasurementTemplate(lv->Items->Item[n]->SubItems->Strings[1]))
            break;

         formSpikeWare->SetAutoResultPath(m_usSubPath);

         lv->Selected = lv->Items->Item[n];
         lv->Items->Item[n]->SubItems->Strings[0] = "running";

         m_nAction = BA_CONTINUE;

         UpdateGUI();

         // if insitu, then check, if used out channels have changed since last insitu meas and force
         // calibration in that case!

         if (formSpikeWare->IsInSitu())
            {
            vi = formSpikeWare->m_smp.m_viMeasChannelsOutUsed;
            if (vi.size() != m_viLastOutChannels.size())
               btnInSituCalBeforeNext->Tag = 1;
            else
               {
               unsigned int m;
               for (m = 0; m < vi.size(); m++)
                  {
                  if (vi[m] != m_viLastOutChannels[m])
                     {
                     btnInSituCalBeforeNext->Tag = 1;
                     break;
                     }
                  }
               }
            m_viLastOutChannels = vi;
            }


         // evaluate 'doinsitucal' for next meas
         m_bDoInsituCal = btnInSituCalBeforeNext->Down || btnInSituCalBeforeNext->Tag;

         // start meas WITHOUT resume flag
         swrr = formSpikeWare->RunMeasurement(false);
         if (swrr == SWRR_ERROR)
            {
            formSpikeWare->DeleteCurrentResults(false);
            lv->Items->Item[n]->SubItems->Strings[0] = "pending";
            break;
            }
         // reset internal values
         btnInSituCalBeforeNext->Down = false;
         btnInSituCalBeforeNext->Tag = 0;

         // if it returns 'pause' then enter while loop
         while (swrr == SWRR_PAUSE)
            {
            lv->Items->Item[n]->SubItems->Strings[0] = "paused";
            // wait for pause button to change it's tag (or STOP)
            while (btnPause->Tag)
               {
               Sleep(1);
               Application->ProcessMessages();
               }
            // run meas again WITH resume flag
            lv->Items->Item[n]->SubItems->Strings[0] = "running";
            swrr = formSpikeWare->RunMeasurement(true);
            }

         UpdateGUI();

         // check action
         // save?
         if (m_nAction == BA_SAVESKIP || m_nAction == BA_CONTINUE)
            {
            formSpikeWare->btnSaveClick(NULL);
            UpdateGUI();
            // update status
            lv->Items->Item[n]->SubItems->Strings[0] = m_nAction ==  BA_SAVESKIP ? "aborted" : "done";
            }
         // repeat?
         else if (m_nAction == BA_REPEAT)
            {
            formSpikeWare->DeleteCurrentResults(false);
            formSpikeWare->SetMeasurementChanged(false, true);
            lv->Items->Item[n]->SubItems->Strings[0] = "pending";
            }

         if (m_nAction != BA_CONTINUE || btnStopBeforeNext->Down)
            break;
         n++;
         }
      btnStopBeforeNext->Down = false;
      lv->MultiSelect = true;
      }
   __finally
      {
      m_bBatchRunning = false;
      UpdateGUI();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// pauses/unpauses measurement
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnPauseClick(TObject *Sender)
{
   btnPause->Enabled = false;
   btnPause->Tag = !btnPause->Tag;

   if (btnPause->Tag)
      {
      formSpikeWare->SetGUIStatus(SWGS_PAUSE);
      formSpikeWare->m_smp.Stop();
      }
   UpdateGUI();
   formSpikeWare->SetGUIStatus();
   btnPause->Enabled = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// updates this GUI and optionally main GUI
//------------------------------------------------------------------------------
void TformBatch::UpdateGUI(bool bUpdateMainGUI)
{
   btnLoad->Enabled     = !(formSpikeWare->IsRunning() || m_bBatchRunning);
   btnSave->Enabled     = btnLoad->Enabled;
   btnAdd->Enabled      = btnLoad->Enabled;
   btnDelete->Enabled   = btnLoad->Enabled && lv->SelCount > 0;
   btnUp->Enabled       = btnLoad->Enabled && lv->SelCount == 1;
   btnDown->Enabled     = btnUp->Enabled;
   btnRun->Enabled      = btnLoad->Enabled && HasUnfinishedFilesInList();
   btnPause->Enabled    = formSpikeWare->IsRunning();
   btnStopBeforeNext->Enabled    = btnPause->Enabled;
   btnStopRejectRepeat->Enabled  = btnPause->Enabled;
   btnStopSaveStep->Enabled      = btnPause->Enabled;

   if (bUpdateMainGUI)
      formSpikeWare->SetGUIStatus();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// returns true if ANY item is pending
//------------------------------------------------------------------------------
bool TformBatch::HasUnfinishedFilesInList()
{
   int n;
   for (n = 0; n < lv->Items->Count; n++)
      {
      if (ItemIsPending(n))
         return true;
      }
   return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// user defined window procedure handling drag/drop and forbid dragging of 
/// first two column widths
//------------------------------------------------------------------------------
void __fastcall TformBatch::lvWndProc(TMessage& Msg)
{
   bool bHandled = false;
   if (Msg.Msg == WM_DROPFILES)
      {
      if (btnLoad->Enabled)
         {
         // grab a handle to the drop structure
         HDROP HDrop = (HDROP)Msg.WParam;

         // find the number of files dropped
         unsigned int nNumFiles = DragQueryFileW(HDrop, 0xFFFFFFFF, NULL, NULL);
         if (!nNumFiles)
            {
            Msg.Result = 0;
            return;
            }
         unsigned int n;
         for (n = 0; n < nNumFiles; n++)
            {
             // load the file
             unsigned int nNameLength = DragQueryFileW(HDrop, n, NULL, NULL) + 1;
             wchar_t *szFileName = new wchar_t[nNameLength];
             DragQueryFileW(HDrop, n, szFileName, nNameLength);
             AddItem(szFileName);
             // clean up
             delete [] szFileName;
            }
         DragFinish(HDrop);
         UpdateGUI();
         }
      Msg.Result = 0;
      bHandled = true;
      }
   else if ( Msg.Msg == WM_NOTIFY )
      {
      const HD_NOTIFY* phdn = reinterpret_cast<HD_NOTIFY*>(Msg.LParam);
      if (phdn->iItem == 0 || phdn->iItem == 1)
         {
         NMHDR *nmhdr = (NMHDR*)Msg.LParam;
         if (  nmhdr->code == HDN_BEGINTRACKW
            || nmhdr->code == HDN_BEGINTRACKA
            || nmhdr->code == HDN_DIVIDERDBLCLICKA
            || nmhdr->code == HDN_DIVIDERDBLCLICKW
            )
            {
            Msg.Result = 1;
            bHandled = true;
            }
         }
      }

   else if (Msg.Msg == CM_RECREATEWND)
      {
      m_lvOldWndProc(Msg);  // call the default handler first
      DragAcceptFiles(lv->Handle, TRUE);
      bHandled = true;
      }
   if(!bHandled)
      m_lvOldWndProc(Msg);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adds a file to batch
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnAddClick(TObject *Sender)
{
   formSpikeWare->od->FileName   = "";
   formSpikeWare->od->InitialDir = ExpandFileName(formSpikeWare->m_usTemplatePath);
   TOpenOptions odo = formSpikeWare->od->Options;
   formSpikeWare->od->Options << ofAllowMultiSelect;
   try
      {
      if (formSpikeWare->od->Execute())
         {
         int n;
         for (n = 0; n < formSpikeWare->od->Files->Count; n++)
            AddItem(formSpikeWare->od->Files->Strings[n]);
         UpdateGUI();
         }
      }
   __finally
      {
      formSpikeWare->od->Options = odo;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes a file from batch
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnDeleteClick(TObject *Sender)
{
   if (!lv->SelCount)
      return;
   int n;
   for (n = lv->Items->Count-1; n >= 0; n--)
      {
      if (lv->Items->Item[n]->Selected && ItemIsPending(n))
         lv->Items->Delete(n);
      }
   m_bSaved = false;
   UpdateGUI();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// moves an item within item list
//------------------------------------------------------------------------------
#pragma argsused
void TformBatch::MoveItem(TListItem* pli, bool bUp)
{
   if (!pli || lv->SelCount != 1 || !ItemIsPending(pli->Index))
      return;

   int nNewIndex = bUp ? pli->Index-1 : pli->Index+1;

   if (nNewIndex < 0 || nNewIndex >= lv->Items->Count)
      return;

   // do not allow to move a template above a non-pending one
   if (bUp && !ItemIsPending(nNewIndex))
      return;

   lv->Items->BeginUpdate();
   try
      {
      bool bChecked = lv->Selected->Checked;
      TListItem* pliNew = new TListItem(lv->Items);
      pli->Assign(lv->Selected);
      lv->Selected->Delete();
      lv->Items->Insert(nNewIndex);
      lv->Items->Item[nNewIndex]->Assign(pliNew);
      lv->Selected = lv->Items->Item[nNewIndex];
      lv->Selected->Checked = bChecked;
      }
   __finally
      {
      m_bSaved = false;
      lv->Items->EndUpdate();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// moves item one up
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnUpClick(TObject *Sender)
{
   MoveItem(lv->Selected, true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// moves item one down
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnDownClick(TObject *Sender)
{
   MoveItem(lv->Selected, false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of listview: calls UpdateGUI
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::lvClick(TObject *Sender)
{
   UpdateGUI();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// allwow to use DEL button to delete an item from batch
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::lvKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
   if (Key == VK_DELETE)
      btnDeleteClick(NULL);
   UpdateGUI();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// resizing function of listview to adjust third column width
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::lvResize(TObject *Sender)
{
   lv->Columns->Items[2]->Width = lv->Width - 95;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// highlight selected item in styled mode
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::lvAdvancedCustomDrawItem(TCustomListView *Sender, TListItem *Item,
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

//------------------------------------------------------------------------------
/// callback to stop an item, save it and proceed to next item
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnStopSaveStepClick(TObject *Sender)
{
   if (ID_YES != MessageBox(  Handle,
                        "Are you sure that you want to stop current measurement, save the status and step to next item?",
                        "Question",
                        MB_ICONQUESTION | MB_YESNO)
      )
      return;
   m_nAction = BA_SAVESKIP;
   formSpikeWare->btnStopClick(NULL);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// callback to stop an item, reject data and repeat it
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformBatch::btnStopRejectRepeatClick(TObject *Sender)
{
   if (ID_YES != MessageBox(  Handle,
                        "Are you sure that you want to stop current measurement and repeat it?",
                        "Question",
                        MB_ICONQUESTION | MB_YESNO)
      )
      return;
   m_nAction = BA_REPEAT;
   formSpikeWare->btnStopClick(NULL);

}
//------------------------------------------------------------------------------

