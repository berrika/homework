#include <iostream>
#include <vector>
#include "HalfEdgeDataStructure.h"
#include "EulerOperation.h"
using namespace std;

Solid* EulerOperation::mvsf(double x, double y, double z) {
	Solid* solid = new Solid();
	Face* face = new Face();
	Loop* loop = new Loop();
	Vertex* vertex = new Vertex(x, y, z);

	solid->sfaces.push_back(face);
	solid->sorigin = vertex;
	face->fsolid = solid;
	face->floops.push_back(loop);
	loop->lface = face;

	return solid;
}

Edge* EulerOperation::mev(Vertex* startv, Vertex* newv, Loop* loop) {
	Edge* edge = new Edge();
	HalfEdge* he1 = new HalfEdge(startv, newv, edge);
	HalfEdge* he2 = new HalfEdge(newv, startv, edge);
	Solid* solid = loop->lface->fsolid;

	edge->he1 = he1;
	edge->he2 = he2;

	if (loop->ledge == NULL) {
		he1->nxt = he2;
		he2->nxt = he1;
		he1->prv = he2;
		he2->prv = he1;
		loop->ledge = he1;
	} else {
		HalfEdge* tmphe = loop->ledge;
		while (tmphe->endv != startv) tmphe = tmphe->nxt;
		he1->nxt = he2;
		he2->prv = he1;
		he2->nxt = tmphe->nxt;
		tmphe->nxt->prv = he2;
		tmphe->nxt = he1;
		he1->prv = tmphe;
	}
	he1->wloop = loop;
	he2->wloop = loop;

	return edge;
}

Face* EulerOperation::mef(Vertex* startv, Vertex* endv, Face* face) {
	Face* newface = new Face();
	Loop* newloop = new Loop();
	Loop* loop = face->floops[0];
	Solid* solid = face->fsolid;
	Edge* edge = new Edge();
	HalfEdge* he1 = new HalfEdge(startv, endv, edge);
	HalfEdge* he2 = new HalfEdge(endv, startv, edge);
	edge->he1 = he1;
	edge->he2 = he2;
	solid->sfaces.push_back(newface);
	newface->fsolid = solid;
	newface->floops.push_back(newloop);
	newloop->lface = newface;

	HalfEdge* tmphe = loop->ledge;
	HalfEdge* tmphe1;
	HalfEdge* tmphe2;
	HalfEdge* tmphe3;
	while (tmphe->endv != startv) tmphe = tmphe->nxt;
	tmphe1 = tmphe;
	while (tmphe->endv != endv) tmphe = tmphe->nxt;
	tmphe2 = tmphe;
	// 判断新面匹配哪一个面环
	tmphe3 = tmphe->nxt;
	while (tmphe3->endv != endv) tmphe3 = tmphe3->nxt;
	bool flag = false;
	if (tmphe2 == tmphe3)
		flag = true;
	// 将1个环分为2个环
	he1->nxt = tmphe2->nxt;
	tmphe2->nxt->prv = he1;
	he2->nxt = tmphe1->nxt;
	tmphe1->nxt->prv = he2;
	tmphe1->nxt = he1;
	he1->prv = tmphe1;
	tmphe2->nxt = he2;
	he2->prv = tmphe2;
	if (flag) {
		newloop->ledge = he1;
		he1->wloop = newloop;
		loop->ledge = he2;
		he2->wloop = loop;
	} else {
		newloop->ledge = he2;
		he2->wloop = newloop;
		loop->ledge = he1;
		he1->wloop = loop;
	}
	// 更新loop中半边的loop关系
	for (HalfEdge* he = he1->nxt; he != he1; he = he->nxt) {
		he->wloop = he1->wloop;
	}
	for (HalfEdge* he = he2->nxt; he != he2; he = he->nxt) {
		he->wloop = he2->wloop;
	}
	return newface;
}

Loop* EulerOperation::kemr(Edge* edge) {
	HalfEdge* he1 = edge->he1;
	HalfEdge* he2 = edge->he2;
	Loop* newloop = new Loop();
	Loop* loop = he1->wloop;
	Face* face = loop->lface;

	newloop->ledge = he1->nxt;
	loop->ledge = he1->prv;
	face->floops.push_back(newloop);

	he1->prv->nxt = he2->nxt;
	he2->nxt->prv = he1->prv;
	he2->prv->nxt = he1->nxt;
	he1->nxt->prv = he2->prv;
	delete he1;
	delete he2;
	delete edge;

	return newloop;
}

void EulerOperation::kfmrh(Face* bigf, Face* smallf) {
	Loop* small_loop = smallf->floops[0];
	bigf->floops.push_back(small_loop);
	vector<Face*>::iterator it = bigf->fsolid->sfaces.begin();
	while (it != bigf->fsolid->sfaces.end()) {
		if (*it == smallf) {
			bigf->fsolid->sfaces.erase(it);
			break;
		}
		it++;
	}
	delete smallf;
}
