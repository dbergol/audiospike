//------------------------------------------------------------------------------
/// \file frmSetParameters.h
///
/// \author Berg
/// \brief Implementation of a form to set one ore two parameter values, chart
/// axis properties or chart shape values
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
#ifndef frmSetParametersH
#define frmSetParametersH
#include <System.Classes.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeeShape.hpp>
#include <Vcl.Mask.hpp>
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// form to set one ore two parameter values, chart axis properties or chart shape 
/// values
//------------------------------------------------------------------------------
class TformSetParameters : public TForm
{
   __published:	// IDE-verwaltete Komponenten
      TButton *btnOk;
      TPanel *pnl2;
      TLabel *lbU2;
      TSpeedButton *btnReset2;
      TLabeledEdit *led2;
      TPanel *pnl1;
      TLabel *lbU1;
      TLabeledEdit *led1;
      TSpeedButton *btnReset1;
      void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
      void __fastcall ledChange(TObject *Sender);
      void __fastcall ledKeyPress(TObject *Sender, wchar_t &Key);
      void __fastcall FormShow(TObject *Sender);
      void __fastcall btnReset2Click(TObject *Sender);
      void __fastcall btnReset1Click(TObject *Sender);
   public:		// Benutzer-Deklarationen
      double   m_dDefaultMin;
      double   m_dDefaultMax;
      bool     ShowMinMax( TChartAxis* pca,
                           double dDefaultMin,
                           double dDefaultMax,
                           double dMin,
                           double dMax,
                           TForm* pfrm = NULL);
      bool     SetAxisMinMax( TChartAxis* pca,
                              double dDefaultMin,
                              double dDefaultMax,
                              double dMin,
                              double dMax,
                              TForm* pfrm = NULL);
      void     SetMinMaxView();
      __fastcall TformSetParameters(TComponent* Owner);
      bool SetParameter(UnicodeString usName, UnicodeString usUnit, double& dValue, TForm* pfrm = NULL);
      bool SetParameters(  UnicodeString usName1, UnicodeString usUnit1, double& dValue1,
                           UnicodeString usName2, UnicodeString usUnit2, double& dValue2,
                           TForm* pfrm = NULL);
      bool SetParameter(UnicodeString usName, UnicodeString usUnit, float& fValue, TForm* pfrm = NULL);
      bool SetParameters(  UnicodeString usName1, UnicodeString usUnit1, float& fValue1,
                           UnicodeString usName2, UnicodeString usUnit2, float& fValue2,
                           TForm* pfrm = NULL);
      bool SetChartShapeMinMax(TChartAxis* pca, TChartShape *pcs, TForm* pfrm = NULL);
      bool SetAxisMinMax(TChartAxis* pca, double dDefaultMin, double dDefaultMax, TForm* pfrm = NULL);
      void SetPos(TForm* pfrm);
};
//------------------------------------------------------------------------------
#endif
