****************************************************************************
	Description of AudioSpike source code files

	Copyright 2023 Daniel Berg, Oldenburg, Germany
****************************************************************************

****************************************************************************
    AudioSpike is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AudioSpike is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AudioSpike.  If not, see <http:www.gnu.org/licenses/>.
****************************************************************************

The source code archive of AudioSpike contains all source code files and
C++-Builder project files (*.cbproj) to compile all executables and libraries
of AudioSpike. If not stated differently in separate _README.txt files within 
a projects subdirectory all projects were compiled using Embarcadero C++-Builder
11.3 Alexandria Professional. They were compiled using "Steema TChart Pro". The Pro
version is needed for histograms and calculation functions used within AudioSpike, 
compiling with the using "Steema TChart Standard" shipped with C++-Builder will fail.
You might need to adjust paths and/or 3rd party source code files and/or libraries 
within the project files for a successful build. 


Please extract the Development-directory conained in the archive to the installation 
directory of AudioSpike. 


****************************************************************************
A. Prerequisites

The following 3rd party libraries are needed for AudioSpike (not all projects will 
need all of these libaries). 

1.FFTW 3.x
-----------
The FFTW package was developed at MIT by Matteo Frigo and Steven G. Johnson. It is
licensed under GNU GPL and can be downloaded from the fftw.org website. 
AudioSpike projects using the libfftw3f-3.dll link to an import library created 
from the original DLL using Embarcaderos implib.exe or mkexp.exe respectively.

2. libsndfile
-------------
AudioSpike uses libsndfile 1.x for reading sound files, see http://www.mega-nerd.com/libsndfile/ 
for more information.
AudioSpike projects using the libsndfile link to an import library created from the original DLL
using using Embarcaderos implib.exe or mkexp.exe respectively.

3. Steinberg VST-SDK 2.4
------------------------
The VST-Plugins shipped with AudioSpike uses the VST-SDK 2.4 by Steinberg. It can be downloaded 
from the Steinberg website. AudioSpike projects using the VST-SDK by linking VSTSDK.lib which 
was compiled and linked from the original files. You may replace VSTSDK.lib by adding the 
corresponding source from the original SDK 
	AudioEffect.cpp and audioeffectx.cpp
to the projects instead.
****************************************************************************

4. SoundDllPro by Daniel Berg
-------------------------------
AudioSpike uses the DLL AudioSpike.dll. It is part of the Open Source Freeware AudioSpike by 
Daniel Berg. The source code of AudioSpike is available on the website www.AudioSpike.de.

5. MATLAB libraries
-------------------
The XML-to-MAT conversion tools (AudioSpike2MATLib, see below) links against MathWorks (MATLAB) libraries.
****************************************************************************

****************************************************************************
B. Directories/Projects

NOTE: the directories/projects are described here in the order they have to be compiled. For detailed
information please refer refer to the source files in the project folders.  

1. AudioSpike
-------------
This directory contains three projects:
- AudioSpikeMATLib.cbproj: static library for converting XML to MATLAB MAT files. 
- AudioSpike2MAT.cbproj: executable for converting XML files to MATLAB MAT files. Uses AudioSpikeMATLib
- AudioSpike.cbproj: main executable of AudioSpike. Uses AudioSpikeMATLib by default. If you cannot or 
  don't want to link against MathWorks (MATLAB) libraries you need to define the global compiler define 
  "NOWRITEMAT".

2. HtVstEqAS
------------
The directory contains four VST-Plugins used by AudioSpike. They all compile the same source files, 
but set different global compiler defines:
HtVSTEqAS.cbproj      - no defines              (no GUI, no external calibration callbacks)
HtVSTEqVisAS.cbproj   - VISUAL_PLUGIN           (with GUI, no external calibration callbacks)
HtVSTCalAS.cbproj     - EXTPROCS                (no GUI, with external calibration callbacks)
HtVSTCalVisAS.cbproj  - VISUAL_PLUGIN;EXTPROCS  (with GUI, with external calibration callbacks)


NOTE: some of the subfolders may contain separate _README.txt files with important information!!!
*************************************************************************************************

