/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/29                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "fastSA.h"

#define _DETAIL_

template<typename T, typename U>
void plot_2d(vector<T>& data, vector<U> axis, bool logscale, uint limit) {
    assert(axis.size() >= limit || axis.size() == data.size());
    Gnuplot gp;
    if (logscale) gp << "set logscale y 2" << endl;
    gp << "set nokey" << endl;
    gp << "set style line 1 lc rgb \'#0060ad\' pt 6" << endl;
    for (uint i = 1; i < data.size() && i < limit; ++i)
        gp << "set arrow " << i << " from " << axis[i-1] << "," << data[i-1]
            << " to " << axis[i] << "," << data[i] << " nohead" << endl;
        gp << "plot '-' w p ls 1" << endl;
    for (uint i = 0; i < data.size() && i < limit; ++i)
        gp << axis[i] << " " << data[i] << endl;
    gp << "e\npause -1" << endl;
}

FastSA::FastSA()
{
    T = 0, ORI_T = 0;
    P = 0.99; // initial acceptance rate
    K = 8;
    C = 150;
    T_LOWER_BOUND = 0.00001;
    initN = 0; // inital perturbs
    N = 0; // how many tries per iteration
    alpha = 0;
    alpha_base = 0.5;
    beta = 0.1;
    fplans = 0;
    f_fplans = 0;
    avgUphill = 0;
    avgCost = 0;
}

void FastSA::initPerturb(Floorplanner& fp, int times)
{
    fp.initNorm(times);
    fp.initResult();
    fp.keepCurResult();
    avgUphill = 0;
    double cost = 0, up = 0;
    double pre = fp.Cost();

    while (avgUphill == 0) {
        up = 0;
        for (int i = 0; i < times; ++i) {
            fp.perturb();
            fp.pack();
            cost = fp.Cost();
            if (cost - pre > 0) {
                avgUphill += cost - pre;
                pre = cost;
                ++up;
            }
        }
    }
    avgUphill /= up;
    fp.recoverCur();
    fp.pack();
}

void FastSA::simulate(Floorplanner& fp)
{
    cout << "Start SA..." << endl;
    double oriArea = fp.curA();

    alpha_base = fp.orialpha();   
    alpha = alpha_base;
    fp.setalpha(alpha);
    fp.setbeta(beta);
    
    initN = 20 * fp.nBlock();
    N     = 5 * fp.nBlock();

    initPerturb(fp, initN);

    // start SA
    ORI_T = -avgUphill / log(P);
    T = ORI_T;
   // vector<double> TS;
   // TS.push_back(T);
   int a = 0;
    while (1) {
        ++fplans;
        #ifdef _DETAIL_
        cout << "fplans: " << fplans << endl;
        cout << "f_fplans: " << f_fplans << endl;
        #endif
        double cost = 0;
        avgCost = 0;
        int reject = 0;
        for (int i = 0; i < N; ++i) {
            fp.perturb();
            fp.pack();
            cost = fp.Cost(); 
            double dcost = cost - fp.getCurResult().cost;
            avgCost += fabs(dcost);
            P = min(1., exp(-dcost / T));
            if (dcost < 0 || (double)(rand() % 100 + 1) / 100 <= P) {
                fp.keepCurResult();
                if (cost < fp.getBestResult().cost) {
                    if (fp.isfit()) {
                        fp.keepBestResult();
                        ++f_fplans;
                    }
                    alpha = alpha_base + (1 - alpha_base) * f_fplans / N;
                    fp.setalpha(alpha);
                }
            }
            else {
                fp.recoverCur();
                ++reject;
            }
        }
        avgCost /= N;
        
        if (2 <= fplans && fplans < K) {
            T = ORI_T * avgCost / fplans / C;
        }
        else T = ORI_T * avgCost / fplans;
       // TS.push_back(T);
       
        #ifdef _DETAIL_
        cout << "alpha: " << alpha << endl;
        cout << "P: " << P << " T: " << T << endl;
        cout << "Reject %: " << (double)reject / N  << endl;
        cout << "Current Cost: " << fp.getCurResult().cost << endl;
        cout << endl;
        #endif
        if ((double)reject / N > 0.985 || T < T_LOWER_BOUND
            || fplans > 20 * fp.nBlock()) {
            if (f_fplans > 0) break;
            else {
                T = ORI_T;
                P = 0.99;
                fplans = 0;
                alpha = alpha_base;
            }
        }

    }
    fp.recoverBest();
    fp.pack();
    
    cout << "Origi Area: " << oriArea << endl;
    cout << "Final Area: " << fp.curA() << endl;
    cout << "Finish SA..." << endl;
   // vector<int> axis(1000);
   // iota(axis.begin(), axis.end(), 1);
   // plot_2d(TS, axis, true, 1000);

}

