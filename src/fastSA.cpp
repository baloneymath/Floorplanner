/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "fastSA.h"

namespace fastSA {

double  T = 0, ORI_T = 0;
double  P = 0.99; // initial acceptance rate
const int K = 7;
const double C = 200;
const double T_LOWER_BOUND = 0.00001;
int initN = 0; // inital perturbs
int N = 0; // how many tries per iteration
double alpha = 0;
double alpha_base = 0;
double beta = 0.1;
double fplans = 1;
double f_fplans = 0;

double normA = 0;
double normW = 0;
double avgUphill = 0;
double avgCost = 0;


void initPerturb(Floorplanner& fp, int times)
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

bool isFit(Floorplanner& fp)
{
    double H = fp.curH(), W = fp.curW();
    double fpH = fp.height(), fpW = fp.width();
    if (W < H) swap(W, H);
    if (fpW < fpH) swap(fpW, fpH);

    if (H <= fpH && W <= fpW) return true;
    else return false;
}


void FastSA(Floorplanner& fp)
{
    cout << "Start SA..." << endl;
    double oriArea = fp.curA();
    
    alpha_base = fp.alpha();
    alpha = alpha_base;
    fp.setbeta(beta);
    cerr << "alpha_base: " << alpha_base << endl;

    initN = 3 * fp.nBlock();
    N     = 5 * fp.nBlock();

    initPerturb(fp, initN);

    // start SA
    ORI_T = -avgUphill / log(P);
    T = ORI_T;
    cerr << "Init T: " << T << endl;
    
    while (T > T_LOWER_BOUND) {
        ++fplans;
        cerr << "fplans: " << fplans << endl;
        double cost = 0;
        avgCost = 0;
        int reject = 0;
        for (int i = 0; i < N; ++i) {
            f_fplans = 0;
            fp.perturb();
            fp.pack();
            cost = fp.Cost();
            double dcost = cost - fp.getCurResult().cost;
            avgCost += fabs(dcost);
            P = min(1., exp(-dcost / T));
            if (dcost < 0 || (double)(rand() % 100 + 1) / 100 <= P) {
                fp.keepCurResult();
                if (cost < fp.getBestResult().cost) {
                    fp.keepBestResult();
                    if (isFit(fp)) {
                        ++f_fplans;
                        //fp.gnuplot();
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
       
        cerr << "alpha: " << alpha << endl;
        cerr << "P: " << P << " T: " << T << endl;
        cerr << "Current Cost: " << fp.getCurResult().cost << endl;
        //cerr << "Reject %: " << (double)reject / N  << endl;
        if ((double)reject / N > 0.99)  break;

    }
    fp.recoverBest();
    fp.pack();
    cerr << "Origi Area: " << oriArea << endl;
    cerr << "Final Area: " << fp.curA() << endl;
    cout << "Finish SA..." << endl;
}

}
