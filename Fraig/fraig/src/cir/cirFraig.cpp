/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
HashKey
keyGen(CirGate* gate){
    size_t a = (gate->getFanin(0)->getGateId() << 1) + gate->isInv(0);
    size_t b = (gate->getFanin(1)->getGateId() << 1) + gate->isInv(1);
    return (a < b) ? HashKey(a, b) : HashKey(b, a);
}
/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash() 
{
    // Bug: merging floating gate still exist by floating reportQQ
    // Update: fix it!!(_gateVarList[] = 0 and delete _dfsList[])
    HashMap<HashKey, CirGate*> h(getHashSize(_M_count));
    for(size_t i = 0; i < _dfsList.size(); ++i){
        CirGate* gate = _dfsList[i];
        if(gate->getType() != AIG_GATE) continue;
        HashKey k = keyGen(gate);
        if(!h.insert(k, gate)){
            CirGate* merge;
            h.query(k, merge);
            string str = "Strashing: ";
            replace(gate, merge, false, str); //Strash05.aag cirw is wrong!!!
            _gateVarList[_dfsList[i]->getGateId()] = 0;
            delete _dfsList[i];
            --_A_count;
        }
    }
    DFSConstruct();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
