#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <polypartition.h>

using namespace std;

int main() {

    int index = 0;

    Vertex v1(150.33, 44.53, index++);
    Vertex v2(239.5, 124.53, index++);
    Vertex v3(283.67, 74.53, index++);
    Vertex v4(332.33, 203.0, index++);
    Vertex v5(316.67, 314.47, index++);
    Vertex v6(244.0, 261.97, index++);
    Vertex v7(136.33, 314.47, index++);
    Vertex v8(206.67, 197.0, index++);

    Polygon polygon;
    polygon.Init(8);
    polygon.points[0] = v1;
    polygon.points[1] = v2;
    polygon.points[2] = v3;
    polygon.points[3] = v4;
    polygon.points[4] = v5;
    polygon.points[5] = v6;
    polygon.points[6] = v7;
    polygon.points[7] = v8;

    list<Polygon> input;
    input.push_back(polygon);
    list<Polygon> monotones;

    list<Polygon> triangles;

    PolygonPartition *part = new PolygonPartition();

//    part->MonotonePartition(&input, &monotones);
    cout << "OUTPUT: " << part->Triangulate_MONO(&polygon, &triangles) << endl;

    for (Polygon poly : triangles) {
        cout << "========================" << endl;
        for (int i = 0; i < poly.GetNumPoints(); i++) {
            cout << poly.GetPoint(i).index  << endl;
        }


    }




}

