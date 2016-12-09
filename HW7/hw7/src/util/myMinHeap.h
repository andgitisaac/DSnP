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

   // TODOd
   const Data& min() const { return _data[0]; } // Done
   void insert(const Data& d) { // Done
        size_t t = size();
        _data.push_back(d);
        while(t > 0){
            size_t p = (t - 1) / 2;
            if(d >= _data[p]) break;
            _data[t] = _data[p];
            t = p;
        }
        _data[t] = d;
   }
   void delMin() { delData(0); } // Done
   void delData(size_t i) { // Done
        // Same as delMin in pdf but assign beginning idx as i.
        Data tmp = _data.back();
        _data.pop_back();

        size_t p = i, t = 2 * p + 1, n = size();
        if(i >= n) return;
        // Go Down
        while(t < n){
            if(t < n - 1) // has right child
                if(_data[t] > _data[t + 1])
                    ++t; // to the smaller child
            if(tmp < _data[t]) break;
            _data[p] = _data[t];
            p = t;
            t = 2 * p + 1;
        }
        // Go Up
        t = (p - 1) / 2;
        while(t > 0){
            if(tmp >= _data[p]) break;
            _data[p] = _data[t];
            p = t;
            t = (p - 1) / 2;
        }
        _data[p] = tmp;
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
