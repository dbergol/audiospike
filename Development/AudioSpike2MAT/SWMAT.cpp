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
   SNT_SUBNODES_MAX           ///< parse subnodes by names subnode wt the most fields
};
//------------------------------------------------------------------------------

//  local prototypes
mxArray* XMLValue2mxArray(_di_IXMLNode xml, AnsiString asFieldName);
void     AddMATFromXML(  MATFile *pmat, _di_IXMLNode xml, TSubNodeType snt, AnsiString asNodeName = "");


//---------------------------------------------------------------------------
/// class for stroring field names of an XML node and a char pointer list
/// to the names
//---------------------------------------------------------------------------
class MLStructFields
{
   public:
      vas            m_vasNames;
      MLStructFields(_di_IXMLNode xml);
      const char**   GetPNames();
   private:
      vapc           m_vapcPNames;
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/// constructor. retreives all childnode names, adds them to m_vasNames and
/// adds char* to each name to m_vapcPNames
//---------------------------------------------------------------------------
MLStructFields::MLStructFields(_di_IXMLNode xml)
{
   unsigned int nCount = (unsigned int)xml->ChildNodes->Count;
   m_vapcPNames.resize(nCount);

   // loop through children
   unsigned int n;
   for (n = 0; n < nCount; n++)
      {
      _di_IXMLNode xmlChild = xml->ChildNodes->Nodes[(int)n];
      // add name to AnsiString vector
      m_vasNames.push_back(AnsiString(xmlChild->GetNodeName()).c_str());
      // add pointer to first char to char* valarray
      m_vapcPNames[n] = m_vasNames[n].c_str();
      }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// returns char** to the char pointer array
//---------------------------------------------------------------------------
const char**   MLStructFields::GetPNames()
{
   return &m_vapcPNames[0];
}
//---------------------------------------------------------------------------


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

               
      // create field names and char pointer list for mxCreateStructArray either
      // node itself or from subnode 
      int nNode; 
      _di_IXMLNode xmlTmp = xml;
      if (snt != SNT_NONE)
         {
         xmlTmp = xml->ChildNodes->Nodes[0];
         if (snt == SNT_SUBNODES_MAX)
            {
            int nMax = 0;
            int nCur;
            for (nNode = 0; nNode < nNodeCount; nNode++)
               {
               nCur = xml->ChildNodes->Nodes[nNode]->ChildNodes->Count;
               if (nCur > nMax)
                  {
                  nMax = nCur;
                  xmlTmp = xml->ChildNodes->Nodes[nNode];
                  }
               }
            }
         }
      
      
      MLStructFields ml(xmlTmp);

      // find number of fields
      int nFieldCount = (int)ml.m_vasNames.size();

      // create MATLAB sub-struct
      int dims[2] = {1, nNodeCount};
      mArray = mxCreateStructArray(2, dims, nFieldCount, ml.GetPNames());
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
            if (xmlChild->ChildNodes->Count <= nField)
               break;
            mxSetFieldByNumber(mArray, nNode, nField, XMLValue2mxArray(xmlChild, ml.m_vasNames[(unsigned int)nField]));
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
      AddMATFromXML(pmat, xmlParams, SNT_SUBNODES_MAX);

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


