//------------------------------------------------------------------------------
/// \file SWTools_Shared.cpp
/// \author Berg
/// \brief Implementation of shared Tools for AudioSpike (used in AudioSpikeMATLib 
/// as well)
///
/// Project AudioSpike
/// Module  AudioSpike.exe
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

#include "SWTools_Shared.h"
#include <math.h>
#include <algorithm>

#pragma package(smart_init)

// local prototypes
void trim(std::string& str, char cTrim);
void ParseDoubleValues( std::vector<double >& rvd,
                        UnicodeString us,
                        UnicodeString usName,
                        char szDelimiter
                        );
void ParseIntValuesChar( std::vector<int >& rvi,
                        const char* lpcsz,
                        UnicodeString usName,
                        char szDelimiter,
                        bool bUnique,
                        bool bPositive);
                        


//------------------------------------------------------------------------------
/// removes brackets [] from passed string 
//------------------------------------------------------------------------------
void RemoveBrackets(UnicodeString &rus)
{
   if (!rus.Length())
      return;
   if (rus[1] == L'[')
      rus = rus.SubString(2, rus.Length());
   if (!rus.Length())
      return;
   if (rus[rus.Length()] == L']')
      rus = rus.SubString(1, rus.Length()-1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes trailing delimiter from passed string. Default delimiter is ','
//------------------------------------------------------------------------------
void RemoveTrailingDelimiter(UnicodeString &us, wchar_t szDelimiter)
{
   if (us.Length() && (us[us.Length()] == szDelimiter))
      us = us.SubString(1, us.Length()-1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// removes leading delimiter from passed string.
//------------------------------------------------------------------------------
void RemoveLeadingDelimiter(UnicodeString &us, wchar_t szDelimiter)
{
   if (us.Length() && (us[1] == szDelimiter))
      us = us.SubString(2, us.Length());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// tool function triming an std::string in place
//------------------------------------------------------------------------------
void trim(std::string& str, char cTrim)
{
  std::string::size_type pos1 = str.find_first_not_of(cTrim);
  std::string::size_type pos2 = str.find_last_not_of(cTrim);
  str = str.substr(pos1 == std::string::npos ? 0 : pos1,
         pos2 == std::string::npos ? str.length() - 1 : pos2 - pos1 + 1);
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
/// calls const char version of ParseValues. Default delimiter is ','
//--------------------------------------------------------------------------------
void ParseValues(TStrings *psl, UnicodeString us, char szDelimiter)
{
   AnsiString as = us;
   ParseValues(psl, as.c_str(), szDelimiter);
}
//--------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a delimited string list into passed TStringList. Default delimiter is ','
//------------------------------------------------------------------------------
void ParseValues(TStrings *psl, const char* lpcsz, char szDelimiter)
{
   psl->Clear();

   std::string str = lpcsz;
   std::string strTmp;
   std::string::size_type pos;
   while (1)
      {
      pos = str.find_first_of(szDelimiter);
      if (pos == str.npos)
         strTmp = str;
      else
         strTmp = str.substr(0, pos);
      trim(strTmp, '"');
      if (!!strTmp.length())
         psl->Add(strTmp.c_str());
      if (pos == str.npos)
         break;
      str = str.erase(0, pos+1);
      }

}
//--------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a delimited string list into passed vector of doubles. Passed usName
/// only used for generating an error message
//------------------------------------------------------------------------------
void ParseDoubleValues( std::vector<double >& rvd,
                        UnicodeString us,
                        UnicodeString usName,
                        char szDelimiter
                        )
{
   rvd.clear();

   std::string str = AnsiString(us).c_str();
   std::string strTmp;
   std::string::size_type pos;
   double d;
   while (1)
      {
      pos = str.find_first_of(szDelimiter);
      if (pos == str.npos)
         strTmp = str;
      else
         strTmp = str.substr(0, pos);
      trim(strTmp, '"');
      if (!TryStrToDouble(strTmp.c_str(), d))
         throw Exception("'" + usName + "' contains invalid value (not a double)");
      rvd.push_back(d);
      if (pos == str.npos)
         break;
      str = str.erase(0, pos+1);
      }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// tries to call ParseMLVector with try/catch returning success/failure in bool
//------------------------------------------------------------------------------
bool TryParseMLVector(UnicodeString us, vved &rvved)
{
   try
      {
      rvved = ParseMLVector(us, "");
      return true;
      }
   catch(...)
      {
      }
   return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// parses a MATLAB vector/matrix into vector of double valarrays (vved)
//------------------------------------------------------------------------------
vved ParseMLVector(UnicodeString us, UnicodeString usName)
{
   RemoveBrackets(us);
   TStringList *pslRows = new TStringList();
   vved vvedReturn;
   vved vvedTmp;
   try
      {
      // rows (!) are ';' delimited
      ParseValues(pslRows, us, ';');
      if (!pslRows->Count)
         return vvedReturn;
      vvedTmp.resize((unsigned int)pslRows->Count);
      unsigned int n;
      for (n = 0; n < (unsigned int)pslRows->Count; n++)
         {
         ParseDoubleValues(vvedTmp[n], pslRows->Strings[(int)n], usName, ' ');
         if (n > 0 && vvedTmp[n].size() != vvedTmp[n-1].size())
            throw Exception("'" + usName + "' contains rows with different number of values: " + us);
         }
      vvedReturn.resize(vvedTmp[0].size());

      // transpose it
      unsigned int nRow, nCol;
      for (nRow = 0; nRow < vvedTmp.size(); nRow++)
         {
         for (nCol = 0; nCol < vvedTmp[nRow].size(); nCol++)
            {
            vvedReturn[nCol].push_back(vvedTmp[nRow][nCol]);
            }
         }
      }
   __finally
      {
      TRYDELETENULL(pslRows);
      }
   return vvedReturn;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a delimited string list into passed vector of ints
//------------------------------------------------------------------------------
void ParseIntValuesChar( std::vector<int >& rvi,
                        const char* lpcsz,
                        UnicodeString usName,
                        char szDelimiter,
                        bool bUnique,
                        bool bPositive)
{
   rvi.clear();

   std::string str = lpcsz;
   std::string strTmp;
   std::string::size_type pos;
   int n;
   while (1)
      {
      pos = str.find_first_of(szDelimiter);
      if (pos == str.npos)
         strTmp = str;
      else
         strTmp = str.substr(0, pos);
      trim(strTmp, '"');
      if (!TryStrToInt(strTmp.c_str(), n))
         throw Exception("'" + usName + "' contains invalid value (not an integer)");
      if (bPositive && n < 0)
         throw Exception("'" + usName + "' contains invalid integer value (must be >= 0)");
      if (bUnique)
         {
         if ( std::find(rvi.begin(), rvi.end(), n) != rvi.end() )
            throw Exception("'" + usName + "' contains doublette integer value");
         }
      rvi.push_back(n);
      if (pos == str.npos)
         break;
      str = str.erase(0, pos+1);
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a delimited UnicodeString into passed vector of ints
//------------------------------------------------------------------------------
void  ParseIntValues(   std::vector<int >& rvi,
                        UnicodeString us,
                        UnicodeString usName,
                        char szDelimiter,
                        bool bUnique,
                        bool bPositive)
{
   rvi.clear();
   RemoveBrackets(us);
   AnsiString as = us;
   if (!as.Length())
      return;
   ParseIntValuesChar(rvi, as.c_str(), usName, szDelimiter, bUnique, bPositive);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// tries to read a ChildValue from an XML node by name
//------------------------------------------------------------------------------
UnicodeString  GetXMLValue(_di_IXMLNode &rNode, UnicodeString usName)
{
   UnicodeString us;
   try
      {
      us = rNode->ChildValues[usName];
      }
   catch (...)
      {
      us = "";
      }
   return us;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// checks if passed string is double
//------------------------------------------------------------------------------
bool IsDouble(AnsiString s)
{
   char *endptr = NULL;
   s = Trim(s);
   strtod(s.c_str(), &endptr);
   if (*endptr != NULL || s.Length()==0)
      return false;
   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a double to a string using fix DecimalSeparator '.'
//------------------------------------------------------------------------------
AnsiString DoubleToStr(double val)
{
   FormatSettings.DecimalSeparator = '.';
   return FloatToStr((long double)val);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// converts a string to a double
//------------------------------------------------------------------------------
double StrToDouble(AnsiString s)
{
   char *endptr = NULL;
   double value;
   s = Trim(s);
   value = strtod(s.c_str(), &endptr);
   if (*endptr != NULL || s.Length()==0)
      throw Exception("not a double");
   return value;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// tries to convert a string to a double and returns success/failure in bool
//------------------------------------------------------------------------------
bool TryStrToDouble(AnsiString s, double &d)
{
   char *endptr = NULL;
   s = Trim(s);
   d = strtod(s.c_str(), &endptr);
   if (*endptr != NULL || s.Length()==0)
      return false;
   return true;
}
//------------------------------------------------------------------------------


