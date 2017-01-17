/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class FECGroup
{
    friend class CirMgr;

public:
    FECGroup() {}
    FECGroup(CirGate* gate) { _gateList.push_back(gate); }
    ~FECGroup() { clear(); }

    CirGate* getHead() { return _gateList[0]; }

    #define NEG 0x1

    class iterator
    {
        friend class CirMgr;
        friend class FECGroup;

    public:
        iterator() {}
        iterator(std::vector<CirGate*>::iterator i) { _it = i; }

        CirGate* operator * () { return (CirGate*)((size_t)(*_it) & ~size_t(NEG)); }
        const CirGate* operator* () const { return (CirGate*)((size_t)(*_it) & ~(size_t)(0x1)); }

        bool isInv() const { return ((size_t)(*_it) & (size_t)(0x1)); }

        iterator& operator++ () { _it++; return (*this); }
        iterator operator ++ (int) { iterator iter=(*this); ++(*this); return iter; }

        iterator& operator = (const iterator& i) { _it = i._it; return (*this); }

        bool operator == (const iterator& i) const { return (_it == i._it); }
        bool operator != (const iterator& i) const { return !(*this == i); }

    private:
        vector<CirGate*>::iterator _it; 
        // Since it's a vector, maybe not overload its iterator...
    };

    iterator begin() { return iterator(_gateList.begin()); }
    iterator end() { return iterator(_gateList.end()); }
    size_t size() const { return _gateList.size(); }
    iterator erase(iterator& i) { i._it = _gateList.erase(i._it); return i; }
    void clear() { _gateList.clear(); }

    FECGroup& add(const CirGate* gate, const bool &inv)
    {      
        _gateList.push_back((CirGate*)((size_t)(gate) | (size_t)inv));
        return(*this);
    }

private:
    vector<CirGate*> _gateList;
};

class SimValue
{
public:
    SimValue() {}
    // SimValue(unsigned val = 0) : _value(val) {}
    SimValue(size_t val = 0) : _value(val) {}
    ~SimValue() {}

    // unsigned operator () () const { return _value; }
    size_t operator () () const { return _value; }
    bool operator == (const SimValue& v) const { return (_value == v._value); }

private:
    // unsigned _value;
    size_t _value;
};

class CirMgr
{
public:
   CirMgr() : flag(false) {} // Done
   ~CirMgr(); // Done

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { return (gid < _M_count + _O_count + 1) ? (_gateVarList[gid]) : (0); } // Done

   // Member functions about circuit construction
   bool readCircuit(const string&); // Done
   CirGate* linkToExistGateOrUndefGate(unsigned gid); // Done

   // Member functions about circuit optimization
   void sweep(); // Done
   void optimize(); // Done

   // Member functions about simulation
   void randomSim(); // Done
   void fileSim(ifstream&); // Done
   void setSimLog(ofstream *logFile) { _simLog = logFile; } // Done

   // Member functions about fraig
   void strash(); // Done
   void printFEC() const; // Where??
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const; // Done
   void printNetlist() const; // Done. With tmpOut Bug!!!!!!!! Fix!!!!
   void printPIs() const; // Done
   void printPOs() const; // Done
   void printFloatGates() const; // Done
   void printFECPairs(); // Done
   void writeAag(ostream&) const; // Done
   void writeGate(ostream&, CirGate*) const; // Done

   void DFSConstruct(); // Done
   void UpdateDFSList(CirGate* gate) { _dfsList.push_back(gate); } // Done. Used for DFSConsturct
   void flagReset() const; // Done

private:
   ofstream           *_simLog;
   vector<string> tmpSymbol;
   IdList tmpIn, tmpOut, tmpAND;
   GateList _gateVarList, _dfsList;
   unsigned _M_count, _I_count, _L_count, _O_count, _A_count;
   bool flag; // Ture if the circuit net list has been constructed.
   // bool strashFlag; // Ture if already do strash.
   list<FECGroup> _fecGrps;

   // private function of Optimization
   void replace(CirGate*, CirGate*, bool, string&); // Done
   
   // private function of Simulation
   // void simEachGate(HashMap<SimValue, FECGroup>& , unsigned&, const bool& , unsigned* const & = 0); // Done
   void simEachGate(HashMap<SimValue, FECGroup>& , unsigned&, const bool& , size_t* const & = 0); // Done
   void splitFECGroup(CirGate*, HashMap<SimValue, FECGroup>&); // Done
   void writeLog(); // Done

   // private function of fraig
   void createCNF(SatSolver&);
   bool solve(Var& , bool, Var& , bool, SatSolver&);
   void solveMSG(CirGate*, bool, CirGate*, bool);
   void collectPattern(int& , SatSolver&);
   void SATSplit(FECGroup::iterator&, FECGroup&, SatSolver&);
};

#endif // CIR_MGR_H
