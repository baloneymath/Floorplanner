/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#ifndef _FAST_SA_H
#define _FAST_SA_H

#include "B_Tree.h"
#include <cmath>
#include <limits>
using namespace std;

namespace fastSA {

double  T = DBL_MAX, ORI_T = DBL_MAX;
double  P = 0.99; // initial acceptance rate
const int K = 7;
const int N = 5; // how many tries per iteration
const double C = 100.;
const double T_LOWER_BOUND = 0.00001;

void fastSA(Floorplanner& fp)
{
    const double alpha = fp.alpha();
    double avgA = 0, avgW = 0, avgUphill = 0, avgCost = 0;
    vector<double> cost(N, 0.);
    vector<double> A(N, 0.);
    vector<double> W(N, 0.);
    vector<Result> R(N);
    
    fp.initResult();
    int iteration = 0;
    while (T > T_LOWER_BOUND) {
        ++iteration;
        Result orires = fp.storeResult();
        for (int i = 0; i < N; ++i) { // perturb N times
            fp.perturb();
            fp.unpack();
            fp.pack();
            R[i] = fp.storeResult();
            A[i] = fp.Area();
            W[i] = fp.HPWL();
            avgA += A[i];
            avgW += W[i];
        }
        avgA /= N; avgW /= N;
        
        for (int i = 0; i < N; ++i) { // compute N costs
            cost[i] = alpha * A[i] / avgA + (1 - alpha) * W[i] / avgW;
            R[i].cost = cost[i];
            if (cost[i] > orires.cost) { // uphill cost
                avgUphill += cost[i];
            }
            avgCost += cost[i];
        }
        avgUphill /= N; avgCost /= N;
        
        // adapt temperature
        if (iteration == 1) {
            ORI_T = avgUphill  / log(P);
            T = ORI_T;
        }
        else if (2 <= iteration <= K) {
            T = ORI_T * avgCost / (iteration * C);
        }
        else {
            T = ORI_T * avgCost / iteration;
        }

        Result bestmove = R[0];
        for (int i = 0; i < N; ++i) {
            bestmove = R[i].cost < bestmove.cost? R[i] : bestmove;
        }
        P = min(1., exp((bestmove.cost - orires.cost) / T));
        // decide if accept or not
        if (bestmove.cost - orires.cost > 0) {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(1, 100);

            if ((double)dis(gen) / 100 < P) { // accept
                fp.restoreResult(bestmove);
            }
            else fp.restoreResult(orires);
        }
        else fp.restoreResult(bestmove);
    }
    fp.pack();
}
}
#endif