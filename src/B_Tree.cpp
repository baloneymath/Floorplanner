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

	auto ran = [] (int i) -> int { return rand() % i; };
	//random_shuffle(cpBlocks.begin(), cpBlocks.end(), ran);
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
	_yContour.resize(_nBlock, Contour());
}

void B_Tree::pack()
{
	DFSPack(_root);
}

void B_Tree::DFSPack(Node* nn)
{
	placeBlock(nn);
//	for (int c = _conRoot; c != NIL; c = _yContour[c].next) {
//		cerr << _yContour[c].data->name << ' ';
//	} cerr << endl;
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
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, _nBlock - 1);
	int n1 = dis(gen), n2 = dis(gen);
	swapNode(_nodes[n1], _nodes[n2]);
	for (int i = 0; i < 3; ++i) {
		rotateBlock(_nodes[dis(gen)]);
	}
	//deleteNode(_nodes[1]);
}

void B_Tree::rotateBlock(Node* n)
{
	n->data->rotate = true;
}

void B_Tree::insertNode(Node* parent, Node* n, bool isleft)
{
}

void B_Tree::deleteNode(Node* n)
{
}

void B_Tree::swapNode(Node* n1, Node* n2) 
{
	if (n1 == n2) return;
	if (_root == n1) _root = n2;
	else if (_root == n2) _root = n1;
	auto __swapNode  = [] (Node* n1, Node* n2, vector<Node*> _nodes) {
		if (n1->left != NIL) _nodes[n1->left]->parent = n2->id;
		if (n1->right != NIL) _nodes[n1->right]->parent = n2->id;
		if (n1->parent != NIL) {
			Node* p = _nodes[n1->parent];
			if (p->left == n1->id) p->left = n2->id;
			else p->right = n2->id;
		}
		if (n2->left != NIL) _nodes[n2->left]->parent = n1->id;
		if (n2->right != NIL) _nodes[n2->right]->parent = n1->id;
		if (n2->parent != NIL) {
			Node* p = _nodes[n2->parent];
			if (p->left == n2->id) p->left = n1->id;
			else p->right = n1->id;
		}
		swap(n1->parent, n2->parent);
		swap(n1->left, n2->left);
		swap(n1->right, n2->right);
	};
	if (n1->parent != n2->id && n2->parent != n1->id) { // not connected
		__swapNode(n1, n2, _nodes);
	}
	else { // connected node
		if (n2->parent != n1->id) swap(n1, n2);
		bool isleft = n1->left == n2->id? true : false;
		if (n1->left == n2->id) n1->left = NIL;
		else n1->right = NIL;
		__swapNode(n1, n2, _nodes);
		n1->parent = n2->id;
		if (isleft) n2->left = n1->id;
		else n2->right = n1->id;
	}
}
void B_Tree::transplant(Node* u, Node* v)
{
	Node* pu = u->parent == NIL? 0 : _nodes[u->parent];
	Node* lu = u->left == NIL? 0 : _nodes[u->left];
	Node* ru = u->right == NIL? 0 : _nodes[u->right];
	
	u->parent = u->left = u->right = NIL;
	bool isleft = u->id == pu->left? true : false;
	if (pu != 0) {
		if (isleft) pu->left = v->id;
		else pu->right = v->id;
		v->parent = pu->id;	
	}
	else v->parent = NIL;
	if (lu != 0) {
		lu->parent = v->id;
		v->left = lu->id;
	}
	else v->left = NIL;
	if (ru != 0) {
		ru->parent = v->id;
		v->right = ru->id;
	}
	else v->right = NIL;
}

double B_Tree::Area()
{
    double width = 0, height = 0;
    for (auto i : _yContour) {
        double x = i.data->width, y = i.data->height;
        if (i.data->rotate) swap(x, y);
        width = max(width, i.data->leftdown.first + x);
        height = max(height, i.data->leftdown.second + y);
    }
    return width * height;
}


void B_Tree::initResult()
{
	_cur.root = _root;
	_cur.nodes.resize(_nBlock);
	for (int i = 0; i < _nBlock; ++i) {
		_cur.nodes[i] = *_nodes[i];
	}
	// init cost => norm = 1
	_cur.cost = _alpha * Area() + (1 - _alpha) * HPWL();
	_best = _cur;
}

Result B_Tree::storeResult()
{
	Result r;
	r.root = _cur.root;
	r.nodes = _cur.nodes;
	r.cost = _cur.cost;
	return r;
}

void B_Tree::restoreResult(Result& re)
{
	_cur.root = re.root;
	_cur.nodes = re.nodes;
	_cur.cost = re.cost;
}

void B_Tree::keepBestResult(Result& re)
{
	if (_best.cost > re.cost) {
		_best = re;
	}
}