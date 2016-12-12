/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; } // Done
   void insert(const Data& d) { // Done
        size_t t = size();
        _data.push_back(d);
        while(t > 0){
            size_t p = (t - 1) / 2;
            if(d < _data[p]){
                _data[t] = _data[p];
                t = p;
            }
            else break;
        }
        _data[t] = d;
   }
   void delMin() { delData(0); } // Done
   void delData(size_t i) { // Done
        // Same as delMin in pdf but assign beginning idx as i.
        if(i >= size()) return;
        size_t p = i, t = 2 * (p + 1) - 1, n = size();
        // p, t is the index of parent, left child(thus minus 1), respectively.
        bool sink = false;
        while(t < n){
           if(t != n-1) // right child exist
              if(_data[t+1] < _data[t]) ++t; // go to the smallest child
           if(_data.back() < _data[t] || _data.back().getLoad() == _data[t].getLoad()) 
              break;
           _data[p] = _data[t];
           sink = true;
           p = t, t = 2 * (p + 1) - 1;
        }
        _data[p] = _data.back();
        _data.resize(n-1);

        if(!sink){
           p = i;
           while(p > 0){
              t = (p + 1) / 2 - 1;
              if(_data[t] < _data[p] || _data[t].getLoad() == _data[p].getLoad()) 
                 break;
              swap(_data[p], _data[t]);
              p = t;
           }
        }
   }

   void swap(Data& x, Data& y) {
      Data tmp = x;
      x = y;
      y = tmp;
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
