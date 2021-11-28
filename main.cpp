#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include <iostream>
#include <cmath>
#include "draw.h"
#include <limits.h>

using namespace std;

typedef unsigned int uint;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 128, 0, 255);
const TGAColor blue = TGAColor(0, 255, 255, 255);
const TGAColor magenta = TGAColor(255, 0, 255, 255);

const int HEIGHT = 800;
const int WIDTH = 800;
const int DEPTH = 225;

Vec3f light_dir(0,0,-1);
Vec3f camera(0,0,3);

// Print Pixel values
void display_pixels(TGAImage image)
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{

			uint pixel_value = image.get(i, j).val;
			if (pixel_value != 0)
			{
				std::cout << "(x=" << i << " , y=" << j << ") " << pixel_value << endl;
			}
		}
	}
}
// World coordinates to screen coordinates
Vec3f world2screen(Vec3f v)
{
	return Vec3f(int((v.x + 1.) * WIDTH / 2. + .5), int((v.y + 1.) * HEIGHT / 2. + .5), v.z);
}

// Matrix to viewport
// TODO: why do this ?

Vec3f m2v(Matrix m) {
	return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
	Matrix m(4,1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}

Matrix viewport(int x,int y, int w, int h) {
	Matrix m = Matrix::identity(4);

	m[0][3] = x+w/2.f;
	m[1][3] = y+h/2.f;
	m[2][3] = DEPTH/2.f;

	m[0][0] = w/2.f;
	m[1][1] = h/2.f;
	m[2][2] = DEPTH/2.f;

	return m;
}

// Draws a wiremesh of a model (model.h) on Image (image.h)
void draw_wire_mesh(Model *model, TGAImage &image)
{

	float *zbuffer = new float[WIDTH * HEIGHT];
	for (int i = WIDTH * HEIGHT; i--; zbuffer[i] = INT_MIN)
		;
	
	Matrix projection = Matrix::identity(4);
	Matrix viewPort = viewport(WIDTH/8,HEIGHT/8, WIDTH*3/4,HEIGHT*3/4);
	projection[3][2] = -1.f/camera.z;

	for (int i = 0; i < model->nfaces(); i++)
	{
		vector<int> face = model->face(i);
		Vec3f pts[3];
		Vec3f world_coords[3];

		for (int j = 0; j < 3; j++) {
			pts[j] = m2v(viewPort*projection*v2m(model->vert(face[j])));
			world_coords[j] = model->vert(face[j]);
		}

		//TODO: inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }

		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();

		cout<<n<<endl;

		float intensity = n*light_dir;

		if(intensity > 0)
			triangle(pts, image, TGAColor(intensity *  255, intensity * 255, intensity * 255, 255), zbuffer);
	}
	
}

void rasterize(Vec2i v1, Vec2i v2, TGAImage &image, TGAColor color, int ybuffer[])
{
	if (v2.x < v1.x)
	{
		std::swap(v1, v2);
	}
	for (int x = v1.x; x <= v2.x; x++)
	{
		float t = (x - v1.x) / (float)(v2.x - v1.x);
		int y = v1.y * (1. - t) + v2.y * t;

		if (ybuffer[x] < y)
		{
			ybuffer[x] = y;
			for (int k = 0; k < 16; k++)
				image.set(x, k, color);
		}
	}
}

int main(int argc, char **argv)
{

	// Creates a blank image
	TGAImage image(HEIGHT, WIDTH, TGAImage::RGB);

	Model *model = NULL;

	if (argc == 2)
	{
		model = new Model(argv[1]);
	}
	else
	{
		model = new Model("./obj/african_head.obj");
	}

	draw_wire_mesh(model, image);
	// drawline(100,100,100,150,image,red);
	// drawline(100,150,140,150,image,red);
	// drawline(140,150,150,140,image,red);
	// drawline(150,140,150,100,image,red);
	// drawline(150,100,100,100,image,red);
	
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
