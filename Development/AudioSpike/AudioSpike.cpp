//------------------------------------------------------------------------------
/// \file AusioSpike.cpp
///
/// \author Berg
/// \brief Main for AusioSpike. Implements winmain, dows some global VCL settings
/// and handlse command line parameters
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
#include <complex>


#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>

USEFORM("frmWait.cpp", formWait);
USEFORM("SpikeWareMain.cpp", formSpikeWare);
USEFORM("frmVersionCheck.cpp", formVersionCheck);
USEFORM("frmSpikeSettings.cpp", formSpikeSettings);
USEFORM("frmSpikes.cpp", formSpikes);
USEFORM("frmSoundSettings.cpp", formSoundSettings);
USEFORM("frmVectorStrength.cpp", formVectorStrength);
USEFORM("frmTriggerTest.cpp", formTriggerTest);
USEFORM("frmStimuli.cpp", formStimuli);
USEFORM("frmBatch.cpp", formBatch);
USEFORM("frmASUI.cpp", formASUI);
USEFORM("frame_SpikeSettings.cpp", frameSpikeSettings); /* TFrame: File Type */
USEFORM("frmCalibration.cpp", formCalibration);
USEFORM("frmBubblePlot.cpp", formBubblePlot);
USEFORM("frmBubbleData.cpp", formBubbleData);
USEFORM("frame_BubbleData.cpp", frameBubbleData); /* TFrame: File Type */
USEFORM("formAbout.cpp", AboutBox);
USEFORM("frmCalibrationCalibrator.cpp", formCalibrationCalibrator);
USEFORM("frmSelectChannels.cpp", formSelectChannels);
USEFORM("frmSelectChannel.cpp", formSelectChannel);
USEFORM("frmSelect.cpp", formSelect);
USEFORM("frmSignalPSTH.cpp", formSignalPSTH);
USEFORM("frmSettings.cpp", formSettings);
USEFORM("frmSetParameters.cpp", formSetParameters);
USEFORM("frmSearchFree.cpp", formSearchFree);
USEFORM("frmEpocheWindow.cpp", formEpocheWindow);
USEFORM("frmEpoche.cpp", formEpoches);
USEFORM("frmCluster.cpp", formCluster);
USEFORM("frmPSTH.cpp", formPSTH);
USEFORM("frmFileSel.cpp", formFileSel);
USEFORM("frmFFTEdit.cpp", formFFTEdit);
//---------------------------------------------------------------------------
#include "SpikeWareMain.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
   try
   {
      // never remove !!
      FormatSettings.ThousandSeparator = 0;
      FormatSettings.DecimalSeparator = L'.';

      EnsureNewIniLocation();
      // first of all we have to set settings name
      TformSpikeWare::ms_usSettingsName = ReadSettingsName();

      if (!MulipleInstanceAllowed())
         {
         HWND hwnd = ShowRunningAppWindow();
         if (!!hwnd)
            {
            // if programm already running AND multiple instances
            // not allowed in settings, then write passed parameters
            // to an INI file and post inform running instance by sending
            // a special window message
            if (ParamCount() > 1)
               {
               WriteParamStrIni();
               PostMessage(hwnd, WM_SWCMD, 0, 0);
               }
            return 0;
            }
         }

       Application->Initialize();
       Application->MainFormOnTaskBar = true;
       Application->CreateForm(__classid(TformSpikeWare), &formSpikeWare);
       Application->CreateForm(__classid(TformSpikeSettings), &formSpikeSettings);
       Application->CreateForm(__classid(TformWait), &formWait);
       Application->CreateForm(__classid(TformTriggerTest), &formTriggerTest);
       Application->CreateForm(__classid(TformFileSel), &formFileSel);
       Application->CreateForm(__classid(TformCalibration), &formCalibration);
       Application->CreateForm(__classid(TformCalibrationCalibrator), &formCalibrationCalibrator);
       Application->CreateForm(__classid(TformFFTEdit), &formFFTEdit);
       Application->CreateForm(__classid(TAboutBox), &AboutBox);
       if (ParamCount() > 0)
          formSpikeWare->ProcessCommandLine(true);

       BringWindowToTop(formSpikeWare->Handle);
       SetForegroundWindow(formSpikeWare->Handle);
       Application->Run();
   }
   catch (Exception &exception)
   {
       Application->ShowException(&exception);
   }
   catch (...)
   {
       try
       {
          throw Exception("");
       }
       catch (Exception &exception)
       {
          Application->ShowException(&exception);
       }
   }
   return 0;
}
//------------------------------------------------------------------------------
