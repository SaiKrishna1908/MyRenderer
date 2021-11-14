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

// Routine to draw traingles in a image with given color

void drawtriangle(Vec2i points[], TGAImage &image, TGAColor color) {

	for(int i=0;i<3;i++) {
		drawline(points[i].x,points[i].y, points[(i+1)%3].x, points[(i+1)%3].y, image, color);
		
	}
	filltriangle(points, image, color);
}

// Routine to Fill a triangle with a color

void filltriangle(Vec2i points[], TGAImage &image, TGAColor color) {

	// sort the points in ascending order of y co-ordinate

	if (points[0].y > points[1].y) std::swap(points[0], points[1]);
	if (points[0].y > points[2].y) std::swap(points[0], points[2]);
	if (points[1].y > points[2].y) std::swap(points[1], points[2]);


	// Height of least y to highest y
	int total_height = points[2].y - points[0].y;

	for(int y=points[0].y;y<=points[1].y;y++) {
		// Height between 0 and 1 points
		int segment_height = points[1].y - points[0].y;

		// What is alpha and beta ?
		float alpha = (float) (y-points[0].y)/total_height;
		float beta = (float) (y-points[0].y)/segment_height;

		Vec2i A = points[0]+(points[2]-points[0])*alpha;
		Vec2i B = points[0]+(points[1]-points[0])*beta;

		if (A.x > B.x) std::swap(A,B);

		for(int j=A.x;j<=B.x;j++) {
			image.set(j,y,color);
		}		
	}

	for(int y=points[1].y;y<=points[2].y;y++) {
		int segment_height = points[2].y - points[1].y;

		float alpha = (float) (y-points[0].y)/total_height;
		float beta = (float) (y-points[1].y)/segment_height;

		Vec2i A = points[0] + (points[2]-points[0])*alpha;
		Vec2i B = points[1] + (points[2]-points[1])*beta;

		if (A.x > B.x) std::swap(A,B);

		for(int j=A.x;j<=B.x;j++) {
			image.set(j,y,color);
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

  /*
  
  // Draw lines from vertices and faces
  for(int i=0;i<model->nfaces();i++) {
    std::vector<int> face = model->face(i);

    for(int j=0;j<3;j++) {

      Vec3f v0 = model->vert(face[j]);
      Vec3f v1 = model->vert(face[(j+1)%3]);
      int x0 = (v0.x+1)*WIDTH/2;
      int y0 = (v0.y+1)*HEIGHT/2;
      int x1 = (v1.x+1)*WIDTH/2;
      int y1 = (v1.y+1)*HEIGHT/2;
      
      drawline(x0,y0,x1,y1,image,white);
    }
  }
	
	*/
  // display_pixels(image);
  Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
  Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
  Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 

  drawtriangle(t0, image, red);
  filltriangle(t0,image, red); 
	drawtriangle(t1, image, white); 
	filltriangle(t1,image,white);
	drawtriangle(t2, image, green);
	filltriangle(t2,image,green);
  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  image.write_tga_file("output.tga");
  return 0;
}

