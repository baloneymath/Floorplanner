/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#ifndef _FLOORPLANNER_H
#define _FLOORPLANNER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <utility> // for pair
#include <algorithm>
#include "../lib/gnuplot-iostream.h"
using namespace std;

typedef pair<double, double>        Location;
typedef pair<Location, Location>    Rect;
typedef unordered_map<int, int>     IntIntMap;
typedef unordered_map<string, int>  StringIntMap;

struct Net {
    Net() {}
    Net(int i = 0, int d = 0, vector<string> p = vector<string>()):
        id(i), degree(d), pins(p) {}
    int     id;
    int     degree;
    vector<string> pins;
};

struct Terminal {
    Terminal() {}
    Terminal(string n = "", Location l = make_pair(0, 0),
        vector<int> nt = vector<int>()):
        name(n), loc(l), nets(nt) {}
    string      name;
    Location    loc;
    vector<int> nets;
};

struct Block {
    Block() {}
    Block(string n = "", int w = 0, int h = 0, 
        bool r = 0, Location loc = make_pair(-1, -1)):
        name(n), width(w), height(h), rotate(r), leftdown(loc) {}
    
    Location center() {
        double x = leftdown.first + (rotate? height : width) / 2;
        double y = leftdown.second + (rotate? width : height) / 2;
        return make_pair(x, y);
    }
    // compare functions
    static bool smaller(Block* b1, Block* b2) {
        return b1->width * b1->height < b2->width * b2->height;
    }
    static bool bigger(Block* b1, Block* b2) {
        return b1->width * b1->height > b2->width * b2->height;
    }
    static bool lesserX(Block* b1, Block* b2) {
        return b1->leftdown.first < b2->leftdown.first;
    }
    static bool lesserY(Block* b1, Block* b2) {
        return b1->leftdown.second < b2->leftdown.second;
    }
    
    string      name;
    double      width;
    double      height;
    bool        rotate;
    Location    leftdown;
    vector<int> nets;
};

class Floorplanner {
    public:
        Floorplanner() {}
        virtual ~Floorplanner() {}

        void parse(double&, string&, string&);
        void parseBlock(string&);
        void parseNet(string&);
        void gnuplot();
        
        virtual void init() = 0;
        virtual void pack() = 0;
        virtual void unpack() = 0;
        virtual void perturb() = 0;

    protected:
        // Outlines, basic
        double  _alpha;
        double  _width, _height;
        int     _nNet, _nBlock, _nTerminal;

        // name map
        StringIntMap    _blocksMap;
        StringIntMap    _terminalsMap;
        
        // main components
        vector<Net*>        _nets;
        vector<Block*>      _blocks;
        vector<Terminal*>   _terminals;
};

#endif