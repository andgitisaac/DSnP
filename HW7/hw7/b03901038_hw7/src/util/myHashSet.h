/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
        iterator(const HashSet<Data>* hash=0, size_t b=0) 
                : _hash(hash), _b(b), _idx(0) {}    
        iterator(const iterator& it) 
                : _hash(it._hash), _b(it._b), _idx(it._idx) {}
        ~iterator() {}

        Data& operator * () { return _hash->_buckets[_b][_idx]; }
        const Data& operator * () const { return _hash->_buckets[_b][_idx]; }

        iterator& operator ++ () {
          if(_b == _hash->_numBuckets) return (*this);
            if(_idx != _hash->_buckets[_b].size() - 1) // not the last in _buckets[_b]
                ++_idx;
            else{
                _idx = 0, ++_b;
                while(_b < _hash->_numBuckets && _hash->_buckets[_b].empty())
                    ++_b;
            }
            return (*this);
        }
        iterator operator ++ (int) {
            iterator it = (*this);
            ++(*this);
            return it;
        }
        iterator& operator -- () {
          if(_idx != 0) --_idx;
            else{
                --_idx;
                while(_b >= 0 && _hash->_buckets[_b].empty())
                    --_b;
                _idx = _hash->_buckets[_b].size() - 1;
            }
            return (*this);
        }
        iterator operator -- (int) {
            iterator it = (*this);
            --(*this);
            return it;
        }
        iterator operator = (const iterator& it) {
            _hash = it._hash;
            _b = it._b;
            _idx = it._idx;
        }

        bool operator != (const iterator& it) {
            return (_b != it._b || _idx != it._idx || _hash != it._hash);
        }
        bool operator == (const iterator& it) { return !(it != (*this)); }

   private:    
        const HashSet<Data>* _hash;
        size_t              _b; // nRows    
        size_t              _idx; // nCols        
   };              

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      size_t i = 0;
      for( ;i < _numBuckets; ++i){
          if(!_buckets[i].empty()) break;
      }
      return iterator(this, i);
   }
   // Pass the end
   iterator end() const { return iterator(this, _numBuckets); }   

   // return true if no valid data
   bool empty() const { return begin() == end(); }
  
   size_t size() const {
        size_t s = 0;
        for(size_t i = 0; i < _numBuckets; ++i)
            s += _buckets[i].size();
        return s;
    }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
        size_t n = bucketNum(d);
        for(size_t i = 0, size = _buckets[n].size(); i < size; ++i)
            if(_buckets[n][i] == d)
                return true;
        return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
        size_t n = bucketNum(d);
        for(size_t i = 0, size = _buckets[n].size(); i < size; ++i){
            if(_buckets[n][i] == d){
                d = _buckets[n][i];
                return true;
            }
        }
        return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
    bool update(const Data& d) {
     size_t n = bucketNum(d);
     for (size_t i = 0, size = _buckets[n].size(); i < size; ++i)
       if (_buckets[n][i] == d) {
         _buckets[n][i] = d;
         return true;
       }
     return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
        if(check(d)) return false;
        size_t n = bucketNum(d);
        _buckets[n].push_back(d);
        return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
        // size_t n = bucketNum(d);
        // for(size_t i = 0, size = _buckets[n].size(); i < size; ++i){
        //     if(_buckets[n][i] == d){
        //         _buckets[n].erase(_buckets[n].begin() + i);
        //         return true;
        //     }
        // }
        // return false;

        size_t n = bucketNum(d);
        for(size_t i = 0, size = _buckets[n].size(); i < size; ++i){
            if(_buckets[n][i] == d){
                _buckets[n][i] = _buckets[n].back();
                size_t size = _buckets[n].size();
                _buckets[n].resize(--size);
                return true;
            }
        }
        return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
}; 

#endif // MY_HASH_SET_H
