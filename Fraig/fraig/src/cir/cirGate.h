/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;
  
// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
  friend class CirMgr;

public:
   CirGate(GateType type, unsigned gateId = 0, unsigned lineNo = 0):
    _type(type), _gateId(gateId), _lineNo(lineNo), _value(0), _visited(false) {}
   virtual ~CirGate() { _faninList.clear(); _fanoutList.clear(); }

   #define NEG 0x1 // Use for an inverting I/O

   // Basic access methods
   string getTypeStr() const; // Done
   unsigned getLineNo() const { return _lineNo; } // Done
   unsigned getGateId() const { return _gateId; } // Done
   GateType getType() const { return _type; } // Done
   string getSymbol() const { return _symbol; } // Done
   void setSymbol(string symbol) { _symbol = symbol; } // Done

   // Boolin functions
   virtual bool isAig() const { return _type == AIG_GATE; } // Done   
   bool isUnused() const { return _fanoutList.empty(); } // Done. Maybe Unused.
   bool isVisited() const { return _visited; } // Done

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void reportOut(int level, bool isInv, int currentLevel) const;
   void reportIn(int level, bool isInv, int currentLevel) const;

   // List functions
   void DFSConstruct(); // Done

   // I/O access functions
   CirGate* getFanin(size_t i) const; // Done
   void addFanin(CirGate* gate, size_t phase = 0); // Done, phase = 1 if inverting
   CirGate* getFanout(size_t i) const; // Done
   void addFanout(CirGate* gate); // Done
   void removeFanin(size_t); // Done
   void removeFanin(CirGate*, bool); // Done
   void removeFanout(size_t); // Done
   void removeFanout(CirGate*); // Done
   bool isInv(size_t i) const { return (size_t)_faninList[i] & NEG; } // Done
   void printGateDetail(string& detail) const;
   void resetGateFlag() const { _visited = false; } // Done
   void validAig(string& str, unsigned& aig) const; // Done
   
private:

protected:
  GateType _type;
  unsigned _gateId, _lineNo, _value;
  string _symbol;
  mutable bool _visited; // gate has been visited or not
  GateList _faninList, _fanoutList;
};

class ConstGate : public CirGate
{
public:
    ConstGate() : CirGate(CONST_GATE) {};
    ~ConstGate() {}
    void printGate() const;
};

class PIGate : public CirGate
{
public:
    PIGate(unsigned i, unsigned l) : CirGate(PI_GATE, i, l) {};
    ~PIGate() {}    
    void printGate() const;
};

class POGate : public CirGate
{
public:
    POGate(unsigned i, unsigned l) : CirGate(PO_GATE, i, l) {};
    ~POGate() {}
    void printGate() const;
};

class UndefGate : public CirGate
{
public:
    UndefGate(unsigned i) : CirGate(UNDEF_GATE, i) {};
    ~UndefGate() {}
    void printGate() const{ return; };
};

class AigGate : public CirGate
{
public:
    AigGate(unsigned i, unsigned l) : CirGate(AIG_GATE, i, l) {};
    ~AigGate() {}
    void printGate() const;
};

#endif // CIR_GATE_H
