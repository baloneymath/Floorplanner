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

class FastSA {
	public:
		FastSA();
		void initPerturb(Floorplanner&, int);
		void simulate(Floorplanner&);

	private:
		double  T, ORI_T;
		double  P; // initial acceptance rate
		int 	K;
		double 	C;
		double 	T_LOWER_BOUND;
		int 	initN;
		int 	N; // how many tries per iteration
		double 	alpha;
		double 	alpha_base;
		double 	beta;
		double 	fplans;
		double 	f_fplans; // feasable
		double 	avgUphill;
		double 	avgCost;
};



#endif
