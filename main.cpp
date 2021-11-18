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

const int HEIGHT = 1024;
const int WIDTH = 1024;

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

// Draws a wiremesh of a model (model.h) on Image (image.h)
void draw_wire_mesh(Model model, TGAImage &image)
{

	Vec3f light_dir(0, 0, -1);

	for (int i = 0; i < model.nfaces(); i++)
	{

		std::vector<int> face = model.face(i);
		Vec2i screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v0 = model.vert(face[j]);
			// Screen coords = [(xcoord)*width/2, (ycoord)*height/2]
			screen_coords[j] = Vec2i((v0.x + 1.) * WIDTH / 2., (v0.y + 1) * HEIGHT / 2.);
			world_coords[j] = v0;
		}

		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();

		float intensity = n * light_dir;

		if (intensity > 0)
			triangle_with_line(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
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

	//   draw_wire_mesh(*model, image);
	TGAImage scene(WIDTH, HEIGHT, TGAImage::RGB);

	Vec2i v1(20, 34);
	Vec2i v2(744, 400);
	Vec2i v3(120, 434);
	Vec2i v4(444, 400);
	Vec2i v5(330, 463);
	Vec2i v6(594, 200);

	// scene "2d mesh"
	drawline(20, 34, 744, 400, scene, red);
	drawline(120, 434, 444, 400, scene, green);
	drawline(330, 463, 594, 200, scene, blue);

	// screen line
	drawline(10, 10, 790, 10, scene, magenta);

	int ybuffer[WIDTH];

	// initially ybuffer should be negative distance
	for (int i = 0; i < WIDTH; i++)
	{
		ybuffer[i] = INT_MIN;
	}

	rasterize(Vec2i(20, 34), Vec2i(744, 400), scene, red, ybuffer);
	rasterize(Vec2i(120, 434), Vec2i(444, 400), scene, green, ybuffer);
	rasterize(Vec2i(330, 463), Vec2i(594, 200), scene, blue, ybuffer);

	scene.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	scene.write_tga_file("scene.tga");

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
