#include <iostream>
#include <cassert>
#include <vector>
#include <random>
#include <string>
#include <memory>
#include <limits.h>
using namespace std;


enum COLOR
{
    RED,
    BLACK
};

class Node {
public:
    COLOR color;
    int val;
    shared_ptr<Node>  left;
    shared_ptr<Node>  right;
    shared_ptr<Node>  parent;
    Node(int val_ = 0) :val(val_), color(COLOR::RED) {}
};


class rbtree {
public:
    shared_ptr<Node>  rbtSearch(int val);
    void rbtInsert(int val);
    void rbtDelete(int val);
    void rbtPlot(string filename, shared_ptr<Node>  p);
    bool rbtCheck();
    void startTest(unsigned int epoch);
    void rbtreeInit();
    shared_ptr<Node>  root;
    shared_ptr<Node>  nil;
private:
    void LRotate(shared_ptr<Node>  n);
    void RRotate(shared_ptr<Node>  n);
    shared_ptr<Node>  rbtSearchCore(shared_ptr<Node>  p, int val);
    void rbtInsertFix(shared_ptr<Node>  N);
    void rbtDeleteFix(shared_ptr<Node>  N);
    void rbtTransplant(shared_ptr<Node>  toDetele, shared_ptr<Node>  replace);
    shared_ptr<Node>  rbtSearchNext(shared_ptr<Node>  p);
    void rbtPrintNode(shared_ptr<Node>  p, FILE* fp);
    void rbtPrintTree(shared_ptr<Node>  p, FILE* fp);
    bool rbtCheckRoot(shared_ptr<Node>  p);
    bool rbtCheckRed(shared_ptr<Node>  p);
    int rbtCheckPathLenCore(shared_ptr<Node>  p, bool& equal);
    bool rbtCheckPathLen(shared_ptr<Node>  p);
    bool rbtCheckNil(shared_ptr<Node>  p);
};

void rbtree::rbtreeInit() {
    this->nil = make_shared<Node>();
    this->root = this->nil;
    this->nil->color = BLACK;
    this->nil->parent = this->root;
}

void rbtree::LRotate(shared_ptr<Node>  n) {
    shared_ptr<Node>  p = n->right;
    n->right = p->left;
    if (p->left != this->nil)
        p->left->parent = n;

    p->left = n;
    if (n->parent == this->nil) {
        this->root = p;
    }
    else {
        if (n == n->parent->left) {
            n->parent->left = p;
        }
        else {
            n->parent->right = p;
        }
    }
    p->parent = n->parent;
    n->parent = p;
}

void rbtree::RRotate(shared_ptr<Node>  n) {
    shared_ptr<Node>  p = n->left;
    n->left = p->right;
    if (p->right != this->nil)
        p->right->parent = n;

    p->right = n;
    if (n->parent == this->nil) {
        this->root = p;
    }
    else {
        if (n == n->parent->left) {
            n->parent->left = p;
        }
        else {
            n->parent->right = p;
        }
    }
    p->parent = n->parent;
    n->parent = p;
}

shared_ptr<Node>  rbtree::rbtSearchCore(shared_ptr<Node>  p, int val) {
    if (p == this->nil) return p;
    this->nil->parent = p; // always ptr to insert location node's parent;
    if (p->val == val) {
        return p;
    }
    else if (p->val < val) {
        return rbtSearchCore(p->right, val);
    }
    else {
        return rbtSearchCore(p->left, val);
    }
}

shared_ptr<Node>  rbtree::rbtSearch(int val) {
    shared_ptr<Node>  p = this->root;
    if (p == this->nil) return NULL;
    shared_ptr<Node>  ret = rbtSearchCore(p, val);
    return ret == this->nil ? NULL : ret;
}

void rbtree::rbtInsertFix(shared_ptr<Node>  N) {
    shared_ptr<Node> gParent;
    shared_ptr<Node> uncle;
    shared_ptr<Node> parent;
    while (N->parent->color == RED) {
        parent = N->parent;
        gParent = parent->parent;
        if (parent == gParent->left) {
            uncle = gParent->right;

            if (uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                gParent->color = RED;
                N = gParent;                
            }
            else {
                if (N == parent->right) {
                    LRotate(parent);
                    
                    parent = gParent->left;
                    N = parent->left;
                }
                RRotate(gParent);                
                parent->color = BLACK;
                gParent->color = RED;
             
            }
        }
        else {
            uncle = gParent->left;

            if (uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                gParent->color = RED;
                N = gParent;
                
            }
            else {
                if (N == parent->left) {
                    RRotate(parent);                    
                    parent = gParent->right;
                    N = parent->right;
                }
                LRotate(gParent);                
                parent->color = BLACK;
                gParent->color = RED;
                
            }
        }
    }
    this->root->color = BLACK;   
}

void rbtree::rbtInsert(int val) {
    shared_ptr<Node>  p = rbtSearchCore(this->root, val);
    if (p != this->nil) return; // existed;

    shared_ptr<Node>  insertNode = make_shared<Node>(val);
    insertNode->parent = insertNode->left = insertNode->right = this->nil;

    if (this->root == this->nil) {  // empty tree
        insertNode->color = BLACK;
        this->root = insertNode;
        return;
    }

    shared_ptr<Node>  parent = this->nil->parent;
    insertNode->parent = parent;
    if (parent->val > insertNode->val) {
        parent->left = insertNode;
    }
    else {
        parent->right = insertNode;
    }
  
    if (parent->color == RED) {
        rbtInsertFix(insertNode);
    }
}

void rbtree::rbtDeleteFix(shared_ptr<Node>  N) {
    while (N != this->root && N->color == BLACK) {
        shared_ptr<Node>  parent = N->parent;
        shared_ptr<Node>  brother = N == parent->left ? parent->right : parent->left;
        if (parent->left == N) {//left
            if (brother->color == RED) {
                parent->color = RED;
                brother->color = BLACK;                
                LRotate(parent);                
                brother = parent->right;
            }
            if (brother->left->color == BLACK && brother->right->color == BLACK) {                
                brother->color = RED;                
                N = parent;
            }
            else {
                if (brother->right->color == BLACK) {
                    brother->left->color = BLACK;
                    brother->color = RED;                    
                    RRotate(brother);                    
                    brother = parent->right;
                }
                brother->color = parent->color;
                parent->color = BLACK;
                brother->right->color = BLACK;                
                LRotate(parent);                
                N = this->root;
            }
        }
        else {
            if (brother->color == RED) {
                parent->color = RED;
                brother->color = BLACK;
                RRotate(parent);
                brother = parent->left;
            }
            if (brother->left->color == BLACK && brother->right->color == BLACK) {
                brother->color = RED;
                N = parent;
            }
            else {
                if (brother->left->color == BLACK) {
                    brother->color = RED;
                    brother->right->color = BLACK;
                    LRotate(brother);
                    brother = parent->left;
                }
                brother->left->color = BLACK;
                brother->color = parent->color;
                parent->color = BLACK;
                RRotate(parent);
                N = this->root;
            }
        }
    }
    N->color = BLACK;
    
}

void rbtree::rbtTransplant(shared_ptr<Node>  toDetele, shared_ptr<Node>  replace) {
    if (toDetele->parent == this->nil) {
        this->root = replace;
    }
    else {
        if (toDetele == toDetele->parent->left) {
            toDetele->parent->left = replace;
        }
        else {
            toDetele->parent->right = replace;
        }
    }
    replace->parent = toDetele->parent;
}

shared_ptr<Node>  rbtree::rbtSearchNext(shared_ptr<Node>  p) {
    p = p->right;
    while (p->left != this->nil) {
        p = p->left;
    }
    return p;
}

void rbtree::rbtDelete(int val) {
    shared_ptr<Node>  nodeDelete = rbtSearchCore(this->root, val);
    if (nodeDelete == this->nil) return;
    shared_ptr<Node> replace;
    shared_ptr<Node> replaceChild;
    COLOR origalColor = nodeDelete->color;
    if (nodeDelete->left == this->nil) {
        replaceChild = nodeDelete->right;
        rbtTransplant(nodeDelete, nodeDelete->right);
    }
    else if (nodeDelete->right == this->nil) {
        replaceChild = nodeDelete->left;
        rbtTransplant(nodeDelete, nodeDelete->left);
    }
    else {
        replace = rbtSearchNext(nodeDelete);
        origalColor = replace->color;
        if (replace->parent == nodeDelete) {
            replaceChild = replace->right;
            replaceChild->parent = replace;   // replaceChild could nil
        }
        else {
            rbtTransplant(replace, replace->right);
            replaceChild = replace->right;
            replace->right = nodeDelete->right;
            nodeDelete->right->parent = replace;
        }
        rbtTransplant(nodeDelete, replace);
        replace->left = nodeDelete->left;
        nodeDelete->left->parent = replace;
        replace->color = nodeDelete->color;
    }
    
    if (origalColor == BLACK)
        rbtDeleteFix(replaceChild);
    this->nil->parent = this->root;
}


void rbtree::rbtPrintNode(shared_ptr<Node>  p, FILE* fp) {
    if (p->color == BLACK) {
        fprintf(fp, "node[style=filled,color=black,fontcolor=white];\n");
    }
    else {
        fprintf(fp, "node[style=filled,color=red,fontcolor=white];\n");
    }

    fprintf(fp, "%d [group=%d];\n", p->val, p->val);
    
}

void rbtree::rbtPrintTree(shared_ptr<Node>  p, FILE* fp) {
    if (p == this->nil) return;
    if (p->parent == this->nil) {
        rbtPrintNode(p, fp);
    }
    if (p->left != this->nil) {
        rbtPrintNode(p->left, fp);
        fprintf(fp, "%d -> %d;\n", p->val, p->left->val);
    }
    
    rbtPrintTree(p->left, fp);
    fprintf(fp, "_%d [group=%d, label=\"\",width=0, style=invis];\n", p->val, p->val);
    fprintf(fp, "%d -> _%d [style=invis];\n", p->val, p->val);
    if (p->right != this->nil) {
        rbtPrintNode(p->right, fp);
        fprintf(fp, "%d -> %d;\n", p->val, p->right->val);
    }
    rbtPrintTree(p->right, fp);

    int distance = INT_MAX, leftDistance = 0, rightDistance = 0;
    shared_ptr<Node> leftMax = this->nil;
    shared_ptr<Node> rightMin = this->nil;
    shared_ptr<Node> target;

    if (p->left != this->nil) {
        leftMax = p->left;
        while (leftMax->right != this->nil) {
            leftMax = leftMax->right;
            leftDistance++;
        }
    }
    distance = min(distance, leftDistance);
    target = leftMax;

    if (p->right != this->nil) {
        rightMin = p->right;
        while (rightMin->left != this->nil) {
            rightMin = rightMin->left;
            rightDistance++;
        }
    }
    if (distance > rightDistance) {
        distance = rightDistance;
        target = rightMin;
    }

    if (distance >= 1) {
        fprintf(fp, "{rank=same;_%d;%d};\n", p->val, target->val);
    }
}

void rbtree::rbtPlot(string filename, shared_ptr<Node>  p) {
    FILE* fp;
    fp = fopen(filename.c_str(), "w+");
    fprintf(fp, "digraph G{\n");
    fprintf(fp, "graph [nodesep=0.1];\n");
    fprintf(fp, "graph [dpi=300];\n");
    fprintf(fp, "edge [arrowhead=vee];\n");
    fprintf(fp, "node [shape=circle,width=0.7];\n");
    rbtPrintTree(p, fp);
    fprintf(fp, "}");
    fclose(fp);
}


bool rbtree::rbtCheckRoot(shared_ptr<Node>  p) {
    if (p->color != BLACK) return false;
    return true;
}

bool rbtree::rbtCheckRed(shared_ptr<Node>  p) {
    if (p == this->nil) return true;
    if (p->color == RED) {
        if ((p->left != this->nil && p->left->color == RED)
            || (p->right != this->nil && p->right->color == RED)) {
            rbtPlot(to_string(p->val) + ".dot", p);
            return false;
        }
    }
    return rbtCheckRed(p->left) && rbtCheckRed(p->right);
}

int rbtree::rbtCheckPathLenCore(shared_ptr<Node>  p, bool& equal) {
    if (p == this->nil) return 0;
    int leftLen = rbtCheckPathLenCore(p->left, equal);
    int rightLen = 0;
    if (equal) {
        rightLen = rbtCheckPathLenCore(p->right, equal);
        if (leftLen != rightLen) equal = false;
    }
    return leftLen + (p->color == BLACK ? 1 : 0);
}

bool rbtree::rbtCheckPathLen(shared_ptr<Node>  p) {
    bool ret = true;
    rbtCheckPathLenCore(p, ret);
    return ret;
}

bool rbtree::rbtCheckNil(shared_ptr<Node>  p) {
    if (p->color != BLACK) return false;
    return true;
}

bool rbtree::rbtCheck() {
    assert(rbtCheckRoot(this->root));
    assert(rbtCheckNil(this->nil));
    assert(rbtCheckPathLen(this->root));
    assert(rbtCheckRed(this->root));
    return true;
}

void rbtree::startTest(unsigned int epoch=1000) {
    mt19937 mt(random_device{}());

    vector<int> arr;
    uniform_int_distribution<int> rng{ 1,1000000000 };
    uniform_int_distribution<int> treeSize(100, 10000);

    for (int i = 0; i < epoch; ++i) {
        mt19937 mtRand(mt());
        int size = treeSize(mt);
        printf("Epoch: %d, tree size: %d\n", i + 1, size);

        for (int j = 0; j < size; ++j) { // insert;
            int val = rng(mtRand);
            arr.emplace_back(val);
            rbtInsert(val);
            rbtCheck();
        }

        for (int j = arr.size(); j > 1; --j) { // shuffle
            int idx = mt() % j;
            swap(arr[j - 1], arr[idx]);
        }

        for (int j = 0; j < arr.size(); ++j) { // delete
            rbtDelete(arr[j]);
            rbtCheck();
        }

        arr.clear();
    }
}


void build(rbtree &T) {
    mt19937 mt(46);   
    uniform_int_distribution<int> rng{ 1, 99 };
    for (int j = 0; j < 20; ++j) { // insert;
        int val = rng(mt);
        printf("%d,", val);
        T.rbtInsert(val);
        //rbtCheck();
    }
    printf("\n");
}

int main()
{
    rbtree T;
    T.rbtreeInit();
    T.startTest();   

    build(T);
    T.rbtPlot("rbtree", T.root);

    return 0;
}
