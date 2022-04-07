#include <vector>
#include <cmath>
#include "TgaImage.h"
#include "Model.h"
#include "geometry.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
	//Whole line
	bool steep = false;
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {//if the line is steep,we transpose the image
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}
	if (p0.x > p1.x) { //make it left-to-right
		std::swap(p0, p1);
	}
	for (int x = p0.x; x <= p1.x; x++) {
		float t = (x - p0.x) / (float)(p1.x - p0.x);
		int y = p0.y * (1. - t) + p1.y * t;
		if (steep) {
			image.set(y, x, color);//if transposed,de-transpose
		}
		else {
			image.set(x, y, color);
		}
	}
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	//first type
	//line(t0, t1, image, color);
	//line(t1, t2, image, color);
	//line(t2, t0, image, color);

	//second type
	// sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
	//if (t0.y > t1.y)std::swap(t0, t1);
	//if (t1.y > t1.y)std::swap(t1, t2);
	//if (t0.y > t2.y)std::swap(t0, t2);
	//line(t0, t1, image, green);
	//line(t1, t2, image, green);
	//line(t2, t0, image, red);

	//third type
	//if (t0.y > t1.y)std::swap(t0, t1);
	//if (t1.y > t2.y)std::swap(t1, t2);
	//if (t0.y > t2.y)std::swap(t0, t2);
	//int total_height = t2.y - t0.y;
	//for (int y = t0.y; y <= t1.y; y++) {
	//	int segment_height = t1.y - t0.y + 1;
	//	float alpha = (float)(y - t0.y) / total_height;
	//	float beta = (float)(y - t0.y) / segment_height;
	//	Vec2i A = t0 + (t2 - t0) * alpha;
	//	Vec2i B = t0 + (t1 - t0) * beta;
	//	image.set(A.x, y, red);
	//	image.set(B.x, y, green);
	//}

	//fourth type
	//sort the vertices,t0,t1,t2 lower-to-upper(bubblesort yay!)
	//if (t0.y > t1.y) std::swap(t0, t1);
	//if (t0.y > t2.y) std::swap(t0, t2);
	//if (t1.y > t2.y) std::swap(t1, t2);
	//int total_height = t2.y - t0.y;
	//for (int y = t0.y; y <= t1.y; y++) {
	//	int segment_height = t1.y - t0.y + 1;
	//	float alpha = (float)(y - t0.y) / total_height;
	//	float beta = (float)(y - t0.y) / segment_height; // be careful with divisions by zero 
	//	Vec2i A = t0 + (t2 - t0) * alpha;
	//	Vec2i B = t0 + (t1 - t0) * beta;
	//	if (A.x > B.x)std::swap(A, B);
	//	for (int j = A.x; j < B.x; j++) {
	//		image.set(j, y, color);//attention,due to int casts t0.y+i != A.y
	//	}
	//}
	//for (int y = t1.y; y <= t2.y; y++) {
	//	int segment_height = t2.y - t1.y + 1;
	//	float alpha = (float)(y - t0.y) / total_height;
	//	float beta = (float)(y - t1.y) / segment_height; // be careful with divisions by zero 
	//	Vec2i A = t0 + (t2 - t0) * alpha;
	//	Vec2i B = t1 + (t2 - t1) * beta;
	//	if (A.x > B.x)std::swap(A, B);
	//	for (int j = A.x; j < B.x; j++) {
	//		image.set(j, y, color);//attention,due to int casts t0.y+i != A.y
	//	}
	//}

	//fifth part
	if (t0.y == t1.y && t0.y == t2.y)return;//I dont care about degenerate triangles
	//sort the vertices,t0,t1,t2 lower-to-upper(bubblesort yay!)
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	int total_height = t2.y - t0.y;
	for (int i = 0; i < total_height; i++) {
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;//be careful:with above conditions no division by zero here.
		Vec2i A = t0 + (t2 - t0) * alpha;
		Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
		if (A.x > B.x)std::swap(A,B);
		for (int j = A.x; j <= B.x; j++) {
			image.set(j, t0.y + i, color);//attention,due to int casts t0.y + i !=	A.y;
		}
	}

}

//
//	EXAMPLE CODE
//
/*
	triangle(vec2 points[3]) {
		vec2 bbox[2] = find_bounding_box(points);
		for (each pixel in the bounding box) {
			if (inside(points, pixel)) {
				put_pixel(pixel);
			}
		}
	}
*/


int main(int argc, char** argv) {
	//input file
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("obj/african_head.obj");
	}

	TGAImage image(width, height, TGAImage::RGB);
	
	Vec3f light_dir(0, 0, -1);

	//first type to fifth type
	//Vec2i t0[3] = { Vec2i(10,70),Vec2i(50,160),Vec2i(70,80) };
	//Vec2i t1[3] = { Vec2i(180,50),Vec2i(150,1),Vec2i(70,180) };
	//Vec2i t2[3] = { Vec2i(180,150),Vec2i(120,160),Vec2i(130,180) };

	//triangle(t0[0], t0[1], t0[2], image, red);
	//triangle(t1[0], t1[1], t1[2], image, white);
	//triangle(t2[0], t2[1], t2[2], image, green);

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++) {
			//colorful model use
			//Vec3f world_coords = model->vert(face[j]);
			//screen_coords[j] = Vec2i((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);

			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Vec2i((v.x + 1) * width / 2., (v.y + 1.) * height / 2.);
			world_coords[j] = v;
		}
		//colorful model use
		/*triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));*/

		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0) {
			triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}


	image.flip_vertically();//to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}