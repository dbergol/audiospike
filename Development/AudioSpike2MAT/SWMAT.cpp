//------------------------------------------------------------------------------
/// \file SWMAT.cpp
/// \author Berg
/// \brief Implementation tools for converting AudioSpike-XMLs to MAT-Structs
///
/// Project AudioSpike
/// Module  AudioSpikeMATLib.lib
///
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
#pragma hdrstop

#include "SWMAT.h"
#include "SWTools_Shared.h"
#include "Encddecd.hpp"

//#define CHAR16_T wchar_t
// avoid warnings from MATLAB
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wundef"
#include "mat.h"
#pragma clang diagnostic pop


#pragma comment(lib, "libmat.lib")
#pragma comment(lib, "libmx.lib")

#pragma warn -aus

#define AS_NAME UnicodeString("AudioSpike")



typedef std::vector<AnsiString >       vas;
typedef std::valarray<const char* >    vapc;
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// enum for different bahaviour of subnode-parsing
//------------------------------------------------------------------------------
enum TSubNodeType
{
   SNT_NONE = 0,              ///< do not parse subnodes at all
   SNT_SUBNODES_FIRST,        ///< parse subnodes by names of first subnode
   SNT_SUBNODES_ALL           ///< parse subnodes by ALL occurring field names
};
//------------------------------------------------------------------------------

//  local prototypes
mxArray* XMLValue2mxArray(_di_IXMLNode xml, AnsiString asFieldName);
void     AddMATFromXML(  MATFile *pmat, _di_IXMLNode xml, TSubNodeType snt, AnsiString asNodeName = "");
int      IndexFromName(vas &rvas, AnsiString as);


//------------------------------------------------------------------------------
/// returns index of string in passed vector, or -1 if it doesn't exist
//------------------------------------------------------------------------------
int IndexFromName(vas &rvas, AnsiString as)
{
   unsigned int n;
   for (n = 0; n < rvas.size(); n++)
      {
      if (UpperCase(rvas[n]) == UpperCase(as))
         return (int)n;
      }
   return -1;
}
//------------------------------------------------------------------------------


//---------------------------------------------------------------------------
/// converts a value of an XML subnode to an mxArray and returns pointer to it
/// NOTE: mxArray is created and NOT freed: caller is reesponsible for freeing
/// memory!
//---------------------------------------------------------------------------
mxArray* XMLValue2mxArray(_di_IXMLNode xml, AnsiString asFieldName)
{
   vved vvedData;
   mxArray* mValue = NULL;
   // retrieve XML value as text
   AnsiString as = GetXMLValue(xml, asFieldName);
   // if empty return NULL;
   if (!as.Length())
      return mValue;

   // Field name is 'Data'? Then it's bas64encoded!!
   if (LowerCase(asFieldName) == "data")
      {
      Sysutils::TBytes tbData = DecodeBase64(as);
      int nSamples = (int)tbData.Length/(int)sizeof(double);
      mValue = mxCreateDoubleMatrix(1, nSamples,mxREAL);

      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wcast-align"
      double *pdSrc = (double*)&tbData[0];
      #pragma clang diagnostic pop
      double *pdDst = mxGetPr(mValue);
      while (nSamples--)
         *pdDst++ = *pdSrc++;
      }
   // otherwise try to convert it to doubles
   else if (TryParseMLVector(as, vvedData))
      {
      mValue = mxCreateDoubleMatrix((int)vvedData[0].size(), (int)vvedData.size(), mxREAL);
      unsigned int nRow, nCol;
      double *pd = mxGetPr(mValue);
      for (nRow = 0; nRow < vvedData.size(); nRow++)
         {
         for (nCol = 0; nCol < vvedData[nRow].size(); nCol++)
            {
            *pd++ = vvedData[nRow][nCol];
            }
         }
      }
   // not Data, not double: write it as string
   else
      {
      mValue = mxCreateString(as.c_str());
      }
   return mValue;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/// adds all fields of an XML node  to a struct and writes it to passed MATfile
/// If bSubNodes is true, then this is done in a loop for all subnodes
//---------------------------------------------------------------------------
void AddMATFromXML(  MATFile *pmat, _di_IXMLNode xml, TSubNodeType snt, AnsiString asNodeName)
{
   mxArray *mArray = NULL;
   try
      {
      // of no struct name passed, use passed nodes name
      if (asNodeName == "")
         asNodeName = xml->GetNodeName();

      // set Node count
      int nNodeCount = snt ? xml->ChildNodes->Count : 1;

      if (!nNodeCount)
         return;

      // create array with names of fields as AnsiStrings
      vas vasNames;

      int nNode, nSubNode, nSubNodeCount;

      // for 'initial' names use either node itself ....
      _di_IXMLNode xmlTmp = xml;
      // .... or first subnode respectively
      if (snt != SNT_NONE)
         xmlTmp = xml->ChildNodes->Nodes[0];

      int nCount = xmlTmp->ChildNodes->Count;

      // loop through children and add ALL field names
      int n;
      for (n = 0; n < nCount; n++)
         {
         // OutputDebugString(AnsiString(xmlTmp->ChildNodes->Nodes[(int)n]->GetNodeName()).c_str());
         vasNames.push_back(AnsiString(xmlTmp->ChildNodes->Nodes[(int)n]->GetNodeName()));
         }

      // use ALL fieldnames? This is necessary, if we have subnodes with different dield names and/or counts
      // (e.g. for 'Parameters'). Then we need to create a Matlab struct with ALL existing field names from
      // ALL subnodes.....
      if (snt == SNT_SUBNODES_ALL && nNodeCount > 1)
         {
         AnsiString asName;
         // loop through nodes > 0 and collect field names, that are not yet in vasNames
         for (nNode = 1; nNode < nNodeCount; nNode++)
            {
            OutputDebugString(AnsiString("NODE " + IntToStr((int)nNode)).c_str());
            xmlTmp = xml->ChildNodes->Nodes[nNode];
            nSubNodeCount = xmlTmp->ChildNodes->Count;
            for (n = 0; n < nSubNodeCount; n++)
               {
               // add field only, if not already in vasNames!
               asName = AnsiString(xmlTmp->ChildNodes->Nodes[(int)n]->GetNodeName());
               if (IndexFromName(vasNames, asName) < 0)
                  vasNames.push_back(asName);
               }
            }
         }

      // create vector with pointers to field names
      vapc vapcPNames;
      vapcPNames.resize(size(vasNames));
      unsigned int u;
      for (u = 0; u < size(vasNames); u++)
         {
         vapcPNames[u] = vasNames[u].c_str();
         }


      // total number number of fields
      int nFieldCount = (int)vasNames.size();

      // create MATLAB sub-struct
      int dims[2] = {1, nNodeCount};
      mArray = mxCreateStructArray(2, dims, nFieldCount, &vapcPNames[0]);
      if (!mArray)
         throw Exception("error calling mxCreateStructArray");
      AnsiString as;
      vved vvedData;
      int nField;
      // loop through nodes
      for (nNode = 0; nNode < nNodeCount; nNode++)
         {
         // access node (or or subnode. NOTE: if snt == SNT_NONE, then nNodeCount
         // is always 1 ....)
         _di_IXMLNode xmlChild = snt ? xml->ChildNodes->Nodes[nNode] : xml;
         // loop through fields
         for (nField = 0; nField < nFieldCount; nField++)
            {
            mxSetFieldByNumber(mArray, nNode, nField, XMLValue2mxArray(xmlChild, vasNames[(unsigned int)nField]));
            }
         }
      // put struct into passed MAT-file
      matPutVariable(pmat, asNodeName.c_str(), mArray);
      }
   __finally
      {
      // free the created struct
      if (mArray)
         mxDestroyArray(mArray);
      }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// writes passed AudioSpike XML to MAT-file
//---------------------------------------------------------------------------
void XML2MAT(_di_IXMLNode xmlDoc, UnicodeString& rusMATFile)
{
   MATFile *pmat = NULL;
   try
      {
      UnicodeString usOutDir = ExtractFilePath(ExpandFileName(rusMATFile));
      if (!DirectoryExists(usOutDir))
         throw Exception("Output directory " + usOutDir + " does not exist");

      _di_IXMLNode xmlResultNode = xmlDoc->ChildNodes->FindNode("Result");
      if (!xmlResultNode)
         throw Exception("file contains no result");

      _di_IXMLNode xmlSettings = xmlDoc->ChildNodes->FindNode("Settings");
      if (!xmlSettings)
         throw Exception("file contains no Settings");

      _di_IXMLNode xmlParams = xmlDoc->ChildNodes->FindNode("Parameters");
      if (!xmlParams)
         throw Exception("file contains no Parameters");

      _di_IXMLNode xmlStimuli = xmlDoc->ChildNodes->FindNode("AllStimuli");
      if (!xmlStimuli)
         throw Exception("file contains no Stimuli");

      _di_IXMLNode xmlSpikes = xmlResultNode->ChildNodes->FindNode("Spikes");
      _di_IXMLNode xmlNonSelecteSpikes = xmlResultNode->ChildNodes->FindNode("NonSelectedSpikes");
      // one of them must exist at least
      if (!xmlSpikes && !xmlNonSelecteSpikes)
         throw Exception("file contains no Spikes");

      _di_IXMLNode xmlEpoches = xmlResultNode->ChildNodes->FindNode("Epoches");
      if (!xmlEpoches)
         throw Exception("file contains no Epoches");

      DeleteFile(rusMATFile);

      pmat = matOpen(AnsiString(rusMATFile).c_str(), "w");

      // add settings, NO subnodes (third arg 'false')
      AddMATFromXML(pmat, xmlSettings, SNT_NONE);

      // add parameters (with subnodes)
      OutputDebugString("GO PARAMS");
      AddMATFromXML(pmat, xmlParams, SNT_SUBNODES_ALL);

      // add single value StimulusSequence
      mxArray* mxa = XMLValue2mxArray(xmlResultNode, "StimulusSequence");
      matPutVariable(pmat, "StimulusSequence", mxa);
      mxDestroyArray(mxa);

      // add stimuli (fourth argument 'Stimuli', because node name is 'AllStimuli')
      AddMATFromXML(pmat, xmlStimuli, SNT_SUBNODES_FIRST, "Stimuli");
      // add Spikes and NonSelectedSpikes (if any)
      if (!!xmlSpikes)
         AddMATFromXML(pmat, xmlSpikes, SNT_SUBNODES_FIRST);
      if (!!xmlNonSelecteSpikes)
         AddMATFromXML(pmat, xmlNonSelecteSpikes, SNT_SUBNODES_FIRST);

      // add Epoches with respect to rbEpoches
      AddMATFromXML(pmat, xmlEpoches, SNT_SUBNODES_FIRST);

      }
   __finally
      {
      if (pmat)
         matClose(pmat);
      }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// converts passed XML file to MAT-file
//---------------------------------------------------------------------------
void XMLFile2MAT(UnicodeString usXMLFile, UnicodeString& rusMATFile)
{
   if (!FileExists(usXMLFile))
      throw Exception("XMLFile " + usXMLFile + " not found");

   if (rusMATFile == "")
      rusMATFile = ChangeFileExt(usXMLFile, ".mat");
   rusMATFile = ExpandFileName(rusMATFile);

   TXMLDocument* xml = new TXMLDocument(Application);
   try
      {
      xml->Active = false;
      xml->XML->Text = L"";
      xml->LoadFromFile(usXMLFile);
      xml->Active = true;
      _di_IXMLNode xmlDoc = xml->DocumentElement;
      if (!xmlDoc || xmlDoc->GetNodeName() != AS_NAME)
         throw Exception("'" + xml->FileName + "' is not a " + AS_NAME + " file");

      XML2MAT(xmlDoc, rusMATFile);
      }
   __finally
      {
      TRYDELETENULL(xml);
      }
}
//---------------------------------------------------------------------------
#pragma package(smart_init)


