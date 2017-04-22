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

int B_Tree::getRand()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, _nBlock - 1);
    return dis(gen);
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
    _conRoot = NIL;
    _yContour.clear();
    _yContour.resize(_nBlock, Contour());
}

void B_Tree::pack()
{
    unpack();
    DFSPack(_root);
    updateCurrent();
}

void B_Tree::DFSPack(Node* nn)
{
    placeBlock(nn);
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
    int foo = rand() % 3;
    switch (foo) {
        case 0:
            rotateBlock(_nodes[getRand()]);
            break;
        case 1:
            {
                int c = getRand(), p = getRand();
                while (p == c) p = getRand();
                deleteNode(_nodes[c]);
                insertNode(_nodes[p], _nodes[c]);
                break;
            }
        case 2:
            {
                int c = getRand(), p = getRand();
                while (p == c) p = getRand();
                swapNode(_nodes[c], _nodes[p]);
                break;
            }
    }
}

void B_Tree::rotateBlock(Node* n)
{
    n->data->rotate ^= 1;
}

void B_Tree::insertNode(Node* parent, Node* n)
{
    if (parent == n) return;
    n->parent = parent->id;
    bool pickleft = getRand() % 2;
    int old = pickleft? parent->left : parent->right;
    if (old != NIL) _nodes[old]->parent = n->id;
    if (pickleft) parent->left = n->id;
    else parent->right = n->id;
    if (getRand() % 2) {
        n->left = old;
        n->right = NIL;
    }
    else {
        n->left = NIL;
        n->right = old;
    }
}


void B_Tree::deleteNode(Node* n)
{
    if (n->isleaf()) {
        if (n->id == _nodes[n->parent]->left)
            _nodes[n->parent]->left = NIL;
        else _nodes[n->parent]->right = NIL;
        n->left = n->right = n->parent = NIL;
    }
    else if (n->left != NIL && n->right == NIL) {
        _nodes[n->left]->parent = n->parent;
        if (n->parent == NIL) _root = _nodes[n->left];
        else {
            if (n->id == _nodes[n->parent]->left)
                _nodes[n->parent]->left = n->left;
            else _nodes[n->parent]->right = n->left;
        }
        n->left = n->right = n->parent = NIL;
    }
    else if (n->left == NIL && n->right != NIL) {
        _nodes[n->right]->parent = n->parent;
        if (n->parent == NIL) _root = _nodes[n->right];
        else {
            if (n->id == _nodes[n->parent]->left)
                _nodes[n->parent]->left = n->right;
            else _nodes[n->parent]->right = n->right;
        }
        n->left = n->right = n->parent = NIL;
    }
    else {
        bool pickleft = rand() % 2;
        if (pickleft) swapNode(n, _nodes[n->left]);
        else swapNode(n, _nodes[n->right]);
        deleteNode(n);
    }
}

void B_Tree::__swapNode(Node* n1, Node* n2)
{
    if (n1->left != NIL) _nodes[n1->left]->parent = n2->id;
    if (n1->right != NIL) _nodes[n1->right]->parent = n2->id;
    if (n2->left != NIL) _nodes[n2->left]->parent = n1->id;
    if (n2->right != NIL) _nodes[n2->right]->parent = n1->id;
    if (n1->parent != n1->id) {
        if (n1->parent != NIL) {
            Node* p = _nodes[n1->parent];
            if (p->left == n1->id) p->left = n2->id;
            else p->right = n2->id;
        }
        else _root = _nodes[n2->id];
    }
    if (n2->parent != n2->id) {
        if (n2->parent != NIL) {
            Node* p = _nodes[n2->parent];
            if (p->left == n2->id) p->left = n1->id;
            else p->right = n1->id;
        }
        else _root = _nodes[n1->id];
    }
    swap(n1->parent, n2->parent);
    swap(n1->left, n2->left);
    swap(n1->right, n2->right);
}

void B_Tree::swapNode(Node* n1, Node* n2) 
{
    if (n1 == n2) return;
    if (n1->parent != n2->id && n2->parent != n1->id) { // not connected
        __swapNode(n1, n2);
    }
    else { // connected node
        bool parentn1 = n1->id == n2->parent? true : false;
        bool isleft;
        if (parentn1) {
            if (n1->left == n2->id) {
                n1->left = NIL;
                isleft = true;
            }
            else {
                n1->right = NIL;
                isleft = false;
            }
            n2->parent = n2->id;
        }
        else {
            if (n2->left == n1->id) {
                n2->left = NIL;
                isleft = true;
            }
            else {
                n2->right = NIL;
                isleft = false;
            }
            n1->parent = n1->id;
        }
        __swapNode(n1, n2);
        if (parentn1) {
            n1->parent = n2->id;
            if (isleft) n2->left = n1->id;
            else n2->right = n1->id;
        }
        else {
            n2->parent = n1->id;
            if (isleft) n1->left = n2->id;
            else n1->right = n2->id;
        }
    }
}

void B_Tree::updateCurrent()
{
    double width = 0, height = 0;
    for (auto i : _yContour) {
        double x = i.data->width, y = i.data->height;
        if (i.data->rotate) swap(x, y);
        width = max(width, i.data->leftdown.first + x);
        height = max(height, i.data->leftdown.second + y);
    }
    _curW = width;
    _curH = height;
    _curA = width * height;
}

void B_Tree::initResult()
{
    _cur.root = *_root;
    _cur.nodes.resize(_nBlock);
    _cur.rotates.resize(_nBlock);
    for (int i = 0; i < _nBlock; ++i)
        _cur.nodes[i] = *_nodes[i];
    for (int i = 0; i < _nBlock; ++i)
        _cur.rotates[i] = _nodes[i]->data->rotate;
    // init cost => norm = 1
    _cur.cost = 1;
    _best = _cur;
}

void B_Tree::keepCurResult()
{
    _cur.root = *_root;
    for (int i = 0; i < _nBlock; ++i)
        _cur.nodes[i] = *_nodes[i];
    for (int i = 0; i < _nBlock; ++i)
        _cur.rotates[i] = _nodes[i]->data->rotate;
    _cur.cost = Cost();
}

void B_Tree::keepBestResult()
{
    _best.root = *_root;
    for (int i = 0; i < _nBlock; ++i)
        _best.nodes[i] = *_nodes[i];
    for (int i = 0; i < _nBlock; ++i)
        _best.rotates[i] = _nodes[i]->data->rotate;
    _best.cost = Cost();
}

void B_Tree::recoverCur()
{
    for (int i = 0; i < _nBlock; ++i)
        *_nodes[i] = _cur.nodes[i];
    for (int i = 0; i < _nBlock; ++i)
        _nodes[i]->data->rotate = _cur.rotates[i];
    _root = _nodes[_cur.root.id];
    updateCurrent();
}

void B_Tree::recoverBest()
{
    for (int i = 0; i < _nBlock; ++i)
        *_nodes[i] = _best.nodes[i];
    for (int i = 0; i < _nBlock; ++i)
        _nodes[i]->data->rotate = _best.rotates[i];
    _root = _nodes[_best.root.id];
    updateCurrent();
}