/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "B_Tree.h"
const int NIL = -1;

void B_Tree::init()
{
	_nodes.resize(_nBlock);
	_yContour.resize(_nBlock);
	vector<Block*> cpBlocks = _blocks;
	sort(cpBlocks.begin(), cpBlocks.end(), Block::smaller);
	for (int i = 0; i < _nBlock; ++i) {
		int p = (i + 1) / 2 - 1;
		int l = (2 * i + 1 < _nBlock)? 2 * i + 1 : NIL;
		int r = (2 * i + 2 < _nBlock)? 2 * i + 2 : NIL;
		Node* n = new Node(cpBlock[i], p, l, r);
	}
	_root = _nodes[0];
}

void B_Tree::pack()
{
	_root->data->leftdown = make_pair(0, 0);
	for (int i = 0; i < _nBlock; ++i) {
	}
}

void B_Tree::perturb()
{

}