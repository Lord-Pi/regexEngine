
#ifndef __RED_BLACK_TREE_H_
#define __RED_BLACK_TREE_H_

#include <iostream>
#include <stdexcept>
#include <string>


template <typename T>
class RedBlackNode {
private:
  RedBlackNode<T>* parent;
  RedBlackNode<T>* left;
  RedBlackNode<T>* right;
  T value;
  bool isRed;

  void bstInsert(RedBlackNode<T>* newNode);
  bool bstUniqueInsert(RedBlackNode<T>* newNode);
  void colorCorrect(RedBlackNode<T>* newNode);
  bool isLeftChild() const;
  void rotateLeft();
  void rotateRight();
  RedBlackNode<T>* findRoot();

public:
  RedBlackNode(T t);
  ~RedBlackNode();
  
  RedBlackNode<T>* getParent() const;
  RedBlackNode<T>* getLeft() const;
  RedBlackNode<T>* getRight() const;
  T getValue() const;
  bool getIsRed() const;
  
  void setParent(RedBlackNode<T>* p);
  void setLeft(RedBlackNode<T>* l);
  void setRight(RedBlackNode<T>* r);
  void setIsRed(bool ir);

  RedBlackNode<T>* getGrandparent() const;
  RedBlackNode<T>* getUncle() const;

  RedBlackNode<T>* redBlackInsert(RedBlackNode<T>* newNode);
  RedBlackNode<T>* redBlackUniqueInsert(RedBlackNode<T>* newNode);
  bool find(T val);
  
  void traverseInOrder(std::string delim) const;
  void traversePreOrder(std::string delim) const;
};

template <typename T>
class RedBlackTree {
private:
  RedBlackNode<T>* root;

public:
  RedBlackTree();
  RedBlackTree(T t);
  ~RedBlackTree();
  void redBlackInsert(T val);
  void redBlackUniqueInsert(T val);
  bool find(T val) const;
  void traverseInOrder(std::string delim) const;
  void traversePreOrder(std::string delim) const;
};




template <typename T>
RedBlackNode<T>::RedBlackNode(T t) {
  parent = nullptr;
  left = nullptr;
  right = nullptr;
  value = t;
  isRed = true;
}

template <typename T>
RedBlackNode<T>::~RedBlackNode() {
  delete left;
  delete right;
}

template <typename T>
RedBlackNode<T>* RedBlackNode<T>::getParent() const {
  return parent;
}

template <typename T>
RedBlackNode<T>* RedBlackNode<T>::getLeft() const {
  return left;
}

template <typename T>
RedBlackNode<T>* RedBlackNode<T>::getRight() const {
  return right;
}

template <typename T>
T RedBlackNode<T>::getValue() const {
  return value;
}

template <typename T>
bool RedBlackNode<T>::getIsRed() const {
  return isRed;
}

template <typename T>
void RedBlackNode<T>::setParent(RedBlackNode<T>* p) {
  parent = p;
}

template <typename T>
void RedBlackNode<T>::setLeft(RedBlackNode<T>* l) {
  left = l;
}

template <typename T>
void RedBlackNode<T>::setRight(RedBlackNode<T>* r) {
  right = r;
}

template <typename T>
void RedBlackNode<T>::setIsRed(bool ir) {
  isRed = ir;
}

template <typename T>
RedBlackNode<T>* RedBlackNode<T>::getGrandparent() const {
  if(getParent() == nullptr) return nullptr;
  return getParent()->getParent();
}

// Assumes that this node is not the root
template <typename T>
bool RedBlackNode<T>::isLeftChild() const {
  if(getParent() == nullptr) throw std::runtime_error("Tried to determine left-child-ness with null parent");
  return this == (getParent()->getLeft());
}

template <typename T>
RedBlackNode<T>* RedBlackNode<T>::getUncle() const {
  RedBlackNode<T>* g = getGrandparent();
  if(g == nullptr) return nullptr;
  bool parentIsLeftChild = getParent()->isLeftChild();
  if(parentIsLeftChild) return g->getRight();
  return g->getLeft();
}

// Probably important note: assumes that there are no duplicates in the tree
template <typename T>
void RedBlackNode<T>::bstInsert(RedBlackNode<T>* newNode) {
  if(newNode->getValue() < value) {
    if(left == nullptr) {
      newNode->setParent(this);
      left = newNode;
    } else {
      left->bstInsert(newNode);
    }
  } else {
    if(right == nullptr) {
      newNode->setParent(this);
      right = newNode;
    } else {
      right->bstInsert(newNode);
    }
  }
}

// Actually checks if there are duplicates as we go
// Returns true if insert was successful (inserted element was not already in tree)
// Returns false if insert was not successful (inserted element was already in tree)
template <typename T>
bool RedBlackNode<T>::bstUniqueInsert(RedBlackNode<T>* newNode) {
  if(newNode->getValue() < value) {
    if(left == nullptr) {
      newNode->setParent(this);
      left = newNode;
      return true;
    } else {
      return left->bstUniqueInsert(newNode);
    }
  } else if(newNode->getValue() > value) {
    if(right == nullptr) {
      newNode->setParent(this);
      right = newNode;
      return true;
    } else {
      return right->bstUniqueInsert(newNode);
    }
  } else {
    return false;
  }
}

// Assumes there is a right child to rotate into current position
template <typename T>
void RedBlackNode<T>::rotateLeft() {
  RedBlackNode<T>* upperNode = getParent();
  RedBlackNode<T>* rotationChild = getRight();
  RedBlackNode<T>* middleTree = rotationChild->getLeft();
  // Fix upper arrow
  if(upperNode != nullptr) {
    if(isLeftChild()) upperNode->setLeft(rotationChild);
    else upperNode->setRight(rotationChild);
  }
  rotationChild->setParent(upperNode);
  // Fix middle arrow
  rotationChild->setLeft(this);
  setParent(rotationChild);
  // Fix lower arrow
  setRight(middleTree);
  if(middleTree != nullptr) middleTree->setParent(this);
}

// Assumes there is a right child to rotate into current position
template <typename T>
void RedBlackNode<T>::rotateRight() {
  RedBlackNode<T>* upperNode = getParent();
  RedBlackNode<T>* rotationChild = getLeft();
  RedBlackNode<T>* middleTree = rotationChild->getRight();
  // Fix upper arrow
  if(upperNode != nullptr) {
    if(isLeftChild()) upperNode->setLeft(rotationChild);
    else upperNode->setRight(rotationChild);
  }
  rotationChild->setParent(upperNode);
  // Fix middle arrow
  rotationChild->setRight(this);
  setParent(rotationChild);
  // Fix lower arrow
  setLeft(middleTree);
  if(middleTree != nullptr) middleTree->setParent(this);
}

template <typename T>
void RedBlackNode<T>::colorCorrect(RedBlackNode<T>* newNode) {
  RedBlackNode<T>* p = newNode->getParent();
  if(p == nullptr) return;			    // Case 3
  if(!p->getIsRed()) return;			    // Case 1
  RedBlackNode<T>* g = newNode->getGrandparent();
  if(g == nullptr) {				    // Case 4
    p->setIsRed(false);
    return;
  }

  // From now on the parent is implicitly red
  RedBlackNode<T>* u = newNode->getUncle();
  if(u == nullptr || !u->getIsRed()) {		    // Case 5/6
    // Left cases
    if(p->isLeftChild()) {
      // Left/left case
      if(newNode->isLeftChild()) {
	g->rotateRight();
	g->setIsRed(true);
	p->setIsRed(false);
	return;
      }
      // Left/right case
      else {
	p->rotateLeft();
	g->rotateRight();
	g->setIsRed(true);
	newNode->setIsRed(false);
	return;
      }
    }
    // Right cases
    else {
      // Right/left case
      if(newNode->isLeftChild()) {
	p->rotateRight();
	g->rotateLeft();
	g->setIsRed(true);
	newNode->setIsRed(false);
	return;
      }
      // Right/right case
      else {
	g->rotateLeft();
	g->setIsRed(true);
	p->setIsRed(false);
	return;
      }
    }
  } else {	   		   		    // Case 2
    p->setIsRed(false);
    u->setIsRed(false);
    g->setIsRed(true);
    colorCorrect(g);
    return;
  }
}


template <typename T>
RedBlackNode<T>* RedBlackNode<T>::findRoot() {
  if(getParent() == nullptr) return this;
  return getParent()->findRoot();
}		 

// Returns the new root node of the tree, in case rotations have happened
template <typename T>
RedBlackNode<T>* RedBlackNode<T>::redBlackInsert(RedBlackNode<T>* newNode) {
  bstInsert(newNode);
  colorCorrect(newNode);
  return findRoot();
}

// Returns the new root node of the tree, in case rotations have happened
template <typename T>
RedBlackNode<T>* RedBlackNode<T>::redBlackUniqueInsert(RedBlackNode<T>* newNode) {
  if(bstUniqueInsert(newNode)) {
    colorCorrect(newNode);
  }
  return findRoot();
}

template <typename T>
bool RedBlackNode<T>::find(T val) {
  if(val == value) return true;
  if(val < value) {
    if(left == nullptr) return false;
    return left->find(val);
  } else {
    if(right == nullptr) return false;
    return right->find(val);
  }
}

template <typename T>
void RedBlackNode<T>::traverseInOrder(std::string delim) const {
  if(getLeft() != nullptr) getLeft()->traverseInOrder(delim);
  //  std::cout << getValue() << delim;
  if(getRight() != nullptr) getRight()->traverseInOrder(delim);
}

template <typename T>
void RedBlackNode<T>::traversePreOrder(std::string delim) const {
  //  std::cout << getValue() << delim;
  if(getLeft() != nullptr) getLeft()->traversePreOrder(delim);
  if(getRight() != nullptr) getRight()->traversePreOrder(delim);
}





template <typename T>
RedBlackTree<T>::RedBlackTree() {
  root = nullptr;
}

template <typename T>
RedBlackTree<T>::RedBlackTree(T t) {
  root = nullptr;
  redBlackInsert(t);
}

template <typename T>
RedBlackTree<T>::~RedBlackTree() {
  delete root;
}

template <typename T>
void RedBlackTree<T>::redBlackInsert(T val) {
  RedBlackNode<T>* newNode = new RedBlackNode<T>(val);
  if(root == nullptr) {
    root = newNode;
  } else {
    root = root->redBlackInsert(newNode);
  }
}

template <typename T>
void RedBlackTree<T>::redBlackUniqueInsert(T val) {
  RedBlackNode<T>* newNode = new RedBlackNode<T>(val);
  if(root == nullptr) {
    root = newNode;
  } else {
    root = root->redBlackUniqueInsert(newNode);
  }
}

template <typename T>
bool RedBlackTree<T>::find(T val) const {
  return root->find(val);
}

template <typename T>
void RedBlackTree<T>::traverseInOrder(std::string delim) const {
  root->traverseInOrder(delim);
}

template <typename T>
void RedBlackTree<T>::traversePreOrder(std::string delim) const {
  root->traversePreOrder(delim);
}


#endif
