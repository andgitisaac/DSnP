/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

unsigned
myStr2Unsigned(const string& str)
{
   unsigned num = 0;
   size_t i = 0;
   for (; i < str.size(); ++i) {
         num *= 10;
         num += unsigned(str[i] - '0');
   }
   return num;
}

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printing, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream ifs(fileName.c_str(), ifstream::in);
   if(!ifs.is_open()) return false;

   // Rule: aag*M*I*L*O*A\n (* means space)
   string line, tok;
   vector<unsigned> coef;
   unsigned id;
   size_t begin;

   // Parse aag MILOA
   getline(ifs, line);

   if(line.empty()){
        cout << "First line is empty" << endl;
        return false;
   }

   for(int i = 0; i < 6; ++i){
        if(i == 0){ // extract head "aag"            
            begin = myStrGetTok(line, tok);
            if(tok != "aag"){
                cout << "aag is wrong or missing" << endl;
                return false;
            }
        }
        else{ // extract MILOA
            begin = myStrGetTok(line, tok, begin);
            id = myStr2Unsigned(tok);
            coef.push_back(id);
            cerr << "last coef:" << coef.back() << endl;
        }        
   }
   if(coef.size() != 5){
        cout << "aag first line format is wrong or missing" << endl;
        return false;
   }
   cerr << "lineNo:" << lineNo << endl << endl;
   ++lineNo;

   _gateVarList.resize(1 + coef[0] + coef[3], 0); // CONST + M + O

   // Add ConstGate
   _gateVarList[0] = new ConstGate;

   // Parse inputs
   for(unsigned i = 0; i < coef[1]; ++i){
        getline(ifs, line);
        if(line.empty()){
            cout << "line " << lineNo+1 << "is empty" << endl;
            return false;
        }
        cerr << "input:" << line << "." << endl;
        if(!isdigit(line[0]) || !isdigit(line[line.length()-1])){
            cout << "line " << lineNo+1 << "format invalid" << endl;
            return false;
        }
        myStrGetTok(line, tok);
        id = myStr2Unsigned(tok);
        cerr << "input ID:" << id  << " ,lineNo:" << lineNo << endl;

        _gateVarList[id / 2] = new PIGate(id/2, lineNo);
        ++lineNo;
   }
   cerr << endl;
   
   // Parse outputs into vector tmpOut
   vector<unsigned> tmpOut;
   tmpOut.resize(coef[3], 0);

   for(unsigned i = 0; i < coef[3]; ++i){
        getline(ifs, line);
        if(line.empty()){
            cout << "line " << lineNo+1 << "is empty" << endl;
            return false;
        }
        cerr << "outputs:" << line << "." << endl;
        if(!isdigit(line[0]) || !isdigit(line[line.length()-1])){
            cout << "line " << lineNo+1 << "format invalid" << endl;
            return false;
        }
        myStrGetTok(line, tok);
        id = myStr2Unsigned(tok);
        cerr << "output ID:" << id  << " ,lineNo:" << lineNo << endl;
        
        ++lineNo;
   }
   cerr << endl;

   // Parse AND GATEs
   













   cerr << "finished" << endl;

   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
}

void
CirMgr::printNetlist() const
{
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
