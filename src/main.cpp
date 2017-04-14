/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include <iostream>
#include <cassert>
#include <ctime>
#include "B_Tree.h"
#include "floorplanner.h"
#include "fastSA.h"
using namespace std;

int main(int argc, char* argv[])
{
    assert(argc >= 3);
    srand(time(0));    
    clock_t start, stop;
    assert((start = clock()) != -1);

    double alpha = atoi(argv[1]);
    double beta = 0.5;
    string bfile = argv[2];
    string nfile = argv[3];
    B_Tree bt;
    bt.parse(alpha, beta, bfile, nfile);
    bt.init();
    bt.pack();
    bt.gnuplot();
    bt.perturb();
    bt.unpack();
    bt.pack();
    bt.gnuplot();
    //cout << bt.HPWL() << endl;
    //cout << bt.Area() << endl;
    //fastSA::fastSA(bt);
    stop = clock();
    cout << "\nRuntime: " 
         << (double)(stop - start) / CLOCKS_PER_SEC 
         << "s" << endl;
    return 0;
}
