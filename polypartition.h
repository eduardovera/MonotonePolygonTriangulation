#include <list>
using namespace std;

#define TPPL_CCW 1
#define TPPL_CW -1

//2D point structure
struct Vertex {

    double x;
    double y;

    int index;

    Vertex() {

    }

    Vertex(double x, double y, int index) {
        this->x = x;
        this->y = y;
        this->index = index;
    }

    Vertex operator + (const Vertex& p) const {
        Vertex r;
		r.x = x + p.x;
		r.y = y + p.y;
		return r;
	}

    Vertex operator - (const Vertex& p) const {
        Vertex r;
		r.x = x - p.x;
		r.y = y - p.y;
		return r;
	}

    Vertex operator * (const double f ) const {
        Vertex r;
		r.x = x*f;
		r.y = y*f;
		return r;
	}

    Vertex operator / (const double f ) const {
        Vertex r;
		r.x = x/f;
		r.y = y/f;
		return r;
	}

    bool operator==(const Vertex& p) const {
		if((x == p.x)&&(y==p.y)) return true;
		else return false;
	}

    bool operator!=(const Vertex& p) const {
		if((x == p.x)&&(y==p.y)) return false;
		else return true;
	}
};

class Polygon {
    public:

        Vertex *points;
        long numpoints;
        bool hole;
        //constructors/destructors
        Polygon();
        ~Polygon();

        Polygon(const Polygon &src);
        Polygon& operator=(const Polygon &src);

        //getters and setters
        long GetNumPoints() {
            return numpoints;
        }

        bool IsHole() {
            return hole;
        }

        void SetHole(bool hole) {
            this->hole = hole;
        }

        Vertex &GetPoint(long i) {
            return points[i];
        }

        Vertex *GetPoints() {
            return points;
        }

        Vertex& operator[] (int i) {
            return points[i];
        }

        //clears the polygon points
        void Clear();

        //inits the polygon with numpoints vertices
        void Init(long numpoints);

        //creates a triangle with points p1,p2,p3
        void Triangle(Vertex &p1, Vertex &p2, Vertex &p3);

};

class PolygonPartition {
    protected:

        struct MonotoneVertex {
            Vertex p;
            long previous;
            long next;
        };

        class VertexSorter{
            MonotoneVertex *vertices;
            public:
                VertexSorter(MonotoneVertex *v) : vertices(v) {}
                bool operator() (long index1, long index2);
        };

        //edge that intersects the scanline
        struct ScanLineEdge {
            long index;
            Vertex p1;
            Vertex p2;

            //determines if the edge is to the left of another edge
            bool operator< (const ScanLineEdge & other) const;

            bool IsConvex(const Vertex& p1, const Vertex& p2, const Vertex& p3) const;
        };

        //standard helper functions
        bool IsConvex(Vertex& p1, Vertex& p2, Vertex& p3);
        bool Below(Vertex &p1, Vertex &p2);
        void AddDiagonal(MonotoneVertex *vertices, long *numvertices, long index1, long index2);

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
