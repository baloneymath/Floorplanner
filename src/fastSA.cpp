/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "fastSA.h"

namespace fastSA {

double  T = DBL_MAX, ORI_T = DBL_MAX;
double  P = 0.99; // initial acceptance rate
const int K = 7;
const int N = 5; // how many tries per iteration
const double C = 100.;
const double T_LOWER_BOUND = 0.01;
double alpha = 0;
double alpha_base = 0;
double fplans = 0;
double f_fplans = 0;

void fastSA(Floorplanner& fp)
{
    alpha_base = fp.alpha();
    double alpha = alpha_base;
    double avgA = 0, avgW = 0, avgUphill = 0, avgCost = 0;
    vector<double> cost(N, 0.);
    vector<double> A(N, 0.);
    vector<double> W(N, 0.);
    vector<double> R(N, 0.);
    vector<Result> result(N, Result());
    
    fp.initResult();
    while (T > T_LOWER_BOUND) {
        ++fplans;
    cerr << fplans << endl;
        Result orires = fp.storeResult();
        for (int i = 0; i < N; ++i) { // perturb N times
            fp.perturb();
            fp.pack();
            result[i] = fp.storeResult();
            A[i] = fp.Area();
            W[i] = fp.HPWL();
            R[i] = fp.curH() / fp.curW();
            avgA += A[i];
            avgW += W[i];
    cerr << "HAHAHAH" << endl;
        }
        avgA /= N; avgW /= N;
        
        // adapt alpha
        alpha = alpha_base + (1 - alpha_base) * f_fplans / fplans;
        for (int i = 0; i < N; ++i) { // compute N costs
            cost[i] = Cost(fp, alpha, A[i], avgA, W[i], avgW, R[i]);
            result[i].cost = cost[i];
            if (cost[i] > orires.cost) {
                avgUphill += cost[i];
            }
            avgCost += cost[i];
        }
        avgUphill /= N; avgCost /= N;
        
        // adapt temperature
        if (fplans == 1) {
            ORI_T = avgUphill  / log(P);
            T = ORI_T;
        }
        else if (2 <= fplans <= K) {
            T = ORI_T * avgCost / (fplans * C);
        }
        else {
            T = ORI_T * avgCost / fplans;
        }
        Result bestmove = result[0];
        for (int i = 0; i < N; ++i) {
            bestmove = result[i].cost < bestmove.cost? result[i] : bestmove;
        }
        P = min(1., exp((bestmove.cost - orires.cost) / T));
        // decide if accept or not
        if (bestmove.cost - orires.cost > 0) {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(1, 100);

            if ((double)(dis(gen) % 100) / 100 < P) { // accept
                fp.restoreResult(bestmove);
            }
            else fp.restoreResult(orires);
        }
        else fp.restoreResult(bestmove);
        if (fp.Area() <= fp.width() * fp.height()) ++f_fplans;
    }
    fp.pack();
}

double Cost(Floorplanner& fp, double alpha, double A, double avgA,
            double W, double avgW, double R)
{
    double Ratio = fp.height() / fp.width();
    double a = alpha;
    double b = fp.beta();
    return a * A / avgA + b * W / avgW + (1 - a - b) 
            * (R - Ratio) * (R - Ratio);
}

}