#include <iostream>
#include <vector>
#include <string>

using namespace std;

namespace DCEL {

    enum VertexType {
        START_VERTEX_TYPE,
        END_VERTEX_TYPE,
        SPLIT_VERTEX_TYPE,
        MERGE_VERTEX_TYPE,
        REGULAR_VERTEX_TYPE
    };

    struct Vertex;
    struct Edge;


    struct Vertex {
        double x;
        double y;

        VertexType type;

        int index;

        Edge *edgeIn;
        Edge *edgeOut;

        Vertex(double x, double y) {
            this->x = x;
            this->y = y;
        }
    };

    std::vector<Vertex> vertexes;

    struct Edge {
        Vertex *src;
        Vertex *tgt;

        Edge() {}

        Edge(Vertex *src, Vertex *tgt) {
            this->src = src;
            this->tgt = tgt;
        }

        string printEdge() {
            string content;
            return content;
        }
    };

    std::vector<Edge> edges;
    std::vector<Vertex*> queue;


    void insert(Vertex &p) {
        p.index = vertexes.size();
        vertexes.push_back(p);
    }

    void generateDS() {
        edges.push_back(Edge(&vertexes[vertexes.size() - 1], &vertexes[0]));
        for (size_t i = 1; i < vertexes.size(); i++) {
            edges.push_back(Edge(&vertexes[i - 1], &vertexes[i]));
        }

        for (size_t i = 0; i < vertexes.size() - 1; i++) {
            vertexes[i].edgeIn = &edges[i];
            vertexes[i].edgeOut = &edges[i + 1];
            queue.push_back(&vertexes[i]);
        }
        vertexes[vertexes.size() - 1].edgeIn = &edges[edges.size() - 1];
        vertexes[vertexes.size() - 1].edgeOut = &edges[0];
        queue.push_back(&vertexes[vertexes.size() - 1]);
    }

    bool isAbove(const Vertex *v1, const Vertex *v2) {
        return v1->y > v2->y;
    }

    bool isBelow(const Vertex *v1, const Vertex *v2) {
        return v1->y < v2->y;
    }
}
