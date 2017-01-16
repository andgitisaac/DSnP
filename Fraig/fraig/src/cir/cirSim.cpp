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

static bool
compareVec(CirGate* a, CirGate* b)
{
    a = (CirGate*)((size_t)a & ~(size_t)(0x1));
    b = (CirGate*)((size_t)b & ~(size_t)(0x1));
    return((a->getGateId() < b->getGateId()));
}

static bool
compareList(FECGroup &a, FECGroup &b)
{
    CirGate* x = (CirGate*)((size_t)a.getHead() & ~(size_t)(0x1));
    CirGate* y = (CirGate*)((size_t)b.getHead() & ~(size_t)(0x1));
    return((x->getGateId() < y->getGateId()));
}
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void    
CirMgr::randomSim()
{    
    _fecGrps.clear();
    unsigned bitRead = 0, fail = 0, maxFail = magicNum(_I_count + _A_count); // Must be size dependent.
    list<FECGroup> &fecGrps = _fecGrps;
    cout << "MAX_FAIL = " << maxFail << endl;

    // srand(time(NULL));
    HashMap<SimValue, FECGroup> newFecGrps(_dfsList.size());

    // newFecGrps.insert(SimValue(0), FECGroup(_gateVarList[0])); // insert CONST

    // First time to simulate and generate FEC group.
    simEachGate(newFecGrps, bitRead, true);

    // Collect Valid FECGroup    
    for(HashMap<SimValue, FECGroup>::iterator iter = newFecGrps.begin(); iter != newFecGrps.end(); ++iter){
        if((*iter).second.size() > 1) fecGrps.push_back((*iter).second);
    }

    if(_simLog) writeLog();

    while(fail < maxFail){
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
    cout << "Total # of FEC Groups:" << fecGrps.size() << endl;

    // FECGroup sorting    
    for(list<FECGroup>::iterator iter = fecGrps.begin(); iter != fecGrps.end(); ++iter){
        FECGroup* grp = &(*iter);
        std::sort(grp->_gateList.begin(), grp->_gateList.end(), compareVec);
        for(FECGroup::iterator it = grp->begin(); it != grp->end(); ++it){
            (*it)->_fecGrp = (size_t)grp | (size_t)(it).isInv();
        }
    }
    fecGrps.sort(compareList);

    cout << endl << bitRead << " patterns simlated." << endl;
    strashFlag = false;
    // printFECPairs();    
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    list<FECGroup> &fecGrps = _fecGrps;
    unsigned bitRead = 0;
    size_t pattern[_I_count];
    string str;
    memset(pattern, 0, sizeof(pattern)); // init with 0
    HashMap<SimValue, FECGroup> newFecGrps(_dfsList.size());

    while(patternFile >> str){
        if(str.size() != _I_count){
            cout << "Error: Pattern(" << str << ") length(" << str.length() << ")" 
                << "does not match the number of inputs(" << _I_count 
                << ") in a circuit!!" << endl;
            return;
        }
        for(size_t i = 0; i < _I_count; ++i){
            if(str[i] != '0' && str[i] != '1'){
                cout << "Error: Pattern(" << str << ") contains a non-0/1 characters(\'" 
                << str[i]  << "\')." << endl;
                return ;
            }
            else{
                pattern[i] = (pattern[i] << 1) | (str[i] - '0'); // push file into pattern                
            }
        }
        
        // Simulation Begins when read 32 bit
        if(++bitRead % 32 == 0){
            // for(size_t i = 0; i < _I_count; ++i) cout << bitset<32>(pattern[i]) << endl;
            // cerr << "sim First..." << endl;
            // printFECPairs(); 
            if (bitRead == 32){ // Simulation First Time
                // newFecGrps.insert(SimValue(0), FECGroup(_gateVarList[0]));
                simEachGate(newFecGrps, bitRead, true, &pattern[0]);
                if(_simLog) writeLog();

                // Collect Valid FECGroup
                HashMap<SimValue, FECGroup>::iterator iter = newFecGrps.begin();
                for(; iter != newFecGrps.end(); ++iter){
                    if((*iter).second.size() > 1) fecGrps.push_back((*iter).second);
                }
            }
            else{
                // cerr << "sim Once..." << endl;           
                simEachGate(newFecGrps, bitRead, false, &pattern[0]);
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
            }
        }
    }

    // pattern not mutiple of 32
    if(bitRead % 32){
        simEachGate(newFecGrps, bitRead, false, &pattern[0]);
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
    }
    cout << "Total # of FEC Groups:" << fecGrps.size() << endl;

    // FECGroup sorting    
    for(list<FECGroup>::iterator iter = fecGrps.begin(); iter != fecGrps.end(); ++iter){
        FECGroup* grp = &(*iter);
        std::sort(grp->_gateList.begin(), grp->_gateList.end(), compareVec);
        for(FECGroup::iterator it = grp->begin(); it != grp->end(); ++it){
            (*it)->_fecGrp = (size_t)grp | (size_t)(it).isInv();
        }
    }
    fecGrps.sort(compareList);

    cout << endl << bitRead << " patterns simlated." << endl;
    strashFlag = false;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::simEachGate(HashMap<SimValue, FECGroup> &newFecGrps, unsigned& bitRead, const bool& firstTime, size_t* const & file)
{
    if(file){
        for(size_t i = 0, n = 0; i < _dfsList.size() && n < _I_count; ++i){
            if(_dfsList[i]->getType() == PI_GATE){
                unsigned simtest = file[n];
                // cerr << "PI " << _dfsList[i]->getGateId() << ":              " 
                //     << bitset<32>(simtest) << endl;
                _dfsList[i]->setSimValue(simtest);
                ++n;
            }
        }
    }
    for(size_t i = 0; i < _dfsList.size(); ++i){
        if((!file) && _dfsList[i]->getType() == PI_GATE){
            // assign or generate value to input
            unsigned simtest = (unsigned)((rnGen(INT_MAX) << 16) | rnGen(INT_MAX));
            _dfsList[i]->setSimValue(simtest);
            continue;
        }
        else if(_dfsList[i]->getType() == CONST_GATE) _dfsList[i]->setSimValue(0);
        else if(_dfsList[i]->getType() == AIG_GATE){
            CirGate* g1 = _dfsList[i]->getFanin(0), * g2 = _dfsList[i]->getFanin(1);
            unsigned v1 = g1->getSimValue(), v2 = g2->getSimValue();

            if(g1->getType() == UNDEF_GATE) g1->setSimValue(0);
            if(g2->getType() == UNDEF_GATE) g2->setSimValue(0);
            
            if(_dfsList[i]->isInv(0)) v1 = ~v1;
            if(_dfsList[i]->isInv(1)) v2 = ~v2;

            // cerr << "input1 of AIGID " << _dfsList[i]->getGateId() << ": " << bitset<32>(v1) << endl;
            // cerr << "input2 of AIGID " << _dfsList[i]->getGateId() << ": " << bitset<32>(v2) << endl;

            _dfsList[i]->setSimValue(v1 & v2);

            // _dfsList[i]->setSimValue(~(unsigned)(0x0)); // change size_t to unsigned matter?
            
            // for(unsigned j = 0; j < 2; ++j){
            //     if(_dfsList[i]->isInv(j)) _dfsList[i]->_simValue &= ~(_dfsList[i]->getFanin(j)->_simValue);
            //     else _dfsList[i]->_simValue &= _dfsList[i]->getFanin(j)->_simValue;
            // }

            // cerr << "Output of AIGID " << _dfsList[i]->getGateId()  << ": " 
            //     << bitset<32>(_dfsList[i]->_simValue) << endl;

            if(firstTime) _dfsList[i]->setFecGrp(0);
        }
        else continue;

        if(firstTime) splitFECGroup(_dfsList[i], newFecGrps);
    }
    if(!file) bitRead += 32;
}

void
CirMgr::splitFECGroup(CirGate* gate, HashMap<SimValue, FECGroup> &newFecGrps)
{
    FECGroup* fecGrp = 0;
    if(newFecGrps.query(SimValue(gate->getSimValue()), fecGrp)){
        fecGrp->add(gate, 0); // FEC
    }
    else if(newFecGrps.query(SimValue(~(gate->getSimValue())), fecGrp)){
        fecGrp->add(gate, 1); // IFEC
    }
    else{ // Create new group
        FECGroup newFecGrp;
        newFecGrp.add(gate, 0);
        newFecGrps.insert(SimValue(gate->getSimValue()), newFecGrp);
    }
}

void
CirMgr::writeLog()
{
    size_t inputPattern[_I_count], outputPattern[_O_count];
    for(unsigned i = 0; i < _I_count; ++i) inputPattern[i] = getGate(tmpIn[i] / 2)->getSimValue();
    for(unsigned i = 0; i < _O_count; ++i){
        outputPattern[i] = getGate(tmpOut[i] / 2)->getSimValue();
    }
    for(unsigned i = 0; i < 32; ++i){
        for(unsigned j = 0; j < _I_count; inputPattern[j] >>=1, ++j)
            *_simLog << (inputPattern[j] & 1);
        *_simLog << " ";
        for(unsigned j = 0; j < _O_count; outputPattern[j] >>=1, ++j)
            *_simLog << (outputPattern[j] & 1);
        *_simLog << endl;
    } 
}