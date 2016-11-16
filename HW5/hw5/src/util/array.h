/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }	// Done
      T& operator * () { return (*_node); }	// Done
      iterator& operator ++ () { ++_node; return (*this); } // Done
      iterator operator ++ (int) { // Done
      	iterator tmp = (*this);
      	++(*this);
      	return tmp;
      }
      iterator& operator -- () { --_node; return (*this); } // Done
      iterator operator -- (int) { // Done
      	iterator tmp = (*this);
      	--(*this);
      	return tmp;
      }
      iterator operator + (int i) const { // Done 	
      	iterator tmp = (*this);
         	tmp += i;
      	return tmp;
      }
      iterator& operator += (int i) { // Done
      	_node += i;
      	return (*this);
      }
      iterator& operator = (const iterator& i) { // Done
      	_node = i._node;
      	return (*this);
      }
      bool operator != (const iterator& i) const { // Done
      	if(_node != i._node) return true;
      	else return false;
      }
      bool operator == (const iterator& i) const { // Done
      	return !(*this != i); // Implement by operator !=
      }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); } // Done
   iterator end() const { return iterator(_data + _size); } // Done
   bool empty() const { return (!_size); } // Done
   size_t size() const { return _size; } // Done

   T& operator [] (size_t i) { return _data[i]; } // Done
   const T& operator [] (size_t i) const { return _data[i]; } // Done

   void push_back(const T& x) { // Done
   		if(_size == _capacity) expand();
   		_data[_size] = x;
   		++_size;
   }
   void pop_front() { // Done
   		if(_size == 1) --_size;
   		if(_size >= 2){
   			_data[0] = _data[_size - 1];
   			--_size;
   		}
   }
   void pop_back() {  if(_size) --_size; } // Done

   bool erase(iterator pos) { // Done
   		if(!_size) return false;   		
   		else if(_size == 1) --_size;
   		else{
   			T* it = pos._node;
   			*(it) = *(_data + _size - 1);   			
   			--_size;
   		}	 			
   		return true;
   }
   bool erase(const T& x) { // Done	
   		for(T* it = _data; it < _data + _size; ++it)
   			if(*(it) == x){
   				erase(it);
   				return true;
   			}
   		return false;
   }

   void clear() { _size = 0; } // Done

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
   void expand(){
   		if(!_capacity) ++ _capacity;
   		else _capacity *= 2;
   		T* tmp = _data;
   		_data = new T[_capacity];
   		for(size_t i = 0; i < _size; ++i)
   			_data[i] = tmp[i];
   		delete [] tmp;
   }
};

#endif // ARRAY_H
