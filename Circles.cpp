
#include "Circles.h"
#include <math.h>

int circlesCount = 100, _3D_ = 0;

cl_double3 boxSize = (cl_double3){0.3,0.3,0.3};
cl_double2 size = (cl_double2){0.001,0.003};

int renderFps = 60;
double fps = 80000;
double speed = 0.1;
double timeInterval = speed/fps;

double max_speed = 0;//0.005;
double E = 1000000*0.05;//((200)/1000000.0)/2; //Silikonkautschuk
double poisson = 0.5; //Gummi
double elastic = 0.999;//0.9;
double gravity = 9.81;
bool saveBool = false, renderBool = true;
int edges = 2*(int)(std::max(4.0,4*log(size.s1/boxSize.s0*400)));
double step = 2*M_PI/edges;
double G = 0;//10000000000.0*6.67384e-11;

bool useCircleExtensions = true;
int traceCount = 100;
