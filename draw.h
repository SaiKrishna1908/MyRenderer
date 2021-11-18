#include "tgaimage.h"
#include "geometry.h"

using namespace std;

void drawline(int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color);
void triangle(Vec2i *points, TGAImage &image, TGAColor color);
Vec3f barycentric(Vec2i *pts, Vec2i P);
void triangle_with_line(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);

// Routine to draw lines in a image with given color
void drawline(int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color)
{

	bool steep = false;

	// HEIGHT is less than WIDTH, transpose to get the line with more pixels
	if (abs(x1 - x2) < abs(y1 - y2))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		steep = true;
	}

	if (x2 < x1)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	int dx = (x2 - x1);
	int dy = (y2 - y1);

	int error = std::abs(dy) * 2;
	int error2 = 0;

	int y = y1;
	for (int x = x1; x <= x2; x++)
	{
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}

		error2 += error;

		if (error2 > dx)
		{
			y += (y2 > y1 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

// Compute barycentric coordinates for a triangle
Vec3f barycentric(Vec2i *pts, Vec2i P)
{
	Vec3f u = cross(Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]), Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));

	// If u.z is less than one that means triangle is degenerate
	if (std::abs(u.z < 1))
		return Vec3f(-1, 1, 1);

	// return (1-u-v, u, v)
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

// Routine to draw traingles in a image with given color using barycentric coordinates
void triangle(Vec2i *points, TGAImage &image, TGAColor color)
{

	// find the boundingbox which contains the triangle
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		bboxmin.x = min(bboxmin.x, points[i].x);
		bboxmin.y = min(bboxmin.y, points[i].y);
	}

	for (int i = 0; i < 3; i++)
	{
		bboxmax.x = max(bboxmax.x, points[i].x);
		bboxmax.y = max(bboxmax.y, points[i].y);
	}

	Vec2i P;

	// For each pixel in bounding box check if current pixel is in triangle or not
	// using barycentric method
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(points, P);

			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
				continue;
			image.set(P.x, P.y, color);
		}
	}
}

void triangle_with_line(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
	if (t0.y == t1.y && t0.y == t2.y)
		return; // I dont care about degenerate triangles
	if (t0.y > t1.y)
		std::swap(t0, t1);
	if (t0.y > t2.y)
		std::swap(t0, t2);
	if (t1.y > t2.y)
		std::swap(t1, t2);
	int total_height = t2.y - t0.y;
	for (int i = 0; i < total_height; i++)
	{
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
		if (A.x > B.x)
			std::swap(A, B);
		for (int j = A.x; j <= B.x; j++)
		{
			image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y
		}
	}
}
