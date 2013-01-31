
#include "Circles.h"
#include <cmath>
#include <algorithm>

/*cl_vector2* clVector(vector2 v){
	cl_vector2* ret = new cl_vector2();
	ret->s0 = v.s[0];
	ret->s1 = v.s[1];
	return ret;
}
cl_vector3* clVector(vector3 v){
	cl_vector3* ret = new cl_vector3();
	ret->s0 = v.s[0];
	ret->s1 = v.s[1];
	ret->s2 = v.s[2];
	return ret;
}*/

int circlesCount = 1000;
int showCirclesCount = std::min(1000,circlesCount);
bool manyCircles = (showCirclesCount>20);

vector3 boxSize = (vector3){1,1,1};
vector2 size = (vector2){0.02,0.02};

int renderFpsMax = 50, renderFps = renderFpsMax;
scalar speed = 0.01, speedCorrection = 1.0;
scalar fps = 1000000, minFps = 4000;
scalar timeInterval = speed*speedCorrection/fps;

scalar max_speed = 0.5;
scalar E = 10*1000000*0.05;//((200)/1000000.0)/2; //Silikonkautschuk
scalar poisson = 0.5; //Gummi
scalar elastic = 1.0;//0.05;//0.9;//0.999;//0.9;
scalar gravity_abs = 9.81;
vector gravity = (vector){0,gravity_abs
	#if _3D_
	,0
	#endif
	};
bool saveBool = false, renderBool = true;
int edges = 2*(int)(std::max(4.0,4*log(size.s[1]/boxSize.s[0]*400)));
scalar step = 2*M_PI/edges;
scalar G = 0;//10*10000000000.0*6.67384e-11;
scalar G_fact = 1;
scalar airResistance = 0;
bool wallResistance = true;

bool useColoursBool = true;// && (circlesCount<=100);
bool useTrace = true && (!manyCircles);
long traceCount = renderFpsMax*50/sqrt(std::max(1.0,log(showCirclesCount)));
bool connectTracePoints = true;
int renderBufferCount = renderFpsMax;

bool reflections = true && (showCirclesCount<=100);
bool useSplitKernels = true;

vector2 autoRotation = (vector2){0,0//.2
									};
