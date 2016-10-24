/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;   
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
  vector<string> options;
  if(!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;
  if(options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, ""); 
  bool doArr = false;
  int numObj, sizeArr = 0;
  size_t arrPos;

  // Three valid type: (1) mtn 2 ,(2) mtn 2 -a 3 ,(3) mtn -a 2 3
  for(size_t i = 0; i < options.size(); ++i){
    if(myStrNCmp("-Array", options[i], 2) == 0){
      arrPos = i;
      doArr = true;
    }
  }
  if(!doArr){
    if(!myStr2Int(options[0], numObj) || numObj < 1)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    if(options.size() != 1)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
    mtest.newObjs(numObj);
  }
  else{
    if(arrPos == 0){ // case: mtn -a 2 3
      if(options.size() == 1) // case: mtn -a
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
      if(!myStr2Int(options[1], sizeArr) || sizeArr < 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      if(options.size() < 3)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
      if(!myStr2Int(options[2], numObj) || numObj < 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      if(options.size() > 3)
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]); 
    }
    else if(arrPos == 1){ // case: mtn 2 -a 3
      if(!myStr2Int(options[0], numObj) || numObj < 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
      if(options.size() < 3)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
      if(!myStr2Int(options[2], sizeArr) || sizeArr < 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
      if(options.size() > 3)
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
    }
    else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    mtest.newArrs(numObj, sizeArr);
  }
  return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
  vector<string> options;
  if(!CmdExec::lexOptions(option, options)) return CMD_EXEC_ERROR;
  if(options.empty()) return CmdExec::errorOption(CMD_OPT_MISSING, "");

  int index;
  size_t arrPos, actionPos;
  bool doArr = false, doRandom = false, doAlready = false;

  // Valid case: (1) mtd -i/-r 3 ,(2) mtd -i/-r 3 -a ,(3) mtd -a -i/-r 3  
  for(size_t i = 0; i < options.size(); ++i){
    if(myStrNCmp("-Array", options[i], 2) == 0){
      if(doArr) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      doArr = true;
      arrPos = i;
    }
    else if(myStrNCmp("-Index", options[i], 2) == 0){
      if(doAlready) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);      
      if(i == options.size())
        return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
      actionPos = i++; // Since index must follow after -i/-r
      if(!myStr2Int(options[i], index) || index < 0)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      doAlready = true;
    }
    else if(myStrNCmp("-Random", options[i], 2) == 0){
      if(doAlready) return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      if(i == options.size())
        return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
      actionPos = i++; // Since index must follow after -i/-r
      if(!myStr2Int(options[i], index) || index < 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      doAlready = true;
      doRandom = true;
    }
    else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
  }
  if(!doAlready) return CmdExec::errorOption(CMD_OPT_MISSING, "");
  else if(doArr){
    if(arrPos != 0 && actionPos != 0)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[0]);
    else if(arrPos == 0 && actionPos > 1)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[0]);
    else if(actionPos == 0 && arrPos > 2)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);

    if(doRandom){
      if(mtest.getArrListSize() == 0){
        cerr << "Size of array list is 0!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[actionPos]);
      }
      for(int i = 0; i < index; ++i)
        mtest.deleteArr(rnGen(mtest.getArrListSize()));
    }
    else{
      if(index >= int(mtest.getArrListSize())){
        cerr << "Size of array list (" << mtest.getArrListSize() 
            << ") is <= " << index << "!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[actionPos + 1]);
      }
      mtest.deleteArr(index);
    }
  }
  else{
    if(actionPos != 0)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[0]);
    else if(options.size() > 2)
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);

    if(doRandom){
      if(mtest.getObjListSize() == 0){
        cerr << "Size of object list is 0!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[actionPos]);
      }
      for(int i = 0; i < index; ++i)
        mtest.deleteObj(rnGen(mtest.getObjListSize()));
    }
    else{
      if(index >= int(mtest.getObjListSize())){
        cerr << "Size of object list (" << mtest.getObjListSize() 
            << ") is <= " << index << "!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[actionPos + 1]);
      }
      mtest.deleteObj(index);
    }
  }
  return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


