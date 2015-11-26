#include <iostream>
#include <dcel.h>
#include <vector>
#include <algorithm>

using namespace std;
using namespace DCEL;

int main() {
    Vertex v1(0, 0);
    Vertex v2(10, 16);
    Vertex v3(15, 16);

    insert(v1);
    insert(v2);
    insert(v3);

    generateDS();

    std::sort(queue.begin(), queue.end(), [](Vertex *a, Vertex *b) {
        return (a->y < b->y) || (a->y == b->y && a->x > b->x) ;
    });

    Vertex *v = queue.back();

    queue.pop_back();

    cout << v->index << endl;

    for (Vertex *v : queue) {
        cout << v->index << " left: " << v->edgeIn->src->index << " right: " << v->edgeOut->tgt->index << endl;
    }
}

