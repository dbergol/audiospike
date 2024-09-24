//------------------------------------------------------------------------------
/// \file frmSetParameters.cpp
///
/// \author Berg
/// \brief Implementation of a form to set one ore two parameter values, chart
/// axis properties or chart shape values
/// single named parameter respectively
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
#include "frmSetParameters.h"
#include "SpikeWareMain.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//------------------------------------------------------------------------------
/// constructor - empty
//------------------------------------------------------------------------------
__fastcall TformSetParameters::TformSetParameters(TComponent* Owner)
   : TForm(Owner)
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets position of form
//------------------------------------------------------------------------------
void TformSetParameters::SetPos(TForm* pfrm)
{
   if (!pfrm)
      Position = poMainFormCenter;
   else
      {
      Position = poDesigned;
      Top = pfrm->Top + (pfrm->Height - Height)/2;
      Left = pfrm->Left + (pfrm->Width - Width)/2;

      // adjust it, if the parent is scrollbox of mainform!
      if (pfrm->Parent == formSpikeWare->scb)
         {

         Types::TPoint TopLeft = formSpikeWare->scb->ClientToScreen(Point(0, 0));
         Top += TopLeft.y;
         Left += TopLeft.x;
         }
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets default values for control
//------------------------------------------------------------------------------
void TformSetParameters::SetMinMaxView()
{
   led1->EditLabel->Caption = "Minimum";
   btnReset1->Visible = true;
   lbU1->Caption = "";
   pnl1->Visible = true;

   led2->EditLabel->Caption = "Maximum";
   btnReset2->Visible = true;
   lbU2->Caption = "";
   pnl2->Visible = true;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// function for setting ONE single paramater by name
//------------------------------------------------------------------------------
bool TformSetParameters::SetParameter( UnicodeString usName,
                                       UnicodeString usUnit,
                                       float& fValue,
                                       TForm* pfrm)
{
   double d = (double)fValue;
   bool b = SetParameter(usName, usUnit, d, pfrm);
   fValue = (float)d;
   return b;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets dialog properties for setting ONE single paramater by name
//------------------------------------------------------------------------------
bool TformSetParameters::SetParameter( UnicodeString usName,
                                       UnicodeString usUnit,
                                       double &dValue,
                                       TForm* pfrm)
{
   SetPos(pfrm);
   pnl1->Visible = true;
   pnl2->Visible = false;

   btnReset1->Visible = false;
   led1->EditLabel->Caption = usName;
   led1->Text = DoubleToStr(dValue);
   lbU1->Caption      = usUnit;

   bool bReturn = (mrOk == ShowModal());
   if (bReturn)
      {
      dValue = StrToDouble(led1->Text);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// function for setting TWO paramaters by name
//------------------------------------------------------------------------------
bool TformSetParameters::SetParameters(UnicodeString usName1, UnicodeString usUnit1, float& fValue1,
                                       UnicodeString usName2, UnicodeString usUnit2, float& fValue2,
                                       TForm* pfrm)
{
   double d1 = (double)fValue1;
   double d2 = (double)fValue2;
   bool b = SetParameters(usName1, usUnit1, d1, usName2, usUnit2, d2, pfrm);
   fValue1 = (float)d1;
   fValue2 = (float)d2;
   return b;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets dialog properties for setting TWO paramaters by name
//------------------------------------------------------------------------------
bool TformSetParameters::SetParameters(UnicodeString usName1, UnicodeString usUnit1, double& dValue1,
                                       UnicodeString usName2, UnicodeString usUnit2, double& dValue2,
                                       TForm* pfrm
                                       )
{
   SetPos(pfrm);

   btnReset1->Visible = false;
   led1->EditLabel->Caption = usName1;
   led1->Text = DoubleToStr(dValue1);
   lbU1->Caption      = usUnit1;
   btnReset2->Visible = false;
   led2->EditLabel->Caption = usName2;
   led2->Text = DoubleToStr(dValue2);
   lbU2->Caption      = usUnit2;

   pnl1->Visible = true;
   pnl2->Visible = true;


   bool bReturn = (mrOk == ShowModal());
   if (bReturn)
      {
      dValue1 = StrToDouble(led1->Text);
      dValue2 = StrToDouble(led2->Text);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets dialog properties for setting min and max of a TChartShape
//------------------------------------------------------------------------------
bool TformSetParameters::SetChartShapeMinMax(TChartAxis* pca, TChartShape *pcs, TForm* pfrm)
{
   if (!pca || !pcs)
      return false;

   double dMin = Min(pcs->X0, pcs->X1);
   double dMax = Max(pcs->X0, pcs->X1);

   bool bReturn = ShowMinMax(pca, dMin, dMax, dMin, dMax, pfrm);
   if (bReturn)
      {
      pcs->X0 = StrToDouble(led1->Text);
      pcs->X1 = StrToDouble(led2->Text);
      }
   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets dialog properties for setting chart axis min/max
//------------------------------------------------------------------------------
bool TformSetParameters::SetAxisMinMax(TChartAxis* pca,
                                 double dDefaultMin,
                                 double dDefaultMax,
                                 TForm* pfrm)
{
   return SetAxisMinMax(pca, dDefaultMin, dDefaultMax, pca->Minimum, pca->Maximum, pfrm);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// sets dialog properties for setting chart axis min/max
//------------------------------------------------------------------------------
bool TformSetParameters::SetAxisMinMax(TChartAxis* pca,
                                 double dDefaultMin,
                                 double dDefaultMax,
                                 double dMin,
                                 double dMax,
                                 TForm* pfrm)
{
   if (!pca)
      throw Exception("invalid axis passed");



   m_dDefaultMin = dDefaultMin;
   m_dDefaultMax = dDefaultMax;


   led1->Text = DoubleToStr(dMin);
   led2->Text = DoubleToStr(dMax);

   bool bReturn = ShowMinMax(pca, dDefaultMin, dDefaultMax, dMin, dMax, pfrm);
   if (bReturn)
      pca->SetMinMax(StrToDouble(led1->Text), StrToDouble(led2->Text));

   return bReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// shows chart axis min/max
//------------------------------------------------------------------------------
bool TformSetParameters::ShowMinMax(TChartAxis* pca,
                                 double dDefaultMin,
                                 double dDefaultMax,
                                 double dMin,
                                 double dMax,
                                 TForm* pfrm)
{
   SetPos(pfrm);

   if (!pca)
      throw Exception("invalid axis passed");

   SetMinMaxView();

   // extract unit (if any)
   UnicodeString us = pca->Title->Text;
   us = us.SubString(us.Pos("["), us.Length());
   lbU1->Caption = us;
   lbU2->Caption = lbU1->Caption;


   m_dDefaultMin = dDefaultMin;
   m_dDefaultMax = dDefaultMax;


   led1->Text = DoubleToStr(dMin);
   led2->Text = DoubleToStr(dMax);

   return (mrOk == ShowModal());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnReset1: restes value 1 to it's default
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSetParameters::btnReset1Click(TObject *Sender)
{
   led1->Text = DoubleToStr(m_dDefaultMin);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnClick callback of btnReset2: restes value 2 to it's default
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSetParameters::btnReset2Click(TObject *Sender)
{
   led2->Text = DoubleToStr(m_dDefaultMax);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnShow callback sets default properties/sizes ....
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSetParameters::FormShow(TObject *Sender)
{
   led1->SetFocus();
   led1->SelectAll();
   btnReset1->Left = lbU1->Left + lbU1->Width + 4;
   btnReset2->Left = btnReset1->Left;
   ClientWidth = btnReset1->Left + btnReset1->Width + 4;
   btnOk->Left = led1->Left;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnKeyPress callback: allows closing form by pressing ESCAPE
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSetParameters::FormKeyPress(TObject *Sender, wchar_t &Key)
{
   if (Key == VK_ESCAPE)
      ModalResult = mrCancel;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnChange callback for LabeledEdit controls: enables/disables OK button
//------------------------------------------------------------------------------
#pragma argsused
void __fastcall TformSetParameters::ledChange(TObject *Sender)
{
   double dMin, dMax;
   btnOk->Enabled =     TryStrToDouble(led1->Text, dMin);
   if (pnl2->Visible && btnOk->Enabled)
      {
      btnOk->Enabled =     TryStrToDouble(led2->Text, dMax)
                        && dMin < dMax;
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// OnKeyPress callback for LabeledEdit controls: only allow numbers, a leading
/// minus and ONE decimalseparator
//------------------------------------------------------------------------------
void __fastcall TformSetParameters::ledKeyPress(TObject *Sender, wchar_t &Key)
{
   TLabeledEdit* pled = dynamic_cast<TLabeledEdit* >(Sender);
   if (!pled)
      return;
   if (  (Key >= L'0' && Key <= L'9')
      || (Key == L'-' && pled->SelStart == 0)
      || (Key == FormatSettings.DecimalSeparator && pled->Text.Pos(FormatSettings.DecimalSeparator) == 0)
      || Key == VK_BACK
      )
      return;

   Key = 0;
}
//------------------------------------------------------------------------------

