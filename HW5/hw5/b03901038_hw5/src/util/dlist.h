/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>
// #include <iostream>
// using namespace std;

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      // _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; } // Done
      T& operator * () { return _node->_data; } // Done
      iterator& operator ++ () { // Done
      	_node = _node->_next;
      	return *(this);
      }
      iterator operator ++ (int) { // Done
      	iterator tmp = (*this);
      	(*this) = _node->_next;
      	// _node = _node->_next;
      	return tmp;
      }
      iterator& operator -- () { // Done
      	_node = _node->_prev;
      	return *(this);
      }
      iterator operator -- (int) { // Done
      	iterator tmp = (*this);
      	(*this) = _node->_prev;
      	// _node = _node->_prev;
      	return tmp;
      }
      iterator& operator = (const iterator& i) { // Done
      	_node = i._node;
      	return *(this);
      }
      bool operator != (const iterator& i) const { // Done
      	if(_node != i._node) return true;
      	else return false;
      }
      bool operator == (const iterator& i) const { // Done
      	return !(*this != i);
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   // It seems that _head = dummy node.
   iterator begin() const { return iterator(_head->_next); } // Done
   iterator end() const { return iterator(_head); } // Done
   bool empty() const { return _head == _head->_next; } // Done
   size_t size() const { // Done
   		size_t size = 0;
   		for (iterator it = begin(); it != end(); ++it)
   			++size;
   		return size;
   }

   void push_back(const T& x) { // Done
   		// Insert x between dummy(end()) and last element(end()->_prev)
   		DListNode<T>* dummy = end()._node;
   		DListNode<T>* newNode = new DListNode<T>(x, dummy->_prev, dummy);
   		newNode->_prev->_next = newNode;
   		end()._node->_prev = newNode;
   }
   void pop_front() { erase(begin()); } // Done
   void pop_back() { erase(--end()); } // Done

   // return false if nothing to erase
   bool erase(iterator pos) { // Done
   		// if(empty()) return false;
   		// DListNode<T>* deleteNode = pos._node;
   		// deleteNode->_next->_prev = deleteNode->_prev;
   		// deleteNode->_prev->_next = deleteNode->_next;
   		// delete pos._node;
   		// return true;
   		if(pos == end()) return false;
   		else {
   		   DListNode<T>* d = pos._node;
   		   d->_prev->_next = d->_next;
   		   d->_next->_prev = d->_prev;
   		   delete pos._node;
   		   return true;
   		}
   }
   bool erase(const T& x) { // Done
   		for(iterator it = begin(); it != end(); ++it){
   			if(*(it) == x){
   				erase(it);
   				return true;
   			}
   		}
   		return false;
   }

   void clear() { // Done
   		if(!empty()){
   			DListNode<T>* node = _head->_prev;
   			while(node != _head){
   				node = node->_prev;
   				delete node->_next;
   			}
   			_head->_prev = _head->_next = _head;
   		}
   }  // delete all nodes except for the dummy node

   void sort() const {
   	// if(!_isSorted) quickSort(begin(), --end());
      bubbleSort(begin(), end()); 
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   
   // void quickSort(iterator start, iterator end) const{
   //    if(start == end) return;


   // 	T mid = *(end);
   // 	iterator left = start, right = --end;
   //    ++end;
   // 	cerr << *(end) << endl;
   // 	while(left != right){
   // 		while(*(left) < mid && left != right){
   //          cerr << "left = " << *(left) << endl;
   // 			++left;
   //       }
   // 		while(*(right) >= mid && left != right){
   //          cerr << "right = " << *(right) << endl;
   // 			--right;
   //       }
   //       cerr << "left = " << *(left) << "right = " << *(right) << endl;            
   // 		swap(left, right);
   // 	}
   // 	cerr << *(left) << " " << *(right) << endl;

   // 	if(*(left) >= mid) swap(left, end);
   // 	else ++left;

   //    cerr << *(left) << " " << *(right) << endl;

   // 	if(iterator(start._node->_next) != left) quickSort(start, --left);
   // 	if(left != end) quickSort(left, end);
   // }
   
   void bubbleSort(iterator start, iterator end) const{
      for(iterator i, next; start != end; --end)
         for(next = start, i = next, ++next; next != end; i = next, ++next)
            if(*next < *i)
               swap(i, next);
   }

   void swap(iterator x, iterator y) const{
   		T tmp = *x;
   		*x = *y;
   		*y = tmp;
   }
};

#endif // DLIST_H
