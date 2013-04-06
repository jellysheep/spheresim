#ifdef ENGINE_READ

#include <QFileDialog>

#include "FileCalculator.h"

#include "NanosecondTimer.h"

#include <iomanip>
#include <iostream>

const char* getFileName(const char* c1, const char* c2){
	return (std::string(c1)+'.'+c2).c_str();
}

FileCalculator::FileCalculator():Calculator(){
	hexadec = false;
	eof = false;
	
	playBool = true;
	
	initialized = false;
	
	const char* file = getFileName(filename,getViewFileExtension());
	
	const char* filter = (std::string("SphereSim View File (*.")+getViewFileExtension()+")").c_str();
	QString str = QFileDialog::getOpenFileName(0, ("Open file"), (std::string("./save.")+getViewFileExtension()).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		exit(0);
	}
	file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);

	f.open(file, std::fstream::in);
	bool b = true;
	if (f.is_open())
	{
		//readLine();
		int _3d;
		saveInVar(f, _3d);
		if((_3d!=0) != use3D){
			if(_3d == 0){
				std::cerr<<"You have to open this file with 2D viewer."<<std::endl;
			}else{
				std::cerr<<"You have to open this file with 3D viewer."<<std::endl;
			}
			return;
		}
		int newSpheresCount;
		saveInVar(f, newSpheresCount);
		int newMaxSpheresCount;
		saveInVar(f, newMaxSpheresCount);
		
		saveInVar(f, boxSize.s0);
		saveInVar(f, boxSize.s1);
		if(use3D){
			saveInVar(f, boxSize.s2);
		}
		saveInVar(f, sphereSize.s0);
		saveInVar(f, sphereSize.s1);
		saveInVar(f, renderFpsMax);
		timeInterval = 1000/renderFpsMax;
		saveInVar(f, renderFps);
		saveInVar(f, speed);
		saveInVar(f, fps);
		saveInVar(f, minFps);
		saveInVar(f, max_speed);
		saveInVar(f, E);
		saveInVar(f, poisson);
		saveInVar(f, elastic);
		saveInVar(f, gravity_abs);
		saveInVar(f, G_fact);
		saveInVar(f, airResistance);
		int _wallResistance;
		saveInVar(f, _wallResistance);
		wallResistance = (_wallResistance != 0);
		spheres = new Sphere[newSpheresCount];
		spheres2 = new Sphere[newSpheresCount];
		
		for(int i = 0; i<newSpheresCount; i++){
			saveInVar(f, spheres2[i].size);
			//printf("sphere size: %5f\n", spheres2[i].size);
			saveInVar(f, spheres2[i].mass);
			saveInVar(f, spheres[i].pos.s0);
			saveInVar(f, spheres[i].pos.s1);
			if(use3D){
				saveInVar(f, spheres[i].pos.s2);
			}
		}
		
		readNum_render = std::min(newMaxSpheresCount,newSpheresCount);
		renderBuffer = new vector*[renderBufferCount];
		for(int i = 0; i<renderBufferCount; i++){
			renderBuffer[i] = new vector[readNum_render];
		}
		
		
		sphereCountChanged(newSpheresCount);
		//spheresCount = newSpheresCount;

		maxSphereCountChanged(newMaxSpheresCount);
		//maxShowSpheresCount = newMaxSpheresCount;
		
		initialized = true;
		
		bufferReadIndex = 0;
		bufferWriteIndex = 0;
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		saveFrame();
		bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		
		printf("Spheres to render: %d\n", readNum_render);
		
		printf("FileCalculator initialized!\n");
	}
	else
	{
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		exit(0);
	}
}

bool FileCalculator::isFixed(int i){
	return false;
}

void FileCalculator::doStep(){
	//printf("%10f ; target fps: %10f\n", calcSpeedFact, (renderFpsMax/calcSpeedFact));
	QThread::msleep(1000/(renderFpsMax/calcSpeedFact));
}

void FileCalculator::save(){
	//nothing to do
}

void FileCalculator::updateG(){
	//nothing to do
}

void FileCalculator::updateGridSize(){
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
	for(int i = 0; i<spheresCount; i++){
		saveInVar(f, renderBuffer[bufferWriteIndex][i].s0);
		saveInVar(f, renderBuffer[bufferWriteIndex][i].s1);
		if(use3D){
			saveInVar(f, renderBuffer[bufferWriteIndex][i].s2);
		}
	}
	return true;
}

Sphere* FileCalculator::getSphere(int i){
	spheres2[i].pos = renderBuffer[bufferReadIndex][i];
	return &spheres2[i];
}

Sphere* FileCalculator::getDirectSphere(int i){
	spheres2[i].pos = renderBuffer[bufferWriteIndex][i];
	return &spheres2[i];
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

void FileCalculator::sphereCountChanged_subclass(int i){
	//nothing to do
}

void FileCalculator::maxSphereCountChanged_subclass(int i){
	//nothing to do
}

void FileCalculator::loadConfig(){
	//
}

void FileCalculator::updateMagnitude(){
	//
}

#endif
