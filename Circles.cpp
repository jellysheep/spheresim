
#include "Circles.h"
#include <cmath>
#include <algorithm>

int circlesCount = 2000;
int maxShowCirclesCount = 2000;
bool manyCircles = (min(circlesCount,maxShowCirclesCount)>20);

vector3 boxSize = (vector3){1,1,1};
vector2 sphereSize = (vector2){0.015,0.015};

int renderFpsMax = 60, renderFps = renderFpsMax;
scalar speed = 1, speedCorrection = 1.0;
scalar fps = 1000000, minFps = 800;
scalar timeInterval = speed*speedCorrection/fps;

scalar max_speed = 0.5;
scalar E = 2*1000000*0.05;//((200)/1000000.0)/2; //Silikonkautschuk
scalar poisson = 0.5; //Gummi
scalar elastic = 0.5;//0.05;//0.9;//0.999;//0.9;
scalar gravity_abs = 9.81;
vector gravity = (vector){0,-gravity_abs
	#if _3D_
	,0
	#endif
	};
bool saveBool = false, renderBool = true;
int edges = 2*2*2*(int)(std::max(4.0,4*log(sphereSize.s[1]/boxSize.s[0]*400)));
scalar step = 2*M_PI/edges;
scalar G = 0;//10*10000000000.0*6.67384e-11;
scalar G_fact = 1;
scalar airResistance = 0;
bool wallResistance = true;

bool useColorsBool = true;// && (circlesCount<=100);
bool useColorHSV = true;
scalar hueStep = -0.15;
scalar hueOffset;
bool useTrace = true && (!manyCircles);
long traceCount = renderFpsMax*5/sqrt(std::max(1.0,log((maxShowCirclesCount))));
bool connectTracePoints = true;
int renderBufferCount = renderFpsMax;

bool reflections = true && (min(circlesCount,maxShowCirclesCount)<=3000);
bool wireframe = false;
bool useSplitKernels = false;

vector3 autoRotation = (vector3){0,0,0};

int rani(int i){
	return rand()%(i+1);
}
scalar rans(scalar s){
	return (rand()%65536)/65536.0*s;
}
scalar rans(scalar s1, scalar s2){
	return s1+rans(s2-s1);
}
