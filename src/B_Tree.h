/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#ifndef _B_TREE_H
#define _B_TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "chip.h"
using namespace std;

struct Node {
    Node() {
        me = parent = left = right = 0;
    }
    Node(Block* b, Block* p, Block* l, Block* r) {
        me = b;
        parent = p;
        left = l;
        right = r;
    }

    Block  *me, *parent, *left, *right;
};

class B_Tree {
    public:
        B_Tree() {}
    private:

};



#endif