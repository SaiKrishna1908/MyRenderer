#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include <iostream>
#include <cmath>

using namespace std;

typedef unsigned int uint;

void drawline(int x1, int y1, int x2,int y2, TGAImage &image, TGAColor color);
// void drawtriangle(Vec2f p1,Vec2f p2,Vec2f p3, TGAImage &image, TGAColor color);
void filltriangle(Vec2i points[], TGAImage &image, TGAColor color);

const TGAColor white = TGAColor(255,255,255,255);
const TGAColor red   = TGAColor(255,0,0,255);
const TGAColor green = TGAColor(0,128,0,255);

const int HEIGHT = 256;
const int WIDTH = 256;

// Print Pixel values
void display_pixels(TGAImage image) {
	for(int i=0;i<HEIGHT;i++) {
	    for(int j=0;j<WIDTH;j++) {

	    	uint pixel_value = image.get(i,j).val;
	      if (pixel_value != 0) {
					std::cout<<"(x="<<i<<" , y="<<j<<") "<<pixel_value<<endl;
	      }
	    }
	  }
}


// Routine to draw lines in a image with given color

void drawline(int x1,int y1, int x2, int y2, TGAImage &image, TGAColor color) {

	bool steep =false;

	// HEIGHT is less than WIDTH, transpose to get the line with more pixels
	if(abs(x1-x2) < abs(y1-y2)) {
		std::swap(x1,y1);
		std::swap(x2,y2);
		steep=true;
	}
  
	if(x2 < x1) {
		std::swap(x1, x2);		
		std::swap(y1, y2);
	}

	int dx = (x2-x1);
	int dy = (y2-y1);

	int error = std::abs(dy)*2;
	int error2 = 0;

	int y = y1;
	for(int x=x1;x<=x2;x++) {		
		if(steep) {
			image.set(y,x, color);
		} else {
			image.set(x,y,color);
		}

		error2+=error;

		if(error2 > dx) {
			y+=(y2>y1 ? 1 : -1);
			error2-=dx*2;
		}
	}	

}

Vec3f barycentric(Vec2i *pts, Vec2i P) {
	Vec3f u = cross(Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]), Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]));
	
	// If u.z is less than one that means triangle is degenerate 
	if(std::abs(u.z<1)) return Vec3f(-1,1,1);

	// return (1-u-v, u, v)
	return Vec3f(1.f - (u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

// Routine to draw traingles in a image with given color
void triangle(Vec2i *points,TGAImage &image,TGAColor color) {

	// find the boundingbox which contains the triangle
	Vec2i bboxmin(image.get_width()-1, image.get_height()-1);
	Vec2i bboxmax(0,0);
	Vec2i clamp(image.get_width()-1, image.get_height()-1);

	for(int i=0;i<3;i++) {
		bboxmin.x = min(bboxmin.x,points[i].x);
		bboxmin.y = min(bboxmin.y, points[i].y);
	}

	for(int i=0;i<3;i++) {
		bboxmax.x = max(bboxmax.x,points[i].x);
		bboxmax.y = max(bboxmax.y, points[i].y);
	}

	Vec2i P;

	// For each pixel in bounding box check if current pixel is in triangle or not 
	// using barycentric method
	for(P.x = bboxmin.x;P.x<=bboxmax.x;P.x++) {
		for(P.y=bboxmin.y;P.y <=bboxmax.y;P.y++) {
			Vec3f bc_screen = barycentric(points, P);

			if(bc_screen.x<0||bc_screen.y<0||bc_screen.z<0) continue;
			image.set(P.x,P.y,color);
		}
	}
}
// Draws a wiremesh of a model (model.h) on Image (image.h)
void draw_wire_mesh(Model &model, TGAImage &image) {

	for(int i=0;i<model.nfaces();i++) {
    std::vector<int> face = model.face(i);

    for(int j=0;j<3;j++) {

      Vec3f v0 = model.vert(face[j]);
      Vec3f v1 = model.vert(face[(j+1)%3]);
      int x0 = (v0.x+1)*WIDTH/2;
      int y0 = (v0.y+1)*HEIGHT/2;
      int x1 = (v1.x+1)*WIDTH/2;
      int y1 = (v1.y+1)*HEIGHT/2;
      
      drawline(x0,y0,x1,y1,image,white);
    }
  }
}

int main(int argc, char** argv) {

  // Creates a blank image
  TGAImage image(HEIGHT, WIDTH, TGAImage::RGB);

  Model *model = NULL;

  if(argc == 2) {
    model = new Model(argv[1]);
  } else {
    model = new Model("./obj/african_head.obj");
  }
  Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
  Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
  Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
  triangle(t0,image,red);
  triangle(t1,image,green);
  triangle(t2,image,white);
  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  image.write_tga_file("output.tga");
  return 0;
}

