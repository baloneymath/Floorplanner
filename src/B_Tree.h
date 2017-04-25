/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/26                *
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
    Node(const Node& n) {
        data = n.data;
        id = n.id;
        parent = n.parent;
        left = n.left;
        right = n.right;
    }
    Node(Block* b, int i, int p, int l, int r) {
        data = b;
        id = i;
        parent = p;
        left = l;
        right = r;
    }
    bool    isleaf() {
        return left == NIL && right == NIL;
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

struct Result {
    Result() {
        root;
        cost = 0;
    }
    Node root;
    vector<Node> nodes;
    vector<bool> rotates;
    double cost;
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


        // for SA
        virtual void    initResult();
        virtual Result  getCurResult() {return _cur;}
        virtual Result  getBestResult() {return _best;}
        virtual void    keepCurResult();
        virtual void    keepBestResult();
        virtual void    recoverBest();
        virtual void    recoverCur();

    protected:
        // packing
        void    DFSPack(Node*);
        void    placeBlock(Node*);
        double  updateContour(Node*, int, bool);
        void    updateCurrent();

        // perturb
        void    rotateBlock(Node*);
        void    insertNode(Node*, Node*);
        void    deleteNode(Node*);
        void    swapNode(Node*, Node*);
        void    __swapNode(Node*, Node*);
        int     getRand();
    private:
        Node*           _root;
        vector<Node*>   _nodes;
        int             _conRoot;
        vector<Contour> _yContour;
        
        // for SA
        Result      _best, _cur;
};



#endif