/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#ifndef _FLOORPLANNER_H
#define _FLOORPLANNER_H

#include "chip.h"
#include "B_Tree.h"
using namespace std;


class Floorplanner {
    public:
        Floorplanner() {}
    private:
        Chip*       _chip;
        B_Tree*     _btree;
};

#endif