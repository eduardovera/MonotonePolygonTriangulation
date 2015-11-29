#include <list>
using namespace std;

//2D point structure
struct Vertex {

    double x;
    double y;

    int index;

    Vertex() {

    }

    Vertex(double x, double y) {
        this->x = x;
        this->y = y;
    }

    Vertex(double x, double y, int index) {
        this->x = x;
        this->y = y;
        this->index = index;
    }
};

class Polygon {
    public:

        Vertex *points;
        int numpoints;

        Polygon();
        ~Polygon();

        Polygon(const Polygon &src);

        int GetNumPoints() {
            return numpoints;
        }

        Vertex &GetPoint(int i) {
            return points[i];
        }

        Vertex *GetPoints() {
            return points;
        }

        Vertex& operator[] (int i) {
            return points[i];
        }

        void Clear();

        void Init(int numpoints);

        void Triangle(Vertex &p1, Vertex &p2, Vertex &p3);

};

class PolygonPartition {
    protected:

        struct MonotoneVertex {
            Vertex p;
            int previous;
            int next;
        };

        class VertexSorter{
            MonotoneVertex *vertices;
            public:
                VertexSorter(MonotoneVertex *v) : vertices(v) {}
                bool operator() (int index1, int index2);
        };

        struct ScanLineEdge {
            int index;
            Vertex p1;
            Vertex p2;

            bool operator< (const ScanLineEdge & other) const;

            bool IsConvex(const Vertex& p1, const Vertex& p2, const Vertex& p3) const;
        };

        bool IsConvex(Vertex& p1, Vertex& p2, Vertex& p3);
        bool Below(Vertex &p1, Vertex &p2);
        void AddDiagonal(MonotoneVertex *vertices, int *numvertices, int index1, int index2);

        int TriangulateMonotone(Polygon *inPoly, list<Polygon> *triangles);

    public:

        //triangulates a polygons by firstly partitioning it into monotone polygons
        //time complexity: O(n*log(n)), n is the number of vertices
        //space complexity: O(n)
        //params:
        //   poly : an input polygon to be triangulated
        //          vertices have to be in counter-clockwise order
        //   triangles : a list of triangles (result)
        //returns 1 on success, 0 on failure
        int Triangulate_MONO(Polygon *poly, list<Polygon> *triangles);

        //triangulates a list of polygons by firstly partitioning them into monotone polygons
        //time complexity: O(n*log(n)), n is the number of vertices
        //space complexity: O(n)
        //params:
        //   inpolys : a list of polygons to be triangulated (can contain holes)
        //             vertices of all non-hole polys have to be in counter-clockwise order
        //             vertices of all hole polys have to be in clockwise order
        //   triangles : a list of triangles (result)
        //returns 1 on success, 0 on failure
        int Triangulate_MONO(list<Polygon> *inpolys, list<Polygon> *triangles);

        //creates a monotone partition of a list of polygons that can contain holes
        //time complexity: O(n*log(n)), n is the number of vertices
        //space complexity: O(n)
        //params:
        //   inpolys : a list of polygons to be triangulated (can contain holes)
        //             vertices of all non-hole polys have to be in counter-clockwise order
        //             vertices of all hole polys have to be in clockwise order
        //   monotonePolys : a list of monotone polygons (result)
        //returns 1 on success, 0 on failure
        int MonotonePartition(list<Polygon> *inpolys, list<Polygon> *monotonePolys);

};
