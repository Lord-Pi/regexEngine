#ifndef __RED_BLACK_TREE_T_
#define __RED_BLACK_TREE_T_

template <typename T>
RedBlackNode<T>::RedBlackNode(T t) {
parent = nullptr;
left = nullptr;
right = nullptr;
value = t;
isRed = true;
}

template <typename T>
RedBlackNode<T>::RedBlackNode<T>* getParent() const {
return parent;
}

template <typename T>
RedBlackNode<T>::RedBlackNode<T>* getLeft() const {
return left;
}

template <typename T>
RedBlackNode<T>::RedBlackNode<T>* getRight() const {
return right;
}

template <typename T>
RedBlackNode<T>::getValue() const {
return value;
}

template <typename T>
RedBlackNode<T>::getIsRed() const {
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


#endif