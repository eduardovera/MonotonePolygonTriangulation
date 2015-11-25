#include <iostream>
#include <dcel.h>
#include <vector>
#include <algorithm>

using namespace std;
using namespace DCEL;

int main() {
    Vertex v1(0, 0);
    insert(v1);

    Vertex v2(10, 10);
    insert(v2);

    Vertex v3(15, 16);
    insert(v3);

    generateDS();

    std::sort(queue.begin(), queue.end());

    for (Vertex *v : queue) {
        cout << v->index << " left: " << v->edgeIn->src->index << " right: " << v->edgeOut->tgt->index << endl;
    }
}

