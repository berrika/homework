#include "ModelingData.h"


void modelingData::print_coordinate(Solid* solid)
{
	{
		vector<Face*> faces = solid->sfaces;
		for (int i = 0; i < faces.size(); i++) {
			cout << "face[" << i << "]" << endl;
			vector<Loop*> loops = faces[i]->floops;
			for (int j = 0; j < loops.size(); j++) {
				cout << "face[" << i << "] -> " << "loop[" << j << "]" << endl;
				Loop* loop = loops[j];
				HalfEdge* he = loop->ledge;
				HalfEdge* src_edge = he;
				cout << "[" << he->startv->x << ", " << he->startv->y << ", " << he->startv->z << "]  ";
				he = he->nxt;
				while (he != src_edge) {
					cout << "[" << he->startv->x << ", " << he->startv->y << ", " << he->startv->z << "]  ";
					he = he->nxt;
				}
				cout << endl;
			}
		}
	}
}

Face* modelingData::build_face()
{
	fstream in;
	in.open("in.txt");
	int loop_cnt;
	in >> loop_cnt;
	// 外环
	int vtx_cnt;
	in >> vtx_cnt;
	double x, y, z;
	in >> x >> y >> z;
	Solid* solid = EulerOperation::mvsf(x, y, z);
	Face* face = solid->sfaces[0];
	Loop* loop = face->floops[0];
	Vertex* prvv = solid->sorigin;
	for (int i = 1; i < vtx_cnt; i++) {
		in >> x >> y >> z;
		Vertex* newv = new Vertex(x, y, z);
		EulerOperation::mev(prvv, newv, loop);
		prvv = newv;

	}
	EulerOperation::mef(prvv, solid->sorigin, face);
	// 内环
	for (int i = 1; i < loop_cnt; i++) {
		in >> vtx_cnt;
		prvv = solid->sorigin;
		in >> x >> y >> z;
		Vertex* newv = new Vertex(x, y, z);
		Edge* edge = EulerOperation::mev(prvv, newv, loop);
		prvv = edge->he1->endv;
		for (int j = 1; j < vtx_cnt; j++) {
			in >> x >> y >> z;
			Vertex* newv = new Vertex(x, y, z);
			EulerOperation::mev(prvv, newv, loop);
			prvv = newv;
		}
		EulerOperation::mef(prvv, edge->he1->endv, face);
		EulerOperation::kemr(edge);
	}

	in >> sweep_x >> sweep_y >> sweep_z;
	in.close();
	return face;
}

vector<float> modelingData::build_vertex_array(Solid* solid)
{
	vector<float> vertices;
	vector<Face*> faces = solid->sfaces;
	default_random_engine engine;
	uniform_real_distribution<float> float_rand(0.0, 1.0);

	for (int i = 0; i < faces.size(); i++) {
		// 同一个面的点生成同一种rgb颜色
		float r = float_rand(engine);
		float g = float_rand(engine);
		float b = float_rand(engine);

		vector<Loop*> loops = faces[i]->floops;

		// 如果一个面有多个环，就不绘制这个面
		if (loops.size() > 1)
			continue;

		for (int j = 0; j < loops.size(); j++) {

			// 同一个环上的点生成同一种颜色
			r = float_rand(engine);
			g = float_rand(engine);
			b = float_rand(engine);

			Loop* loop = loops[j];
			HalfEdge* he = loop->ledge;
			HalfEdge* src_edge = he;

			vector<float> tmp;

			tmp.push_back((float)he->startv->x);
			tmp.push_back((float)he->startv->y);
			tmp.push_back((float)he->startv->z);
			tmp.push_back(r);
			tmp.push_back(g);
			tmp.push_back(b);
			he = he->nxt;
			while (he != src_edge) {
				tmp.push_back((float)he->startv->x);
				tmp.push_back((float)he->startv->y);
				tmp.push_back((float)he->startv->z);
				tmp.push_back(r);
				tmp.push_back(g);
				tmp.push_back(b);
				he = he->nxt;
			}

			for (int k = 0; k < tmp.size() / 6 - 2; k++) {
				for (int t = 0; t < 6; t++) {
					vertices.push_back(tmp[t]);
				}
				for (int t = (k + 1) * 6; t < (k + 1) * 6 + 12; t++) {
					vertices.push_back(tmp[t]);
				}
			}
		}
	}
	// 规范化，将点的坐标限制在[-1,1]范围内
	float max_val = *max_element(vertices.begin(), vertices.end());
	float min_val = *min_element(vertices.begin(), vertices.end());
	float max_abs = max_val + min_val > 0 ? max_val : -min_val;
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] /= max_abs;
		if ((i + 1) % 3 == 0)
			i += 3;
	}

	return vertices;
}

