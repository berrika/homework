#pragma once

#include "HalfEdgeDataStructure.h"
using namespace std;

class EulerOperation {

private:

public:
	// 新建体、面和环，新建点
	static Solid* mvsf(double x, double y, double z);
	// 基于vertex建立新点，同时连接新点与指定点v1，形成一条边
	static Edge* mev(Vertex* startv, Vertex* newv, Loop* loop);
	// 将loop中不相邻的v1和v2连接起来，形成新的面加入solid中
	static Face* mef(Vertex* startv, Vertex* endv, Face* face);
	// 在loop1中去除v1和v2连成的边，断开后将形成的loop2也加入面中
	static Loop* kemr(Edge* edge);
	// 删除loop所在的面，将loop加入到outterloop所在的面
	static void kfmrh(Face* bigf, Face* smallf);
};
