/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.

class HashKey
{
public:
   HashKey(size_t a, size_t b) : _key((a << 32) + b) {} // Might Need Improve...

   size_t operator() () const { return _key; }

   bool operator == (const HashKey& k) const { return _key == k._key; }

private:
   size_t _key;
};

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(HashMap<HashKey, HashData>* h = 0, size_t b = 0, size_t idx = 0)
      : _hash(h), _b(b), _idx(idx) {} // Done
      iterator(const iterator& i) : _hash(i._hash), _b(i._b), _idx(i._idx) {} // Done
      ~iterator() {} // Done, Keep HashData from deleting!!!!!

      HashNode& operator * () { return(*_hash)[_b][_idx]; } // Done
      const HashNode& operator * () const{ return (*_hash)[_b][_idx]; } // Done
      
      iterator& operator ++ (){ // Done
         if(!_hash) return (*this);
         if(_b >= _hash->_numBuckets) return (*this); // Last bucket
         if(++_idx >= (*_hash)[_b].size()){ // Change _b to next bucket
            while ((++_b < _hash->_numBuckets) && (*_hash)[_b].empty());
            _idx = 0;
         }
         return (*this);
      }
      iterator operator ++ (int){ // Done
         iterator i = *this;
         ++(*this);
         return i;
      }

      iterator& operator -- (){ // Done
         if(!_hash) return (*this);
         if(_idx == 0){ // First one in bucket _b
            if(_b == 0) return (*this); // First bucket
            while((*_hash)[--_b].empty())
               if(_b == 0)
                  return (*this);
            _idx = (*_hash)[_b].size() - 1;
         }
         else --_idx;
         return (*this);
      }      
      iterator operator -- (int){ // Done
         iterator i = *this;
         --(*this);
         return i;
      }

      iterator& operator = (const iterator& i){ // Done
         _hash = i._hash;
         _b = i._bId;
         _idx = i._bnId;
         return (*this);
      }

      bool operator != (const iterator& i) const{ // Done
         return (_hash != i._hash || _b != i._b || _idx != i._idx);
      }
      bool operator == (const iterator& i) const{ return !((*this) != i); } // Done


   private:
      HashMap<HashKey, HashData>*   _hash;
      size_t                        _b; // row, 0 ~ bucketNumbers
      size_t                        _idx; // col
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { // Done
      if(_buckets == 0) return end();      
      for(size_t i = 0; i < _numBuckets; ++i)
         if(!_buckets[i].empty())
            return iterator(const_cast<HashMap<HashKey, HashData>*>(this), i);
      return end();
   }
   // Pass the end
   iterator end() const { return iterator(const_cast<HashMap<HashKey, HashData>*>(this), _numBuckets); } // Done
   // return true if no valid data
   bool empty() const { // Done
      // return (begin() == end());
      for(size_t i = 0; i < _numBuckets; ++i)
         if(!_buckets[i].empty()) return false;
      return true;
   }
   // number of valid data
   size_t size() const { // Done
      size_t s = 0;
      for(size_t i = 0; i < _numBuckets; ++i)
         s += _buckets[i].size();
      return s;
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const { // Done
      size_t b = bucketNum(k);
      for(size_t i = 0; i < _buckets[b].size(); ++i)
         if(_buckets[b][i].first == k)
            return true;
      return false;   
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const { // Done. check
      size_t b = bucketNum(k);
      for(size_t i = 0; i < _buckets[b].size(); ++i)
         if(_buckets[b][i].first == k){
            d = _buckets[b][i].second;
            return true;
         }
      return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) { // Done. replaceInsert but inverse boolean!
      size_t b = bucketNum(k);
      for(size_t i = 0; i < _buckets[b].size(); ++i)
         if(_buckets[b][i].first == k){
            _buckets[b][i].second = d;
            return true;
         }
      _buckets[b].push_back(HashNode(k, d));
      return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) { // Done. insert
      size_t b = bucketNum(k);
      for(size_t i = 0; i < _buckets[b].size(); ++i)
         if(_buckets[b][i].first == k) return false;
      _buckets[b].push_back(HashNode(k, d));
      return true;   
   }

   // return true if removed successfully (i.e. k is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) { // Done
      size_t b = bucketNum(k);
      for(size_t i = 0, size = _buckets[b].size(); i < size; ++i){
          if(_buckets[b][i].first == k){
              _buckets[b][i] = _buckets[b].back();
              size_t size = _buckets[b].size();
              _buckets[b].resize(--size);
              return true;
          }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
