#include <iostream>
#include <dcel.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>


using namespace std;
using namespace DCEL;

int main() {

//    Vertex v1(150.33, 44.53);
//    Vertex v2(239.5, 124.53);
//    Vertex v3(283.67, 74.53);
//    Vertex v4(332.33, 203.0);
//    Vertex v5(316.67, 314.47);
//    Vertex v6(244.0, 261.97);
//    Vertex v7(136.33, 314.47);
//    Vertex v8(206.67, 197.0);

//    insert(v1);
//    insert(v2);
//    insert(v3);
//    insert(v4);
//    insert(v5);
//    insert(v6);
//    insert(v7);
//    insert(v8);

    Vertex v1(424, 174);
    Vertex v2(321, 112);
    Vertex v3(267, 81);
    Vertex v4(230, 86);
    Vertex v5(202, 144);
    Vertex v6(154, 124);
    Vertex v7(172, 169);
    Vertex v8(83, 150);
    Vertex v9(147, 228);
    Vertex v10(127, 303);
    Vertex v11(196, 289);
    Vertex v12(250, 275);
    Vertex v13(251, 219);
    Vertex v14(427, 265);
    Vertex v15(400, 239);

    insert(v1);
    insert(v15);
    insert(v14);
    insert(v13);
    insert(v12);
    insert(v11);
    insert(v10);
    insert(v9);
    insert(v8);
    insert(v7);
    insert(v6);
    insert(v5);
    insert(v4);
    insert(v3);
    insert(v2);

    generateDS();

    std::sort(queue.begin(), queue.end(), [](Vertex *a, Vertex *b) {
        return (a->y < b->y) || (a->y == b->y && a->x > b->x) ;
    });

    typedef set<Edge*, EdgeSortingFunctor> EdgeSet;
    EdgeSortingFunctor treeSortingFunction(std::numeric_limits<double>::max());

    EdgeSet edgeTree(treeSortingFunction);

    double scanlineY;

    while (!queue.empty()) {
        Vertex *v = queue.back();
        queue.pop_back();
        scanlineY = v->y;

        bool regular = true;

        cout << v->index << " - ";

        if (isAboveNeighboors(v)) {
            regular = false;
            if (isConvex(v)) {
                cout << "START_VERTEX_TYPE" << endl;
                v->type = START_VERTEX_TYPE;
                v->edgeOut->helper = v;
                edgeTree.insert(v->edgeOut);
//                helper[v->edgeOut] = v;
                cout << "INSERTING: " << v->edgeOut->src->index << " - " << v->edgeOut->tgt->index << endl;
            } else {
                cout << "SPLIT_VERTEX_TYPE" << endl;
                v->type = SPLIT_VERTEX_TYPE;

                Edge *edge = *edgeTree.lower_bound(v->edgeIn);
                cout << "FOUND: " << edge->src->index << " - " << edge->tgt->index << endl;
                cout << "LINKING: " << v->index << " - " << edge->helper->index << endl;
                edge->helper = v;
//                helper[edge] = v;
                v->edgeOut->helper = v;
                edgeTree.insert(v->edgeOut);
//                helper[v->edgeIn] = v;
            }
        }
        if (isBelowNeighboors(v)) {
            regular = false;
            if (isConvex(v)) {
                cout << "END_VERTEX_TYPE" << endl;
                v->type = END_VERTEX_TYPE;

                if (v->edgeIn->helper->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << v->edgeIn->helper->index << endl;
                }
                edgeTree.erase(v->edgeIn);



            } else {
                cout << "MERGE_VERTEX_TYPE" << endl;
                v->type = MERGE_VERTEX_TYPE;

                if (v->edgeIn->helper->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << v->edgeIn->helper->index << endl;
                }
                edgeTree.erase(v->edgeIn);
                cout << "ERASING: " << v->edgeIn->src->index << " - " << v->edgeIn->tgt->index << endl;
                Edge *edge = *edgeTree.lower_bound(v->edgeOut);
                cout << "FOUND: " << edge->src->index << " - " << edge->tgt->index << endl;
                if (edge->helper->type == MERGE_VERTEX_TYPE) {
                    cout << "LINKING: " << v->index << " - " << edge->helper->index << endl;
                }
                edge->helper = v;
            }
        }
        if (regular) {
            cout << "REGULAR_VERTEX_TYPE" << endl;
            v->type = REGULAR_VERTEX_TYPE;
            if (isBelowNeighboors(v)) {
                if (v->edgeIn->helper) {
                    cout << "FOUND: " << v->edgeIn->helper->index << endl;
                    if (v->edgeIn->helper->type == MERGE_VERTEX_TYPE) {
                        cout << "LINKING: " << v->index << " - " << v->edgeIn->helper->index << endl;
                    }
                    edgeTree.erase(v->edgeIn);
                    v->edgeOut->helper = v;
                    edgeTree.insert(v->edgeOut);
                } else {
                    Edge *edge = *edgeTree.lower_bound(v->edgeIn);
                    cout << "FOUND: " << edge->src->index << " - " << edge->tgt->index << endl;
                    if (edge->helper->type == MERGE_VERTEX_TYPE) {
                        cout << "LINKING: " << v->index << " - " << edge->helper->index << endl;
                    }
                    edge->helper = v;
                }
            }
        }
    }
}

