#include "characterClass.hpp"
#include "redBlackTree.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
  RedBlackTree<int> rbt;
  rbt.redBlackInsert(4);
  rbt.redBlackInsert(7);
  rbt.redBlackInsert(3);
  rbt.redBlackInsert(1);
  rbt.redBlackInsert(5);
  rbt.redBlackInsert(6);
  rbt.redBlackInsert(2);
  rbt.redBlackInsert(10);
  rbt.redBlackInsert(9);
  rbt.redBlackInsert(8);
  rbt.traverseInOrder("\n");
  std::cout << "\n\n" << std::endl;
  rbt.traversePreOrder("\n");

  CharacterClassDecider c0("a-c", false);
  CharacterClassDecider c1("abcd-f", false);
  CharacterClassDecider c2("a-cdef", false);
  CharacterClassDecider c3("a-cgd-fh-", false);
  
}
