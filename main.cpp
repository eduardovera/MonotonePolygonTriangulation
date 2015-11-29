#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <polypartition.h>
#include <cassert>

using namespace std;

int main() {

    Polygon polygonA;
    int index = 0;
    polygonA.Init(8);
    polygonA.points[0] = Vertex(150.33, 44.53, index++);
    polygonA.points[1] = Vertex(239.5, 124.53, index++);
    polygonA.points[2] = Vertex(283.67, 74.53, index++);
    polygonA.points[3] = Vertex(332.33, 203.0, index++);
    polygonA.points[4] = Vertex(316.67, 314.47, index++);
    polygonA.points[5] = Vertex(244.0, 261.97, index++);
    polygonA.points[6] = Vertex(136.33, 314.47, index++);
    polygonA.points[7] = Vertex(206.67, 197.0, index++);

    Polygon polygonB;
    index = 0;
    polygonB.Init(15);
    polygonB.points[0] = Vertex(424, 174, index++);
    polygonB.points[1] = Vertex(400, 239, index++);
    polygonB.points[2] = Vertex(427, 265, index++);
    polygonB.points[3] = Vertex(251, 219, index++);
    polygonB.points[4] = Vertex(250, 275, index++);
    polygonB.points[5] = Vertex(196, 289, index++);
    polygonB.points[6] = Vertex(127, 303, index++);
    polygonB.points[7] = Vertex(147, 228, index++);
    polygonB.points[8] = Vertex(83, 150, index++);
    polygonB.points[9] = Vertex(172, 169, index++);
    polygonB.points[10] = Vertex(154, 124, index++);
    polygonB.points[11] = Vertex(202, 144, index++);
    polygonB.points[12] = Vertex(230, 86, index++);
    polygonB.points[13] = Vertex(267, 81, index++);
    polygonB.points[14] = Vertex(321, 112, index++);

    list<Polygon> triangles;

    PolygonPartition *partition = new PolygonPartition();
    bool output = partition->Triangulate_MONO(&polygonA, &triangles);
    assert(output == true);

    for (Polygon poly : triangles) {
        cout << "========================" << endl;
        for (int i = 0; i < poly.GetNumPoints(); i++) {
            cout << poly[i].index  << endl;
        }
    }
}

