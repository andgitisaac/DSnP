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
    for(unsigned i = 0; i < _dfsList.size(); ++i)
        _dfsList[i]->_visited = true; // go through dfs
    for(unsigned i = 0; i < _gateVarList.size(); ++i){
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
    for(unsigned i = 0; i < _gateVarList.size(); ++i){
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
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
