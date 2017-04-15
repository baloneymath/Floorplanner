/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "fastSA.h"

namespace fastSA {

double  T = 10, ORI_T = 0;
double  P = 0.99; // initial acceptance rate
const int K = 7;
const int initN = 20; // inital perturbs
const int N = 30; // how many tries per iteration
const double C = 200.;
const double T_LOWER_BOUND = 0.0001;
const double C_LOWER_BOUND = 0.0001;
double alpha = 0;
double alpha_base = 0;
double beta = 0.4;
double fplans = 0;
double f_fplans = 0;

void FastSA(Floorplanner& fp)
{
    fp.initResult();
    alpha_base = fp.alpha();
    alpha = alpha_base;
    cerr << "alpha_base: " << alpha_base << endl;
    
    double oriArea = fp.Area();
    Result orires = fp.storeResult();
    
    double avgA = 0, avgW = 0, avgUphill = 0, avgCost = 0;
    vector<double> cost(max(initN, N), 0.);
    vector<double> A(max(initN, N), 0.);
    vector<double> W(max(initN, N), 0.);
    vector<double> R(max(initN, N), 0.);
    vector<Result> result(max(initN, N), Result());
    
    // initial perturb
    for (int i = 0; i < initN; ++i) { // perturb N initN
        fp.perturb();
        fp.pack();
        result[i] = fp.storeResult();
        A[i] = fp.Area();
        W[i] = fp.HPWL();
        R[i] = fp.curH() / fp.curW();
        avgA += A[i];
        avgW += W[i];
    }
    avgA /= initN; avgW /= initN;
    cerr << "avgA: " << avgA << " avgW: " << avgW << endl;
    if (fplans != 0) alpha = alpha_base + (1 - alpha_base) * f_fplans / fplans;
    cerr << "alpha: " << alpha << endl;
    for (int i = 0; i < initN; ++i) { // compute N costs
        cost[i] = Cost(fp, alpha, A[i], avgA, W[i], avgW, R[i]);
        result[i].cost = cost[i];
		cerr << "cost: " << cost[i] << " orires: " << orires.cost << endl;
        if (cost[i] > orires.cost) {
            avgUphill += cost[i];
        }
        avgCost += cost[i];
    }
    avgUphill /= initN; avgCost /= initN;
    cerr << "avgUphill: " << avgUphill << " avgCost: " << avgCost << endl;
    
    // start SA
    while (T > T_LOWER_BOUND) {
        ++fplans;
        cerr << "fplans: " << fplans << endl;
        orires = fp.storeResult();
        for (int i = 0; i < N; ++i) { // perturb N initN
            fp.perturb();
            fp.pack();
            result[i] = fp.storeResult();
            A[i] = fp.Area();
            W[i] = fp.HPWL();
            R[i] = fp.curH() / fp.curW();
            avgA += A[i];
            avgW += W[i];
        }
        avgA /= N; avgW /= N;
        cerr << "avgA: " << avgA << " avgW: " << avgW << endl;
        if (fplans != 0) alpha = alpha_base + (1 - alpha_base) * f_fplans / fplans;
        cerr << "alpha: " << alpha << endl;
        for (int i = 0; i < N; ++i) { // compute N costs
            cost[i] = Cost(fp, alpha, A[i], avgA, W[i], avgW, R[i]);
            result[i].cost = cost[i];
            if (cost[i] > orires.cost) {
                avgUphill += cost[i];
            }
            avgCost += cost[i];
        }
        avgUphill /= N; avgCost /= N;
        cerr << "avgUphill: " << avgUphill << " avgCost: " << avgCost << endl;
    
    
        // adapt temperature
        if (fplans == 1) {
            ORI_T =  -avgUphill / log(P);
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
        P = min(1., exp( - (bestmove.cost - orires.cost) / T));
        cerr << "P: " << P << endl;
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
        cerr << "Current Cost: " << fp.getResult().cost << endl;

        if (fp.Area() <= fp.width() * fp.height()) ++f_fplans;
    }
    fp.pack();
    cerr << "Origi Area: " << oriArea << endl;
    cerr << "Final Area: " << fp.Area() << endl;
}

double Cost(Floorplanner& fp, double alpha, double A, double avgA,
            double W, double avgW, double R)
{
    double Ratio = fp.height() / fp.width();
    double a = alpha;
    double b = fp.beta();
    //return a * A/ avgA + (1- a) * W / avgW;
    return a * A / avgA + b * W / avgW + (1 - a - b) * (R - Ratio) * (R - Ratio);
}

}