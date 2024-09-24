//------------------------------------------------------------------------------
/// \file eqinput.cpp
///
/// \author Berg
/// \brief Implementation form for displaying spectral filters + input and output
/// to/from filter
///
/// Project AudioSpike
/// Module  HtVSTEqAS.dll
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
#include <limits.h>
#include <inifiles.hpp>

#pragma hdrstop
#include "eqinput.h"

#pragma package(smart_init)

#pragma resource "*.dfm"
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// constructor, initializes members
//------------------------------------------------------------------------------
__fastcall TfrmEqInput::TfrmEqInput()
   :  TForm((TComponent*)NULL)

{
   RawFilterSeries->XValues->Order   = loAscending;
   PreSpecSeriesL->XValues->Order   = loAscending;
   PostSpecSeriesL->XValues->Order  = loAscending;
   Chart->AllowZoom = true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// destructor, empty....
//------------------------------------------------------------------------------
__fastcall TfrmEqInput::~TfrmEqInput()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// reads settings from ini
//------------------------------------------------------------------------------
void TfrmEqInput::ReadSettings()
{
   TIniFile* pIni = NULL;
   try
      {
      pIni           = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
      cbLog->Checked = pIni->ReadBool("Debug", "FFTPluginLogChecked", true);
      }
   catch(...)
      {
      if (pIni)
         delete pIni;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback. Calls ReadSettings and cbLogClick
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmEqInput::FormShow(TObject *Sender)
{
   ReadSettings();
   cbLogClick(NULL);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes chart series and axis properties
//------------------------------------------------------------------------------
void __fastcall TfrmEqInput::Initialize(unsigned int nFFTLen, float fSampleRate)
{
   float f4BinSize  = fSampleRate / (float)nFFTLen;
   PreSpecSeriesL->Clear();
   PostSpecSeriesL->Clear();
   RawFilterSeries->Clear();
   float f4X = 0.0f;
   for (unsigned int i = 0; i < nFFTLen/2; i++)
      {
      f4X += f4BinSize;
      PreSpecSeriesL->AddXY((double)f4X, 1, "", clTeeColor);
      PostSpecSeriesL->AddXY((double)f4X, 1, "", clTeeColor);
      RawFilterSeries->AddXY((double)f4X, 1, "", clTeeColor);
      }
   Chart->BottomAxis->Maximum = (double)fSampleRate/2.0;
   Chart->BottomAxis->Minimum = (double)f4BinSize;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnBeforeDrawAxis callback of chart: paints chartrect black
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmEqInput::ChartBeforeDrawAxes(TObject *Sender)
{
   Chart->Canvas->Brush->Color   = clBlack;
   Chart->Canvas->Pen->Color     = clBlack;
   Chart->Canvas->FillRect(Chart->ChartRect);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of Lin/Log checkbox: updates bottm axis properties
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmEqInput::cbLogClick(TObject *Sender)
{
   Chart->BottomAxis->Logarithmic = cbLog->Checked;
   AdjustLogAxis(Chart->BottomAxis);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// adjusts log axis to have reasonable values/labels
//------------------------------------------------------------------------------
#pragma argsused
void TfrmEqInput::AdjustLogAxis(TChartAxis* pca)
{
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wfloat-equal"
   #pragma clang diagnostic ignored "-Wundefined-func-template"
   pca->Items->Clear();
   pca->Items->Automatic = true;
   pca->AxisValuesFormat = "";
   if (!pca->Logarithmic || pca->Maximum == pca->Minimum)
      return;
   #pragma clang diagnostic pop
   pca->Items->Automatic = false;

   Application->ProcessMessages();
   int n = 10;
   int m, q;
   pca->Items->Add(n,n);
   while (1)
      {
      m = (int)pow(10, floor(log10((float)n)));
      n += m;
      q = n/m;
      UnicodeString us;
      if (q == 2 || q == 5 || q == 10)
         pca->Items->Add(n,n);
      else
         pca->Items->Add(n,"");

      if (n >pca->Maximum)
         break;
      }

   Chart->Repaint();
}
//------------------------------------------------------------------------------


