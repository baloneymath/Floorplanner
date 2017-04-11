/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "floorplanner.h"

void Floorplanner::parse(double& a, string& bfName, string& nfName)
{
    _alpha = a;
    parseBlock(bfName);
    parseNet(nfName);
}

void Floorplanner::parseBlock(string& bfName)
{
    string buf;
    fstream bf;
    bf.open(bfName, ifstream::in);
    if (!bf.is_open()) {
        cerr << "Error: Open xxx.block failed..." << endl;
        exit(0);
    }
    // Outline, NumBlocks, NumTerminals
    bf >> buf; bf >> _width; bf >> _height;
    bf >> buf; bf >> _nBlock;
    bf >> buf; bf >> _nTerminal;
    _blocks.resize(_nBlock);
    _terminals.resize(_nTerminal);
    
    // macros
    for (int i = 0; i < _nBlock; ++i) {
        string n;
        int w, h;
        bf >> n; bf >> w; bf >> h;
        Block* b = new Block(n, w, h);
        _blocks[i] = b;
        _blocksMap[n] = i;
    }
    // terminals
    for (int i = 0; i < _nTerminal; ++i) {
        string n;
        int x, y;
        bf >> n; bf >> buf; bf >> x; bf >> y;
        Terminal* t = new Terminal(n, make_pair(x, y));
        _terminals[i] = t;
        _terminalsMap[n] = i;
    }
    cout << "Finish parse blocks..." << endl;
}

void Floorplanner::parseNet(string& nfName)
{
    string buf;
    fstream nf;
    nf.open(nfName, ifstream::in);
    if (!nf.is_open()) {
        cerr << "Error: Open xxx.nets failed..." << endl;
        exit(0);
    }
    // NumNets
    nf >> buf; nf >> _nNet;
    _nets.resize(_nNet);
    
    int cnt = 0;
    while (nf >> buf) {
        if (buf == "NetDegree:") {
            int id = cnt, deg;
            nf >> deg;
            vector<string> p;
            for (int i = 0; i < deg; ++i) {
                nf >> buf;
                p.push_back(buf);
                if (_blocksMap[buf]) {
                    Block* b = _blocks[_blocksMap[buf]];
                    b->nets.push_back(id);
                }
                else if (_terminalsMap[buf]) {
                    Terminal* t = _terminals[_terminalsMap[buf]];
                    t->nets.push_back(id);
                }
            }
            Net* n = new Net(id, deg, p);
            _nets[cnt++] = n;
        }
    }
    cout << "Finish parse nets..." << endl;
}

void Floorplanner::unpack()
{
    for (auto& i : _blocks) {
        i->leftdown = make_pair(-1, -1);
    }
}