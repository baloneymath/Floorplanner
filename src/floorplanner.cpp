/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "floorplanner.h"

const double DBL_MAX = numeric_limits<double>::max();
const double DBL_MIN = numeric_limits<double>::min();

void Floorplanner::parse(double& a, double& b, string& bfName, string& nfName)
{
    _alpha = a;
    _beta = b;
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

double Floorplanner::HPWL()
{
    double val = 0;
    for (int i = 0; i < _nNet; ++i) {
        double left = DBL_MAX;
        double right = DBL_MIN;
        double down = DBL_MAX;
        double top = 0.;
        for (auto& str : _nets[i]->pins) {
            int pos = 0;
            double x = 0, y = 0;
            if (_blocksMap[str]) {
                pos = _blocksMap[str];
                x = _blocks[pos]->center().first;
                y = _blocks[pos]->center().second;
            }
            else if (_terminalsMap[str]) {
                pos = _terminalsMap[str];
                x = _terminals[pos]->loc.first;
                x = _terminals[pos]->loc.second;
            }
            left = min(left, x);
            right = max(right, x);
            down = min(down, y);
            top = min(top, y);
        }
        val += (right - left) + (top - down);
    }
    return val;
}

void Floorplanner::gnuplot()
{
    Gnuplot gplt;
    gplt << "set size 1,1" << endl;
    double xr = 2 * _width, yr = 2 * _height;
    gplt << "set xrange [" << 0 << ":" << xr << "]" << endl;
    gplt << "set yrange [" << 0 << ":" << yr << "]" << endl;
    gplt << "set object 1 rect from 0,0 to " << _width
         << "," << _height << "fc rgb \"yellow\" back" << endl;
    for (int i = 0; i < _nBlock; ++i) {
        Block* b = _blocks[i];
        if (b->leftdown == make_pair(-1., -1.)) continue;
        double w = b->width, h = b->height;
        if (b->rotate) {
            swap(w, h);
        }
        gplt << " set label \"" << b->name << "\" at "
             << b->center().first << "," << b->center().second << endl;
        gplt << " set object " << i + 2 << " rect from "
             << b->leftdown.first << ","
             << b->leftdown.second << " to "
             << b->leftdown.first + w << ","
             << b->leftdown.second + h << " fc rgb \"green\" " << endl;
        gplt << " plot \'-\' w p ls 1" << endl;
        gplt << " 0 0" << endl;
        gplt << " e" << endl;
        gplt << " pause -1" << endl;
    }
}