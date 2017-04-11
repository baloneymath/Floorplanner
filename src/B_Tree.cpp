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
	vector<Block*> cpBlocks = _blocks;
	sort(cpBlocks.begin(), cpBlocks.end(), Block::bigger);
	for (int i = 0; i < _nBlock; ++i) {
		int p = (i + 1) / 2 - 1;
		int l = (2 * i + 1 < _nBlock)? 2 * i + 1 : NIL;
		int r = (2 * i + 2 < _nBlock)? 2 * i + 2 : NIL;
		Node* n = new Node(cpBlocks[i], i, p, l, r);
	}
	_root = _nodes[0];
}

void B_Tree::unpack()
{
    for (auto& i : _blocks) {
        i->leftdown = make_pair(-1, -1);
    }
	_yContour.clear();
}

void B_Tree::pack()
{
	_root->data->leftdown = make_pair(0, 0);
	double rx = _root->data->rotate? _root->data->height : _root->data->width;
	double ry = _root->data->rotate? _root->data->width : _root->data->height;
	_yContour.push_back(_root->data);
	_contourMap[_root->id] = 0;
}

void B_Tree::perturb()
{

}

void B_Tree::recursivePack(Node* nn)
{
	Node* p = (nn->parent == NIL)? 0 : _nodes[nn->parent];
	Node* l = (nn->left == NIL)? 0 : _nodes[nn->left];
	Node* r = (nn->right == NIL)? 0 : _nodes[nn->right];
	
	double px = p->data->rotate? p->data->height : p->data->width;
	double py = p->data->rotate? p->data->width : p->data->height;
	double x = nn->data->rotate? nn->data->height : p->data->width;
	double y = nn->data->rotate? nn->data->width : p->data->height;
	
	nn->data->leftdown.first = p->data->leftdown.first + px;

	
}

double B_Tree::maxContour(Range& ran)
{
}