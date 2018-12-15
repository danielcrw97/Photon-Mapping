/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2018.
 *
 * Do what you like with this code as long as you retain this comment.
 */

 // This file contains the mail entry point for the raytracer, that
 // creates the scene and then traces it. It also contains the code to
 // write out the framebuffer to a PPM file.

// Defines needed for PNG writer

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "scene.h"
#include "sphere.h"
#include "polymesh.h"
#include "square_light.h"
#include "Plane.h"
#include "RenderingConstants.h"
#include "MathUtils.h"
#include <stb_image_write.h>
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;

#define XSIZE 256
#define YSIZE 256

Colour framebuffer[YSIZE][XSIZE];

void clear_framebuffer()
{
	int x, y;

	for (y = 0; y < YSIZE; y += 1)
	{
		for (x = 0; x < XSIZE; x += 1)
		{
			framebuffer[y][x].r = 0.0f;
			framebuffer[y][x].g = 0.0f;
			framebuffer[y][x].b = 0.0f;
		}
	}
}

void write_framebuffer()
{
	int x, y;
	unsigned char* buffer = new unsigned char[YSIZE * (XSIZE*3)];

	for (y = 0; y < YSIZE; y++)
	{
		for (x = 0; x < XSIZE; x++)
		{
			int index = (XSIZE * y * 3) + (x * 3);
			buffer[index] = (unsigned char)(framebuffer[y][x].r*255.0f);
			buffer[index+1] = (unsigned char)(framebuffer[y][x].g*255.0f);
			buffer[index+2] = (unsigned char)(framebuffer[y][x].b*255.0f);
		}
	}
	
	// Use a single file public domain library to write the framebuffer to a .png image
	// Changed to this as the original .ppm writer seemed to have some issues.
	// Library (and it's corresponding license) can be found here - https://github.com/nothings/stb/blob/master/stb_image_write.h
	stbi_write_png("image.png", XSIZE, YSIZE, 3, &buffer[0], 3*XSIZE);
}

int main(int argc, char *argv[])
{
	auto start = std::chrono::system_clock::now();

	Scene scene;
	int x, y;

	clear_framebuffer();

	// Create Materials.

	// A material that perfectly reflects light.
	Material specular;
	specular.colour = Colour(0.99f, 0.99f, 0.99f);
	specular.tag = MaterialTag::SPECULAR;
	specular.specularCoefficient = 1.0f;

	// Glass that can either reflect or refract light.
	Material glass;
	glass.colour = Colour(1.0f, 1.0f, 1.0f);
	glass.tag = MaterialTag::GLASS;

	Material pinkGlass;
	pinkGlass.colour = Colour(255.0f / 256.0f, 182.0f / 256.0f, 193.0f / 256.0f);
	pinkGlass.tag = MaterialTag::GLASS;
	
	// Different coloured lambertian surfaces.
	// The diffuse coefficient represents the proportion of light diffusely reflected.
	Material diffuseWhite;
	diffuseWhite.colour = Colour(0.8f, 0.8f, 0.8f);
	diffuseWhite.tag = MaterialTag::LAMBERTIAN;
	diffuseWhite.diffuseCoefficient = 0.8f;

	Material diffuseRed;
	diffuseRed.colour = Colour(0.8f, 0.0f, 0.0f);
	diffuseRed.tag = MaterialTag::LAMBERTIAN;
	diffuseRed.diffuseCoefficient = 0.8f;

	Material diffuseBlue;
	diffuseBlue.colour = Colour(0.0f, 0.0f, 0.8f);
	diffuseBlue.tag = MaterialTag::LAMBERTIAN;
	diffuseBlue.diffuseCoefficient = 0.8f;
	
	// Create our light in the ceiling.
	Vertex vl1(1, 4.999f, 8.5f);
	Vertex vl2(-1, 4.999f, 8.5f);
	Vertex vl3(-1, 4.999f, 6.5f);
	Vertex vl4(1, 4.999f, 6.5f);

	Plane lightPlane(Triangle(vl3, vl4, vl2), Triangle(vl1, vl2, vl4));
	shared_ptr<SquareLight> squareLight = make_shared<SquareLight>(lightPlane, Vector(0, -1, 0), Vertex(0, 4.999f, 7.5f), Colour(1.0f, 1.0f, 1.0f), Bounds(vl3, vl1));
	squareLight->scene = &scene;
	scene.addLight(squareLight);

	// Transform for the bunny in world space.
	Transform transform(30.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 30.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -30.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	// Translate the bunny into a better position
	transform.translate(Vector(4.0f, -5.0f, 6.0f));
	// Rotate by 45 degrees in rad//
	transform.rotate_y(-MathConstants::PI / 4.0f);

	shared_ptr<PolyMesh> bunny = make_shared<PolyMesh>((char *)"bunny_zipper.kcply", transform);
	shared_ptr<Object> sphere = make_shared<Sphere>(Vertex(2.5f, 0.0f, 8.0f), 1.5f);

	// Programatically create room the bunny is in.
	Vertex v1(-5, 5, 0);
	Vertex v2(-5, -5, 0);
	Vertex v3(-5, -5, 10.0f);
	Vertex v4(-5, 5, 10.0f);
	Vertex v5(5, 5, 0);
	Vertex v6(5, -5, 0);
	Vertex v7(5, -5, 10.0f);
	Vertex v8(5, 5, 10.0f);

	// Had to offset the back wall slightly.
	Vertex bw1 = v3;
	Vertex bw2 = v7;
	Vertex bw3 = v8;
	Vertex bw4 = v4;
	bw1.x -= 0.01f;
	bw2.x += 0.01;
	bw3.x += 0.01;
	bw4.x -= 0.01;

	shared_ptr<Object> leftWall = make_shared<Plane>(Triangle(v2, v3, v1), Triangle(v4, v1, v3));
	shared_ptr<Object> rightWall = make_shared<Plane>(Triangle(v6, v5, v7), Triangle(v8, v7, v5));
	shared_ptr<Object> backWall = make_shared<Plane>(Triangle(bw1, bw2, bw4), Triangle(bw3, bw4, bw2));
	shared_ptr<Object> floor = make_shared<Plane>(Triangle(v2, v6, v3), Triangle(v7, v3, v6));
	shared_ptr<Object> ceiling = make_shared<Plane>(Triangle(v1, v5, v4), Triangle(v8, v4, v5));

	// Assign the objects materials and add them to the scene.
	bunny->material = &pinkGlass;
	sphere->material = &specular;
	leftWall->material = &diffuseBlue;
	rightWall->material = &diffuseRed;
	backWall->material = &diffuseWhite;
	floor->material = &diffuseWhite;
	ceiling->material = &diffuseWhite;
	scene.addObject(bunny);
	scene.addObject(sphere);
	scene.addObject(leftWall);
	scene.addObject(rightWall);
	scene.addObject(backWall);
	scene.addObject(floor);
	scene.addObject(ceiling);

	// Build global photon map
	if (RenderingConstants::CREATE_GLOBAL_MAP)
	{
		shared_ptr<PhotonMap> globalPhotonMap = scene.buildGlobalPhotonMap(scene.object_list, scene.light_list, 250000);
		scene.globalPhotonMap = globalPhotonMap;
	}

	// Build caustic photon map
	if (RenderingConstants::CREATE_CAUSTIC_MAP)
	{
		shared_ptr<PhotonMap> causticPhotonMap = scene.buildCausticsPhotonMap(scene.object_list, scene.light_list, 10000);
		scene.causticPhotonMap = causticPhotonMap;
	}

	Colour prevColour;

	for (y = 0; y < YSIZE; y += 1)
	{
		cerr << "Line " << y + 1 << " of " << (int)YSIZE << endl;
		float py = (((float)y / (float)YSIZE) - 0.5f)*-1.0f; // 0.5 to -0.5, flipped y axis

		for (x = 0; x < XSIZE; x += 1)
		{
			float px = (((float)x / (float)XSIZE) - 0.5f); // -0.5 to 0.5

			// Use sampling to get rid of aliasing.
			std::vector<Colour> sampleColours;

			Ray root(Vertex(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f));
			root.position.x = 0.0f;
			root.position.y = 0.0f;
			root.position.z = 0.001f;
			root.position.w = 1.0f;

			root.direction.x = px;
			root.direction.y = py;
			root.direction.z = 0.5f;
			root.direction.normalise();

			scene.raytrace(root, 1, scene.object_list, scene.light_list, framebuffer[y][x]);

			// Check if the colour is different enough that we should do anti-aliasing.
			Colour dif = framebuffer[y][x] - prevColour;
			bool bigDifference = (dif.r > RenderingConstants::ALIASING_COLOUR_DIF_CHECK) || 
								(dif.g > RenderingConstants::ALIASING_COLOUR_DIF_CHECK) ||
								(dif.b > RenderingConstants::ALIASING_COLOUR_DIF_CHECK);

			if (RenderingConstants::USE_AA_SAMPLING && bigDifference)
			{
				// Do sampling to reduce aliasing!
				for (int i = 0; i < RenderingConstants::AA_SAMPLE_GRID_SIZE; ++i)
				{
					for (int j = 0; j < RenderingConstants::AA_SAMPLE_GRID_SIZE; ++j)
					{
						Colour newColour;
						float randX = getRandom0To1();
						float randY = getRandom0To1();
						float xDir = px + (i * (1 / XSIZE) * (1 / RenderingConstants::AA_SAMPLE_GRID_SIZE) * randX);
						float yDir = py + (j * (1 / YSIZE) * (1 / RenderingConstants::AA_SAMPLE_GRID_SIZE) * randY);
						Vector dir(xDir, yDir, 0.5f);
						dir.normalise();

						Ray sampleRay(root.position, dir);
						scene.raytrace(sampleRay, 1, scene.object_list, scene.light_list, newColour);
						sampleColours.push_back(newColour);
					}
				}
				Colour averageColour;
				Colour::average(sampleColours, framebuffer[y][x]);
			}

			// Make sure we get no values less than 0 or more than 1 - just in case!
			framebuffer[y][x].clamp();

			prevColour = framebuffer[y][x];
		}
	}

	write_framebuffer();

	// Print out the rendering time
	auto end = std::chrono::system_clock::now();
	auto diff = end - start;
	std::cout << "Overall rendering time: " << diff.count() << std::endl;
}
