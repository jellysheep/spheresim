
#include "Circles.h"
#include <math.h>

int circlesCount = 200, _3D_ = 0;
cl_double3 boxSize = (cl_double3){300,300,300};
double max_speed = 0;//0.005;
double E = ((2)/1000000.0)/2;
double elastic = 0.9;//0.9;
double gravity = 0.00001;
bool saveBool = false;
int edges = 10;
double step = 2*M_PI/edges;
