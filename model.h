#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

// Model representing the obj file
class Model
{
private:
	// Vertices array
	std::vector<Vec3f> verts_;
	// Faces array -> A face is a polygon, in our case a 2D triangle make up of 3 vertices (x1,y1) (x2,y2) (x3,y3)
	std::vector<std::vector<int>> faces_;

public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
};

#endif //__MODEL_H__
