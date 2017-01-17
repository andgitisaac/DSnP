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
    _type(type), _gateId(gateId), _lineNo(lineNo), _simValue(0), _fecGrp(0), _visited(false), _var(0) {}
   virtual ~CirGate() { _faninList.clear(); _fanoutList.clear(); _gateId = 0, _lineNo = 0;}

   #define NEG 0x1 // Use for an inverting I/O

   // Basic access methods
   string getTypeStr() const; // Done
   unsigned getLineNo() const { return _lineNo; } // Done
   unsigned getGateId() const { return _gateId; } // Done
   GateType getType() const { return _type; } // Done
   string getSymbol() const { return _symbol; } // Done
   void setSymbol(string symbol) { _symbol = symbol; } // Done
   void setLineNo(size_t l) { _lineNo = l; } // Done. Use For ResetQQ

   // Simulation access methods
   // void setSimValue(unsigned v) { _simValue = v; } // Done
   // unsigned getSimValue() { return _simValue; } // Done
   void setSimValue(size_t v) { _simValue = v; } // Done
   size_t getSimValue() { return _simValue; } // Done
   void setFecGrp(size_t grp) { _fecGrp = grp; } // Done
   size_t getSFecGrp() const { return _fecGrp; } // Done

   // Fraig access methods
   void setVar(Var& v) { _var = v; } // Done
   Var& getVar() { return _var; } // Done

   // Boolin functions
   virtual bool isAig() const { return _type == AIG_GATE; } // Done   
   bool isUnused() const { return _fanoutList.empty(); } // Done. Maybe Unused.
   bool isVisited() const { return _visited; } // Done

   // Printing functions
   virtual void printGate() const = 0; // Done
   void reportGate() const; // Done
   void reportFanin(int level) const; // Done
   void reportFanout(int level) const; // Done
   void reportOut(int level, bool isInv, int currentLevel) const; // Done
   void reportIn(int level, bool isInv, int currentLevel) const; // Done

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
   void printGateDetail(string& detail) const; // Done
   void resetGateFlag() const { _visited = false; } // Done
   void validAig(string& str, unsigned& aig) const; // Done
   unsigned validAig(vector<string>*, vector<string>*, vector<string>*) const; // Done
   
private:

protected:
  GateType _type;
  unsigned _gateId, _lineNo;
  // unsigned _simValue;  
  size_t _simValue;
  size_t _fecGrp;
  string _symbol;
  mutable bool _visited; // gate has been visited or not
  GateList _faninList, _fanoutList;
  Var _var;
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
