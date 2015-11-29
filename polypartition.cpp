#include <stdio.h>
#include <string.h>
#include <math.h>
#include <list>
#include <algorithm>
#include <set>

using namespace std;

#include "polypartition.h"

#define VERTEX_TYPE_REGULAR 0
#define VERTEX_TYPE_START 1
#define VERTEX_TYPE_END 2
#define VERTEX_TYPE_SPLIT 3
#define VERTEX_TYPE_MERGE 4

Polygon::Polygon() {
	numpoints = 0;
	points = NULL;
}

Polygon::~Polygon() {
    if (points) {
        delete [] points;
    }
}

void Polygon::Clear() {
    if (points) {
        delete [] points;
    }
	numpoints = 0;
	points = NULL;
}

void Polygon::Init(int numpoints) {
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
    numpoints = src.numpoints;
    points = new Vertex[numpoints];
    memcpy(points, src.points, numpoints * sizeof(Vertex));
}

bool PolygonPartition::IsConvex(Vertex& p1, Vertex& p2, Vertex& p3) {
    double tmp;
    tmp = (p3.y - p1.y) * (p2.x - p1.x) - (p3.x - p1.x) * (p2.y - p1.y);
    if (tmp > 0) {
        return 1;
    }
    return 0;
}

int PolygonPartition::MonotonePartition(list<Polygon> *inpolys, list<Polygon> *monotonePolys) {
    list<Polygon>::iterator iter;
	MonotoneVertex *vertices;
    int i, numvertices, vindex, vindex2, newnumvertices, maxnumvertices;
    int polystartindex, polyendindex;
    Polygon *poly;
    MonotoneVertex *v, *v2, *vprev, *vnext;
	ScanLineEdge newedge;
	bool error = false;

	numvertices = 0;
    for (iter = inpolys->begin(); iter != inpolys->end(); iter++) {
		numvertices += iter->GetNumPoints();
	}

    maxnumvertices = numvertices * 3;
	vertices = new MonotoneVertex[maxnumvertices];
	newnumvertices = numvertices;

	polystartindex = 0;
    for (iter = inpolys->begin(); iter != inpolys->end(); iter++) {
		poly = &(*iter);
        polyendindex = polystartindex + poly->GetNumPoints() - 1;
        for (i = 0; i < poly->GetNumPoints(); i++) {
            vertices[i + polystartindex].p = poly->GetPoint(i);
            if (i == 0) {
                vertices[i+polystartindex].previous = polyendindex;
            } else {
                vertices[i+polystartindex].previous = i+polystartindex-1;
            }
            if (i == (poly->GetNumPoints() - 1)) {
                vertices[i+polystartindex].next = polystartindex;
            } else {
                vertices[i+polystartindex].next = i+polystartindex+1;
            }
		}
        polystartindex = polyendindex + 1;
	}

    int *priority = new int[numvertices];
    for (i = 0; i < numvertices; i++) {
        priority[i] = i;
    }
    std::sort(priority, &(priority[numvertices]), VertexSorter(vertices));

	char *vertextypes = new char[maxnumvertices];
    for (i = 0; i < numvertices; i++) {
		v = &(vertices[i]);
		vprev = &(vertices[v->previous]);
		vnext = &(vertices[v->next]);

        if (Below(vprev->p, v->p) && Below(vnext->p, v->p)) {
            if(IsConvex(vnext->p, vprev->p, v->p)) {
                vertextypes[i] = VERTEX_TYPE_START;
			} else {
                vertextypes[i] = VERTEX_TYPE_SPLIT;
			}
        } else if(Below(v->p, vprev->p) && Below(v->p, vnext->p)) {
            if(IsConvex(vnext->p, vprev->p, v->p)) {
                vertextypes[i] = VERTEX_TYPE_END;
			} else {
                vertextypes[i] = VERTEX_TYPE_MERGE;
			}
		} else {
            vertextypes[i] = VERTEX_TYPE_REGULAR;
		}
	}

    int *helpers = new int[maxnumvertices];

	set<ScanLineEdge> edgeTree;
    set<ScanLineEdge>::iterator *edgeTreeIterators, edgeIter;
	edgeTreeIterators = new set<ScanLineEdge>::iterator[maxnumvertices];
    pair<set<ScanLineEdge>::iterator, bool> edgeTreeRet;

    for (i = 0; i < numvertices; i++) {
		vindex = priority[i];
		v = &(vertices[vindex]);
		vindex2 = vindex;
		v2 = v;

		switch(vertextypes[vindex]) {
            case VERTEX_TYPE_START:
				newedge.p1 = v->p;
				newedge.p2 = vertices[v->next].p;
				newedge.index = vindex;
				edgeTreeRet = edgeTree.insert(newedge);
				edgeTreeIterators[vindex] = edgeTreeRet.first;
				helpers[vindex] = vindex;
				break;

            case VERTEX_TYPE_END:
                if(vertextypes[helpers[v->previous]] == VERTEX_TYPE_MERGE) {
                    AddDiagonal(vertices, &newnumvertices, vindex, helpers[v->previous]);
                    vertextypes[newnumvertices - 2] = vertextypes[vindex];
                    edgeTreeIterators[newnumvertices - 2] = edgeTreeIterators[vindex];
                    helpers[newnumvertices - 2] = helpers[vindex];
                    vertextypes[newnumvertices - 1] = vertextypes[helpers[v->previous]];
                    edgeTreeIterators[newnumvertices - 1] = edgeTreeIterators[helpers[v->previous]];
                    helpers[newnumvertices - 1] = helpers[helpers[v->previous]];
				}
				edgeTree.erase(edgeTreeIterators[v->previous]);
				break;

            case VERTEX_TYPE_SPLIT:
                newedge.p1 = v->p;
				newedge.p2 = v->p;
				edgeIter = edgeTree.lower_bound(newedge);
				if(edgeIter == edgeTree.begin()) {
					error = true;
					break;
				}
				edgeIter--;
                AddDiagonal(vertices, &newnumvertices, vindex, helpers[edgeIter->index]);
                vertextypes[newnumvertices - 2] = vertextypes[vindex];
                edgeTreeIterators[newnumvertices - 2] = edgeTreeIterators[vindex];
                helpers[newnumvertices - 2] = helpers[vindex];
                vertextypes[newnumvertices - 1] = vertextypes[helpers[edgeIter->index]];
                edgeTreeIterators[newnumvertices - 1] = edgeTreeIterators[helpers[edgeIter->index]];
                helpers[newnumvertices - 1] = helpers[helpers[edgeIter->index]];
                vindex2 = newnumvertices - 2;
				v2 = &(vertices[vindex2]);
                helpers[edgeIter->index] = vindex;
                newedge.p1 = v2->p;
				newedge.p2 = vertices[v2->next].p;
				newedge.index = vindex2;
				edgeTreeRet = edgeTree.insert(newedge);
				edgeTreeIterators[vindex2] = edgeTreeRet.first;
				helpers[vindex2] = vindex2;
				break;

            case VERTEX_TYPE_MERGE:
                if(vertextypes[helpers[v->previous]] == VERTEX_TYPE_MERGE) {
                    AddDiagonal(vertices, &newnumvertices, vindex, helpers[v->previous]);
                    vertextypes[newnumvertices - 2] = vertextypes[vindex];
                    edgeTreeIterators[newnumvertices - 2] = edgeTreeIterators[vindex];
                    helpers[newnumvertices - 2] = helpers[vindex];
                    vertextypes[newnumvertices - 1] = vertextypes[helpers[v->previous]];
                    edgeTreeIterators[newnumvertices - 1] = edgeTreeIterators[helpers[v->previous]];
                    helpers[newnumvertices - 1] = helpers[helpers[v->previous]];
                    vindex2 = newnumvertices - 2;
					v2 = &(vertices[vindex2]);
				}
				edgeTree.erase(edgeTreeIterators[v->previous]);
				newedge.p1 = v->p;
				newedge.p2 = v->p;
				edgeIter = edgeTree.lower_bound(newedge);
				if(edgeIter == edgeTree.begin()) {
					error = true;
					break;
				}
				edgeIter--;
                if(vertextypes[helpers[edgeIter->index]] == VERTEX_TYPE_MERGE) {
                    AddDiagonal(vertices, &newnumvertices, vindex2, helpers[edgeIter->index]);
                    vertextypes[newnumvertices - 2] = vertextypes[vindex2];
                    edgeTreeIterators[newnumvertices - 2] = edgeTreeIterators[vindex2];
                    helpers[newnumvertices - 2] = helpers[vindex2];
                    vertextypes[newnumvertices - 1] = vertextypes[helpers[edgeIter->index]];
                    edgeTreeIterators[newnumvertices - 1] = edgeTreeIterators[helpers[edgeIter->index]];
                    helpers[newnumvertices - 1] = helpers[helpers[edgeIter->index]];
				}
				helpers[edgeIter->index] = vindex2;
				break;

            case VERTEX_TYPE_REGULAR:
                if(Below(v->p, vertices[v->previous].p)) {
                    if(vertextypes[helpers[v->previous]] == VERTEX_TYPE_MERGE) {
                        AddDiagonal(vertices, &newnumvertices, vindex, helpers[v->previous]);
                        vertextypes[newnumvertices - 2] = vertextypes[vindex];
                        edgeTreeIterators[newnumvertices - 2] = edgeTreeIterators[vindex];
                        helpers[newnumvertices - 2] = helpers[vindex];
                        vertextypes[newnumvertices - 1] = vertextypes[helpers[v->previous]];
                        edgeTreeIterators[newnumvertices - 1] = edgeTreeIterators[helpers[v->previous]];
                        helpers[newnumvertices - 1] = helpers[helpers[v->previous]];
                        vindex2 = newnumvertices - 2;
						v2 = &(vertices[vindex2]);
					}
					edgeTree.erase(edgeTreeIterators[v->previous]);
                    newedge.p1 = v2->p;
					newedge.p2 = vertices[v2->next].p;
					newedge.index = vindex2;
					edgeTreeRet = edgeTree.insert(newedge);
					edgeTreeIterators[vindex2] = edgeTreeRet.first;
					helpers[vindex2] = vindex;
				} else {
                    newedge.p1 = v->p;
					newedge.p2 = v->p;
					edgeIter = edgeTree.lower_bound(newedge);
					if(edgeIter == edgeTree.begin()) {
						error = true;
						break;
					}
					edgeIter--;
                    if(vertextypes[helpers[edgeIter->index]] == VERTEX_TYPE_MERGE) {
                        AddDiagonal(vertices, &newnumvertices, vindex, helpers[edgeIter->index]);
                        vertextypes[newnumvertices - 2] = vertextypes[vindex];
                        edgeTreeIterators[newnumvertices - 2] = edgeTreeIterators[vindex];
                        helpers[newnumvertices - 2] = helpers[vindex];
                        vertextypes[newnumvertices - 1] = vertextypes[helpers[edgeIter->index]];
                        edgeTreeIterators[newnumvertices - 1] = edgeTreeIterators[helpers[edgeIter->index]];
                        helpers[newnumvertices - 1] = helpers[helpers[edgeIter->index]];
					}
					helpers[edgeIter->index] = vindex;
				}
				break;
		}
		if(error) break;
	}

	char *used = new char[newnumvertices];
    memset(used, 0, newnumvertices * sizeof(char));

	if(!error) {
        int size;
        Polygon mpoly;
        for (i = 0; i < newnumvertices; i++) {
            if(used[i]) {
                continue;
            }
			v = &(vertices[i]);
			vnext = &(vertices[v->next]);
			size = 1;
            while(vnext != v) {
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
            while(vnext != v) {
				mpoly[size] = vnext->p;
				used[vnext->next] = 1;
				vnext = &(vertices[vnext->next]);
				size++;
			}
			monotonePolys->push_back(mpoly);
		}
	}

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

void PolygonPartition::AddDiagonal(MonotoneVertex *vertices, int *numvertices, int index1, int index2) {

    int newindex1, newindex2;

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

bool PolygonPartition::VertexSorter::operator() (int index1, int index2) {
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

int PolygonPartition::TriangulateMonotone(Polygon *inPoly, list<Polygon> *triangles) {
    int i, i2, j, topindex, bottomindex, leftindex, rightindex, vindex;
    Vertex *points;
    int numpoints;
    Polygon triangle;

	numpoints = inPoly->GetNumPoints();
	points = inPoly->GetPoints();

    if(numpoints < 3) {
        return 0;
    }

	if(numpoints == 3) {
		triangles->push_back(*inPoly);
	}

    topindex = 0;
    bottomindex = 0;
    for (i = 1; i < numpoints; i++) {
        if(Below(points[i], points[bottomindex])) {
            bottomindex = i;
        }
        if(Below(points[topindex], points[i])) {
            topindex = i;
        }
	}

	i = topindex;
    while(i != bottomindex) {
        i2 = i+1;
        if(i2 >= numpoints) {
            i2 = 0;
        }
        if(!Below(points[i2], points[i])) {
            return 0;
        }
		i = i2;
	}
	i = bottomindex;
    while(i != topindex) {
        i2 = i+1;
        if(i2 >= numpoints) {
            i2 = 0;
        }
        if(!Below(points[i], points[i2])) {
            return 0;
        }
		i = i2;
	}

	char *vertextypes = new char[numpoints];
    int *priority = new int[numpoints];

	priority[0] = topindex;
	vertextypes[topindex] = 0;
    leftindex = topindex+1;
    if(leftindex >= numpoints) {
        leftindex = 0;
    }
    rightindex = topindex-1;
    if(rightindex < 0) {
        rightindex = numpoints-1;
    }
    for (i = 1; i < (numpoints - 1); i++) {
        if(leftindex == bottomindex) {
			priority[i] = rightindex;
            rightindex--;
            if(rightindex < 0) {
                rightindex = numpoints - 1;
            }
			vertextypes[priority[i]] = -1;
        } else if(rightindex == bottomindex) {
			priority[i] = leftindex;
            leftindex++;
            if(leftindex >= numpoints) {
                leftindex = 0;
            }
			vertextypes[priority[i]] = 1;
		} else {
            if(Below(points[leftindex], points[rightindex])) {
				priority[i] = rightindex;
                rightindex--;
                if(rightindex < 0) {
                    rightindex = numpoints-1;
                }
				vertextypes[priority[i]] = -1;
			} else {
				priority[i] = leftindex;
                leftindex++;
                if(leftindex >= numpoints) {
                    leftindex = 0;
                }
				vertextypes[priority[i]] = 1;			
			}
		}
	}
	priority[i] = bottomindex;
	vertextypes[bottomindex] = 0;

    int *stack = new int[numpoints];
    int stackptr = 0;

	stack[0] = priority[0];
	stack[1] = priority[1];
	stackptr = 2;

    for (i = 2; i < (numpoints - 1); i++) {
		vindex = priority[i];
        if(vertextypes[vindex] != vertextypes[stack[stackptr - 1]]) {
            for (j = 0; j < (stackptr - 1); j++) {
                if(vertextypes[vindex] == 1) {
                    triangle.Triangle(points[stack[j + 1]], points[stack[j]], points[vindex]);
				} else {
                    triangle.Triangle(points[stack[j]], points[stack[j + 1]], points[vindex]);
				}
				triangles->push_back(triangle);
			}
            stack[0] = priority[i - 1];
			stack[1] = priority[i];
			stackptr = 2;
		} else {
			stackptr--;
            while(stackptr > 0) {
                if(vertextypes[vindex] == 1) {
                    if(IsConvex(points[vindex], points[stack[stackptr - 1]], points[stack[stackptr]])) {
                        triangle.Triangle(points[vindex], points[stack[stackptr - 1]], points[stack[stackptr]]);
						triangles->push_back(triangle);
						stackptr--;
					} else {
						break;
					}
				} else {
                    if(IsConvex(points[vindex], points[stack[stackptr]], points[stack[stackptr - 1]])) {
                        triangle.Triangle(points[vindex], points[stack[stackptr]], points[stack[stackptr - 1]]);
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
    for (j = 0; j < (stackptr - 1); j++) {
        if(vertextypes[stack[j + 1]] == 1) {
            triangle.Triangle(points[stack[j]], points[stack[j + 1]], points[vindex]);
		} else {
            triangle.Triangle(points[stack[j + 1]], points[stack[j]], points[vindex]);
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

    if(!MonotonePartition(inpolys, &monotone)) {
        return 0;
    }
    for (iter = monotone.begin(); iter != monotone.end(); iter++) {
        if(!TriangulateMonotone(&(*iter), triangles)) {
            return 0;
        }
    }
    return 1;
}

int PolygonPartition::Triangulate_MONO(Polygon *poly, list<Polygon> *triangles) {
    list<Polygon> polys;
	polys.push_back(*poly);

	return Triangulate_MONO(&polys, triangles);
}
