#pragma once
#include <iostream>
#include <fstream>
#include <random>

#include "HalfEdgeDataStructure.h"
#include "EulerOperation.h"
#include "Sweep.h"




class modelingData {
public:
	// 输出顺序也是环的走向
	static void print_coordinate(Solid* solid);

	std::vector<float> printv() {
		return v;
	}

	Solid* solid_pr() {
		return solid;
	}
	Face* build_face();
	vector<float> build_vertex_array(Solid* solid); 
	double sweep_x, sweep_y, sweep_z;
private:
	//构造底面
	Face* face = build_face();
	//扫成
	Solid* solid = Sweep::sweep(face, sweep_x, sweep_y, sweep_z);
	//生成需要绘制的顶点位置属性和颜色属性
	std::vector<float> v = build_vertex_array(solid);
};
