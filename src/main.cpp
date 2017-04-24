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
    assert(argc >= 4);
    srand(time(0));    
    clock_t start, stop;
    assert((start = clock()) != -1);

    double alpha = stod(argv[1]);
    string bfile = argv[2];
    string nfile = argv[3];
    B_Tree bt;
    bt.parse(alpha, bfile, nfile);
    bt.init();
    bt.pack();
    cout << "Area: " << bt.curA() << " HPWL:" << bt.HPWL() << endl;
    bt.gnuplot();
 
    FastSA sa;
    sa.simulate(bt);
   
    if (bt.isfit()) {
        cout << "Feasable floorplan..." << endl;
        if (bt.curH() > bt.height() || bt.curW() > bt.width()) {
            bt.flip();
            cout << "Flip..." << endl;
        }
    }
    bt.gnuplot();
    stop = clock();
    double runtime = (double)(stop - start) / CLOCKS_PER_SEC;
    if (argc == 5) {
        string ofile = argv[4];
        bt.outfile(ofile, runtime);
    }
    cout << "\nRuntime: " << runtime << "s" << endl;
    
   // B_Tree bt[5];
   // FastSA sa[5];
   // for (int i = 0; i < 5; ++i) {
   //     bt[i].parse(alpha, bfile, nfile);
   //     bt[i].init();
   //     bt[i].pack();
   // }
   // for (int i = 0; i < 5; ++i) {
   //     sa[i].simulate(bt[i]);
   // }
   // int midx = 0;
   // for (int i = 0; i < 5; ++i) if (!bt[i].isfit()) ++midx;
   // for (int i = midx; i < 5; ++i) {
   //     if (bt[i].Cost() < bt[midx].Cost() && bt[i].isfit()) midx = i;
   // }
   // bt[midx].gnuplot();
   // if (bt[midx].isfit()) cerr << "fit" << endl;
   // 
   // stop = clock();
   // double runtime = (double)(stop - start) / CLOCKS_PER_SEC;
   // cout << "\nRuntime: " << runtime << "s" << endl;
    
    return 0;
}
