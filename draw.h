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
Vec3f barycentric(Vec3f *pts, Vec3f P)
{
	Vec3f s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = pts[2][i] - pts[0][i];
		s[i][1] = pts[1][i] - pts[0][i];
		s[i][2] = pts[0][i] - P[i];
	}

	Vec3f u = cross(s[0], s[1]);

	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1);
}

// Routine to draw traingles in a image with given color using barycentric coordinates
void triangle(Vec3f *points, TGAImage &image, TGAColor color, float *zbuffer)
{

	// find the boundingbox which contains the triangle
	Vec2f bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2f bboxmax(0, 0);
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], points[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], points[i][j]));
		}
	}

	Vec3f P;

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(points, P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
				continue;
			P.z = 0;
			for (int i = 0; i < 3; i++)
				P.z += points[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y * image.get_width())] < P.z)
			{
				zbuffer[int(P.x + P.y * image.get_width())] = P.z;
				image.set(P.x, P.y, color);
			}
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
