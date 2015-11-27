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

        Vertex *helper;

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

    bool isAboveNeighboors(const Vertex *current) {
        const Vertex *prior = current->edgeIn->src;
        const Vertex *next = current->edgeOut->tgt;
        return (current->y > prior->y || (current->y == prior->y && current->x < prior->x)) && (current->y > next->y || (current->y == next->y && current->x < next->x));
    }

    bool isBelowNeighboors(const Vertex *current) {
        const Vertex *prior = current->edgeIn->src;
        const Vertex *next = current->edgeOut->tgt;
        return (current->y < prior->y || (current->y == prior->y && current->x > prior->x)) && (current->y < next->y || (current->y == next->y && current->x > next->x));
    }

    bool isConvex(const Vertex *current) {
        const Vertex *prior = current->edgeIn->src;
        const Vertex *next = current->edgeOut->tgt;
        return (((prior->y - current->y) * next->x) + ((current->x - prior->x) * next->y) + (((prior->x * current->y) - (prior->y * current->x)))) > 0;
    }

    bool isRight(Vertex *v1, Vertex *v2, Vertex *p) {
        return ((v2->x - v1->x) * (p->y - v1->y) - (v2->y - v1->y) * (p->x - v1->x)) < 0.0;
    }

    struct EdgeSortingFunctor : std::binary_function<Edge*, Edge*, bool> {

        private:
            double m_scanline_y;

        public:
            inline EdgeSortingFunctor(double scanline_y) : m_scanline_y(scanline_y) {

            }

            inline bool operator () (const Edge *edge1, const Edge *edge2) const {
                double x1;
                if (edge1->src->y != edge1->tgt->y) {
                    x1 = (((edge1->src->x - edge1->tgt->x) * m_scanline_y) - (edge1->src->x * edge1->tgt->y) + (edge1->src->y * edge1->tgt->x)) / (edge1->src->y - edge1->tgt->y);
                } else {
                    x1 = std::min(edge1->src->x, edge1->tgt->x);
                }

                double x2;
                if (edge2->src->y != edge2->tgt->y) {
                    x2 = (((edge2->src->x - edge2->tgt->x) * m_scanline_y) - (edge2->src->x * edge2->tgt->y) + (edge2->src->y * edge2->tgt->x)) / (edge2->src->y - edge2->tgt->y);
                } else {
                    x2 = std::min(edge2->src->x, edge2->tgt->x);
                }
                return (x1 > x2);
            }
    };

}
