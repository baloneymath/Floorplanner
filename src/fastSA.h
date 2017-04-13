/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#ifndef _FAST_SA_H
#define _FAST_SA_H

#include "B_Tree.h"
#include <limits>
using namespace std;

const double DBL_MAX = numeric_limits<double>::max();
const double DBL_MIN = numeric_limits<double>::min();

namespace fastSA {

double  T = DBL_MAX;
double  prob = 1; // initial acceptance rate
const int K = 7;

void fastSA(Floorplanner& fp, int& N)
{
    double avgArea = 0, avgHPWL = 0;
    vector<double> cost(N, 0.);
    vector<double> A(N, 0.);
    vector<double> W(N, 0.);
    for (int i = 0; i < N; ++i) { // perturb N times
        fp.perturb();
        A[i] = fp.Area();
        W[i] = fp.HPWL();
        avgArea += A[i];
        avgHPWL += W[i];
    }
    avgArea /= N; avgHPWL /= N;
    
}

}
#endif