#pragma once
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <assert.h>
#define EPSILON 1e-05f
#define EQUALS(a, b) std::abs(a-b) <= EPSILON
#define FLOAT_LOG_PRECISION 6
#define DEGREE2RAD(degree) degree * 0.0174532925f
#define PI 3.1415926

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768