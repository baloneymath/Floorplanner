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
	// init by complete binary tree
	for (int i = 0; i < _nBlock; ++i) {
		int p = (i + 1) / 2 - 1;
		int l = (2 * i + 1 < _nBlock)? 2 * i + 1 : NIL;
		int r = (2 * i + 2 < _nBlock)? 2 * i + 2 : NIL;
		Node* n = new Node(cpBlocks[i], i, p, l, r);
		_nodes[i] = n;
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
	_current = _yContour.begin();
	DFSPack(_root);
	gnuplot();
}

void B_Tree::perturb()
{

}

void B_Tree::DFSPack(Node* nn)
{	
	placeBlock(nn, _current);
	if (nn->left != NIL) {
		++_current;
		DFSPack(_nodes[nn->left]);
	}
	if (nn->right != NIL) {
		--_current;
		DFSPack(_nodes[nn->right]);
	}
}

void B_Tree::placeBlock(Node* nn, list<Node*>::iterator cur)
{
	Node* p = (nn->parent == NIL)? 0 : _nodes[nn->parent];
	Node* l = (nn->left == NIL)? 0 : _nodes[nn->left];
	Node* r = (nn->right == NIL)? 0 : _nodes[nn->right];
	Block* b = nn->data;
	double w = b->width, h = b->height;
	if (b->rotate) {
		swap(w, h);
	}

	if (p == 0) { // nn is root
		b->leftdown.first = 0;
		b->leftdown.second = 0;
		_yContour.insert(cur, nn);
	}
	else if (p != 0 && p->left == nn->id) { // nn is left
		Block* pb = p->data;
		double px = pb->rotate? pb->height : pb->width;
		b->leftdown.first = px + pb->leftdown.first;
		b->leftdown.second = updateContour(nn, next(cur, 1));
		_yContour.insert(next(cur, 1), nn);
		
	}
	else { // nn is right (upper block)
		Block* pb = p->data;
		double px = pb->rotate? pb->height : pb->width;
		b->leftdown.first = px;
		b->leftdown.second = updateContour(nn, cur);
		_yContour.insert(cur, nn);
	}
}

double B_Tree::updateContour(Node* n, list<Node*>::iterator cur) {
	double maxh = 0, nowx = 0;
	for (auto it = cur; it != _yContour.end(); ++it) {
		Block* tb = (*it)->data;
		double tw = tb->width, th = tb->height;
		if (tb->rotate) {
			swap(tw, th);
		}
		maxh = max(maxh, tb->leftdown.second + th);
		if (nowx + tw <= n->data->width) {
			it = _yContour.erase(it);
		}
		else break;
	}
	return maxh;
}