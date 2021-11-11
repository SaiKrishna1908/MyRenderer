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

	// drawline(p1.x,p1.y,p2.x,p2.y,image,color);
	// drawline(p2.x,p2.y,p3.x,p3.y,image,color);
	// drawline(p3.x,p3.y,p1.x,p1.y,image,color);

}

float getArea(Vec2i points[]) {

	int x1 = points[0].x;
	int y1 = points[0].y;
	int x2 = points[1].x;
	int y2 = points[1].y;
	int x3 = points[2].x;
	int y3 = points[2].y;

	return 0.5*abs(x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2));
}

void filltriangle(Vec2i points[], TGAImage &image, TGAColor color) {

	int a = getArea(points);

	for(int i=0;i<HEIGHT;i++) {
		for(int j=0;j<WIDTH;j++) {

			Vec2i seta[] = {points[0],points[1],Vec2i(i,j)};
			Vec2i setb[] = {points[1], points[2],Vec2i(i,j)};
			Vec2i setc[] = {points[0], points[2], Vec2i(i,j)};

			float a1 = getArea(seta);
			float a2 = getArea(setb);
			float a3 = getArea(setc);

			// cout<<a1<<" "<<a2<<" "<<a3<<" "<<endl;

			if ((int)a1+a2+a3 == (int)a) {
				image.set(i,j,color);
			}
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

