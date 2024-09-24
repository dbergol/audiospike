//------------------------------------------------------------------------------
/// \file formVisual.h
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
#ifndef formAboutH
#define formAboutH
//------------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Imaging.pngimage.hpp>

//------------------------------------------------------------------------------
/// About dialog for SoundMexPro
//------------------------------------------------------------------------------
class TAboutBox : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
   TPanel *pnl;
   TLabel *lbVersion;
   TLabel *lbLic;
   TImage *imgAsio;
   TImage *imgVST;
   TBevel *bvlLine1;
   TLabel *lbVersionValue;
   TLabel *lbLicValue;
   TRichEdit *reOwn;
   TRichEdit *reAsio;
   TRichEdit *reVST;
   TButton *OKButton;
   TRichEdit *reLibSndFile;
   TBevel *bvlLine2;
   TPanel *Panel1;
   TBevel *Bevel1;
   void __fastcall FormShow(TObject *Sender);
private:	// Anwender-Deklarationen
public:		// Anwender-Deklarationen
   __fastcall TAboutBox(TComponent* Owner);
   static UnicodeString ms_usVerInfo;
};
//------------------------------------------------------------------------------
extern PACKAGE TAboutBox *AboutBox;

#endif
