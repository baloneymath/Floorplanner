/*******************************************
* Author: Hao Chen <b03901016@ntu.edu.tw>  *
* Last Modified: 2017/04/09                *
* CXXFLAGS: c++11                          *
*******************************************/
#include "B_Tree.h"
B_Tree::~B_Tree()
{
	for (auto& i : _nodes) {
		delete i;
	}
}

void B_Tree::init()
{
	_nodes.resize(_nBlock);
	_yContour.resize(_nBlock, Contour());
	vector<Block*> cpBlocks = _blocks;

	//random_shuffle(cpBlocks.begin(), cpBlocks.end());
	//sort(cpBlocks.begin(), cpBlocks.end(), Block::bigger);
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
	DFSPack(_root);
	gnuplot();
}

void B_Tree::DFSPack(Node* nn)
{
	placeBlock(nn);
	for (int c = _conRoot; c != NIL; c = _yContour[c].next) {
		cerr << _yContour[c].data->name << ' ';
	} cerr << endl;
	if (nn->left != NIL) {
		DFSPack(_nodes[nn->left]);
	}
	if (nn->right != NIL) {
		DFSPack(_nodes[nn->right]);
	}
}


void B_Tree::placeBlock(Node* nn)
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
		_conRoot = nn->id;
		_yContour[_conRoot] = Contour(nn->data, NIL, NIL);
	}
	else if (p != 0 && p->left == nn->id) { // nn is left
		Block* pb = p->data;
		double px = pb->rotate? pb->height : pb->width;
		b->leftdown.first = pb->leftdown.first + px;
		b->leftdown.second = updateContour(nn, p->id, true);
	}
	else if (p != 0 && p->right == nn->id) { // nn is right (upper block)
		Block* pb = p->data;
		b->leftdown.first = pb->leftdown.first;
		b->leftdown.second = updateContour(nn, p->id, false);
	}
}

double B_Tree::updateContour(Node* n, int parent, bool isleft)
{
	double maxh = 0, lpos = 0, rpos = 0;
	double lx = n->data->leftdown.first;
	double nx = n->data->rotate? n->data->height : n->data->width;
	double rx = lx + nx;
	int attach = NIL;
	int start = isleft? _yContour[parent].next : parent;
	for (int c = start; c != NIL; c = _yContour[c].next) {
		Block* b = _yContour[c].data;
		double w = b->width, h = b->height;
		if (b->rotate) {
			swap(w, h);
		}
		lpos = b->leftdown.first; rpos = lpos + w;
		if (lpos < rx && rpos <= rx) {
			maxh = max(maxh, b->leftdown.second + h);
		}
		else if (lpos < rx && rpos > rx) {
			maxh = max(maxh, b->leftdown.second + h);
			attach = c;
			break;
		}
		else {
			attach = c;
			break;
		}
	}
	if (start == NIL) { // left most node
		_yContour[parent].next = n->id;
		_yContour[n->id].prev = parent;
		_yContour[n->id].next = NIL;
		_yContour[n->id].data = n->data;
	}
	else {
		int con = isleft? parent : _yContour[parent].prev;
		if (con == NIL) {
			_conRoot = n->id;
		}
		else {
			_yContour[con].next = n->id;
		}
		_yContour[n->id].prev = con;
		_yContour[n->id].next = attach;
		_yContour[n->id].data = n->data;
	}
	return maxh;
}

void B_Tree::perturb()
{

}

void B_Tree::rotateBlock(Node* n)
{
	n->data->rotate = true;
}

void B_Tree::insertNode(Node* parent, Node* n, bool isleft)
{
	if (isleft && parent->left == NIL) {
		parent->left = n->id;
		n->parent = parent->id;
	}
	else if (isleft && parent->left != NIL) {
		Node* ori = _nodes[parent->left];
		ori->parent = n->id;
		random_device rd;
  		mt19937 gen(rd());
		uniform_int_distribution<> dis(1, 100);
		if (dis(gen) % 2 == 0) n->left = ori->id;
		else n->right = ori->id;
		n->parent = parent->id;
	}
	else if (!isleft && parent->right == NIL) {
		parent->right = n->id;
		n->parent = parent->id;
	}
	else if (!isleft && parent->right != NIL) {
		Node* ori = _nodes[parent->right];
		ori->parent = n->id;
		random_device rd;
  		mt19937 gen(rd());
		uniform_int_distribution<> dis(1, 100);
		if (dis(gen) % 2 == 0) n->left = ori->id;
		else n->right = ori->id;
		n->parent = parent->id;
	}
}

void B_Tree::deleteNode(Node* n)
{
	Node* p = _nodes[n->parent];
	if (n->left == NIL && n->right == NIL) { // leaf node
		n->parent = NIL;
		if (n->id == p->left) p->left = NIL;
		else p->right = NIL;
	}
	else if (n->left != NIL && n->right == NIL) {
		Node* child = _nodes[n->left];
		child->parent = n->parent;
		if (n->id == p->left) p->left = child->id;
		else p->right = child->id;
	}
	else if (n->left == NIL && n->right != NIL) {
		Node* child = _nodes[n->right];
		child->parent = n->parent;
		if (n->id == p->left) p->left = child->id;
		else p->right = child->id;
	}
	else if (n->left != NIL && n->right != NIL) {
		random_device rd;
  		mt19937 gen(rd());
		uniform_int_distribution<> dis(1, 100);
		Node* child = 0;
		if (dis(gen) % 2 == 0) {
			child = _nodes[n->left];
			child->parent = n->parent;
			child->right = n->right;
		}
		else {
			child = _nodes[n->right];
			child->parent = n->parent;
			child->left = n->left;
		}
	}
}

void B_Tree::swapNode(Node* n1, Node* n2) 
{
	swap(n1->parent, n2->parent);
	swap(n1->left, n2->left);
	swap(n1->right, n2->right);
}

double B_Tree::Area()
{
    double val = 0;
    double width = 0, height = 0;
    for (auto i : _yContour) {
        double x = i.data->width, y = i.data->height;
        if (i.data->rotate) swap(x, y);
        width = max(width, i.data->leftdown.first + x);
        height = max(height, i.data->leftdown.second + y);
    }
    return width * height;
}