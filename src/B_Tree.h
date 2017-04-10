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
#include "floorplanner.h"
using namespace std;

typedef pair<double, double>    Range;

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
    int     id;
    Block  *me, *parent, *left, *right;
};

class B_Tree : public Floorplanner {
    public:
        B_Tree() {
            _root = 0;
        }
        virtual ~B_Tree() {}

        virtual void init();
        virtual void pack();
        virtual void perturb();

    private:
        Node*           _root;
        vector<Node*>   _nodes;
        vector<Range>   _yContour;
};



#endif