#ifdef ENGINE_OPENCL

#include "OpenClCalculator.h"

#include <QtWidgets/QApplication>
#include <cstdio>
//#include <stdio.h>
#include <cstdlib>
//#include <stdlib.h>
//#include <time.h>
#include <string>
#include <string.h>
#include <cmath>
#include "NanosecondTimer.h"
#include "GLWidget.h"

OpenClCalculator::OpenClCalculator():Calculator(){
	try{
		printf("sizeof(Sphere): %d\n", sizeof(Sphere));
		srand(20);
		srand(NanosecondTimer::getNS());
		
		glWidget = NULL;
		printf("Initialize OpenCL object and context\n");
		//setup devices and context
		
		//this function is defined in util.cpp
		//it comes from the NVIDIA SDK example code
		///err = oclGetPlatformID(&platform);
		//oclErrorString is also defined in util.cpp and comes from the NVIDIA SDK
		///printf("oclGetPlatformID: %s\n", oclErrorString(err));
		std::vector<cl::Platform> platforms;
		err = cl::Platform::get(&platforms);
		printf("cl::Platform::get(): %s\n", (err));
		printf("numer of platforms: %d\n", platforms.size());
		if (platforms.size() == 0) {
			printf("Platform sphereSize 0\n");
		}
		
		//for right now we just use the first available device
		//later you may have criteria (such as support for different extensions)
		//that you want to use to select the device
		deviceUsed = 0;
		//create the context
		///context = clCreateContext(0, 1, &devices[deviceUsed], NULL, NULL, &err);
		//context properties will be important later, for now we go with defualts
		try{
			cl_context_properties properties[] = 
				{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
		

			context = cl::Context(CL_DEVICE_TYPE_ALL, properties);
				
		}
		catch (cl::Error er) {
			printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
		}
		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		
		printf("number of devices: %d\n", devices.size());
		
		//create the command queue we will use to execute OpenCL commands
		///command_queue = clCreateCommandQueue(context, devices[deviceUsed], 0, &err);
		try{
			queue = cl::CommandQueue(context, devices[deviceUsed], 0, &err);
		}
		catch (cl::Error er) {
			printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
		}
		
		
		//Program Setup
		printf("load the program\n");
		

		int pl;
		char *kernel_source;
		kernel_source = file_contents("../SpheresCalculator.cl", &pl);
		if(kernel_source == NULL){
			printf("ERROR: OpenCL initialization failed!\n");
			return;
		}
		kernel_source[13] = '0'+(use3D?1:0); //define OpenCL version of use3D
		kernel_source[32] = '0'+_double_; //define OpenCL version of use3D
		if(G == 0) kernel_source[46] = '0';

		//pl = kernel_source.boxSize();
		printf("kernel boxSize: %d\n", pl);
		//printf("kernel: \n %s\n", kernel_source.c_str());
		try
		{
			cl::Program::Sources source(1,
				std::make_pair(kernel_source, pl));
			program = cl::Program(context, source);
		}
		catch (cl::Error er) {
			printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
		}

		printf("build program\n");
		try
		{
			err = program.build(devices, "");//-cl-fast-relaxed-math -cl-single-precision-constant
		}
		catch (cl::Error er) {
			printf("program.build: %s\n", oclErrorString(er.err()));
		//if(err != CL_SUCCESS){
		}
		printf("done building program\n");
		printf("Build Status:  %u\n", program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]));
		printf("Build Options: %s\n", program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]).c_str());
		printf("Build Log:	%s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]).c_str());
		printf("OpenCL initialization finished!\n");
		printf("\n");
		
		
		//initialize our kernel from the program
		try{
			moveStep_kernel = cl::Kernel(program, "moveStep2", &err);
			randomFill_kernel = cl::Kernel(program, "randomFill", &err);
			moveStep_addInterForces_kernel = cl::Kernel(program, "moveStep3_addInterForces", &err);
			moveStep_addWallForces_kernel = cl::Kernel(program, "moveStep3_addWallForces", &err);
			moveStep_updatePositions_kernel = cl::Kernel(program, "moveStep3_updatePositions", &err);
		}
		catch (cl::Error er) {
			printf("ERROR: %s(%d)\n", er.what(), er.err());
		}

		//initialize our CPU memory arrays, send them to the device and set the kernel arguements

		///Sphere sphere[spheresCount];
		//sphere.A = .5f;
		//sphere.B = 10.0f;
		//sphere.C = 3;
		/*
		sphere[i].D = .01;
		sphere[i].E = 1;
		*/
		uint* m_z = new uint(rand());
		//for(int i = 0; i<10000; i++);
		uint* m_w = new uint(rand());
		
		int cl_mem_method = CL_MEM_COPY_HOST_PTR;
		//cl_mem_method = CL_MEM_USE_HOST_PTR;

		printf("Creating OpenCL arrays\n");
		//*
		if(false || (cl_mem_method & CL_MEM_USE_HOST_PTR)>0){
			cl_spheres = cl::Buffer(context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, sizeof(Sphere)*spheresCount, spheresBuffer = new Sphere[spheresCount], &err);
			if(err!=CL_SUCCESS)printf("ERROR: creating spheres buffer: %s\n", oclErrorString(err));
			printf("spheres buffer allocated.\n");
			spheresBufferUsed = true;
		}else // */
		{
			cl_spheres = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Sphere)*spheresCount, NULL, &err);
			spheresBufferUsed = false;
		}
//		cl_vector3* boxSize_cl = clVector(boxSize);
//		cl_vector2* sphereSize_cl = clVector(sphereSize);
//		cl_vector* gravity_cl = clVector(gravity);
		cl_m_z = cl::Buffer(context, CL_MEM_READ_WRITE|cl_mem_method, sizeof(uint), m_z, &err);
		cl_m_w = cl::Buffer(context, CL_MEM_READ_WRITE|cl_mem_method, sizeof(uint), m_w, &err);
		cl_boxSize = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(vector3), &boxSize, &err);
		cl_sphereSize = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(vector2), &sphereSize, &err);
		cl_max_speed = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(scalar), &max_speed, &err);
		cl_spheresCount = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(int), &spheresCount, &err);
		scalar* E_ = new scalar(1000000.0*E);
		cl_E = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(scalar), E_, &err);
		cl_elastic = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(scalar), &elastic, &err);
		cl_gravity = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(vector), &gravity, &err);
		cl_timeInterval = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(scalar), &timeInterval, &err);
		cl_poisson = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(scalar), &poisson, &err);
		scalar* G_ = new scalar(G*G_fact);
		cl_G = cl::Buffer(context, CL_MEM_READ_ONLY|cl_mem_method, sizeof(scalar), G_, &err);
		cl_flags = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int)*spheresCount, NULL, &err);

		printf("Pushing data to the GPU\n");
		//push our CPU arrays to the GPU

		//write the sphere struct to GPU memory as a buffer
		//err = queue.enqueueWriteBuffer(cl_spheres, CL_TRUE, 0, sizeof(Sphere), &sphere, NULL, &event);
		/*err = queue.enqueueWriteBuffer(cl_m_z, CL_TRUE, 0, sizeof(uint), m_z, NULL, &event);
		err = queue.enqueueWriteBuffer(cl_m_w, CL_TRUE, 0, sizeof(uint), m_w, NULL, &event);
		err = queue.enqueueWriteBuffer(cl_boxSize, CL_TRUE, 0, sizeof(double_vec), boxSize2D, NULL, &event);
		err = queue.enqueueWriteBuffer(cl_max_speed, CL_TRUE, 0, sizeof(double), &max_speed, NULL, &event);
		err = queue.enqueueWriteBuffer(cl_spheresCount, CL_TRUE, 0, sizeof(int), &spheresCount, NULL, &event);
		err = queue.enqueueWriteBuffer(cl_E, CL_TRUE, 0, sizeof(double), &E, NULL, &event);*/
		

		//Wait for the command queue to finish these commands before proceeding
		queue.finish();
	  


		//set the arguements of our kernel
		err = randomFill_kernel.setArg(0, cl_spheres);
		err = randomFill_kernel.setArg(1, cl_m_z);
		err = randomFill_kernel.setArg(2, cl_m_w);
		err = randomFill_kernel.setArg(3, cl_boxSize);
		err = randomFill_kernel.setArg(4, cl_max_speed);
		err = randomFill_kernel.setArg(5, cl_sphereSize);
		err = randomFill_kernel.setArg(6, cl_poisson);
		err = randomFill_kernel.setArg(7, cl_E);
		err = randomFill_kernel.setArg(8, cl_spheresCount);
		err = randomFill_kernel.setArg(9, cl_flags);
		
		err = moveStep_kernel.setArg(0, cl_spheres);
		err = moveStep_kernel.setArg(1, cl_spheresCount);
		err = moveStep_kernel.setArg(2, cl_boxSize);
		err = moveStep_kernel.setArg(3, cl_elastic);
		err = moveStep_kernel.setArg(4, cl_gravity);
		err = moveStep_kernel.setArg(5, cl_timeInterval);
		err = moveStep_kernel.setArg(6, cl_G);
		
		err = moveStep_addInterForces_kernel.setArg(0, cl_spheres);
		err = moveStep_addInterForces_kernel.setArg(1, cl_elastic);
		err = moveStep_addInterForces_kernel.setArg(2, cl_G);
		err = moveStep_addInterForces_kernel.setArg(3, cl_flags);
		
		err = moveStep_addWallForces_kernel.setArg(0, cl_spheres);
		err = moveStep_addWallForces_kernel.setArg(1, cl_boxSize);
		err = moveStep_addWallForces_kernel.setArg(2, cl_elastic);
		
		err = moveStep_updatePositions_kernel.setArg(0, cl_spheres);
		err = moveStep_updatePositions_kernel.setArg(1, cl_gravity);
		err = moveStep_updatePositions_kernel.setArg(2, cl_timeInterval);

		//Wait for the command queue to finish these commands before proceeding
		queue.finish();
		
		
		
		printf("runKernel\n");
		if(useSplitKernels){
			events = new cl::Event[numEvents*3];
		}else{
			events = new cl::Event[numEvents];
		}
		eventCounter = 0;
		eventsFull = false;
		//std::vector<cl::Event> event2(1);
		if(saveBool){
			file = fopen("save.txt","w");
			fprintf(file, "%u ", (use3D?1:0));
			fprintf(file, "%g ", boxSize.s[0]);
			fprintf(file, "%g ", boxSize.s[1]);
			if(use3D){
				fprintf(file, "%g ", boxSize.s[2]);
			}
			fprintf(file, "%u\n", spheresCount);
			fclose(file);
			
			//~ readNum_save = min(1000,spheresCount/2);
			//~ c_CPU_save[0] = new Sphere[readNum_save];
			//~ c_CPU_save[1] = new Sphere[readNum_save];
		}
			
		long long ns = NanosecondTimer::getNS();
		err = queue.enqueueNDRangeKernel(randomFill_kernel, cl::NullRange, cl::NDRange(spheresCount), cl::NullRange, NULL, &event); 
		if(err!=CL_SUCCESS)printf("clEnqueueNDRangeKernel: %s\n", oclErrorString(err));
		queue.finish();
		printf("kernel randomFill executed successfully!\nTime needed: %8.3f ms\n", (NanosecondTimer::getNS()-ns)/1000000.0);
		
		if(renderBool){
			//c_CPU_render[0] = new Sphere[readNum_render];
			//c_CPU_render[1] = new Sphere[readNum_render];
			readNum_render = std::min(maxShowSpheresCount,spheresCount);
			c_CPU_render = new Sphere*[renderBufferCount];
			for(int i = 0; i<renderBufferCount; i++){
				c_CPU_render[i] = new Sphere[readNum_render];
			}
			bufferReadIndex = 0;
			bufferWriteIndex = 0;
			saveFrame();
			//err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, 0, sizeof(Sphere)*readNum_render, c_CPU_render[bufferWriteIndex], NULL, NULL);//&event);
			bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
			saveFrame();
			//err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, 0, sizeof(Sphere)*readNum_render, c_CPU_render[bufferWriteIndex], NULL, NULL);//&event);
			bufferWriteIndex = ((bufferWriteIndex+1)%renderBufferCount);
		}
		
		/*double r,x,y,z;
		int j = 0;
		int offset = 0;
		Sphere c;
		err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, sizeof(Sphere)*offset, sizeof(Sphere)*min((spheresCount-offset),readNum), c_CPU_render[j], NULL, &event);
		//offset+=readNum;
		for(; offset<spheresCount; offset+=readNum){
			event.wait();
			if(spheresCount-(offset+readNum)>0){
				err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, sizeof(Sphere)*(offset+readNum), sizeof(Sphere)*min((spheresCount-(offset+readNum)),readNum), c_CPU_render[((j+1)%2)], NULL, &event);
			}
			//queue.finish();

			for(int i=0; i < min((spheresCount-offset),readNum); i++)
			{
				//printf("%4u: %5u\n", j, offset+i);
				c = c_CPU_render[j][i];
				r = c.size;
				x = c.pos.s[0];
				y = c.pos.s[1];
				z = 0;
				printf("Sphere sphereSize(%3f) mass(%3f) E(%3f) ",r,c.mass,c.E);
				printf("pos(%4f|%4f|%4f) ",x,y,z);
				printf("speed(%4f|%4f|%4f) ",c.speed.s[0],c.speed.s[1],0.0);
				printf("force(%4f|%4f|%4f)\n",c.force.s[0],c.force.s[1],0.0);
			}
		}*/
		
		
		/*
		err = queue.enqueueNDRangeKernel(moveStep_kernel, cl::NullRange, cl::NDRange(spheresCount), cl::NullRange, NULL, NULL); 
		queue.finish();
		
		printf("after kernel moveStep:\n");
		
		j = 0;
		offset = 0;
		err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, sizeof(Sphere)*offset, sizeof(Sphere)*min((spheresCount-offset),readNum), c_CPU_render[j], NULL, &event);
		//offset+=readNum;
		for(; offset<spheresCount; offset+=readNum){
			if(spheresCount-(offset+readNum)>0){
				err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, sizeof(Sphere)*(offset+readNum), sizeof(Sphere)*min((spheresCount-(offset+readNum)),readNum), c_CPU_render[((j+1)%2)], NULL, &event);
			}
			//queue.finish();

			for(int i=0; i < min((spheresCount-offset),readNum); i++)
			{
				//printf("%4u: %5u\n", j, offset+i);
				c = c_CPU_render[j][i];
				r = c.size;
				x = c.pos.s[0];
				y = c.pos.s[1];
				z = 0;
				printf("Sphere sphereSize(%3f) mass(%3f) E(%3f) poisson(%3f) ",r,c.mass,c.E,c.poisson);
				printf("pos(%4f|%4f|%4f) ",x,y,z);
				printf("speed(%4f|%4f|%4f) ",c.speed.s[0],c.speed.s[1],0.0);
				printf("force(%4f|%4f|%4f)\n",c.force.s[0],c.force.s[1],0.0);
			}
		}*/
	}
	catch (cl::Error er) {
		printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
	}
}

void OpenClCalculator::fpsChanged(scalar timeInterval){
	err = queue.enqueueWriteBuffer(cl_timeInterval, CL_TRUE, 0, sizeof(scalar), &timeInterval, NULL, NULL);
}

void OpenClCalculator::boxSizeChanged(){
	//cl_vector3* boxSize_cl = clVector(boxSize);
	err = queue.enqueueWriteBuffer(cl_boxSize, CL_TRUE, 0, sizeof(vector3), &boxSize, NULL, NULL);
}

void OpenClCalculator::gravityChanged(){
	//cl_vector* gravity_cl = clVector(gravity);
	err = queue.enqueueWriteBuffer(cl_gravity, CL_TRUE, 0, sizeof(vector), &gravity, NULL, NULL);
}

void OpenClCalculator::save(){
	file = fopen("save.txt","a");
	int j = 0;
	int offset = 0;
	err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, sizeof(Sphere)*offset, sizeof(Sphere)*std::min((spheresCount-offset),readNum_render), c_CPU_save[j=((j+1)%2)], NULL, &event);
	offset+=readNum_render;
	for(; offset<spheresCount; offset+=readNum_render){
		event.wait();
		err = queue.enqueueReadBuffer(cl_spheres, CL_TRUE, sizeof(Sphere)*offset, sizeof(Sphere)*std::min((spheresCount-offset),readNum_render), c_CPU_save[j=((j+1)%2)], NULL, &event);
		//queue.finish();

		for(int i=0; i < std::min((spheresCount-offset),readNum_render); i++)
		{
			/*addHex(file, c_CPU_save[j][i].size);
			fprintf(file," ");
			addHex(file, c_CPU_save[j][i].pos.s[0]);
			fprintf(file," ");
			addHex(file, c_CPU_save[j][i].pos.s[1]);*/
			//if(use3D){
			//	f<<" ";
			//	add(f, c_CPU_save[j][i].pos.s[2]);
			//}
			fprintf(file,"\n");
		}
	}
	fclose(file);
}

Sphere* OpenClCalculator::getSphere(int i){
	return &c_CPU_render[bufferReadIndex][i];
}

Sphere* OpenClCalculator::getDirectSphere(int i){
	return &c_CPU_render[bufferWriteIndex][i];
}

void OpenClCalculator::doStep(){
	if(useSplitKernels){
		err = queue.enqueueNDRangeKernel(moveStep_addInterForces_kernel , cl::NullRange, cl::NDRange(spheresCount,spheresCount), cl::NDRange(1,1), NULL, &events[eventCounter]);
		//err = queue.enqueueNDRangeKernel(moveStep_addInterForces_kernel , cl::NullRange, cl::NDRange(spheresCount,spheresCount), cl::NDRange(1,1), NULL, &events[eventCounter]);
		err = queue.enqueueNDRangeKernel(moveStep_addWallForces_kernel, cl::NullRange, cl::NDRange(spheresCount), cl::NullRange, NULL, &events[eventCounter+1]); 
		err = queue.enqueueNDRangeKernel(moveStep_updatePositions_kernel, cl::NullRange, cl::NDRange(spheresCount), cl::NullRange, NULL, &events[eventCounter+2]); 
		eventCounter++;
	}else{
		err = queue.enqueueNDRangeKernel(moveStep_kernel, cl::NullRange, cl::NDRange(spheresCount), cl::NDRange(256), NULL, &events[eventCounter++]);
	}
	//printf("step %i\n", i++);
	if(eventCounter>=numEvents){
		eventCounter = 0;
		eventsFull = true;
	}
	if(eventsFull){
		//event2.at(0)=events[eventCounter];
		//printf(
		//queue.enqueueWaitForEvents(event2);
		if(useSplitKernels){
			events[eventCounter].wait();
			events[eventCounter].~Event();
			events[eventCounter+1].wait();
			events[eventCounter+1].~Event();
			events[eventCounter+2].wait();
			events[eventCounter+2].~Event();
		}else{
			events[eventCounter].wait();
			events[eventCounter].~Event();
		}
		//context.release(events[eventCounter]);
		//queue.release(events[eventCounter]);
	}else{
		queue.finish();
	}
	//if(err!=CL_SUCCESS)printf("clEnqueueNDRangeKernel: %s\n", oclErrorString(err));
	//queue.finish();
}

bool OpenClCalculator::saveFrame(){
	err = queue.enqueueReadBuffer(cl_spheres, CL_FALSE, 0, sizeof(Sphere)*readNum_render, c_CPU_render[bufferWriteIndex], NULL, NULL);
	return true;
}

bool OpenClCalculator::isFixed(int i){
	return false;
}

void OpenClCalculator::sphereCountChanged_subclass(int i){
	
}
void OpenClCalculator::maxSphereCountChanged_subclass(int i){
	
}
void OpenClCalculator::updateG(){
	
}
void OpenClCalculator::updateAirResistance(){
	
}
void OpenClCalculator::updateWallResistance(){
	
}
void OpenClCalculator::updateEModul(){
	
}
void OpenClCalculator::updatePoisson(){
	
}
void OpenClCalculator::updateElasticity(){
	
}
void OpenClCalculator::updateGridSize(){
	
}
void OpenClCalculator::loadConfig(){
	
}
void OpenClCalculator::updateMagnitude(){
	
}

char* OpenClCalculator::file_contents(const char *filename, int *length)
{
	FILE *f = fopen(filename, "r");
	void *buffer;

	if (!f) {
		fprintf(stderr, "Unable to open %s for reading\n", filename);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = malloc(*length+1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	((char*)buffer)[*length] = '\0';

	return (char*)buffer;
}

// Helper function to get error string
// *********************************************************************
const char* OpenClCalculator::oclErrorString(cl_int error)
{
	static const char* errorString[] = {
		"CL_SUCCESS",
		"CL_DEVICE_NOT_FOUND",
		"CL_DEVICE_NOT_AVAILABLE",
		"CL_COMPILER_NOT_AVAILABLE",
		"CL_MEM_OBJECT_ALLOCATION_FAILURE",
		"CL_OUT_OF_RESOURCES",
		"CL_OUT_OF_HOST_MEMORY",
		"CL_PROFILING_INFO_NOT_AVAILABLE",
		"CL_MEM_COPY_OVERLAP",
		"CL_IMAGE_FORMAT_MISMATCH",
		"CL_IMAGE_FORMAT_NOT_SUPPORTED",
		"CL_BUILD_PROGRAM_FAILURE",
		"CL_MAP_FAILURE",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"CL_INVALID_VALUE",
		"CL_INVALID_DEVICE_TYPE",
		"CL_INVALID_PLATFORM",
		"CL_INVALID_DEVICE",
		"CL_INVALID_CONTEXT",
		"CL_INVALID_QUEUE_PROPERTIES",
		"CL_INVALID_COMMAND_QUEUE",
		"CL_INVALID_HOST_PTR",
		"CL_INVALID_MEM_OBJECT",
		"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
		"CL_INVALID_IMAGE_SIZE",
		"CL_INVALID_SAMPLER",
		"CL_INVALID_BINARY",
		"CL_INVALID_BUILD_OPTIONS",
		"CL_INVALID_PROGRAM",
		"CL_INVALID_PROGRAM_EXECUTABLE",
		"CL_INVALID_KERNEL_NAME",
		"CL_INVALID_KERNEL_DEFINITION",
		"CL_INVALID_KERNEL",
		"CL_INVALID_ARG_INDEX",
		"CL_INVALID_ARG_VALUE",
		"CL_INVALID_ARG_SIZE",
		"CL_INVALID_KERNEL_ARGS",
		"CL_INVALID_WORK_DIMENSION",
		"CL_INVALID_WORK_GROUP_SIZE",
		"CL_INVALID_WORK_ITEM_SIZE",
		"CL_INVALID_GLOBAL_OFFSET",
		"CL_INVALID_EVENT_WAIT_LIST",
		"CL_INVALID_EVENT",
		"CL_INVALID_OPERATION",
		"CL_INVALID_GL_OBJECT",
		"CL_INVALID_BUFFER_SIZE",
		"CL_INVALID_MIP_LEVEL",
		"CL_INVALID_GLOBAL_WORK_SIZE",
	};

	const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

	const int index = -error;

	return (index >= 0 && index < errorCount) ? errorString[index] : "";
}

#endif
