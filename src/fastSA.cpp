/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "fastSA.h"

namespace fastSA {

double  T = 0, ORI_T = 0;
double  P = 0.9; // initial acceptance rate
const int K = 7;
const double C = 150;
const double T_LOWER_BOUND = 0.00001;
int initN = 0; // inital perturbs
int N = 0; // how many tries per iteration
double alpha = 0;
double alpha_base = 0;
double beta = 0.1;
double fplans = 0;
double f_fplans = 0;

double normA = 0;
double normW = 0;
double avgUphill = 0;
double avgCost = 0;

void initNorm(Floorplanner& fp, int times)
{
    fp.initResult();
    Result ori = fp.getResult();
    alpha_base = fp.alpha();
    alpha = alpha_base;

    normA = normW = 0;
    for (int i = 0; i < times; ++i) {
        fp.perturb();
        fp.pack();
        normA += fp.Area();
        normW += fp.HPWL();
    }
    normA /= times;
    normW /= times;
    fp.restoreResult(ori);
}

void initPerturb(Floorplanner& fp, int times)
{
    avgUphill = 0;
    double cost = 0, up = 0;
    double pre = fp.getResult().cost;
    while (avgUphill == 0) {
        up = 0;
        for (int i = 0; i < times; ++i) {
            fp.perturb();
            fp.pack();
            cost = Cost(fp);
            if (cost - pre > 0) {
                avgUphill += cost - pre;
                pre = cost;
                ++up;
            }
        }
    }
    avgUphill /= up;

}

double Cost(Floorplanner& fp)
{
    double H = fp.curH(), W = fp.curW();
    if (W < H) swap(W, H);
    double fpH = fp.height(), fpW = fp.width();
    if (fpW < fpH) swap(fpW, fpH);
    double dR = H / W - fpH / fpW;
    double a = alpha;
    double b = beta;
    //return a * fp.Area() / normA + (1 - a) * fp.HPWL() / normW;
    return a * fp.Area() / normA + b * fp.HPWL() / normW + (1 - a - b) * dR * dR;
}

void FastSA(Floorplanner& fp)
{
    Result origin = fp.getResult();
    double oriArea = fp.Area();
    
    alpha_base = fp.alpha();
    alpha = alpha_base;
    cerr << "alpha_base: " << alpha_base << endl;

    initN = 3 * fp.nBlock();
    N     = 3 * fp.nBlock();

    initNorm(fp, initN);
    initPerturb(fp, initN);

    // start SA
    ORI_T = -avgUphill / log(P);
    T = ORI_T;
    cerr << "Init T: " << T << endl;
    Result best = fp.getResult();
    Result pre = fp.getResult();
    int reject = 0;
    while (T > T_LOWER_BOUND) {
        ++fplans;
        cerr << "fplans: " << fplans << endl;
        double cost = 0;
        int reject = 0;
        avgCost = 0;
        for (int i = 0; i < N; ++i) {
            fp.perturb();
            fp.pack();
            cost = Cost(fp);
            avgCost += fabs(cost - pre.cost);
            P = min(1., exp(-(cost - pre.cost) / T));
            if (cost < pre.cost || (double)(rand() % 100) / 100 < P) {
                pre = fp.getResult();
                pre.cost = cost;
                if (cost < best.cost) {
                    best.cost = cost;
                    best = fp.getResult();
                    if (fp.curW() <= fp.width() && fp.curH() <= fp.width())
                        ++f_fplans;
                    alpha = alpha_base + (1 - alpha_base) * f_fplans / fplans;
                    fp.restoreResult(best);
                }
            }
            else {
                fp.restoreResult(pre);
                ++reject;
            }
        }
        avgCost /= N;
        if (2 <= fplans && fplans < K) {
            T = ORI_T * avgCost / fplans / C;
        }
        else T = ORI_T * avgCost / fplans;
       
        cerr << "P: " << P << " T: " << T << endl;
        cerr << "Current Cost: " << fp.getResult().cost << endl;
        //cerr << "Reject %: " << (double)reject / N  << endl;
        //if ((double)reject / N > 0.99)  break;

    }
    fp.pack();
    cerr << "Origi Area: " << oriArea << endl;
    cerr << "Final Area: " << fp.Area() << endl;
    cout << "Finish SA..." << endl;
}

}