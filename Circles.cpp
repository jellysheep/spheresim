
#include "Circles.h"
#include <cmath>

int circlesCount = 50;
bool manyCircles = (circlesCount>100);

vector3 boxSize = (vector3){0.3,0.3,0.3};
vector2 size = (vector2){0.005,0.005};

int renderFpsMax = 60, renderFps = renderFpsMax;
scalar speed = 0.1, speedCorrection = 1.0;
scalar fps = 80000, minFps = 2000*speed;
scalar timeInterval = speed*speedCorrection/fps;

scalar max_speed = 0.2;
scalar E = 1000000*0.05;//((200)/1000000.0)/2; //Silikonkautschuk
scalar poisson = 0.5; //Gummi
scalar elastic = 1.0;//0.999;//0.9;
scalar gravity = 9.81;
bool saveBool = false, renderBool = true;
int edges = 2*(int)(std::max(4.0,4*log(size.s1/boxSize.s0*400)));
scalar step = 2*M_PI/edges;
scalar G = 0;//10000000000.0*6.67384e-11;

bool useCircleExtensions = true;
bool useTrace = true && (!manyCircles);
long traceCount = renderFpsMax*10/std::max(1.0,log(circlesCount));
bool connectTracePoints = true;
int renderBufferCount = renderFpsMax;
