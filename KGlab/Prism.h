#pragma once

#include <random>
#include "MyOGL.h"
#include <stdio.h>
#include <Math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <chrono>


#include "Render.h"

#pragma once

struct Point {
	double x = 0;
	double y = 0;
	double z = 0;

	const double* p() const {
		return &x;
	};
};

void WriteSquare(Point A, Point B, Point C, Point D);

void Squads(Point Gap);