
#include "Calculator.h"
#include "GLWidget.h"
#include <exception>
#include <cmath>

Calculator::Calculator(){
	hasStopped = true;
	running = false;
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

bool Calculator::getRunning(){
	return running||!hasStopped;
}

void Calculator::run(){
	printf("OpenClCalculator running!\n");
	int i = 0;
	try{
		while(running){
			doStep();

			if(saveBool)
			{
				save();
			}
			frameCounter++;
			
			if(renderBool){
				elapsedFrames++;
				//printf("frames: (%d/%d)=%d | %d\n", (int)fps, (int)renderFps, (int)(fps/renderFps), elapsedFrames);
				if(elapsedFrames > (int)(fps/renderFps) && glWidget != NULL){// && glWidget->drawingFinished){
					if(bufferReadIndex == ((bufferWriteIndex+1)%renderBufferCount)){
						printf("frame buffer full: %3d | %3d\r",bufferReadIndex,bufferWriteIndex);
					}else{
						saveFrame();
						bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
					
					
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
				bufferReadIndex = (bufferReadIndex+1)%renderBufferCount;
			}
		}
		if(bufferReadIndex == bufferWriteIndex){
			printf("error!!!\n");
		}
	}

	// Draw a cube
	glColor3d(0.2,0.2,0.2);
	glBegin(GL_LINE_LOOP);
	glVertex3d(0,0,0);
	glVertex3d(boxSize.s[0],0,0);
	glVertex3d(boxSize.s[0],boxSize.s[1],0);
	glVertex3d(0,boxSize.s[1],0);
	glEnd();
	if(_3D_!=0){
		glColor3d(0.2,0.2,0.2);
		glBegin(GL_LINE_LOOP);
		glVertex3d(0,0,boxSize.s[2]);
		glVertex3d(boxSize.s[0],0,boxSize.s[2]);
		glVertex3d(boxSize.s[0],boxSize.s[1],boxSize.s[2]);
		glVertex3d(0,boxSize.s[1],boxSize.s[2]);
		glEnd();
		glColor3d(0.2,0.2,0.2);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(0,0,boxSize.s[2]);
		glVertex3d(boxSize.s[0],0,0);
		glVertex3d(boxSize.s[0],0,boxSize.s[2]);
		glVertex3d(boxSize.s[0],boxSize.s[1],0);
		glVertex3d(boxSize.s[0],boxSize.s[1],boxSize.s[2]);
		glVertex3d(0,boxSize.s[1],0);
		glVertex3d(0,boxSize.s[1],boxSize.s[2]);
		glEnd();
	}
	
	scalar r,x,y,z,d;
	
	int i,k,h,j;
	Circle* c;
	CircleExtension* ce;
	#if _3D_
		int color = 102+(102*256)+(102*256*256);
	#else
		int color = 30+(30*256)+(30*256*256);
	#endif
	//err = queue.enqueueReadBuffer(cl_circles, CL_TRUE, 0, sizeof(Circle)*readNum_render, c_CPU_render[bufferReadIndex], NULL, NULL);//&event);
	//printf("waiting for reading...\n");
	//event.wait();
	//printf("ready!\n");
	//queue.finish();
	if(useTrace){
		glDisable(GL_LIGHTING);
		for(i=0; i < readNum_render; i++)
		{
			//printf("%4u: %5u\n", j, offset+i);
			//c = c_CPU_render[bufferReadIndex][i];
			c = getCircle(i);
			r = c->size;
			x = c->pos.s[0];
			y = c->pos.s[1];
#if _3D_
			z = c->pos.s[2];
#else
			z = 0;
#endif
			ce = &ceBuffer[i];
			if(useColoursBool)
				color = ce->color;
			ce->trace[ce->traceCount] = (vector){x,y
#if _3D_
				,z
#endif
				};
			glColor4b(color/256/256,color/256%256,color%256,0);
			if(connectTracePoints)
				glBegin(GL_LINE_STRIP);
			else
				glBegin(GL_POINTS);
			k = 0;
			for(h = (ce->traceFull?((ce->traceCount+1)%traceCount):0); h!=ce->traceCount; h=((h+1)%traceCount)){
				glColor4b(color/256/256,color/256%256,color%256,255*std::pow((k++)*1.0/traceCount,0.5)/2);
				#if _3D_
					glVertex3d(ce->trace[h].s[0], ce->trace[h].s[1], ce->trace[h].s[2]);
				#else
					glVertex2d(ce->trace[h].s[0], ce->trace[h].s[1]);
				#endif
			}
			glColor4b(color/256/256,color/256%256,color%256,255*std::pow((k++)*1.0/traceCount,0.5)/2);
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
		r = c->size;
		x = c->pos.s[0];
		y = c->pos.s[1];
#if _3D_
		z = c->pos.s[2];
#else
		z = 0;
#endif
		if(useColoursBool){
			color = ceBuffer[i].color;
		}
		//color = 102+(102*256)+(102*256*256);
		//printf("Circle (r=%3f): (%4f|%4f|%4f) (%4f|%4f|%4f) (%4f|%4f|%4f)\n",r,x,y,z,c.speed.s[0],c.speed.s[1],0,c.force.s[0],c.force.s[1],0);
		//if(_3D_==0)
		//{
		#if !_3D_
			glBegin(GL_TRIANGLE_FAN);
			#if onlyOneC
				if(i==cCount-1)
					glColor3d(1,1,1); 
				else
					glColor3d(0.1, 0.1, 0.1); 
			#else
				glColor3d(1,1,1); 
			#endif
			glVertex3d(x-(r/3),y+(r/3),z);
			#if onlyOneC
				if(i==cCount-1)
					//glColor3bv((byte*)&color);
					glColor3b(color/256/256,color/256%256,color%256); 
				else
					glColor3d(0.05, 0.05, 0.05);
			#else
				//glColor3bv((GLbyte*)&color);
				glColor3b(color/256/256,color/256%256,color%256); 
			#endif
			d = 0;
			for(j = 0; j<=edges; j++){
				d+=step;
				//cout<<edges<<" "<<j<<endl;
				//cout<<d<<endl;
				glVertex3d(x+cos(d)*r,y+sin(d)*r,z);
			}

			glEnd();
		//}else{ //3D
		#else
			#if onlyOneC
				if(i==cCount-1)
					//glColor3bv((byte*)&color);
					glColor3b(color/256/256,color/256%256,color%256); 
				else
					glColor3d(0.05, 0.05, 0.05);
			#else
				//glColor3bv((GLbyte*)&color);
				glColor3b(color/256/256,color/256%256,color%256); 
			#endif
			glPushMatrix();
			glTranslated(x,y,z);
			glWidget->drawsphere(1,r);
			//glTranslated(-x,-y,-z);
			glPopMatrix();
		//}
		#endif
	}
}
