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

