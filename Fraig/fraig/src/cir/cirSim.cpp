/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <climits>
#include <bitset>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static inline unsigned
magicNum(unsigned n)
{
    return floor(2 * log2((double)n));
}
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void    
CirMgr::randomSim()
{
    unsigned bitRead = 0, fail = 0, maxFail = magicNum(_I_count + _A_count); // Must be size dependent.
    list<FECGroup> &fecGrps = _fecGrps;
    cout << "MAX_FAIL = " << maxFail << endl;

    srand(time(NULL));

    HashMap<SimValue, FECGroup> newFecGrps(_dfsList.size());

    newFecGrps.insert(SimValue(0), FECGroup(_gateVarList[0])); // insert CONST

    // First time to simulate and generate FEC group.
    simEachGate(newFecGrps, bitRead, true);

    // Collect Valid FECGroup
    HashMap<SimValue, FECGroup>::iterator iter = newFecGrps.begin();
    for(; iter != newFecGrps.end(); ++iter){
        if((*iter).second.size() > 1) fecGrps.push_back((*iter).second);
    }

    if(_simLog) writeLog();

    while(fail < maxFail){
        cout << "Total # of FEC Groups:" << fecGrps.size() << endl;

        simEachGate(newFecGrps, bitRead, false);
        if(_simLog) writeLog();
        // Update FECGroup
        size_t nGrps = fecGrps.size(), i = 0;
        list<FECGroup>::iterator iter = fecGrps.begin();
        for(; iter != fecGrps.end() && i < nGrps; ++i){
            FECGroup fecGrp = (*iter);
            newFecGrps.init(fecGrp.size());
            FECGroup::iterator it = fecGrp.begin();
            for(; it != fecGrp.end(); ++it){
                splitFECGroup((*it), newFecGrps);
            }
            iter = fecGrps.erase(iter); // Remove this Group

            // Collect Valid FECGroup
            HashMap<SimValue, FECGroup>::iterator iter = newFecGrps.begin();
            for(; iter != newFecGrps.end(); ++iter){
                if((*iter).second.size() > 1) fecGrps.push_back((*iter).second);
            }
        }

        if(fecGrps.size() == nGrps) ++fail;        
    }

    // FECGroup sorting
    // Not Complete!!!
}

void
CirMgr::fileSim(ifstream& patternFile)
{
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::simEachGate(HashMap<SimValue, FECGroup> &newFecGrps, unsigned& bitRead, const bool& firstTime)
{
    for(size_t i = 0; i < _dfsList.size(); ++i){
        if(_dfsList[i]->getType() == PI_GATE){
            // assign or generate value to input
            // size_t simtest = (size_t)((rnGen(INT_MAX) << 16) | rnGen(INT_MAX)); // WTF. simValue not conmposed of 0 and 1...
            size_t simtest = (size_t)((size_t)rand() << 32) + ((size_t)rand());
            cout << std::bitset<64>(simtest)  << endl;
            _dfsList[i]->setSimValue(simtest);
            continue;
        }
        else if(_dfsList[i]->getType() == CONST_GATE) _dfsList[i]->setSimValue(0);
        else if(_dfsList[i]->getType() == AIG_GATE){
            CirGate* g1 = _dfsList[i]->getFanin(0), * g2 = _dfsList[i]->getFanin(1);
            // size_t v1 = g1->getSimValue(), v2 = g2->getSimValue();
            
            // if(_dfsList[i]->isInv(0)) v1 = !v1;
            // if(_dfsList[i]->isInv(1)) v2 = !v2;
            // _dfsList[i]->setSimValue(v1 & v2);

            _dfsList[i]->setSimValue(~(size_t)(0x0));
            if(g1->getType() == UNDEF_GATE) g1->setSimValue(0);
            if(g2->getType() == UNDEF_GATE) g2->setSimValue(0);
            for(unsigned j = 0; j < 2; ++j){
                if(_dfsList[i]->isInv(j)) _dfsList[i]->_simValue &= ~(_dfsList[i]->getFanin(j)->_simValue);
                else _dfsList[i]->_simValue &= _dfsList[i]->getFanin(j)->_simValue;
            }

            if(firstTime) _dfsList[i]->setFecGrp(0);
        }
        else continue;

        if(firstTime) splitFECGroup(_dfsList[i], newFecGrps);
    }
    bitRead += 64;
}

void
CirMgr::splitFECGroup(CirGate* gate, HashMap<SimValue, FECGroup> &newFecGrps)
{
    FECGroup* fecGrp = 0; // = 0 matter??

    cout << std::bitset<64>(gate->getSimValue()) << endl;

    if(newFecGrps.query(SimValue(gate->getSimValue()), *fecGrp)){
        cerr << "A" << endl;
        fecGrp->add(gate, 0); // FEC
    }
    else if(newFecGrps.query(SimValue(~(gate->getSimValue())), *fecGrp)){
        cerr << "B" << endl;
        fecGrp->add(gate, 1); // IFEC
    }
    else{ // Create new group
        cerr << "C" << endl;
        FECGroup newFecGrp;
        newFecGrp.add(gate, 0);
        newFecGrps.insert(SimValue(gate->getSimValue()), newFecGrp);
    }
}

void
CirMgr::writeLog()
{
    size_t inputPattern[_I_count], outputPattern[_O_count];
    for(unsigned i = 0; i < _I_count; ++i) inputPattern[i] = getGate(tmpIn[i])->getSimValue();
    for(unsigned i = 0; i < _O_count; ++i) outputPattern[i] = getGate(tmpOut[i])->getSimValue();
    for(unsigned i = 0; i < 64; ++i){
        for(unsigned j = 0; j < _I_count; inputPattern[j] >>=1, ++j)
            *_simLog << (inputPattern[j] & 1);
        cout << " ";
        for(unsigned j = 0; j < _O_count; outputPattern[j] >>=1, ++j)
            *_simLog << (outputPattern[j] & 1);
        cout << endl;
    }   
}