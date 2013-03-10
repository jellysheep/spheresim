
#include "Circles.h"
#include <cmath>
#include <algorithm>

bool use3D = false;
bool fastRender = true;

int circlesCount = 1000;
int maxShowCirclesCount = 20000;
bool manyCircles = (std::min(circlesCount,maxShowCirclesCount)>20);

vector3 boxSize = (vector3){1,1,1};
vector2 sphereSize = (vector2){0.015,0.015};

int renderFpsMax = 60, renderFps = renderFpsMax;
scalar speed = 1, speedCorrection = 1.0;
scalar fps = 1000000, minFps = 1000;
scalar timeInterval = speed*speedCorrection/fps;

scalar max_speed = 0.5;
scalar E = 2*0.05;//((200)/1000000.0)/2; //Silikonkautschuk
scalar poisson = 0.5; //Gummi
scalar elastic = 0.1;//0.93;//0.05;//0.9;//0.999;//0.9;
scalar gravity_abs = 9.81;
vector gravity = (vector){0,-gravity_abs, 0};
bool saveBool = false, renderBool = true, playBool = false;
int edges = 2*(int)(std::max(4.0,4*log(sphereSize.s[1]/boxSize.s[0]*400)));
scalar step = 2*M_PI/edges;
scalar G = 10000000000.0*6.67384e-11;
scalar G_fact = 0;
scalar airResistance = 0.1;
bool wallResistance = true;

bool useColorsBool = true;// && (circlesCount<=100);
bool useColorHSV = true;
scalar hueStep = -0.15;
scalar hueOffset;
bool useTrace = false && (!manyCircles);
const char *filename = "save";
const char *viewFileExtension3D = "s3v", *configFileExtension3D = "s3c";
const char *viewFileExtension2D = "s2v", *configFileExtension2D = "s2c";
long traceCount = renderFpsMax*50/sqrt(std::max(1.0,log((maxShowCirclesCount))));
bool connectTracePoints = true;
scalar traceAmount = 1.0;
int renderBufferCount = renderFpsMax;

bool reflections = true && (std::min(circlesCount,maxShowCirclesCount)<=1000);
bool wireframe = false;
bool showCube = true, showLights = false;
bool useSplitKernels = true;
bool showGraph = true;
int plotFps = 5;

vector3 autoRotation = (vector3){0,0,0};

scalar calcSpeedFact;

int rani(int i){
	return rand()%(i+1);
}
scalar rans(scalar s){
	return (rand()%65536)/65536.0*s;
}
scalar rans(scalar s1, scalar s2){
	return s1+rans(s2-s1);
}

const char* getViewFileExtension(){
	if(use3D){
		return viewFileExtension3D;
	}else{
		return viewFileExtension2D;
	}
}
const char* getConfigFileExtension(){
	if(use3D){
		return configFileExtension3D;
	}else{
		return configFileExtension2D;
	}
}
