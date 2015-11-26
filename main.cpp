#include <iostream>
#include <dcel.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

using namespace std;
using namespace DCEL;

int main() {
    Vertex v1(150.33, 44.53);
    Vertex v2(239.5, 124.53);
    Vertex v3(283.67, 74.53);
    Vertex v4(332.33, 203.0);
    Vertex v5(316.67, 314.47);
    Vertex v6(244.0, 261.97);
    Vertex v7(136.33, 314.47);
    Vertex v8(206.67, 195.0);

    insert(v1);
    insert(v2);
    insert(v3);
    insert(v4);
    insert(v5);
    insert(v6);
    insert(v7);
    insert(v8);

    generateDS();

    std::sort(queue.begin(), queue.end(), [](Vertex *a, Vertex *b) {
        return (a->y < b->y) || (a->y == b->y && a->x > b->x) ;
    });

    typedef set<Edge*, EdgeSortingFunctor> EdgeSet;
    EdgeSortingFunctor treeSortingFunction(std::numeric_limits<double>::max());

    EdgeSet edgeTree(treeSortingFunction);

    map<Edge*, Vertex*> helper;


    double scanlineY;

    while (!queue.empty()) {
        Vertex *v = queue.back();
        queue.pop_back();
        scanlineY = v->y;


        cout << v->index << " ";

        bool regular = true;

        if (isAboveNeighboors(v)) {
            regular = false;
            if (isConvex(v)) {
                cout << "START_VERTEX_TYPE" << endl;
                v->type = START_VERTEX_TYPE;
                edgeTree.insert(v->edgeOut);
                helper[v->edgeOut] = v;
                cout << "INSERTING: " << v->edgeOut->src->index << " - " << v->edgeOut->tgt->index << endl;
            } else {
                cout << "SPLIT_VERTEX_TYPE" << endl;
                v->type = SPLIT_VERTEX_TYPE;

                Edge *edge = *edgeTree.find(v->edgeIn);
                cout << "FOUND: " << edge->src->index << " - " << edge->tgt->index << endl;
                helper[edge] = v;
                edgeTree.insert(v->edgeIn);
                helper[v->edgeIn] = v;
            }
        }
        if (isBelowNeighboors(v)) {
            regular = false;
            if (isConvex(v)) {
                cout << "END_VERTEX_TYPE" << endl;
                v->type = END_VERTEX_TYPE;

                if (helper[v->edgeIn]->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << helper[v->edgeIn]->index << endl;
                }
                edgeTree.erase(v->edgeIn);



            } else {
                cout << "MERGE_VERTEX_TYPE" << endl;
                v->type = MERGE_VERTEX_TYPE;

                if (helper[v->edgeIn]->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << helper[v->edgeIn]->index << endl;
                }
                edgeTree.erase(v->edgeIn);
                cout << "ERASING: " << v->edgeIn->src->index << " - " << v->edgeIn->tgt->index << endl;
                Edge *edge = *edgeTree.find(v->edgeOut);
                cout << "FOUND: " << edge->src->index << " - " << edge->tgt->index << endl;
                if (helper[edge]->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << helper[edge]->index << endl;
                }
                helper[edge] = v;
            }
        }
        if (regular) {
            cout << "REGULAR_VERTEX_TYPE" << endl;
            v->type = REGULAR_VERTEX_TYPE;
            //check
            if (helper[v->edgeIn]) {
                cout << "FOUND: " << helper[v->edgeIn]->index << endl;
                if (helper[v->edgeIn]->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << helper[v->edgeIn]->index << endl;
                }
                edgeTree.erase(v->edgeIn);
                edgeTree.insert(v->edgeOut);
                helper[v->edgeOut] = v;
            } else {
                Edge *edge = *edgeTree.find(v->edgeOut);
                cout << "viush" << endl;
                cout << "FOUND: " << edge->src->index << " - " << edge->tgt->index << endl;
                if (helper[edge]->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << helper[edge]->index << endl;
                }
                helper[edge] = v;
            }
        }
    }
}

