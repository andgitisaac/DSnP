/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    for(size_t i = 0; i < _dfsList.size(); ++i)
        _dfsList[i]->_visited = true; // go through dfs
    for(size_t i = 0; i < _gateVarList.size(); ++i){
        if(_gateVarList[i] && !_gateVarList[i]->isVisited()){
            if(_gateVarList[i]->isAig() || _gateVarList[i]->getType() == UNDEF_GATE){
                for(unsigned j = 0; j < 2; ++j){
                    CirGate* gate = _gateVarList[i]->getFanin(j);
                    if(gate && (gate->isVisited() || gate->getType() == PI_GATE))
                        gate->removeFanout(_gateVarList[i]);
                }
            }
        }
    }
    for(size_t i = 0; i < _gateVarList.size(); ++i){
        if(_gateVarList[i] && !_gateVarList[i]->isVisited()){
            if(_gateVarList[i]->isAig() || _gateVarList[i]->getType() == UNDEF_GATE){
                if(_gateVarList[i]->isAig()) --_A_count;
                cout << "Sweeping: " + _gateVarList[i]->getTypeStr()
                    << '(' << _gateVarList[i]->getGateId() << ") removed..."
                    << endl;
                delete _gateVarList[i];
                _gateVarList[i] = 0;
            }
        }
    }
    flagReset();
    // strashFlag = false;
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    for(size_t i = 0; i < _dfsList.size(); ++i){
        if(_dfsList[i]->getType() == AIG_GATE){            
            // CirGate* in1 = _dfsList[i]->getFanin(0), * in2 = _dfsList[i]->getFanin(1);
            vector<CirGate*> &withPhase = _dfsList[i]->_faninList;
            CirGate* noPhase[2] = {_dfsList[i]->getFanin(0), _dfsList[i]->getFanin(1)};
            string str = "Simplifying: ";
            if(withPhase[0] == withPhase[1]) // A & A = A
                replace(_dfsList[i], noPhase[0], (size_t)withPhase[0] & 1, str);
            else if(noPhase[0] == noPhase[1]) // A & ~A = 0, replace with Const 0.
                replace(_dfsList[i], _gateVarList[0], false, str);
            else if(withPhase[0] == _gateVarList[0] 
                    || withPhase[1] == _gateVarList[0]) // 0 & A = 0
                replace(_dfsList[i], _gateVarList[0], false, str);
            else if(noPhase[0] == _gateVarList[0] 
                    || noPhase[1] == _gateVarList[0]){ // 1 & A = A
                size_t index = (noPhase[0] == _gateVarList[0]) ? 1 : 0;
                replace(_dfsList[i], noPhase[index], (size_t)withPhase[index] & 1, str);
            } 
            else continue;

            _gateVarList[_dfsList[i]->getGateId()] = 0;
            delete _dfsList[i];
            --_A_count;                
        }
    }
    DFSConstruct();
    // strashFlag = false;    
}
/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
// This function replace "new" with "old" one.
// The relationship between new, old, out is:
// new -> old(AIG) -> out.
// inv indicates whether the fanin of old is inverse or not.
void 
CirMgr::replace(CirGate* old, CirGate* tar, bool inv, string& str)
{
    cout << str << tar->getGateId() << " merging "
        << (inv ? "!" : "") << old->getGateId() << "..." << endl;

    // Process Fanin of out
    for(size_t i = 0; i < old->_fanoutList.size(); ++i){
        CirGate* out = old->getFanout(i);
        for(size_t j = 0; j < 2; ++j){
            CirGate* tmp = out->getFanin(j);
            if(tmp == old){
                // tmp = (CirGate*)((size_t)tar | (size_t)(inv ^ (tmp->isInv(j)))); // Rebuild the fanin and phase.
                out->_faninList[j] = (CirGate*)((size_t)tar | (size_t)(inv ^ ((size_t)out->_faninList[j] & 1))); // Rebuild the fanin and phase.
                tar->addFanout(out);
            }
        }       
    }   

    // Process Fanout of new
    for(size_t i = 0; i < 2; ++i){
        CirGate* in = old->getFanin(i);
        for(size_t j = 0; j < in->_fanoutList.size(); ++j){
            if(in->getFanout(j) == old)
                in->removeFanout(j);
        }
        if(in->isUnused() && in->getType() == UNDEF_GATE){
            cout << str << in->getGateId() << " removed..." << endl;
            --_A_count;
            _gateVarList[in->getGateId()] = 0; // remove the gate
            delete in;
        }
    }
}