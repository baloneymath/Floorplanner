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

typedef pair<double, double>        Location;
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
        bool r = 0, Location loc = make_pair(0, 0)):
        name(n), width(w), height(h), rotate(r), leftdown(loc) {}
    
    Location center() {
        double x = leftdown.first + (rotate? width : height);
        double y = leftdown.second + (rotate? height : width);
        return make_pair(x, y);
    }
    
    string      name;
    double      width;
    double      height;
    bool        rotate;
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
        double  _width, _height;
        double  _nNet, _nBlock, _nTerminal;

        // name map
        StringIntMap    _blocksMap;
        StringIntMap    _terminalsMap;
        
        // main components
        vector<Net*>        _nets;
        vector<Block*>      _blocks;
        vector<Terminal*>   _terminals;
};

#endif