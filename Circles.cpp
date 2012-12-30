
#include "Circles.h"
#include <math.h>

int circlesCount = 10000, _3D_ = 0;
cl_double3 boxSize = (cl_double3){600,600,0};
double max_speed = 0.05;
double E = ((20000)/1000000.0)/2;
bool saveBool = false;
int edges = 10;
double step = 2*M_PI/edges;
