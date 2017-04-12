/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include <iostream>
#include <cassert>
#include "B_Tree.h"
#include "floorplanner.h"
using namespace std;

int main(int argc, char* argv[])
{
    assert(argc >= 3);
    double alpha = atoi(argv[1]);
    string bfile = argv[2];
    string nfile = argv[3];
    B_Tree bt;
    bt.parse(alpha, bfile, nfile);
    bt.init();
    bt.pack();
    return 0;
}
