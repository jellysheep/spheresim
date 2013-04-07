
#include "Calculator.h"
#include "GLWidget.h"
#include <exception>
#include <cmath>
#include <cstring>
#include "PlotWidget.h"

#include <iostream>
#include <iomanip>

#include <QFileDialog>

Calculator::Calculator(){
	
	glWidget = NULL;
	
	QObject::connect(this, SIGNAL(destroyed()), 
		this, SLOT(quit()), Qt::AutoConnection);
	
	performingAction = false;
	hasStopped = true;
	running = false;
	
	newFrame = false;
	elapsedFrames = 0;
	
	if(useColorHSV){
		if(use3D){
			lightTarget = 50;
		}else{
			lightTarget = 0;
		}
	}
	hueOffset = rans(360);
	ceBuffer = new SphereExtension[spheresCount];
	for(int i = 0; i<spheresCount; i++){
		initCeBuffer(i);
	}
	
	plotWg = NULL;
	forceCounter = 0;
	forceCounter2 = 0;
	numWalls = 6;
	wallForces = new scalar*[renderBufferCount];
	for(int i = 0; i<renderBufferCount; i++){
		wallForces[i] = new scalar[numWalls];
		for(int j = 0; j<numWalls; j++){
			wallForces[i][j] = 0;
		}
	}
	curWallForces = new scalar[numWalls];
	for(int j = 0; j<numWalls; j++){
		curWallForces[j] = 0;
	}
	bufferFilled = false;
	plotNext = false;
	
	curSimTime = QDateTime::currentDateTime();
}
/*
double Calculator::readHex(FILE* file){
	double d;
	unsigned long long* c = (unsigned long long*)&d;
	//always read double values
	fscanf(file, "%llX%llX", c[0], c[1]);
	return d;
}*/
void Calculator::addHex(std::ostream &f, double d, bool newLine=true){
	addHex(f, (float)d, newLine);
}
void Calculator::addHex(std::ostream &f, float fl, bool newLine=true){
	if(newLine) f<<"\nh";
	else f<<' ';
	f<<std::hex<<*((unsigned int*)&fl);
}
void Calculator::addHex(std::ostream &f, int i, bool newLine=true){
	if(newLine) f<<"\nh";
	else f<<' ';
	f<<std::hex<<i;
}
void Calculator::initFileSave(){
	const char* filter = (std::string("SphereSim View File (*.")+getViewFileExtension()+")").c_str();
	QString str = QFileDialog::getSaveFileName(0, ("Save file"), (std::string("./save.")+getViewFileExtension()).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		//exit(0);
		return;
	}
	const char* file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);
	
	f.open(file, std::fstream::out|std::fstream::trunc);
	//magic number, identifying SphereSim files
	f<<"# "<<getViewFileExtension()<<'\n';
	f<<"# SphereSim"<<(use3D?3:2)<<" View File\n\n";
	f<<"# h=hexadecimal, d=decimal values in line\n";
	f<<"\n# 3D:";
	addHex(f, (use3D?1:0));
	f<<"\n# sphere count:";
	addHex(f, spheresCount);
	f<<"\n# max. display sphere count:";
	addHex(f, maxShowSpheresCount);
	
	f<<"\n# box size (x,y"<<(use3D?",z":"")<<"):";
	addHex(f, boxSize.s0);
	addHex(f, boxSize.s1);
	if(use3D){
		addHex(f, boxSize.s2);
	}
	f<<"\n# sphere size (min,max):";
	addHex(f, sphereSize.s0);
	addHex(f, sphereSize.s1);
	
	f<<"\n# render fps max:";
	addHex(f, renderFpsMax);
	f<<"\n# render fps:";
	addHex(f, renderFps);
	
	f<<"\n# speed:";
	addHex(f, speed);
	f<<"\n# fps:";
	addHex(f, fps);
	f<<"\n# minFps:";
	addHex(f, minFps);
	
	f<<"\n# initial max. speed:";
	addHex(f, max_speed);
	f<<"\n# E modulus:";
	addHex(f, E);
	f<<"\n# poisson's value:";
	addHex(f, poisson);
	f<<"\n# elasticity:";
	addHex(f, elastic);
	f<<"\n# gravity:";
	addHex(f, gravity_abs);
	f<<"\n# gravitation factor:";
	addHex(f, G_fact);
	f<<"\n# air resistance factor:";
	addHex(f, airResistance);
	f<<"\n# wall resistance:";
	addHex(f, (wallResistance?1:0));
	f<<"\n\n# sizes, masses and initial positions of the spheres (r,m,x,y"<<(use3D?",z":"")<<"):";
	Sphere* c;
	for(int i = 0; i<spheresCount; i++){
		c = getSphere(i);
		addHex(f, c->size);
		addHex(f, c->mass, false);
		addHex(f, c->pos.s0, false);
		addHex(f, c->pos.s1, false);
		if(use3D){
			addHex(f, c->pos.s2, false);
		}
	}
	f<<"\n\n# positions of the spheres (x,y"<<(use3D?",z":"")<<"):";
	saveBool = true;
	//~ f.close();
}
void Calculator::saveConfig(){
	const char* filter = (std::string("SphereSim Config File (*.")+getConfigFileExtension()+")").c_str();
	QString str = QFileDialog::getSaveFileName(0, ("Save config"), (std::string("./save.")+getConfigFileExtension()).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		spheresCount = 0;
		//exit(0);
		return;
	}
	const char* file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);
	
	f2.open(file, std::fstream::out|std::fstream::trunc);
	//magic number, identifying SphereSim files
	f2<<"# "<<getViewFileExtension()<<'\n';
	f2<<"# SphereSim"<<(use3D?3:2)<<" View File\n\n";
	f2<<"# h=hexadecimal, d=decimal values in line\n";
	f2<<"\n# 3D:";
	addHex(f2, (use3D?1:0));
	f2<<"\n# sphere count:";
	addHex(f2, spheresCount);
	f2<<"\n# max. display sphere count:";
	addHex(f2, maxShowSpheresCount);
	
	f2<<"\n# box size (x,y"<<(use3D?",z":"")<<"):";
	addHex(f2, boxSize.s0);
	addHex(f2, boxSize.s1);
	if(use3D){
		addHex(f2, boxSize.s2);
	}
	f2<<"\n# sphere size (min,max):";
	addHex(f2, sphereSize.s0);
	addHex(f2, sphereSize.s1);
	
	f2<<"\n# render fps max:";
	addHex(f2, renderFpsMax);
	f2<<"\n# render fps:";
	addHex(f2, renderFps);
	
	f2<<"\n# speed:";
	addHex(f2, speed);
	f2<<"\n# fps:";
	addHex(f2, fps);
	f2<<"\n# minFps:";
	addHex(f2, minFps);
	
	f2<<"\n# initial max. speed:";
	addHex(f2, max_speed);
	f2<<"\n# E modulus:";
	addHex(f2, E);
	f2<<"\n# poisson's value:";
	addHex(f2, poisson);
	f2<<"\n# elasticity:";
	addHex(f2, elastic);
	f2<<"\n# gravity:";
	addHex(f2, gravity_abs);
	f2<<"\n# gravitation factor:";
	addHex(f2, G_fact);
	f2<<"\n# air resistance factor:";
	addHex(f2, airResistance);
	f2<<"\n# wall resistance:";
	addHex(f2, (wallResistance?1:0));
	f2<<"\n\n# fixed flag, sizes, masses, initial positions and speeds of the spheres (f,r,m,x,y"<<(use3D?",z":"")<<",vx,vy"<<(use3D?",vz":"")<<"):";
	Sphere* c;
	for(int i = 0; i<spheresCount; i++){
		c = getSphere(i);
		addHex(f2, (int)c->fixed);
		addHex(f2, c->size, false);
		addHex(f2, c->mass, false);
		addHex(f2, c->pos.s0, false);
		addHex(f2, c->pos.s1, false);
		if(use3D){
			addHex(f2, c->pos.s2, false);
		}
		addHex(f2, c->speed.s0, false);
		addHex(f2, c->speed.s1, false);
		if(use3D){
			addHex(f2, c->speed.s2, false);
		}
	}
	f2<<"\n\n# "<<getConfigFileExtension()<<" end of file";
	f2.close();
}
void Calculator::readLine(std::fstream& f){
	bool retry;
	int tries = 0;
	do{
		tries++;
		//std::cout<<"try "<<tries<<"... \n";
		retry = false;
		if(!getline(f, line)){
			retry = true;
			continue;
		}else{
			//std::cout<<"_line: "<<line<<"\n";
			if(line[0] == '#'){
				retry = true;
				continue;
			}
		}
		///remove comments
		int i = line.find('#');
		if(i!=std::string::npos){
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
	//std::cout<<"line: "<<line<<"\n";
	///std::decide between std::decimal and hexadecimal chars
	if(line[0] == 'h' || line[0] == 'H'){
		hexadec = true;
		//std::cout<<"std::hex.\n";
		line.erase(line.begin());
	}else{
		hexadec = false;
		//std::cout<<"std::dec.\n";
	}
	iss.clear();
	iss.str(line);
	//std::cout<<"new line: \""<<line<<"\"\n";
}

float Calculator::readHexFloat(std::fstream& f){
	float f_;
	unsigned int l = 0;
	//cout<<"line: \""<<line<<"\"\n";
	for(int _i = 0; !(iss>>std::hex>>l) && _i<5; _i++){
		readLine(f);
	}
	f_ = *((float*)&l);
	return f_;
}

float Calculator::readFloat(std::fstream& f){
	iss.peek();
	if(!iss) readLine(f);
	if(hexadec){
		//cout<<"reading std::hex...\n";
		return readHexFloat(f);
	}
	float f_ = 0;
	//cout<<"reading std::dec...\n";
	for(int _i = 0; !(iss>>std::dec>>f_) && _i<5; _i++){
		readLine(f);
		if(hexadec) return readHexFloat(f);
	}
	return f_;
}

void Calculator::saveInVar_(std::fstream& f, scalar &s){
	float f_ = readFloat(f);
	if(!eof){
		s = f_;
	}
}

void Calculator::saveInVar_(std::fstream& f, int &i){
	int j = 0;
	_read(f, iss, j);
	if(!eof) i = j;
}

void Calculator::stopFileSave(){
	bool run = running;
	if(run) stop();
	saveBool = false;
	///wait for file savings...
	f<<"\n\n# "<<getViewFileExtension()<<" end of file";
	f.close();
	if(run) start();
}

int norm(int i){
	if(i<0) return i+renderBufferCount;
	else return i;
}

int normHue(int i){
	while(i<0) i+=360;
	return i%360;
}

void Calculator::initCeBuffer(int i){
	int light;
	QColor* color;
	if(useColorHSV){
		if(use3D){
			lightTarget = 50;
		}else{
			lightTarget = 0;
		}
		ceBuffer[i].hsvColor = QColor::fromHsv(normHue(rani(60)), 255, 70+rani(90)+lightTarget);
		ceBuffer[i].color = ceBuffer[i].hsvColor.toRgb();
		//printf("H:%3d S:%3d V:%3d  R:%3d G:%3d B:%3d\n",i%360,255,255,ceBuffer[i].color.red(),ceBuffer[i].color.green(),ceBuffer[i].color.blue());
	}else{
		ceBuffer[i].color = QColor(rani(255),rani(255),rani(255));
		color = &ceBuffer[i].color;
		light = pow(pow(color->red(),2)+pow(color->green(),2)+pow(color->blue(),2),0.5);
		//printf("light before: %4d\n",light);
		color->setRed(color->red()*lightTarget/light);
		color->setGreen(color->green()*lightTarget/light);
		color->setBlue(color->blue()*lightTarget/light);
		light = pow(pow(color->red(),2)+pow(color->green(),2)+pow(color->blue(),2),0.5);
		//light = ((color%256)+(color/256%256)+(color/256/256))/3;
		//printf("light after:  %4d\n",light);
	}
	if(useColorsBool){
		ceBuffer[i].trace = new vector[traceCount];
		ceBuffer[i].traceCount = 0;
		ceBuffer[i].traceFull = false;
	}
}

void Calculator::set(GLWidget* w){
	glWidget = w;
}

scalar Calculator::getFrameBufferLoad(){
	int ri = bufferReadIndex, wi = bufferWriteIndex;
	while(wi<ri){
		wi += renderBufferCount;
	}
	return (1.0*(wi-ri))/renderBufferCount;
}

bool Calculator::isRunning(){
	return running||!hasStopped;
}

void Calculator::saveFrameToFile(){
	Sphere* c;
	for(int i = 0; i<readNum_render; i++){
		c = getDirectSphere(i);
		if(c == NULL){
			c = new Sphere();
			//printf("NULL error!\n");
		}
		addHex(f, c->pos.s0);
		addHex(f, c->pos.s1, false);
		if(use3D){
			addHex(f, c->pos.s2, false);
		}
	}
}

void Calculator::run(){
	printf("Calculator running!\n");
	int i = 0;
	try{
		while(running){
			doStep();

			if(saveBool)
			{
				save();
			}
			frameCounter++;
			
			elapsedFrames++;
			
			if(renderBool){
				//printf("fps: %8f renderFps: %8d\n", fps, renderFps);
				//printf("elapsed frames: %8d frames to elapse: %8f\n", elapsedFrames, (float)(fps/renderFps));
				//printf("frames: (%d/%d)=%d | %d\n", (int)fps, (int)renderFps, (int)(fps/renderFps), elapsedFrames);
				if(elapsedFrames > (int)(fps/renderFps) && glWidget != NULL){// && glWidget->drawingFinished){
					if(bufferReadIndex == ((bufferWriteIndex+1)%renderBufferCount)){
						printf("frame buffer full: %3d | %3d\r",bufferReadIndex,bufferWriteIndex);
					}else{
						if(saveFrame()){
							if(saveBool) saveFrameToFile();
							if(++forceCounter2 >= (renderFpsMax/plotFps)){
								bufferFilled = true;
								forceCounter2 = 0;
								for(int i = 0; i<numWalls; i++){
									wallForces[bufferWriteIndex][i] = curWallForces[i]/(scalar)forceCounter;
								}
								forceCounter = 0;
								plotNext = true;
							}else{
								for(int i = 0; i<numWalls; i++){
									wallForces[bufferWriteIndex][i] = wallForces[(renderBufferCount-1+bufferWriteIndex)%renderBufferCount][i];
								}
							}
							bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
						}
					
					
						elapsedFrames = 0;
					}
				}
				
				newFrame = true;
			}
		}
	}
	catch (std::exception& er) {
		printf("ERROR: %s\n", er.what());
	}
	hasStopped = true;
}

void Calculator::maxSphereCountChanged(int i){
	if(performingAction) return;
	performingAction = true;
	
	maxSphereCountChanged_subclass(i);
	maxShowSpheresCount = i;
	performingAction = false;
	if(glWidget!=NULL) glWidget->timeToRender2();
}

void Calculator::sphereCountChanged(int i){
	if(performingAction) return;
	printf("SpheresCount: %5d new: %5d\n", spheresCount, i);
	
	renderFpsMax = (autoSlowRender?(int)(45.0*pow(1/3.0, spheresCount/1000.0)+15):60);
	renderFps = renderFpsMax;
	
	if(i == spheresCount) return;
	performingAction = true;
	
	bool run = isRunning();
	if(run)stop();
	sphereCountChanged_subclass(i);
	
	ceBuffer = newCopy<SphereExtension>(ceBuffer, spheresCount, i);
	for(int j = spheresCount; j<i; j++){
		initCeBuffer(j);
	}
	//if(i<=spheresCount) //debug
	spheresCount = i;
	
	if(run)start();
	performingAction = false;
	if(glWidget!=NULL) glWidget->timeToRender2();
}

void Calculator::set(PlotWidget* plw){
	plotWg = plw;
}

#define onlyOneC 0
void Calculator::paintGL(bool readNewFrame){
	// Apply some transformations
	//glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	/*
	glRotatef(Clock.GetElapsedTime() * 50, 1.f, 0.f, 0.f);
	glRotatef(Clock.GetElapsedTime() * 30, 0.f, 1.f, 0.f);
	glRotatef(Clock.GetElapsedTime() * 90, 0.f, 0.f, 1.f);//*/
	
	if(readNewFrame){
		if(glWidget->newFrame){
			glWidget->newFrame = false;
			if(((bufferReadIndex+2)%renderBufferCount) == bufferWriteIndex){
				printf("frame buffer empty: %3d | %3d\r",bufferReadIndex,bufferWriteIndex);
			}else{
				// new buffer content
				bufferReadIndex = (bufferReadIndex+1)%renderBufferCount;
				// new point for plot
				if(plotNext && bufferFilled){
					plotNext = false;
					plotWg->addValue(wallForces[bufferReadIndex][0], wallForces[bufferReadIndex][1]);
				}
			}
		}
		if(bufferReadIndex == bufferWriteIndex){
			printf("error!!!\n");
		}
	}

	// Draw a cube
	if(showCube){
		glPushMatrix();
		glScalef(curUnit.size*boxSize.s[0], curUnit.size*boxSize.s[1], curUnit.size*boxSize.s[2]);
		glCallList(glWidget->displayList+1);
		glPopMatrix();
	}
	if(wireframe){
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}else{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
	
	scalar r,x,y,z;
	
	int i,k,h;
	Sphere* c;
	SphereExtension* ce;
	QColor* color;
	if(use3D){
		color = new QColor(102,102,102);
	}else{
		color = new QColor(30,30,30);
	}
	//err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, 0, .sizeof(Sphere)*readNum_render, c_CPU_render[bufferReadIndex], NULL, NULL);//&event);
	//printf("waiting for reading...\n");
	//event.wait();
	//printf("ready!\n");
	//queue.finish();
	if(useColorsBool && useColorHSV && readNewFrame){
		hueOffset+=hueStep;
		if(hueOffset>360)
			hueOffset -= 360;
		if(hueOffset<0)
			hueOffset += 360;
		for(i=0; i < readNum_render; i++){
			color = &ceBuffer[i].hsvColor;
			ceBuffer[i].color = QColor::fromHsv(normHue(color->hue()+(int)hueOffset+(isFixed(i)?100:0)), color->saturation(), color->value());
		}
	}
	
	if(useTrace){
		int traceCountNew = (int)(traceAmount/100.0*traceCount);
		glDisable(GL_LIGHTING);
		for(i=0; i < readNum_render; i++)
		{
			//printf("%4u: %5u\n", j, offset+i);
			//c = c_CPU_render[bufferReadIndex][i];
			c = getSphere(i);
			if(c == NULL) continue;
			r = c->size;
			x = c->pos.s[0];
			y = c->pos.s[1];
			if(use3D){
				z = c->pos.s[2];
			}else{
				z = 0;
			}
			ce = &ceBuffer[i];
			if(useColorsBool)
				color = &ce->color;
			ce->trace[ce->traceCount] = (vector){x,y,z};
			glColor4f(color->redF(),color->greenF(),color->blueF(),0);
			if(connectTracePoints)
				glBegin(GL_LINE_STRIP);
			else
				glBegin(GL_POINTS);
			k = 0;
			for(h = (ce->traceFull?((traceCount+ce->traceCount+1-traceCountNew)%traceCount):std::max(0,ce->traceCount-traceCountNew)); 
					h!=ce->traceCount; h=((h+1)%traceCount)){
				glColor4f(color->redF(),color->greenF(),color->blueF(),std::pow((k++)*1.0/traceCountNew,0.5)/2);
				if(use3D){
					glVertex3d(ce->trace[h].s[0], ce->trace[h].s[1], ce->trace[h].s[2]);
				}else{
					glVertex2d(ce->trace[h].s[0], ce->trace[h].s[1]);
				}
			}
			glColor4f(color->redF(),color->greenF(),color->blueF(),std::pow((k++)*1.0/traceCountNew,0.5)/2);
			if(use3D){
				glVertex3d(x,y,z);
			}else{
				glVertex2d(x,y);
			}
			if(readNewFrame){
				ce->traceCount++;
				if(ce->traceCount>=traceCount){
					ce->traceCount = 0;
					ce->traceFull = true;
				}
			}
			glEnd();
		}
		if(use3D){
			glEnable(GL_LIGHTING);
		}
	}
	for(i=0; i < readNum_render; i++)
	{
		//printf("%4u: %5u\n", j, offset+i);
		c = getSphere(i);
		if(c == NULL) continue;
		r = c->size;
		x = c->pos.s[0];
		y = c->pos.s[1];
		if(use3D){
			z = c->pos.s[2];
		}else{
			z = 0;
		}
		if(useColorsBool)
		{
			color = &ceBuffer[i].color;
		}
		if(!use3D){
			glPushMatrix();
			glTranslated(x,y,z);
			glScalef(r,r,r);
			#if onlyOneC
				if(i==cCount-1)
					glWidget->drawSphereF2(r,color->redF(),color->greenF(),color->blueF());
				else
					glWidget->drawSphereF2(r, 0.05, 0.05, 0.05);
			#else
				glBegin(GL_TRIANGLE_FAN);
				#if onlyOneC
					if(i==cCount-1)
						glColor3d(1,1,1); 
					else
						glColor3d(0.1, 0.1, 0.1); 
				#else
					glColor3d(1,1,1); 
				#endif
				glVertex3d((1/3.f),(1/3.f),0);
				
				glColor3f(color->redF(),color->greenF(),color->blueF()); 
				glWidget->drawSphereF2(1.f); 
			#endif
			glPopMatrix();
		}else{
			#if onlyOneC
				if(i==cCount-1)
					glColor3f(color->redF(),color->greenF(),color->blueF()); 
				else
					glColor3d(0.05, 0.05, 0.05);
			#else
				glColor3f(color->redF(),color->greenF(),color->blueF()); 
			#endif
			glPushMatrix();
			glTranslated(x,y,z);
			glScalef(r,r,r);
			glWidget->drawsphere2(r);
			glPopMatrix();
		}
	}
	if(use3D){
		if(showLights){
			glWidget->drawLights();
		}
	}
}
