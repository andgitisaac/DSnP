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
// static char buf[1024];
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
CirMgr::~CirMgr()
{
    for(unsigned i = 0; i < _gateVarList.size(); i++)
        delete _gateVarList[i];
    tmpAND.clear(); tmpIn.clear(); tmpOut.clear();
}

CirGate*
CirMgr::linkToExistGateOrUndefGate(unsigned gid)
{
    CirGate* gate = getGate(gid);
    if(!gate){
        // cerr << "Gate Var:" << gid << " Set UNDEF" << endl;
        _gateVarList[gid] = new UndefGate(gid);
        gate = getGate(gid);
    }
    return gate;
}

bool
CirMgr::readCircuit(const string& fileName)
{
   flag = false;
   ifstream ifs(fileName.c_str(), ifstream::in);
   if(!ifs.is_open()){
        cout << "Cannot open design " << fileName << "!!" << endl;
        return false;
   }

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
            // cerr << "last coef:" << coef.back() << endl;
        }        
   }
   if(coef.size() != 5){
        cout << "aag first line format is wrong or missing" << endl;
        return false;
   }
   // cerr << "lineNo:" << lineNo << endl << endl;
   ++lineNo;

   _M_count = coef[0]; _I_count = coef[1]; _L_count = coef[2];
   _O_count = coef[3]; _A_count = coef[4];

   _gateVarList.resize(1 + _M_count + _O_count, 0); // CONST + M + O

   // Add ConstGate
   _gateVarList[0] = new ConstGate;

   // Parse inputs
   // IdList tmpIn;
   tmpIn.resize(_I_count, 0);
   for(unsigned i = 0; i < _I_count; ++i){
        getline(ifs, line);
        if(line.empty()){
            cout << "line " << lineNo+1 << "is empty" << endl;
            return false;
        }
        // cerr << "input:" << line << "." << endl;
        if(!isdigit(line[0]) || !isdigit(line[line.length()-1])){
            cout << "line " << lineNo+1 << "format invalid" << endl;
            return false;
        }
        myStrGetTok(line, tok);
        id = myStr2Unsigned(tok);
        // cerr << "input ID:" << id  << " ,lineNo:" << lineNo << endl;

        tmpIn[i] = id;
        _gateVarList[id / 2] = new PIGate(id/2, lineNo);
        ++lineNo;
   }
   // cerr << endl;
   
   // Parse outputs into vector tmpOut
   // IdList tmpOut;
   tmpOut.resize(_O_count, 0);

   for(unsigned i = 0; i < _O_count; ++i){
        getline(ifs, line);
        if(line.empty()){
            cout << "line " << lineNo+1 << "is empty" << endl;
            return false;
        }
        // cerr << "outputs:" << line << "." << endl;
        if(!isdigit(line[0]) || !isdigit(line[line.length()-1])){
            cout << "line " << lineNo+1 << "format invalid" << endl;
            return false;
        }
        myStrGetTok(line, tok);
        id = myStr2Unsigned(tok);
        tmpOut[i] = id;
        // cerr << "output ID:" << id  << " ,lineNo:" << lineNo << endl;
        
        ++lineNo;
   }
   // cerr << endl;

   // Parse AND GATEs, save fanouts of AND in tmpAND[3k]
   // IdList tmpAND; // outputs ID of kth AND save in tmpAND[3k + 1] and tmpAND[3k + 2]
   tmpAND.resize(3 * _A_count, 0);

   for(unsigned i = 0; i < _A_count; ++i){
        getline(ifs, line);
        if(line.empty()){
            cout << "line " << lineNo+1 << "is empty" << endl;
            return false;
        }
        if(!isdigit(line[0])){
            cout << "line " << lineNo+1 << "Invalid : begin with space" << endl;
            return false;
        }

        begin = myStrGetTok(line, tok);
        id = myStr2Unsigned(tok);
        // cerr << "AND ID:" << id  << " ,lineNo:" << lineNo << endl;

        _gateVarList[id / 2] = new AigGate(id/2, lineNo);
        tmpAND[3*i] = id;
        for(size_t j = 0; j < 2; ++j){
            // Have not HANDLE PARSE ERROR yet...
            begin = myStrGetTok(line, tok, begin);
            id = myStr2Unsigned(tok);
            // cerr << "AND FanOut " << j << " ID:" << id << endl;
            tmpAND[3*i + j + 1] = id;
        }

        ++lineNo;
   }
   // cerr << endl;

   // Construct graph, connenct Fanouts and AIG_GATE
   // cerr << "Connenct Fanouts and AIG_GATE" << endl;
   for(unsigned i = 0; i < _A_count; ++i){
        unsigned fo1ID = tmpAND[3*i + 1], fo2ID = tmpAND[3*i + 2];
        // cerr << "AIG ID:" << tmpAND[3*i] 
            // << " ,Fanout1:" << fo1ID
            // << " ,Fanout2:" << fo2ID << endl;

        CirGate* AIG = getGate(tmpAND[3*i] / 2);
        CirGate* fo1 = linkToExistGateOrUndefGate(fo1ID / 2);
        CirGate* fo2 = linkToExistGateOrUndefGate(fo2ID / 2);

        AIG->addFanin(fo1, fo1ID % 2);
        AIG->addFanin(fo2, fo2ID % 2);
        fo1->addFanout(AIG);
        fo2->addFanout(AIG);
   }
   // cerr << endl;

   // Construct PO
   // cerr << "Connenct POs" << endl;
   for(unsigned i = 0; i < _O_count; ++i){
        unsigned poID = tmpOut[i];
        // cerr << "PO ID:" << poID << endl;
        CirGate* PO = new POGate( _M_count + i + 1, _I_count + 2);
        CirGate* gate = linkToExistGateOrUndefGate(poID / 2);
        PO->addFanin(gate, poID % 2);
        gate->addFanout(PO);

        _gateVarList[_M_count + i + 1] = PO;
   }
   // cerr << endl;

   // Parse Symbol
   // cerr << "Set Symbol" << endl;
   while(getline(ifs, line)){
        if(line.empty()){
            cout << "line " << lineNo+1 << "is empty" << endl;
            return false;
        }
        if(line == "c") break; // Have not handled c with trailing white spaces yet
        bool doInput;
        if(line[0] == 'i') doInput = true;
        else if(line[0] == 'o') doInput = false;
        else{
            cout << "line " << lineNo+1 << "with Invalid I/O name: " << line[0] << endl;
            return false;
        }

        begin = myStrGetTok(line, tok); // i.g. i0 reset / o1 done
        id = myStr2Unsigned(tok.substr(1)); // symbol start from 0
        begin = myStrGetTok(line, tok, begin); // Need handle symbol name contain white spaceQQ
        string symbol = tok;
        // Have not HANDLE PARSE ERROR yet...

        if(doInput){ // Have not set symbol yet...
            id = tmpIn[id];
            getGate(id / 2)->setSymbol(symbol);
            // cerr << "set symbol of input id " << id << " as:" << symbol << endl;
        }
        else{
            getGate(_M_count + id + 1)->setSymbol(symbol);
            // cerr << "set symbol of output id " << _M_count + id + 1 << " as:" << symbol << endl;
        }
        ++lineNo;
   }
   // cerr << endl;
   
   cerr << "Read file finished" << endl;

   flag = true;
   ifs.close();
   line.clear(); tok.clear(); coef.clear();
   // tmpIn.clear(); tmpOut.clear(); tmpAND.clear();

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
    if(!flag){
        cout << "Error: circuit is not yet constructed!!" << endl;
        return;
    }
    cout << endl
        << "Circuit Statistics" << endl
        << "==================" << endl
        << "  PI" << setw(12) << right << _I_count << endl
        << "  PO" << setw(12) << right << _O_count << endl
        << "  AIG" << setw(11) << right << _A_count << endl
        << "------------------" << endl
        << "  Total" << setw(9) << right << _I_count + _O_count + _A_count << endl;
}

void
CirMgr::printNetlist() const
{
    if(!flag){
        cout << "Error: circuit is not yet constructed!!" << endl;
        return;
    }
    int count = 0;
    cout << endl;
    for (unsigned i = 0; i < _M_count + _O_count + 1; ++i) {
        CirGate *gate = getGate(i);
        if (!gate) continue;
        if (gate->getType() == PO_GATE)
            gate->printGate(count);
    }
    flagReset();
}

void
CirMgr::printPIs() const
{
    if(!flag){
        cout << "Error: circuit is not yet constructed!!" << endl;
        return;
    }
    cout << "PIs of the circuit:";
    for(unsigned i = 1; i < _I_count + 1; ++i)
        cout << " " << _gateVarList[i]->getGateId();
    cout << endl;
}

void
CirMgr::printPOs() const
{
    if(!flag){
        cout << "Error: circuit is not yet constructed!!" << endl;
        return;
    }
    cout << "POs of the circuit:";
    for(unsigned i = _M_count + 1; i < _M_count + _O_count + 1; ++i)
        cout << " " << _gateVarList[i]->getGateId();
    cout << endl;
}

void
CirMgr::printFloatGates() const
{
    if(!flag){
        cout << "Error: circuit is not yet constructed!!" << endl;
        return;
    }

    IdList floating, notUsed;
    for(unsigned i = 0; i < _M_count + _O_count + 1; ++i){
        CirGate* gate = getGate(i);
        if(!gate) continue;
        // Traversing floatings.
        if(gate->getType() == AIG_GATE &&
                (gate->getFanin(0)->getType() == UNDEF_GATE ||
                gate->getFanin(1)->getType() == UNDEF_GATE) )
            floating.push_back(gate->getGateId());
        else if(gate->getType() == PO_GATE &&
                gate->getFanin(0)->getType() == UNDEF_GATE)
            floating.push_back(gate->getGateId());
        // Traversing Unused.
        if((gate->getType() == AIG_GATE || gate->getType() == PI_GATE) &&
                !gate->getFanout(0))
            notUsed.push_back(gate->getGateId());
    }

    if(!floating.empty()){
        cout << "Gates with floating fanin(s):";
        for (size_t i = 0; i < floating.size(); ++i) cout << ' ' << floating[i];
        cout << endl;
    }
    if(!notUsed.empty()){
        cout << "Gates defined but not used  :";
        for (size_t i = 0; i < notUsed.size(); ++i) cout << ' ' << notUsed[i];
        cout << endl;
    }
    floating.clear(); notUsed.clear();
}

void
CirMgr::writeAag(ostream& outfile) const
{
    if(!flag){
        cout << "Error: circuit is not yet constructed!!" << endl;
        return;
    }
    string str = "";
    unsigned aig = 0;
    for (unsigned i = _M_count + 1; i <= _M_count + _O_count + 1; ++i) {
        CirGate *gate = getGate(i);
        if (gate && gate->getType() == PO_GATE)
            gate->validAig(str, aig);
    }
    flagReset();

    // Write head
    outfile << "aag " << _M_count << " " << _I_count 
            << " " << _L_count << " " << _O_count
            << " " << aig << endl;

    // Write Inputs
    for(size_t i = 0; i < tmpIn.size(); ++i)
        outfile << tmpIn[i] << endl;
    // Write Outputs
    for(size_t i = 0; i < tmpOut.size(); ++i)
        outfile << tmpOut[i] << endl;
    // Write AIG
    outfile << str;

    // Write Symbol
    for (size_t i = 0; i < _I_count; ++i) {
        string s = _gateVarList[i+1]->getSymbol();
        if (!s.empty())    outfile << 'i' << i << ' ' << _gateVarList[i+1]->getSymbol() << endl;
    }
    for (size_t i = _M_count + 1; i < _M_count + _O_count + 1; ++i) {
        if (!_gateVarList[i] || _gateVarList[i]->getSymbol() == "") continue;
        outfile << 'o' << i - _M_count - 1<< ' ' << _gateVarList[i]->getSymbol() << endl;
    }
    outfile << "c" << endl << "AAG output by Andgit Isaac Peng" << endl;
    str.clear();
}

void 
CirMgr::flagReset() const 
{
    for (unsigned i = 0; i < _M_count + _O_count + 1; ++i){
        CirGate *gate = getGate(i);
        if(gate) gate->resetGateFlag();
    }
}
