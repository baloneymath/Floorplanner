/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#ifndef _CHIP_H
#define _CHIP_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility> // for pair
#include <algorithm>
using namespace std;

typedef pair<int, int>              Location;
typedef pair<Location, Location>    Rect;
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
        Location loc = make_pair(0, 0)):
        name(n), width(w), height(h), leftdown(loc) {}
    string      name;
    int         width;
    int         height;
    Location    leftdown;
    vector<int> nets;
};

class Chip {
    public:
        Chip() {}
        void parse(string&, string&);
        void parseBlock(string&);
        void parseNet(string&);

    private:
        // Outlines, basic
        int     _width, _height;
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