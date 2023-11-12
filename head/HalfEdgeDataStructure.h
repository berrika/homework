#pragma once
#include <string>
#include <vector>
using namespace std;

struct Vertex;
struct HalfEdge;
struct Edge;
struct Face;
struct Loop;
struct Solid;

struct Vertex {
	double x;
	double y;
	double z;
	vector<HalfEdge*> start_he;
	vector<HalfEdge*> end_he;

	Vertex* prevv;
	Vertex* nextv;

	Vertex() {}

	Vertex(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool operator==(const Vertex& v) {
		return (this->x == v.x && this->y == v.y && this->z == v.z);
	}

	bool operator!=(const Vertex& v) {
		return !(this->x == v.x && this->y == v.y && this->z == v.z);
	}

};

struct HalfEdge {
	Vertex* startv;
	Vertex* endv;
	Loop* wloop;
	Edge* edg;

	HalfEdge* prv;
	HalfEdge* nxt;

	HalfEdge() {}
	
	HalfEdge(Vertex* startv, Vertex* endv, Edge* edg) {
		this->startv = startv;
		this->endv = endv;
		this->edg = edg;
	}
};

struct Edge {
	HalfEdge* he1;
	HalfEdge* he2;

	Edge* preve;
	Edge* nexte;
};

struct Loop {
	Face* lface;
	HalfEdge* ledge;

	Loop* nextl;
	Loop* prevl;
};

struct Face {
	// 第一个存储外环，后边存储内环
	vector<Loop*> floops;
	Solid* fsolid;

	Face* prevf;
	Face* nextf;
};

struct Solid {
	vector<Face*> sfaces;
	Vertex* sorigin;

	Solid* prevs;
	Solid* nexts;
};
