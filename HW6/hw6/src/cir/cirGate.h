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

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(GateType type, unsigned gateId = 0, unsigned lineNo = 0):
    _type(type), _gateId(gateId), _lineNo(lineNo), flag(false) {}
   virtual ~CirGate() {}

   #define NEG 0x1 // Use for an inverting I/O

   // Basic access methods
   string getTypeStr() const; // Done

   unsigned getLineNo() const { return _lineNo; } // Done
   void setLineNo(unsigned lineNo) { _lineNo = lineNo; } // Done. Maybe unsed!

   unsigned getGateId() const { return _gateId; } // Done
   void setGateId(unsigned id) { _gateId = id; } // Done. Maybe unsed!

   GateType getType() const { return _type; } // Done
   void setType(GateType type) { _type = type; } // Done. Maybe unsed!

   string getSymbol() const { return _symbol; } // Done. Maybe unused!
   void setSymbol(string symbol) { _symbol = symbol; } // Done

   // Printing functions
   virtual void printGate(int& count) const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   // I/O access functions
   CirGate* getFanin(size_t i) const; // Done
   void addFanin(CirGate* gate, size_t phase = 0); // Done. phase = 1 if inverting
   CirGate* getFanout(size_t i) const; // Done
   void addFanout(CirGate* gate); // Done

   bool isInv(size_t i) const { return (size_t)_faninList[i] & NEG; } // Done

   void printGateDetail(string& detail, int& count) const;
   void resetGateFlag() const { flag = false; }

private:
        

protected:
    // WTF
    GateType _type;
    unsigned _gateId;
    unsigned _lineNo;
    string _symbol;
    mutable bool flag; // gate has been visited or not
    GateList _faninList;
    GateList _fanoutList;
};

class ConstGate : public CirGate
{
public:
    ConstGate() : CirGate(CONST_GATE) {};
    ~ConstGate() {}
    void printGate(int& count) const;
};

class PIGate : public CirGate
{
public:
    PIGate(unsigned i, unsigned l) : CirGate(PI_GATE, i, l) {};
    ~PIGate() {}    
    void printGate(int& count) const;
};

class POGate : public CirGate
{
public:
    POGate(unsigned i, unsigned l) : CirGate(PO_GATE, i, l) {};
    ~POGate() {}
    void printGate(int& count) const;
};

class UndefGate : public CirGate
{
public:
    UndefGate(unsigned i) : CirGate(UNDEF_GATE, i) {};
    ~UndefGate() {}
    void printGate(int& count) const{ return; };
};

class AigGate : public CirGate
{
public:
    AigGate(unsigned i, unsigned l) : CirGate(AIG_GATE, i, l) {};
    ~AigGate() {}
    void printGate(int& count) const;
};

#endif // CIR_GATE_H
