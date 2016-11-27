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

void
CirGate::validAig(string& str, unsigned& aig) const
{
    if (flag) return;
    for (unsigned i = 0; i < _faninList.size(); ++i)
        getFanin(i)->validAig(str, aig);
    if (_type == AIG_GATE) {
        flag = true;
        stringstream ss;
        ss << _gateId * 2
            << " " << getFanin(0)->getGateId() * 2 + isInv(0)
            << " " << getFanin(1)->getGateId() * 2 + isInv(1)
            << endl;
        str += ss.str();
        ++aig;
    }
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
    stringstream ss;
    string str;
    cout << "==================================================" << endl;
    ss << "= " << getTypeStr() << "(" << getGateId() << ")";
    if(!getSymbol().empty()) ss << "\"" << getSymbol() << "\"";
    if(getType() == UNDEF_GATE) ss << ", line " << getLineNo(); 
    else ss << ", line " << getLineNo()+1; 
    str = ss.str();
    cout << setw(49) << left << str << "=" << endl;
    cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   // depthSearchReport(level, true, 0);
   reportIn(level, false, 0);
   cirMgr->flagReset();
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   // depthSearchReport(level, false, 0);
   // reportOut(level, 0);
   reportOut(level, false, 0);
   cirMgr->flagReset();
}

// void
// CirGate::reportOut(int level, int currentLevel) const
// {
//     if(currentLevel > level) return;
//     const GateList* nextLevel = &_fanoutList;

//     if(!(*nextLevel).empty()) flag = true;
//     if(currentLevel == 0){
//         cout << getTypeStr() << " " << getGateId() << endl;
//         flag = true;
//         ++currentLevel;
//     }

//     for(size_t i = 0; i < (*nextLevel).size(); ++i){
//         for(int k = 0; k < currentLevel; ++k) cout << "  ";
//         if((*nextLevel)[i]->isInv(i)) cout << "!";
//         cout << (*nextLevel)[i]->getTypeStr() << " " << (*nextLevel)[i]->getGateId();
//         if((*nextLevel)[i]->visited()) cout << " (*)" << endl;
//         else{
//             cout << endl;
//             (*nextLevel)[i]->reportOut(level, currentLevel+1);
//         }
//     }
// }

void CirGate::reportOut(int level, bool isInv, int currentLevel) const {
    for (int i = 0; i < currentLevel; ++i) cout << "  ";
    if (isInv) cout << '!';
    cout << getTypeStr() << ' ' << getGateId();
    if (level == 0) cout << endl;
    else if(flag) cout << " (*)" << endl;
    else{
        cout << endl;
        if(!_fanoutList.empty()) flag = true;
        for (size_t i = 0; i < _fanoutList.size(); ++i){
            const CirGate* gate = getFanout(i);
            bool inv = false;
            unsigned j = 0;
            while (true) {
                CirGate* g = gate->getFanin(j);
                if (g == 0) break;
                if (this == g){
                    inv = gate->isInv(j);
                    break;
                }
                ++j;
            }
            getFanout(i)->reportOut(level-1, inv, currentLevel+1);
        }
    }
}

void 
CirGate::reportIn(int level, bool isInv, int currentLevel) const
{
    for(int k = 0; k < currentLevel; ++k) cout << "  ";
    if(isInv) cout << "!";
    cout << getTypeStr() << " " << getGateId();
    if(level == 0) cout << endl;
    else if(flag)cout << " (*)" << endl;
    else{
        cout << endl;
        if(!_faninList.empty()) flag = true;
        for(size_t i = 0; i < _faninList.size(); ++i){
            getFanin(i)->reportIn(level-1,  this->isInv(i), currentLevel+1);
        }
    }
}
