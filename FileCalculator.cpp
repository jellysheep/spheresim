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
	do{
		retry = false;
		if(!getline(f, line)){
			retry = true;
		}else if(line[0] == '#'){
			retry = true;
		}
	}while(retry);
	//decide between decimal and hexadecimal chars
	if(line[0] == 'h' || line[0] == 'H'){
		hexadec = true;
		line.erase(line.begin());
	}else{
		hexadec = false;
	}
	//remove comments
	int i = line.find('#');
	if(i!=string::npos){
		line.erase(line.find('#'));
	}
	iss.clear();
	iss.str(line);
	//cout<<"new line: \""<<line<<"\"\n";
}

double FileCalculator::readHexDouble(){
	double d;
	unsigned long long l;
	//cout<<"line: \""<<line<<"\"\n";
	if(!(iss>>hex>>l)){
		readLine();
		if(!(iss>>hex>>l)){
			return 0;
		}
	}
	d = *((double*)&l);
	return d;
}

double FileCalculator::readDouble(){
	iss.peek();
	if(!iss) readLine();
	if(hexadec){
		return readHexDouble();
	}
	double d;
	iss>>d;
	return d;
}

#define _read(stream,var)							\
	for(int _i = 0; !(stream>>var) && _i<5; _i++){		\
		readLine();									\
	}

void FileCalculator::saveInVar(scalar &s){
	double d = readDouble();
	s = d;
}

void FileCalculator::saveInVar(int &i){
	int j;
	_read(iss, j);
	i = j;
}

FileCalculator::FileCalculator():Calculator(){
	hexadec = false;
	f.open(filename, fstream::in);
	bool b = true;
	if (f.is_open())
	{
		readLine();
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
		saveInVar(boxSize.s0);
		saveInVar(boxSize.s1);
		#if _3D_
			saveInVar(boxSize.s2);
		#endif
		saveInVar(circlesCount);
		cout<<"circlesCount: "<<circlesCount<<endl;
	}
	else
	{
		cerr<<"File could not be opened!"<<endl;
	}
}

void FileCalculator::doStep(){
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

void FileCalculator::saveFrame(){
}

Circle* FileCalculator::getCircle(int i){
}

void FileCalculator::fpsChanged(scalar timeInt){
	//nothing to do
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
