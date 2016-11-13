/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <iostream>

using namespace std;

#define REPLACE(A,B) if(B) B->_parent = A->_parent;\
  							if(A->_parent)\
  							(A->_parent->_right == A) ? (A->_parent->_right = B) : (A->_parent->_left = B);\
  							else _root = B;

#define TOMAX(A)     while(A->_right) A = A->_right
#define TOMIN(A)     while(A->_left) A = A->_left

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* p = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* l = 0):
      _data(d), _parent(p), _right(r), _left(l) {}

   T              _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _right;
   BSTreeNode<T>* _left;
};

template <class T>
class BSTree
{
   // TODO: design your own class!!
   // NOTE: Using in-order!!(Left->Parent->Right)
public:
   BSTree() : _size(0) {
      _tail = new BSTreeNode<T>(T());
      _root = _tail;
   }
   ~BSTree() { clear(); delete _tail; }
   class iterator {
      friend class BSTree;
   public:
      iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {}

      const T& operator * () const { return _node->_data; } // Done
      T& operator * () { return _node->_data; } // Done

   		iterator& operator ++ () { // Done
   			// What if we ++ from end()??
   			// Move forward to next larger value.
   			BSTreeNode<T>* p;
   			if(_node->_right){
   				_node = _node->_right;
   				while(_node->_left) _node = _node->_left;
   			}
   			else{
   				p = _node->_parent;
   				while(p && _node == p->_right){
   					_node = p;
   					p = p->_parent;
   				}
   				// if we were previously at the right-most node in
   				// the tree, nodePtr = _tail, and the iterator specifies
   				// the end of the list
   				_node = p;
   			}
   			return (*this);
   		}
   		iterator& operator ++ (int) { // Done
   			iterator tmp = iterator(*this);
   			++(*this);
   			return tmp;
   		}
   		iterator& operator -- () { // Done
   			// Move backward to largest value < current value.
   			BSTreeNode<T>* p;
   			if(_node->_left){
   				_node = _node->_left;
   				while(_node->_right) _node = _node->_right;
   			}
   			else{
   				p = _node->_parent;
   				while(p && _node == p->_left){
   					_node = p;
   					p = p->_parent;
   				}
   				// if we were previously at the left-most node in
   				// the tree, it seems like that I have to well define begin()...
   				_node = p;
   			}
   			return (*this);
   		}
   		iterator operator -- (int) { // Done
   			iterator tmp = (*this);
            --(*this);
            return tmp;
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
   			return !(*this != i);
   		}

   private:
   		BSTreeNode<T>* _node;
   };

   iterator begin() { // Done
   		BSTreeNode<T>* current = _root;
   		if(current)
   			while(current->_left)
   				current = current->_left;
   		return iterator(current);
   }
   iterator end() { return iterator(_tail); } // Done
   bool empty() { return !_size; } // Done
   size_t size() { return _size; } // Done

   void insert(const T& x) {
   		// BSTreeNode<T>* current = _root;
   		// insert_recursive(x, current, false);
   		// ++_size;
   		BSTreeNode<T>* temp = _root;
   		   	while(1) {
   		   		if(temp == _tail) {
   		   			BSTreeNode<T>* n = new BSTreeNode<T>(x, _tail->_parent, _tail);
   		   			if(temp == _root) _root = n;
   		   			else _tail->_parent->_right = n;
   		   			_tail->_parent = n;
   		   			break;
   		   		}
   		   		else if(x < temp->_data) {
   		   			if(temp->_left) temp = temp->_left;
   		   			else {
   		   				BSTreeNode<T>* n = new BSTreeNode<T>(x, temp);
   		   				temp->_left = n;
   		   				break;
   		   			}
   		   		}
   		   		else {
   		   			if(temp->_right) temp = temp->_right;
   		   			else {
   		   				BSTreeNode<T>* n = new BSTreeNode<T>(x, temp);
   		   				temp->_right = n;
   		   				break;
   		   			}
   		   		}
   		   	}
   		   	++_size;
   }
   void pop_front() { erase(begin()); }
   void pop_back() { erase(--end()); }

   bool erase(iterator pos) {
   		if(pos == end()) return false;
   		BSTreeNode<T>* deleteNode = pos._node;

   		if(deleteNode->_right && deleteNode->_right != _tail){
   			if(deleteNode->_left){
   				// BSTreeNode<T>* succ = successor(deleteNode);
   				// T copyData = succ->_data;
   				// deleteNode->_data = copyData;
   				// succ->_parent->_left = 0;
       //         succ->_left = deleteNode->_left;
       //         succ->_right = deleteNode->_right;
       //         succ->_parent = deleteNode->_parent;

               T s = (++pos)._node->_data;
               erase(pos);
               deleteNode->_data = s;
               ++_size;
   			}
   			else{
   				reconnect(deleteNode, deleteNode->_right);
               delete deleteNode;
   			}
   		}
   		else if(deleteNode->_left){
   			reconnect(deleteNode, deleteNode->_left);
   			if(deleteNode->_right == _tail){
   				BSTreeNode<T>* max = deleteNode->_left;
   				while(max->_right) max = max->_right;
   				_tail->_parent = max;
   				max->_right = _tail;
   			}
            delete deleteNode;
   		}
   		else{
   			(deleteNode->_right == _tail) ? (reconnect(deleteNode, _tail))
   										: (reconnect(deleteNode, deleteNode->_left));
            delete deleteNode;
   		}
   		--_size;
   		return true;
   }
   bool erase(const T& x) {
      BSTreeNode<T>* current = _root;
      while(true){
         if(current == 0) return false; // No match.
         if(current->_data == x) return erase(iterator(current));

         if(current->_data > x) current = current->_left;
         else current = current->_right;
      }
   }
   void clear(){ // Done
   	if(!empty()){
   		iterator b = begin(), e = end(), it = b;
   		while(it != e){
   			++it;
   			erase(b);
   			b = it;
   		}
   	}
   	_size = 0;
   }

   void sort() {}
   void print(){ if(!empty()) treePrint(_root, 0); }

private:
	BSTreeNode<T>* _root;
	BSTreeNode<T>* _tail;
	size_t _size;

	void insert_recursive(const T& x, BSTreeNode<T>* current, bool direction) {
		// direction "false": go to left child 
		if(current == _tail){
			BSTreeNode<T>* newNode = new BSTreeNode<T>(x, _tail->_parent, 0, _tail);
			if(empty()){
				_root = newNode;
			}
			else{
				_tail->_parent->_right = newNode;
			}
			_tail->_parent = newNode;
			return;
		}
		if(!current){
			BSTreeNode<T>* newNode = new BSTreeNode<T>(x, current->_parent, 0, 0);
			if(direction)
				current->_parent->_right = newNode;
			else
				current->_parent->_left = newNode;
			return;
		}
		if(x < current->_data){
			current = current->_left;
			insert_recursive(x, current, false);
		}
		else{
			current = current->_right;
			insert_recursive(x, current, true);
		}
		return;
	}

	BSTreeNode<T>* min(BSTreeNode<T>* current) {
		// if(!current->_left) return current;
		// else return min(current->_left);
      while(current->_left) current = current->_left;
      return current; 
	}

	BSTreeNode<T>* successor(BSTreeNode<T>* current) {
		if(current->_right && current->_right != _tail)
			return min(current->_right);
		else if(current->_right == _tail){
         current = current->_left;
         while(current->_right) current = current->_right;
         return current;
      }
		else{
			BSTreeNode<T>* p;
			p = current->_parent;
			while(p && current == p->_right){
				current = p;
				p = p->_parent;
			}
			current = p;
		}
		return current;
	}

	void reconnect(BSTreeNode<T>* x, BSTreeNode<T>* y) {
		if(y)
			y->_parent = x->_parent;
		if(x->_parent){
			if(x->_parent->_right == x)
				x->_parent->_right = y;
			else
				x->_parent->_left = y;
		}
		else
			_root = y;
	}

	void treePrint(BSTreeNode<T>* n, size_t l){
        for (size_t i = 0; i < l; ++i) cout << "  ";
		if (n == 0 || n == _tail) cout << "[0]\n";
		else {
			cout << n->_data << endl;
			treePrint(n->_left, l + 1);
			treePrint(n->_right, l + 1);
		}
   }

};

#endif // BST_H
