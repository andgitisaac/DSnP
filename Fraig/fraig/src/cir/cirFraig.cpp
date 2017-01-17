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
    // cout << "Before: ";
    // for(size_t i = 0; i < tmpOut.size(); ++i)
    //     cout << tmpOut[i] << " ";
    // cout << endl;

    // if(strashFlag){
    //     cout << "Error: strash operation has already been performed!!" << endl;
    //     return;
    // }

    HashMap<HashKey, CirGate*> h(getHashSize(_M_count));
    for(size_t i = 0; i < _dfsList.size(); ++i){
        CirGate* gate = _dfsList[i];
        if(gate->getType() != AIG_GATE) continue;
        HashKey k = keyGen(gate);
        if(!h.insert(k, gate)){
            CirGate* merge;
            h.query(k, merge);
            string str = "Strashing: ";
            replace(gate, merge, false, str);
            _gateVarList[_dfsList[i]->getGateId()] = 0;
            delete _dfsList[i];
            --_A_count;
        }
    }
    DFSConstruct();
    // cout << "After : ";
    // for(size_t i = 0; i < tmpOut.size(); ++i)
    //     cout << tmpOut[i] << " ";
    // cout << endl;
}

void
CirMgr::fraig()
{
    SatSolver solver;    
    bool result;
    int cnt = 0; // max pattern collect to simulate once
    string str = "Fraig: ";

    solver.initialize();
    createCNF(solver);

    // while(!_fecGrps.empty()){
    //     for(list<FECGroup>::iterator its = _fecGrps.begin(); its != _fecGrps.end(); ++its){
    //         FECGroup &fecGrp = (*its);
    //         for(FECGroup::iterator i = (++fecGrp.begin()); i != fecGrp.end(); ++i){
    //             result = solve((*fecGrp.begin())->getVar(), fecGrp.begin().isInv(),
    //                             (*i)->getVar(), i.isInv(), solver);
    //             solveMSG((*fecGrp.begin()), fecGrp.begin().isInv(), 
    //                     (*i), i.isInv()); // print solve message
    //             if(result){ // Need collect pattern...
    //                 collectPattern(cnt, solver);
    //             }
    //         }
    //     }
    // }

    for(list<FECGroup>::iterator its = _fecGrps.begin(); its != _fecGrps.end(); ++its){
        FECGroup &fecGrp = (*its);
        FECGroup::iterator i = fecGrp.begin();
        if((*fecGrp.begin())->getType() == CONST_GATE){
            for(++i; i != fecGrp.end(); ++i){
                result = solve((*fecGrp.begin())->getVar(), fecGrp.begin().isInv(),
                   (*i)->getVar(), !i.isInv(), solver);
                solveMSG((*fecGrp.begin()), fecGrp.begin().isInv(), 
                    (*i), i.isInv()); // print solve message
                if(!result){
                    replace(*i, *fecGrp.begin(), i.isInv(), str);
                    _gateVarList[(*i)->getGateId()] = 0;
                    delete (*i);
                    --_A_count;
                }
                // else{} // NEED??????
            }
        }
        else{
            for(; i != fecGrp.end(); ++i){
                FECGroup::iterator cur = i;
                ++cur;
                while(cur != fecGrp.end()){
                    result = solve((*i)->getVar(), i.isInv(),
                                    (*cur)->getVar(), cur.isInv(), solver);
                    solveMSG((*fecGrp.begin()), fecGrp.begin().isInv(), 
                            (*i), i.isInv()); // print solve message
                    if(!result){
                        replace(*cur, *i, (cur.isInv() ^ i.isInv()), str);
                        _gateVarList[(*cur)->getGateId()] = 0;
                        delete (*cur);
                        --_A_count;
                        cur = fecGrp.erase(cur);
                    }
                    else SATSplit(cur, fecGrp, solver);
                }
            }
        }
    }
    DFSConstruct();
    strash();
    _fecGrps.clear();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void CirMgr::createCNF(SatSolver& solver)
{
    for(unsigned i = 0; i < _dfsList.size(); ++i){
        Var var = solver.newVar();
        if(_dfsList[i]->getType() == PO_GATE){
            if(_dfsList[i]->getFanin(0)){
                // If the only AIG to PO exists
                // Var of POs should be the same as its single fanin.
                _dfsList[i]->setVar(_dfsList[i]->getFanin(0)->getVar());
            }
            else _dfsList[i]->setVar(var); // Should not enter here...                
        }
        // else if(_dfsList[i]->getType() == CONST_GATE){ // How to deal with CONST 0???
        //     _dfsList[i]->setVar(var);
        //     solver.assertProperty(var, false);
        // }
        else if(_dfsList[i]->getType() == AIG_GATE){
            _dfsList[i]->setVar(var);
            CirGate* g1 = _dfsList[i]->getFanin(0), * g2 = _dfsList[i]->getFanin(1);
            // Construct CNF of AIG
            solver.addAigCNF(var, g1->getVar(), _dfsList[i]->isInv(0), g2->getVar(), _dfsList[i]->isInv(1));
        }
        else _dfsList[i]->setVar(var);
    }
}

bool CirMgr::solve(Var& g_var, bool g_phase, Var& f_var, bool f_phase, SatSolver& solver)
{
    Var newV = solver.newVar();
    solver.addXorCNF(newV, g_var, g_phase, f_var, f_phase);

    solver.assumeRelease();
    solver.assumeProperty(newV, true);
    solver.assumeProperty(_gateVarList[0]->getVar(), false);
    return (solver.assumpSolve());
}

void CirMgr::solveMSG(CirGate* g, bool g_inv, CirGate* f, bool f_inv)
{
    if(g->getType() == CONST_GATE){
        cout << "prove " << ((f_inv) ? ("!") : ("")) << f->getGateId() 
            << " = 0..." << flush << char(13);    
    }
    else{
        cout << "prove (" << ((g_inv) ? ("!") : ("")) << g->getGateId() << ", " 
            << ((f_inv) ? ("!") : ("")) << f->getGateId() << ")..."
            << flush << char(13); 
    }
}

void CirMgr::collectPattern(int& cnt, SatSolver& solver)
{
    for(unsigned i = 0; i < _I_count; ++i){
        cout << solver.getValue((getGate(tmpIn[i] / 2)->getVar())) << " ";
        getGate(tmpIn[i] / 2)->_simValue = 
            ((getGate(tmpIn[i] / 2)->_simValue << 1) | (solver.getValue((getGate(tmpIn[i] / 2)->getVar()))));
    }
    ++cnt;
    if(cnt == 64){

    }
}

void CirMgr::SATSplit(FECGroup::iterator& cur, FECGroup& fecGrp, SatSolver& solver)
{
    FECGroup grp;
    bool newCurV, flag = false;
    bool curV = ((cur.isInv()) ? (!solver.getValue((*cur)->getVar())) : solver.getValue((*cur)->getVar()));
    FECGroup::iterator newCur = fecGrp.end();

    ++cur;
    while(cur != fecGrp.end()){
        newCurV = solver.getValue((*cur)->getVar());
        if (cur.isInv()) newCurV = !newCurV;
        if (newCurV == curV){
            grp.add(*cur, cur.isInv());
            cur = fecGrp.erase(cur);
        }
        else{
            if(!flag){
                newCur = cur;
                flag = true;
            }
            ++cur;
        }
    }
    cur = newCur;
    if (grp.size() > 1) _fecGrps.push_back(grp);
}