
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
		this, SLOT(quit()), Qt::DirectConnection);
	
	performingAction = false;
	hasStopped = true;
	running = false;
	
	newFrame = false;
	elapsedFrames = 0;
	
	if(useColorHSV){
		#if _3D_
			lightTarget = 50;
		#else
			lightTarget = 0;
		#endif
	}
	hueOffset = rans(360);
	ceBuffer = new CircleExtension[circlesCount];
	for(int i = 0; i<circlesCount; i++){
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
	const char* filter = (std::string("SphereSim View File (*.")+viewFileExtension+")").c_str();
	QString str = QFileDialog::getSaveFileName(0, ("Save file"), (std::string("./save.")+viewFileExtension).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		circlesCount = 0;
		//exit(0);
		return;
	}
	const char* file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);
	
	f.open(file, std::fstream::out|std::fstream::trunc);
	//magic number, identifying SphereSim files
	f<<"# "<<viewFileExtension<<'\n';
	f<<"# SphereSim"<<(_3D_==1?3:2)<<" View File\n\n";
	f<<"# h=hexadecimal, d=decimal values in line\n";
	f<<"\n# 3D:";
	addHex(f, _3D_);
	f<<"\n# sphere count:";
	addHex(f, circlesCount);
	f<<"\n# max. display sphere count:";
	addHex(f, maxShowCirclesCount);
	
	f<<"\n# box size (x,y"<<(_3D_==1?",z":"")<<"):";
	addHex(f, boxSize.s0);
	addHex(f, boxSize.s1);
	#if _3D_
		addHex(f, boxSize.s2);
	#endif
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
	f<<"\n\n# sizes, masses and initial positions of the spheres (r,m,x,y"<<(_3D_==1?",z":"")<<"):";
	Circle* c;
	for(int i = 0; i<circlesCount; i++){
		c = getCircle(i);
		addHex(f, c->size);
		addHex(f, c->mass, false);
		addHex(f, c->pos.s0, false);
		addHex(f, c->pos.s1, false);
		#if _3D_
			addHex(f, c->pos.s2, false);
		#endif
	}
	f<<"\n\n# positions of the spheres (x,y"<<(_3D_==1?",z":"")<<"):";
	saveBool = true;
	//~ f.close();
}
void Calculator::saveConfig(){
	const char* filter = (std::string("SphereSim Config File (*.")+configFileExtension+")").c_str();
	QString str = QFileDialog::getSaveFileName(0, ("Save config"), (std::string("./save.")+configFileExtension).c_str(), (filter));
	if(str == ""){
		std::cerr<<"File could not be opened!"<<std::endl;
		circlesCount = 0;
		//exit(0);
		return;
	}
	const char* file = (const char*) str.toStdString().c_str();
	
	printf("File: %s\n", file);
	
	f2.open(file, std::fstream::out|std::fstream::trunc);
	//magic number, identifying SphereSim files
	f2<<"# "<<viewFileExtension<<'\n';
	f2<<"# SphereSim"<<(_3D_==1?3:2)<<" View File\n\n";
	f2<<"# h=hexadecimal, d=decimal values in line\n";
	f2<<"\n# 3D:";
	addHex(f2, _3D_);
	f2<<"\n# sphere count:";
	addHex(f2, circlesCount);
	f2<<"\n# max. display sphere count:";
	addHex(f2, maxShowCirclesCount);
	
	f2<<"\n# box size (x,y"<<(_3D_==1?",z":"")<<"):";
	addHex(f2, boxSize.s0);
	addHex(f2, boxSize.s1);
	#if _3D_
		addHex(f2, boxSize.s2);
	#endif
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
	f2<<"\n\n# fixed flag, sizes, masses, initial positions and speeds of the spheres (f,r,m,x,y"<<(_3D_==1?",z":"")<<",vx,vy"<<(_3D_==1?",vz":"")<<"):";
	Circle* c;
	for(int i = 0; i<circlesCount; i++){
		c = getCircle(i);
		addHex(f2, (int)c->fixed);
		addHex(f2, c->size, false);
		addHex(f2, c->mass, false);
		addHex(f2, c->pos.s0, false);
		addHex(f2, c->pos.s1, false);
		#if _3D_
			addHex(f2, c->pos.s2, false);
		#endif
		addHex(f2, c->speed.s0, false);
		addHex(f2, c->speed.s1, false);
		#if _3D_
			addHex(f2, c->speed.s2, false);
		#endif
	}
	f2<<"\n\n# "<<configFileExtension<<" end of file";
	f2.close();
}
void Calculator::readLine(){
	bool retry;
	int tries = 0;
	do{
		tries++;
		retry = false;
		if(!getline(f2, line)){
			retry = true;
			continue;
		}else if(line[0] == '#'){
			retry = true;
			continue;
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
	//cout<<"line: "<<line<<"\n";
	///std::decide between std::decimal and hexadecimal chars
	if(line[0] == 'h' || line[0] == 'H'){
		hexadec = true;
		//cout<<"std::hex.\n";
		line.erase(line.begin());
	}else{
		hexadec = false;
		//cout<<"std::dec.\n";
	}
	iss.clear();
	iss.str(line);
	//cout<<"new line: \""<<line<<"\"\n";
}

float Calculator::readHexFloat(){
	float f;
	unsigned int l = 0;
	//cout<<"line: \""<<line<<"\"\n";
	for(int _i = 0; !(iss>>std::hex>>l) && _i<5; _i++){
		readLine();
	}
	f = *((float*)&l);
	return f;
}

float Calculator::readFloat(){
	iss.peek();
	if(!iss) readLine();
	if(hexadec){
		//cout<<"reading std::hex...\n";
		return readHexFloat();
	}
	float f = 0;
	//cout<<"reading std::dec...\n";
	for(int _i = 0; !(iss>>std::dec>>f) && _i<5; _i++){
		readLine();
		if(hexadec) return readHexFloat();
	}
	return f;
}

void Calculator::saveInVar_(scalar &s){
	float f = readFloat();
	if(!eof){
		s = f;
	}
}

void Calculator::saveInVar_(int &i){
	int j;
	_read(iss, j);
	if(!eof) i = j;
}

void Calculator::stopFileSave(){
	bool run = running;
	if(run) stop();
	saveBool = false;
	///wait for file savings...
	f<<"\n\n# "<<viewFileExtension<<" end of file";
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
		#if _3D_
			lightTarget = 50;
		#else
			lightTarget = 0;
		#endif
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
	Circle* c;
	for(int i = 0; i<readNum_render; i++){
		c = getDirectCircle(i);
		if(c == NULL){
			c = new Circle();
			//printf("NULL error!\n");
		}
		addHex(f, c->pos.s0);
		addHex(f, c->pos.s1, false);
		#if _3D_
			addHex(f, c->pos.s2, false);
		#endif
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

void Calculator::maxCircleCountChanged(int i){
	if(performingAction) return;
	performingAction = true;
	
	maxCircleCountChanged_subclass(i);
	maxShowCirclesCount = i;
	performingAction = false;
	if(glWidget!=NULL) glWidget->timeToRender2();
}

void Calculator::circleCountChanged(int i){
	if(performingAction) return;
	printf("CirclesCount: %5d new: %5d\n", circlesCount, i);
	if(i == circlesCount) return;
	performingAction = true;
	
	bool run = isRunning();
	if(run)stop();
	circleCountChanged_subclass(i);
	
	ceBuffer = newCopy<CircleExtension>(ceBuffer, circlesCount, i);
	for(int j = circlesCount; j<i; j++){
		initCeBuffer(j);
	}
	//if(i<=circlesCount) //debug
	circlesCount = i;
	
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
		glScalef(boxSize.s[0], boxSize.s[1], boxSize.s[2]);
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
	Circle* c;
	CircleExtension* ce;
	QColor* color;
	#if _3D_
		color = new QColor(102,102,102);
	#else
		color = new QColor(30,30,30);
	#endif
	//err = queue.enqueueReadBuffer(cl_circles, CL_TRUE, 0, .sizeof(Circle)*readNum_render, c_CPU_render[bufferReadIndex], NULL, NULL);//&event);
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
			c = getCircle(i);
			if(c == NULL) continue;
			r = c->size;
			x = c->pos.s[0];
			y = c->pos.s[1];
			#if _3D_
				z = c->pos.s[2];
			#else
				z = 0;
			#endif
			ce = &ceBuffer[i];
			if(useColorsBool)
				color = &ce->color;
			ce->trace[ce->traceCount] = (vector){x,y
			#if _3D_
				,z
			#endif
				};
			glColor4f(color->redF(),color->greenF(),color->blueF(),0);
			if(connectTracePoints)
				glBegin(GL_LINE_STRIP);
			else
				glBegin(GL_POINTS);
			k = 0;
			for(h = (ce->traceFull?((traceCount+ce->traceCount+1-traceCountNew)%traceCount):std::max(0,ce->traceCount-traceCountNew)); 
					h!=ce->traceCount; h=((h+1)%traceCount)){
				glColor4f(color->redF(),color->greenF(),color->blueF(),std::pow((k++)*1.0/traceCountNew,0.5)/2);
				#if _3D_
					glVertex3d(ce->trace[h].s[0], ce->trace[h].s[1], ce->trace[h].s[2]);
				#else
					glVertex2d(ce->trace[h].s[0], ce->trace[h].s[1]);
				#endif
			}
			glColor4f(color->redF(),color->greenF(),color->blueF(),std::pow((k++)*1.0/traceCountNew,0.5)/2);
			#if _3D_
				glVertex3d(x,y,z);
			#else
				glVertex2d(x,y);
			#endif
			ce->traceCount++;
			if(ce->traceCount>=traceCount){
				ce->traceCount = 0;
				ce->traceFull = true;
			}
			glEnd();
		}
		#if _3D_
			glEnable(GL_LIGHTING);
		#endif
	}
	for(i=0; i < readNum_render; i++)
	{
		//printf("%4u: %5u\n", j, offset+i);
		c = getCircle(i);
		if(c == NULL) continue;
		r = c->size;
		x = c->pos.s[0];
		y = c->pos.s[1];
#if _3D_
		z = c->pos.s[2];
#else
		z = 0;
#endif
		if(useColorsBool)
		{
			color = &ceBuffer[i].color;
		}
		#if !_3D_
			glPushMatrix();
			glTranslated(x,y,z);
			glScalef(r,r,r);
			#if onlyOneC
				if(i==cCount-1)
					glWidget->drawCircleF2(r,color->redF(),color->greenF(),color->blueF());
				else
					glWidget->drawCircleF2(r, 0.05, 0.05, 0.05);
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
				glWidget->drawCircleF2(1.f); 
			#endif
			glPopMatrix();
		#else
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
		#endif
	}
	#if _3D_
		if(showLights){
			glWidget->drawLights();
		}
	#endif
}
