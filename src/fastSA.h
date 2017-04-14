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

extern double  T, ORI_T;
extern double  P; // initial acceptance rate
extern const int K;
extern const int N; // how many tries per iteration
extern const double C;
extern const double T_LOWER_BOUND;
extern double alpha;
extern double alpha_base;
extern double fplans;
extern double f_fplans; // feasable

void fastSA(Floorplanner&);
double Cost(Floorplanner&, double, double, double, double, double, double);

}
#endif