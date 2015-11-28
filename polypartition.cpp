#include <stdio.h>
#include <string.h>
#include <math.h>
#include <list>
#include <algorithm>
#include <set>

using namespace std;

#include "polypartition.h"

#define TPPL_VERTEXTYPE_REGULAR 0
#define TPPL_VERTEXTYPE_START 1
#define TPPL_VERTEXTYPE_END 2
#define TPPL_VERTEXTYPE_SPLIT 3
#define TPPL_VERTEXTYPE_MERGE 4

Polygon::Polygon() {
	hole = false;
	numpoints = 0;
	points = NULL;
}

Polygon::~Polygon() {
	if(points) delete [] points;
}

void Polygon::Clear() {
	if(points) delete [] points;
	hole = false;
	numpoints = 0;
	points = NULL;
}

void Polygon::Init(long numpoints) {
	Clear();
	this->numpoints = numpoints;
    points = new Vertex[numpoints];
}

void Polygon::Triangle(Vertex &p1, Vertex &p2, Vertex &p3) {
    Init(3);
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
}

Polygon::Polygon(const Polygon &src) {
	hole = src.hole;
	numpoints = src.numpoints;
    points = new Vertex[numpoints];
    memcpy(points, src.points, numpoints*sizeof(Vertex));
}

Polygon& Polygon::operator=(const Polygon &src) {
	Clear();
	hole = src.hole;
	numpoints = src.numpoints;
    points = new Vertex[numpoints];
    memcpy(points, src.points, numpoints*sizeof(Vertex));
	return *this;
}

bool PolygonPartition::IsConvex(Vertex& p1, Vertex& p2, Vertex& p3) {
    double tmp;
    tmp = (p3.y-p1.y)*(p2.x-p1.x)-(p3.x-p1.x)*(p2.y-p1.y);
    if(tmp>0) return 1;
    else return 0;
}

//triangulates a set of polygons by first partitioning them into monotone polygons
//O(n*log(n)) time complexity, O(n) space complexity
//the algorithm used here is outlined in the book
//"Computational Geometry: Algorithms and Applications" 
//by Mark de Berg, Otfried Cheong, Marc van Kreveld and Mark Overmars
int PolygonPartition::MonotonePartition(list<Polygon> *inpolys, list<Polygon> *monotonePolys) {
    list<Polygon>::iterator iter;
	MonotoneVertex *vertices;
	long i,numvertices,vindex,vindex2,newnumvertices,maxnumvertices;
	long polystartindex, polyendindex;
    Polygon *poly;
	MonotoneVertex *v,*v2,*vprev,*vnext;
	ScanLineEdge newedge;
	bool error = false;

	numvertices = 0;
	for(iter = inpolys->begin(); iter != inpolys->end(); iter++) {
		numvertices += iter->GetNumPoints();
	}

	maxnumvertices = numvertices*3;
	vertices = new MonotoneVertex[maxnumvertices];
	newnumvertices = numvertices;

	polystartindex = 0;
	for(iter = inpolys->begin(); iter != inpolys->end(); iter++) {
		poly = &(*iter);
		polyendindex = polystartindex + poly->GetNumPoints()-1;
		for(i=0;i<poly->GetNumPoints();i++) {
			vertices[i+polystartindex].p = poly->GetPoint(i);
			if(i==0) vertices[i+polystartindex].previous = polyendindex;
			else vertices[i+polystartindex].previous = i+polystartindex-1;
			if(i==(poly->GetNumPoints()-1)) vertices[i+polystartindex].next = polystartindex;
			else vertices[i+polystartindex].next = i+polystartindex+1;
		}
		polystartindex = polyendindex+1;
	}

	//construct the priority queue
	long *priority = new long [numvertices];
	for(i=0;i<numvertices;i++) priority[i] = i;
	std::sort(priority,&(priority[numvertices]),VertexSorter(vertices));

	//determine vertex types
	char *vertextypes = new char[maxnumvertices];
	for(i=0;i<numvertices;i++) {
		v = &(vertices[i]);
		vprev = &(vertices[v->previous]);
		vnext = &(vertices[v->next]);

		if(Below(vprev->p,v->p)&&Below(vnext->p,v->p)) {
			if(IsConvex(vnext->p,vprev->p,v->p)) {
				vertextypes[i] = TPPL_VERTEXTYPE_START;
			} else {
				vertextypes[i] = TPPL_VERTEXTYPE_SPLIT;
			}
		} else if(Below(v->p,vprev->p)&&Below(v->p,vnext->p)) {
			if(IsConvex(vnext->p,vprev->p,v->p))
			{
				vertextypes[i] = TPPL_VERTEXTYPE_END;
			} else {
				vertextypes[i] = TPPL_VERTEXTYPE_MERGE;
			}
		} else {
			vertextypes[i] = TPPL_VERTEXTYPE_REGULAR;
		}
	}

	//helpers
	long *helpers = new long[maxnumvertices];

	//binary search tree that holds edges intersecting the scanline
	//note that while set doesn't actually have to be implemented as a tree
	//complexity requirements for operations are the same as for the balanced binary search tree
	set<ScanLineEdge> edgeTree;
	//store iterators to the edge tree elements
	//this makes deleting existing edges much faster
	set<ScanLineEdge>::iterator *edgeTreeIterators,edgeIter;
	edgeTreeIterators = new set<ScanLineEdge>::iterator[maxnumvertices];
	pair<set<ScanLineEdge>::iterator,bool> edgeTreeRet;

	//for each vertex
	for(i=0;i<numvertices;i++) {
		vindex = priority[i];
		v = &(vertices[vindex]);
		vindex2 = vindex;
		v2 = v;

		//depending on the vertex type, do the appropriate action
		//comments in the following sections are copied from "Computational Geometry: Algorithms and Applications"
		switch(vertextypes[vindex]) {
			case TPPL_VERTEXTYPE_START:
				//Insert ei in T and set helper(ei) to vi.
				newedge.p1 = v->p;
				newedge.p2 = vertices[v->next].p;
				newedge.index = vindex;
				edgeTreeRet = edgeTree.insert(newedge);
				edgeTreeIterators[vindex] = edgeTreeRet.first;
				helpers[vindex] = vindex;
				break;

			case TPPL_VERTEXTYPE_END:
				//if helper(ei-1) is a merge vertex
				if(vertextypes[helpers[v->previous]]==TPPL_VERTEXTYPE_MERGE) {
					//Insert the diagonal connecting vi to helper(ei-1) in D.
					AddDiagonal(vertices,&newnumvertices,vindex,helpers[v->previous]);
					vertextypes[newnumvertices-2] = vertextypes[vindex];
					edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
					helpers[newnumvertices-2] = helpers[vindex];
					vertextypes[newnumvertices-1] = vertextypes[helpers[v->previous]];
					edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[v->previous]];
					helpers[newnumvertices-1] = helpers[helpers[v->previous]];
				}
				//Delete ei-1 from T
				edgeTree.erase(edgeTreeIterators[v->previous]);
				break;

			case TPPL_VERTEXTYPE_SPLIT:
				//Search in T to find the edge e j directly left of vi.
				newedge.p1 = v->p;
				newedge.p2 = v->p;
				edgeIter = edgeTree.lower_bound(newedge);
				if(edgeIter == edgeTree.begin()) {
					error = true;
					break;
				}
				edgeIter--;
				//Insert the diagonal connecting vi to helper(ej) in D.
				AddDiagonal(vertices,&newnumvertices,vindex,helpers[edgeIter->index]);
				vertextypes[newnumvertices-2] = vertextypes[vindex];
				edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
				helpers[newnumvertices-2] = helpers[vindex];
				vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
				edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
				helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
				vindex2 = newnumvertices-2;
				v2 = &(vertices[vindex2]);
				//helper(e j)�vi
				helpers[edgeIter->index] = vindex;
				//Insert ei in T and set helper(ei) to vi.
				newedge.p1 = v2->p;
				newedge.p2 = vertices[v2->next].p;
				newedge.index = vindex2;
				edgeTreeRet = edgeTree.insert(newedge);
				edgeTreeIterators[vindex2] = edgeTreeRet.first;
				helpers[vindex2] = vindex2;
				break;

			case TPPL_VERTEXTYPE_MERGE:
				//if helper(ei-1) is a merge vertex
				if(vertextypes[helpers[v->previous]]==TPPL_VERTEXTYPE_MERGE) {
					//Insert the diagonal connecting vi to helper(ei-1) in D.
					AddDiagonal(vertices,&newnumvertices,vindex,helpers[v->previous]);
					vertextypes[newnumvertices-2] = vertextypes[vindex];
					edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
					helpers[newnumvertices-2] = helpers[vindex];
					vertextypes[newnumvertices-1] = vertextypes[helpers[v->previous]];
					edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[v->previous]];
					helpers[newnumvertices-1] = helpers[helpers[v->previous]];
					vindex2 = newnumvertices-2;
					v2 = &(vertices[vindex2]);
				}
				//Delete ei-1 from T.
				edgeTree.erase(edgeTreeIterators[v->previous]);
				//Search in T to find the edge e j directly left of vi.
				newedge.p1 = v->p;
				newedge.p2 = v->p;
				edgeIter = edgeTree.lower_bound(newedge);
				if(edgeIter == edgeTree.begin()) {
					error = true;
					break;
				}
				edgeIter--;
				//if helper(ej) is a merge vertex
				if(vertextypes[helpers[edgeIter->index]]==TPPL_VERTEXTYPE_MERGE) {
					//Insert the diagonal connecting vi to helper(e j) in D.
					AddDiagonal(vertices,&newnumvertices,vindex2,helpers[edgeIter->index]);
					vertextypes[newnumvertices-2] = vertextypes[vindex2];
					edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex2];
					helpers[newnumvertices-2] = helpers[vindex2];
					vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
					edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
					helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
				}
				//helper(e j)�vi
				helpers[edgeIter->index] = vindex2;
				break;

			case TPPL_VERTEXTYPE_REGULAR:
				//if the interior of P lies to the right of vi
				if(Below(v->p,vertices[v->previous].p)) {
					//if helper(ei-1) is a merge vertex
					if(vertextypes[helpers[v->previous]]==TPPL_VERTEXTYPE_MERGE) {
						//Insert the diagonal connecting vi to helper(ei-1) in D.
						AddDiagonal(vertices,&newnumvertices,vindex,helpers[v->previous]);
						vertextypes[newnumvertices-2] = vertextypes[vindex];
						edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
						helpers[newnumvertices-2] = helpers[vindex];
						vertextypes[newnumvertices-1] = vertextypes[helpers[v->previous]];
						edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[v->previous]];
						helpers[newnumvertices-1] = helpers[helpers[v->previous]];
						vindex2 = newnumvertices-2;
						v2 = &(vertices[vindex2]);
					}
					//Delete ei-1 from T.
					edgeTree.erase(edgeTreeIterators[v->previous]);
					//Insert ei in T and set helper(ei) to vi.
					newedge.p1 = v2->p;
					newedge.p2 = vertices[v2->next].p;
					newedge.index = vindex2;
					edgeTreeRet = edgeTree.insert(newedge);
					edgeTreeIterators[vindex2] = edgeTreeRet.first;
					helpers[vindex2] = vindex;
				} else {
					//Search in T to find the edge ej directly left of vi.
					newedge.p1 = v->p;
					newedge.p2 = v->p;
					edgeIter = edgeTree.lower_bound(newedge);
					if(edgeIter == edgeTree.begin()) {
						error = true;
						break;
					}
					edgeIter--;
					//if helper(ej) is a merge vertex
					if(vertextypes[helpers[edgeIter->index]]==TPPL_VERTEXTYPE_MERGE) {
						//Insert the diagonal connecting vi to helper(e j) in D.
						AddDiagonal(vertices,&newnumvertices,vindex,helpers[edgeIter->index]);
						vertextypes[newnumvertices-2] = vertextypes[vindex];
						edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
						helpers[newnumvertices-2] = helpers[vindex];
						vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
						edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
						helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
					}
					//helper(e j)�vi
					helpers[edgeIter->index] = vindex;
				}
				break;
		}

		if(error) break;
	}

	char *used = new char[newnumvertices];
	memset(used,0,newnumvertices*sizeof(char));

	if(!error) {
		//return result
		long size;
        Polygon mpoly;
		for(i=0;i<newnumvertices;i++) {
			if(used[i]) continue;
			v = &(vertices[i]);
			vnext = &(vertices[v->next]);
			size = 1;
			while(vnext!=v) {
				vnext = &(vertices[vnext->next]);
				size++;
			}
			mpoly.Init(size);
			v = &(vertices[i]);
			mpoly[0] = v->p;
			vnext = &(vertices[v->next]);
			size = 1;
			used[i] = 1;
			used[v->next] = 1;
			while(vnext!=v) {
				mpoly[size] = vnext->p;
				used[vnext->next] = 1;
				vnext = &(vertices[vnext->next]);
				size++;
			}
			monotonePolys->push_back(mpoly);
		}
	}

	//cleanup
	delete [] vertices;
	delete [] priority;
	delete [] vertextypes;
	delete [] edgeTreeIterators;
	delete [] helpers;
	delete [] used;

	if(error) {
		return 0;
	} else {
		return 1;
	}
}

//adds a diagonal to the doubly-connected list of vertices
void PolygonPartition::AddDiagonal(MonotoneVertex *vertices, long *numvertices, long index1, long index2) {

    long newindex1, newindex2;

    newindex1 = *numvertices;
    (*numvertices)++;
    newindex2 = *numvertices;
    (*numvertices)++;

    vertices[newindex1].p = vertices[index1].p;
    vertices[newindex2].p = vertices[index2].p;

    vertices[newindex2].next = vertices[index2].next;
    vertices[newindex1].next = vertices[index1].next;

    vertices[vertices[index2].next].previous = newindex2;
    vertices[vertices[index1].next].previous = newindex1;

    vertices[index1].next = newindex2;
    vertices[newindex2].previous = index1;

    vertices[index2].next = newindex1;
    vertices[newindex1].previous = index2;
}

bool PolygonPartition::Below(Vertex &p1, Vertex &p2) {
    if(p1.y < p2.y) {
        return true;
    } else if(p1.y == p2.y) {
        if(p1.x < p2.x) {
            return true;
        }
	}
	return false;
}

//sorts in the falling order of y values, if y is equal, x is used instead
bool PolygonPartition::VertexSorter::operator() (long index1, long index2) {
    if(vertices[index1].p.y > vertices[index2].p.y) {
        return true;
    } else if(vertices[index1].p.y == vertices[index2].p.y) {
        if(vertices[index1].p.x > vertices[index2].p.x) {
            return true;
        }
	}
	return false;
}

bool PolygonPartition::ScanLineEdge::IsConvex(const Vertex& p1, const Vertex& p2, const Vertex& p3) const {
    double tmp;
    tmp = (p3.y - p1.y) * (p2.x - p1.x) - (p3.x - p1.x) * (p2.y - p1.y);
    if (tmp > 0) {
        return 1;
    }
    return 0;
}

bool PolygonPartition::ScanLineEdge::operator < (const ScanLineEdge & other) const {
	if(other.p1.y == other.p2.y) {
		if(p1.y == p2.y) {
            if(p1.y < other.p1.y) {
                return true;
            }
            return false;
		}
        if(IsConvex(p1, p2, other.p1)) {
            return true;
        }
        return false;
	} else if(p1.y == p2.y) {
        if(IsConvex(other.p1, other.p2, p1)) {
            return false;
        }
        return true;
	} else if(p1.y < other.p1.y) {
        if(IsConvex(other.p1, other.p2, p1)) {
            return false;
        }
        return true;
	} else {
        if(IsConvex(p1, p2, other.p1)) {
            return true;
        }
        return false;
	}
}

//triangulates monotone polygon
//O(n) time, O(n) space complexity
int PolygonPartition::TriangulateMonotone(Polygon *inPoly, list<Polygon> *triangles) {
    long i, i2, j, topindex, bottomindex, leftindex, rightindex, vindex;
    Vertex *points;
	long numpoints;
    Polygon triangle;

	numpoints = inPoly->GetNumPoints();
	points = inPoly->GetPoints();

	//trivial calses
	if(numpoints < 3) return 0;
	if(numpoints == 3) {
		triangles->push_back(*inPoly);
	}

	topindex = 0; bottomindex=0;
	for(i=1;i<numpoints;i++) {
		if(Below(points[i],points[bottomindex])) bottomindex = i;
		if(Below(points[topindex],points[i])) topindex = i;
	}

	//check if the poly is really monotone
	i = topindex;
	while(i!=bottomindex) {
		i2 = i+1; if(i2>=numpoints) i2 = 0;
		if(!Below(points[i2],points[i])) return 0;
		i = i2;
	}
	i = bottomindex;
	while(i!=topindex) {
		i2 = i+1; if(i2>=numpoints) i2 = 0;
		if(!Below(points[i],points[i2])) return 0;
		i = i2;
	}

	char *vertextypes = new char[numpoints];
	long *priority = new long[numpoints];

	//merge left and right vertex chains
	priority[0] = topindex;
	vertextypes[topindex] = 0;
	leftindex = topindex+1; if(leftindex>=numpoints) leftindex = 0;
	rightindex = topindex-1; if(rightindex<0) rightindex = numpoints-1;
	for(i=1;i<(numpoints-1);i++) {
		if(leftindex==bottomindex) {
			priority[i] = rightindex;
			rightindex--; if(rightindex<0) rightindex = numpoints-1;
			vertextypes[priority[i]] = -1;
		} else if(rightindex==bottomindex) {
			priority[i] = leftindex;
			leftindex++;  if(leftindex>=numpoints) leftindex = 0;
			vertextypes[priority[i]] = 1;
		} else {
			if(Below(points[leftindex],points[rightindex])) {
				priority[i] = rightindex;
				rightindex--; if(rightindex<0) rightindex = numpoints-1;
				vertextypes[priority[i]] = -1;
			} else {
				priority[i] = leftindex;
				leftindex++;  if(leftindex>=numpoints) leftindex = 0;
				vertextypes[priority[i]] = 1;			
			}
		}
	}
	priority[i] = bottomindex;
	vertextypes[bottomindex] = 0;

	long *stack = new long[numpoints];
	long stackptr = 0;

	stack[0] = priority[0];
	stack[1] = priority[1];
	stackptr = 2;

	//for each vertex from top to bottom trim as many triangles as possible
	for(i=2;i<(numpoints-1);i++) {
		vindex = priority[i];
		if(vertextypes[vindex]!=vertextypes[stack[stackptr-1]]) {
			for(j=0;j<(stackptr-1);j++) {
				if(vertextypes[vindex]==1) {
					triangle.Triangle(points[stack[j+1]],points[stack[j]],points[vindex]);
				} else {
					triangle.Triangle(points[stack[j]],points[stack[j+1]],points[vindex]);
				}
				triangles->push_back(triangle);
			}
			stack[0] = priority[i-1];
			stack[1] = priority[i];
			stackptr = 2;
		} else {
			stackptr--;
			while(stackptr>0) {
				if(vertextypes[vindex]==1) {
					if(IsConvex(points[vindex],points[stack[stackptr-1]],points[stack[stackptr]])) {
						triangle.Triangle(points[vindex],points[stack[stackptr-1]],points[stack[stackptr]]);
						triangles->push_back(triangle);
						stackptr--;
					} else {
						break;
					}
				} else {
					if(IsConvex(points[vindex],points[stack[stackptr]],points[stack[stackptr-1]])) {
						triangle.Triangle(points[vindex],points[stack[stackptr]],points[stack[stackptr-1]]);
						triangles->push_back(triangle);
						stackptr--;
					} else {
						break;
					}
				}
			}
			stackptr++;
			stack[stackptr] = vindex;
			stackptr++;
		}
	}
	vindex = priority[i];
	for(j=0;j<(stackptr-1);j++) {
		if(vertextypes[stack[j+1]]==1) {
			triangle.Triangle(points[stack[j]],points[stack[j+1]],points[vindex]);
		} else {
			triangle.Triangle(points[stack[j+1]],points[stack[j]],points[vindex]);
		}
		triangles->push_back(triangle);
	}

	delete [] priority;
	delete [] vertextypes;
	delete [] stack;

	return 1;
}

int PolygonPartition::Triangulate_MONO(list<Polygon> *inpolys, list<Polygon> *triangles) {
    list<Polygon> monotone;
    list<Polygon>::iterator iter;

	if(!MonotonePartition(inpolys,&monotone)) return 0;
	for(iter = monotone.begin(); iter!=monotone.end();iter++) {
		if(!TriangulateMonotone(&(*iter),triangles)) return 0;
	}
	return 1;
}

int PolygonPartition::Triangulate_MONO(Polygon *poly, list<Polygon> *triangles) {
    list<Polygon> polys;
	polys.push_back(*poly);

	return Triangulate_MONO(&polys, triangles);
}
