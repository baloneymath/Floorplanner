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
#include <random>
#include "floorplanner.h"
using namespace std;

const int NIL = -1;

struct Node {
    Node() {
        data = 0;
        parent = left = right = NIL;
    }
    Node(Block* b, int i, int p, int l, int r) {
        data = b;
        id = i;
        parent = p;
        left = l;
        right = r;
    }
    Block*  data;
    int     id;
    int     parent, left, right;
};

struct Contour {
    Contour() {
        data = 0;
        prev = next = NIL;
    }
    Contour(Block* b, int p, int n) {
        data = b;
        prev = p;
        next = n;
    }
    Block*  data;
    int     prev, next;
};

class B_Tree : public Floorplanner {
    public:
        B_Tree() {
            _root = 0;
            _conRoot = NIL;
        }
        virtual ~B_Tree();

        virtual void init();
        virtual void pack();
        virtual void unpack();
        virtual void perturb();

        virtual double Area();

        vector<Contour> yContour() {return _yContour;}
    
    protected:
        // packing
        void    DFSPack(Node*);
        void    placeBlock(Node*);
        double  updateContour(Node*, int, bool);

        // perturb
        void    rotateBlock(Node*);
        void    insertNode(Node*, Node*, bool);
        void    deleteNode(Node*);
        void    swapNode(Node*, Node*);
    private:
        Node*           _root;
        vector<Node*>   _nodes;
        int             _conRoot;
        vector<Contour> _yContour;
};



#endif