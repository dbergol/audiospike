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
#include <stdio.h>

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
   SNT_NONE = 0,              ///< do not parse subnodes at all, use fields from passed node
   SNT_SUBNODES_FIRST,        ///< parse subnodes by names of first subnode
   SNT_SUBNODES_ALL           ///< parse subnodes by ALL occurring field names
};
//------------------------------------------------------------------------------

//  local prototypes
void     PrintElapsed(DWORD &dw, UnicodeString us);
mxArray* AnsiString2mxArray(AnsiString as, bool bData = false);
void     AddNodeNames(_di_IDOMNode dom, vas& rvasNames);
void     AddMATFromXML(  MATFile *pmat, _di_IXMLNode xml, TSubNodeType snt, AnsiString asNodeName = "");
int      IndexFromName(vas &rvas, AnsiString as);


//------------------------------------------------------------------------------
/// debugging function: writes passed string to OutputDebugString with timing
/// info and writes crrent tick count to passed DWORD
//------------------------------------------------------------------------------
void PrintElapsed(DWORD &dw, UnicodeString us)
{
   UnicodeString usP = us + ": " + IntToStr((int)(GetTickCount() - dw));
   printf("\n%ls", usP.w_str());
   dw = GetTickCount();
}
//------------------------------------------------------------------------------


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


//------------------------------------------------------------------------------
///
//------------------------------------------------------------------------------
mxArray* AnsiString2mxArray(AnsiString as, bool bData)
{
   vved vvedData;
   mxArray* mValue = NULL;


   // if empty return NULL;
   if (!as.Length())
      return mValue;

   // Field name is 'Data'? Then it's bas64encoded!!
   if (bData)
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
/// adds all fields names from dom to assed vector, if not already contained in it
//---------------------------------------------------------------------------
void AddNodeNames(_di_IDOMNode dom, vas& rvasNames)
{
   AnsiString asName;
   // go through childnodes to find named node
   _di_IDOMNode child = dom->childNodes->item[0];
   while (child)
      {
      asName = AnsiString(child->nodeName);
      if (IndexFromName(rvasNames, asName) < 0)
         rvasNames.push_back(asName);
      child = child->nextSibling;
      }

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// adds all fields of an XML node to a struct and writes it to passed MATfile
//---------------------------------------------------------------------------
void AddMATFromXML(  MATFile *pmat, _di_IXMLNode xml, TSubNodeType snt, AnsiString asNodeName)
{
   // NOTE: xml interface is horribly slow, thus we use the DOM interface instead

   mxArray *mArray = NULL;
   try
      {
      // of no struct name passed, use passed nodes name
      if (asNodeName == "")
         asNodeName = xml->GetNodeName();

      _di_IDOMNode dom = xml->GetDOMNode();

      if (!dom->childNodes->length)
         return;

      // set Node count
      int nNodeCount = snt ? dom->childNodes->length : 1;


      // IMPORTANT NOTE: in MATLAB a struct has a fix number of (named) members.
      // In AudioSpike the 'Paramaters' subnodes may have different field counts
      // with different field names. Thus we have to know all field names
      // beforehand for the call to mxCreateStructArray
      vas vasNames;
      // use root values names ....
      if (snt == SNT_NONE)
         AddNodeNames(dom, vasNames);
      // ... or names from first child ....
      else if (snt == SNT_SUBNODES_FIRST)
         AddNodeNames(dom->childNodes->item[0], vasNames);
      // ... or unique names from ALL children
      else
         {
         _di_IDOMNode domTmp = dom->childNodes->item[0];
         while (domTmp)
            {
            AddNodeNames(domTmp, vasNames);
            domTmp = domTmp->nextSibling;
            }
         }

      // create vector with pointers to field names (needed for mx interface below)
      vapc vapcPNames;
      vapcPNames.resize(size(vasNames));
      unsigned int u;
      for (u = 0; u < size(vasNames); u++)
         vapcPNames[u] = vasNames[u].c_str();


      // get total number number of fields
      int nFieldCount = (int)vasNames.size();

      // create MATLAB sub-struct
      int dims[2] = {1, nNodeCount};
      mArray = mxCreateStructArray(2, dims, nFieldCount, &vapcPNames[0]);
      if (!mArray)
         throw Exception("error calling mxCreateStructArray");

      AnsiString as;
      int nField;

      _di_IDOMNode domTmp = snt ? dom->childNodes->item[0] : dom;
      int nNode = 0;
      while (domTmp)
         {
         if (!domTmp->childNodes->length)
            continue;
         _di_IDOMNode domChild = domTmp->childNodes->item[0];
         while (domChild)
            {
            mxSetField( mArray,
                        nNode,
                        AnsiString(domChild->nodeName).c_str(),
                        AnsiString2mxArray(domChild->childNodes->item[0]->nodeValue, LowerCase(domChild->nodeName)=="data"));
            domChild = domChild->nextSibling;
            }
         if (snt == SNT_NONE)
            break;
         domTmp = domTmp->nextSibling;
         nNode++;
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
      AddMATFromXML(pmat, xmlParams, SNT_SUBNODES_ALL);

      // add single value StimulusSequence
      mxArray* mxa = AnsiString2mxArray(GetNodeChildValue(xmlResultNode->GetDOMNode(), "StimulusSequence"), false);
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


