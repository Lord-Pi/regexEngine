#ifndef __RED_BLACK_TREE_H_
#define __RED_BLACK_TREE_H_


template <typename T>
class RedBlackNode {
private:
  RedBlackNode<T>* parent;
  RedBlackNode<T>* left;
  RedBlackNode<T>* right;
  T value;
  bool isRed;

public:
  RedBlackNode(T t);

  RedBlackNode<T>* getParent() const;
  RedBlackNode<T>* getLeft() const;
  RedBlackNode<T>* getRight() const;
  T getValue() const;
  bool getIsRed() const;
  
  void setParent(RedBlackNode<T>* p);
  void setLeft(RedBlackNode<T>* l);
  void setRight(RedBlackNode<T>* r);
  void setIsRed(bool ir);
  
  
};



#include "redBlackTree.tpp"

#endif
