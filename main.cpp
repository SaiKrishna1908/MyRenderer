#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include <iostream>
#include <cmath>


using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int height = 128;
const int width = 128;



void drawline(int x1,int y1, int x2, int y2, TGAImage &image, TGAColor color) {

	bool steep =false;

	// Height is less than width, transpose to get the line with more pixels
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

int main(int argc, char** argv) {

  // Creates a blank image
  TGAImage image(height, width, TGAImage::RGB);

  Model *model = NULL;

  if(argc == 2) {
    model = new Model(argv[1]);
  } else {
    model = new Model("./obj/african_head.obj");
  }

  // for(int i=0;i<model->nfaces();i++) {
  //   std::vector<int> face = model->face(i);

  //   for(int j=0;j<3;j++) {

  //     Vec3f v0 = model->vert(face[j]);
  //     Vec3f v1 = model->vert(face[(j+1)%3]);
  //     int x0 = (v0.x+1)*width/2;
  //     int y0 = (v0.y+1)*height/2;
  //     int x1 = (v1.x+1)*width/2;
  //     int y1 = (v1.y+1)*height/2;
      
  //     drawline(x0,y0,x1,y1,image,white);
  //   }
  // }

  // drawline(0,0,128,128,image,white);
  // drawline(128,128,255,0,image,white);

  drawline(2,1,5,4,image,white);

  for(int i=0;i<128;i++) {

    for(int j=0;j<128;j++) {

      if (image.get(i,j).val != 0) {
	std::cout<<i<<" "<<j<<endl;
      }
    }
  }
  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  image.write_tga_file("output.tga");
  return 0;
}

