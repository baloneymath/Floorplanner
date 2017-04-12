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
        data = 0;
        parent = left = right = 0;
    }
    Node(Block* b, int i, int p, int l, int r) {
        data = b;
        id = i;
        parent = p;
        left = l;
        right = r;
    }
    int     id;
    Block*  data;
    int     parent, left, right;
};

class B_Tree : public Floorplanner {
    public:
        B_Tree() {
            _root = 0;
        }
        virtual ~B_Tree() {}

        virtual void init();
        virtual void pack();
        virtual void unpack();
        virtual void perturb();

        void    DFSPack(Node*);
        void    placeBlock(Node*, list<Node*>::iterator);
        double  updateContour(Node*, list<Node*>::iterator);
    private:
        Node*           _root;
        vector<Node*>   _nodes;

        list<Node*>::iterator   _current;
        list<Node*>             _yContour;
};



#endif