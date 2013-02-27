#ifdef ENGINE_READ

#include "FileCalculator.h"

#include <iostream>
using namespace std;

#include "NanosecondTimer.h"

#include <iomanip>
#include <iostream>
using namespace std;

void FileCalculator::readLine(){
	bool retry;
	int tries = 0;
	do{
		tries++;
		retry = false;
		if(!getline(f, line)){
			retry = true;
			continue;
		}else if(line[0] == '#'){
			retry = true;
			continue;
		}
		///remove comments
		int i = line.find('#');
		if(i!=string::npos){
			line.erase(line.find('#'));
		}
		//*
		if(line == ""){
			retry = true;
			continue;
		}// */
	}while(retry && tries<=5);
	if(f.eof()){
		eof = true;
		return;
	}
	//cout<<"line: "<<line<<"\n";
	///decide between decimal and hexadecimal chars
	if(line[0] == 'h' || line[0] == 'H'){
		hexadec = true;
		//cout<<"hex.\n";
		line.erase(line.begin());
	}else{
		hexadec = false;
		//cout<<"dec.\n";
	}
	iss.clear();
	iss.str(line);
	//cout<<"new line: \""<<line<<"\"\n";
}

float FileCalculator::readHexFloat(){
	float f;
	unsigned long int l = 0;
	//cout<<"line: \""<<line<<"\"\n";
	for(int _i = 0; !(iss>>hex>>l) && _i<5; _i++){
		readLine();
	}
	f = *((float*)&l);
	return f;
}

float FileCalculator::readFloat(){
	iss.peek();
	if(!iss) readLine();
	if(hexadec){
		//cout<<"reading hex...\n";
		return readHexFloat();
	}
	float f = 0;
	//cout<<"reading dec...\n";
	for(int _i = 0; !(iss>>(hexadec?hex:dec)>>f) && _i<5; _i++){
		readLine();
	}
	return f;
}

#define _read(stream,var)												\
	for(int _i = 0; !(stream>>(hexadec?hex:dec)>>var) && _i<5; _i++){	\
		readLine();														\
	}

void FileCalculator::saveInVar_(scalar &s){
	float f = readFloat();
	if(!eof){
		s = f;
	}
}

void FileCalculator::saveInVar_(int &i){
	int j;
	_read(iss, j);
	if(!eof) i = j;
}

#define saveInVar(x) {			\
	saveInVar_(x);				\
	/*cout<<#x<<": "<<x<<"\n";*/	\
}

FileCalculator::FileCalculator():Calculator(){
	hexadec = false;
	eof = false;
	initialized;
	f.open((std::string(filename)+'.'+viewFileExtension).c_str(), fstream::in);
	bool b = true;
	if (f.is_open())
	{
		//readLine();
		int _3d;
		saveInVar(_3d);
		if(_3d != _3D_){
			if(_3d == 0){
				cerr<<"You have to open this file with 2D viewer."<<endl;
			}else{
				cerr<<"You have to open this file with 3D viewer."<<endl;
			}
			return;
		}
		int newCirclesCount;
		saveInVar(newCirclesCount);
		int newMaxCirclesCount;
		saveInVar(newMaxCirclesCount);
		
		saveInVar(boxSize.s0);
		saveInVar(boxSize.s1);
		#if _3D_
			saveInVar(boxSize.s2);
		#endif
		saveInVar(sphereSize.s0);
		saveInVar(sphereSize.s1);
		saveInVar(renderFpsMax);
		timeInterval = 1000/renderFpsMax;
		saveInVar(renderFps);
		saveInVar(speed);
		saveInVar(fps);
		saveInVar(minFps);
		saveInVar(max_speed);
		saveInVar(E);
		saveInVar(poisson);
		saveInVar(elastic);
		saveInVar(gravity_abs);
		saveInVar(G_fact);
		saveInVar(airResistance);
		int _wallResistance;
		saveInVar(_wallResistance);
		wallResistance = (_wallResistance != 0);
		circles = new Circle[newCirclesCount];
		circles2 = new Circle[newCirclesCount];
		
		for(int i = 0; i<newCirclesCount; i++){
			saveInVar(circles2[i].size);
			//printf("circle size: %5f\n", circles2[i].size);
			saveInVar(circles2[i].mass);
			saveInVar(circles[i].pos.s0);
			saveInVar(circles[i].pos.s1);
			#if _3D_
				saveInVar(circles[i].pos.s2);
			#endif
		}
		
		readNum_render = min(newMaxCirclesCount,newCirclesCount);
		renderBuffer = new vector*[renderBufferCount];
		for(int i = 0; i<renderBufferCount; i++){
			renderBuffer[i] = new vector[readNum_render];
		}
		
		
		circleCountChanged(newCirclesCount);
		//circlesCount = newCirclesCount;

		maxCircleCountChanged(newMaxCirclesCount);
		//maxShowCirclesCount = newMaxCirclesCount;
		
		initialized = true;
		
		bufferReadIndex = 0;
		bufferWriteIndex = 0;
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		
		printf("Circles to render: %d\n", readNum_render);
		
		printf("FileCalculator initialized!\n");
	}
	else
	{
		cerr<<"File could not be opened!"<<endl;
		circlesCount = 0;
		exit(0);
	}
}

void FileCalculator::doStep(){
	QThread::msleep(1000/(renderFpsMax*calcSpeedFact));
}

void FileCalculator::save(){
	//nothing to do
}

void FileCalculator::updateG(){
	//nothing to do
}

void FileCalculator::updateSphereSize(){
	//nothing to do
}

void FileCalculator::updateAirResistance(){
	//nothing to do
}

void FileCalculator::updateWallResistance(){
	//nothing to do
}

void FileCalculator::updateEModul(){
	//nothing to do
}

void FileCalculator::updatePoisson(){
	//nothing to do
}

void FileCalculator::updateElasticity(){
	//nothing to do
}

bool FileCalculator::saveFrame(){
	if(eof || !initialized) return false;
	for(int i = 0; i<circlesCount; i++){
		saveInVar(renderBuffer[bufferWriteIndex][i].s0);
		saveInVar(renderBuffer[bufferWriteIndex][i].s1);
		#if _3D_
			saveInVar(renderBuffer[bufferWriteIndex][i].s2);
		#endif
	}
	return true;
}

Circle* FileCalculator::getCircle(int i){
	circles2[i].pos = renderBuffer[bufferReadIndex][i];
	return &circles2[i];
}

Circle* FileCalculator::getDirectCircle(int i){
	circles2[i].pos = renderBuffer[bufferWriteIndex][i];
	return &circles2[i];
}

void FileCalculator::fpsChanged(scalar timeInt){
	interval = timeInt;
}

void FileCalculator::boxSizeChanged(){
	//nothing to do
}

void FileCalculator::gravityChanged(){
	//nothing to do
}

void FileCalculator::circleCountChanged_subclass(int i){
	//nothing to do
}

void FileCalculator::maxCircleCountChanged_subclass(int i){
	//nothing to do
}

#endif