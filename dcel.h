#ifndef DCEL
#define DCEL

#include <iostream>
#include <vector>

namespace DCEL {

    struct Vertex {
        double x;
        double y;

        int index;
    };

    std::vector<Vertex> vertexes;

    struct Edge {
        Vertex *src;
        Vertex *tgt;

        Edge() {

        }

    };

    std::vector<Edge> edges;

    void insert(Vertex &p) {
        p.index = vertexes.size();
        vertexes.push_back(p);
        Edge e()
    }


}

#endif // DCEL

