/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include <cmath>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(size_t i = 0; i < r.size(); i++){
    (r[i] != INT_MAX) ? (os << r[i]) : (os << ".");
    if(i != r.size()-1) cout << " ";
   }
   cout << endl;
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for(size_t i = 0; i < t.nRows(); i++){
       for(size_t j = 0; j < t.nCols(); j++){
         (t[i][j] != INT_MAX) ? (os << setw(6) << right << t[i][j])
                             : (os << setw(6) << right << ".");
       }
       cout << endl;
     }
    cout << endl;
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
   
   bool endOfLine = false, peekComma = true;

   DBRow* Row = new DBRow();
   while(true){
      if(endOfLine){
        endOfLine = false;
        t.addRow(*Row);
        delete Row;
        Row = new DBRow();
      }
      
      if (peekComma){
        if (ifs.peek() == ','){
           ifs.ignore();          
           Row->addData(INT_MAX);
         }
        else if (ifs.peek() == '\r' || ifs.peek() == '\n'){
          ifs.ignore();
          Row->addData(INT_MAX);
          endOfLine = true;
        }
        else{
          peekComma = false;
          int item;
          ifs >> item;
          Row->addData(item);
        }          
      }
      else if (ifs.peek() == ','){          
        ifs.ignore();
        peekComma = true;
      }
      else if (ifs.peek() == '\r'){
        ifs.ignore();
        if(ifs.peek() == '\r' || ifs.peek() == EOF){
          t.addRow(*Row);
          break;
        }
        peekComma = true;
        endOfLine = true;
      }
      else{
        peekComma = false;
        int item;
        ifs >> item;
        Row->addData(item);
      }
    }
    
    delete Row;
    ifs.clear();
    return ifs;  
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   _data.erase(_data.begin() + c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   //       if the data in r1 < r2, return true
  vector<size_t>::const_iterator it = _sortOrder.begin();
  // why it must use const_iterator???????
  for( ; it != _sortOrder.end(); ++it){
      if(r1[*it] < r2[*it]) return true;
      else if(r1[*it] == r2[*it]) continue; 
      else return false;     
  }  
  return false;  
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
  for(size_t i = 0; i < _table.size(); i++) _table[i].reset();
  vector<DBRow> tmp;
  tmp.swap(_table);

}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   // seems that we don't have to deal with the case which d is shorter
   // than nRows()
  for(size_t i = 0; i < nRows(); i++){
      _table[i].addData(d[i]);
  }
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
  _table[c].reset();
  _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
  	bool allNull = true;
  	float ans = INT_MIN;
  	for(size_t i = 0; i < nRows(); i++)
  	   	if(_table[i][c] != INT_MAX){
      		allNull = false;
      		if(_table[i][c] > ans)
      	  	ans = _table[i][c];
      	}
 	return (allNull) ? NAN : ans;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
  	bool allNull = true;
  	float ans = INT_MAX;
  	for(size_t i = 0; i < nRows(); i++)
      	if(_table[i][c] != INT_MAX){
      		allNull = false;
        	if(_table[i][c] < ans)
          	ans = _table[i][c];
  	 	}
  	return (allNull) ? NAN : ans;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
	bool allNull = true;
  	float sum = 0;
  	for(size_t i = 0; i < nRows(); i++)
      	if(_table[i][c] != INT_MAX){
      		allNull = false;
        	sum += _table[i][c];
      	}
  	return (allNull) ? NAN : sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
  vector<float> v;
  for(size_t i = 0; i < nRows(); i++)
    if(_table[i][c] != INT_MAX)
      v.push_back(_table[i][c]); 
  ::sort(v.begin(), v.end());
  vector<float>::iterator it = unique(v.begin(), v.end());
  v.erase(it, v.end());
  return int(v.size());
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
 	float sum = 0, cnt = 0;
  	for(size_t i = 0; i < nRows(); i++)
      	if(_table[i][c] != INT_MAX){        
          	sum += _table[i][c];
          	cnt++;
      	}
    return (cnt) ? (sum / cnt) : NAN;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   // using object as comp
   // std::sort (myvector.begin(), myvector.end(), myobject);
  ::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
  for(size_t i = 0; i < nRows(); i++){
      (_table[i][c] != INT_MAX) ? (cout << _table[i][c]) : (cout << ".");
      if(i != nRows()-1) cout << " ";
  }
  cout << endl;
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}
