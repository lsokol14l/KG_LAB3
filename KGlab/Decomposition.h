#pragma once
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>

#define EPSILON 1e-7

#define M_PI 3.14159265358979323846

struct Point {
	double x = 0;
	double y = 0;
	double z = 0;

	const double* p() const {
		return &x;
	};
};

// 
void Squads(Point Gap);
//  
void WriteSquare(Point A, Point B, Point C, Point D);
void SemiCircle(std::mt19937& gen, std::uniform_real_distribution<double>& r);
void WriteNormal(Point A, Point B, Point C, Point D);
void WriteNormalTriangle(Point A, Point B, Point C);

void displayNormal(Point A, Point B, Point C, Point N);

Point getNormal(Point A, Point B, Point C);

Point getNormalSquare(Point A, Point B, Point C, Point D);

void displayNormalSquare(Point A, Point B, Point C, Point D, Point N);