/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

// Global functions
string
unsigned2Str(unsigned n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

/**************************************/
/*   class CirGate member functions   */
/**************************************/

// Basic access methods
string
CirGate::getTypeStr() const
{
    switch(_type) {
        case CONST_GATE: return "CONST";
        case PI_GATE:    return "PI";
        case PO_GATE:    return "PO";
        case UNDEF_GATE: return "UNDEF";
        case AIG_GATE:   return "AIG";
        case TOT_GATE:   return "TOT";
        default:         return "UNDEF";
    }
}

// I/O access functions
CirGate*
CirGate::getFanin(size_t i) const
{
    if(i >= _faninList.size()) return 0; // Use vector.at() instead?
    return (CirGate*)((size_t)_faninList[i] & ~size_t(NEG));
}

void
CirGate::addFanin(CirGate* gate, size_t phase)
{
    gate = (CirGate*)((size_t)gate + phase);
    _faninList.push_back(gate);
}

CirGate*
CirGate::getFanout(size_t i) const
{
    if(i >= _fanoutList.size()) return 0;
    return _fanoutList[i];
}

void
CirGate::addFanout(CirGate* gate)
{
    _fanoutList.push_back(gate);
}

// Inheritance printing functions
void
CirGate::printGateDetail(string &detail, int& count) const
{
    for(size_t i = 0; i < _faninList.size(); ++i){
        detail.append(" ");
        if(isInv(i)) detail.append("!");
        if(getFanin(i)->getType() == UNDEF_GATE) detail.append("*");
        detail.append(unsigned2Str(getFanin(i)->getGateId()));
    }
    if(!_symbol.empty()) detail.append(" (" + _symbol + ")");
    cout << "[" << count++ << "] "
        << setw(4) << left << getTypeStr() << detail << endl;
}

void
ConstGate::printGate(int& count) const
{
    if(flag) return;
    // for(size_t i = 0; i < _faninList.size(); ++i)
    //     getFanin(i)->printGate(count);

    string detail = "0";
    printGateDetail(detail, count);
    flag = true;    
}

void
PIGate::printGate(int& count) const
{
    if(flag) return;
    for(size_t i = 0; i < _faninList.size(); ++i)
        getFanin(i)->printGate(count);

    string detail = unsigned2Str(_gateId);
    printGateDetail(detail, count);
    flag = true;
}

void
POGate::printGate(int& count) const
{
    if(flag) return;
    for(size_t i = 0; i < _faninList.size(); ++i)
        getFanin(i)->printGate(count);

    string detail = unsigned2Str(_gateId);
    printGateDetail(detail, count);
    flag = true;
}

void
AigGate::printGate(int& count) const
{
    if(flag) return;
    for(size_t i = 0; i < _faninList.size(); ++i)
        getFanin(i)->printGate(count);

    string detail = unsigned2Str(_gateId);
    printGateDetail(detail, count);
    flag = true;
}

// Printing functions
void
CirGate::reportGate() const
{
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

