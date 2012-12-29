
#include "ClTimer.h"
#include "Circles.h"

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <string.h>

ClTimer::ClTimer(CirclesHolder* ch){
	circlesHolder = ch;
	QThread::start();
}

void ClTimer::run(){
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
	printf("circlesCountber of platforms: %d\n", platforms.size());
	if (platforms.size() == 0) {
		printf("Platform size 0\n");
	}
	
	//for right now we just use the first available device
	//later you may have criteria (such as support for different extensions)
	//that you want to use to select the device
	deviceUsed = 0;
	//create the context
	///context = clCreateContext(0, 1, &devices[deviceUsed], NULL, NULL, &err);
	//context properties will be important later, for now we go with defualts
	cl_context_properties properties[] = 
		{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};

	context = cl::Context(CL_DEVICE_TYPE_ALL, properties);
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	
	//create the command queue we will use to execute OpenCL commands
	///command_queue = clCreateCommandQueue(context, devices[deviceUsed], 0, &err);
	try{
		queue = cl::CommandQueue(context, devices[deviceUsed], 0, &err);
	}
	//catch (cl::Error er) {
	catch (std::exception er) {
		printf("ERROR: %s(%d)\n", er.what());//, er.err());
	}
	
	
	//Program Setup
	printf("load the program\n");
	

	int pl;
	char *kernel_source;
	kernel_source = file_contents("../CirclesCalculator.cl", &pl);
	if(kernel_source == NULL){
		printf("ERROR: OpenCL initialization failed!\n");
		return;
	}

	//pl = kernel_source.size();
	printf("kernel size: %d\n", pl);
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
		err = program.build(devices, "");
	}
	catch (cl::Error er) {
		printf("program.build: %s\n", oclErrorString(er.err()));
	//if(err != CL_SUCCESS){
	}
	printf("done building program\n");
	printf("Build Status:  %u\n", program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]));
	printf("Build Options: %s\n", program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]).c_str());
	printf("Build Log:	 %s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]).c_str());
	printf("OpenCL initialization finished!\n");
	printf("\n");
	
	
	//initialize our kernel from the program
	try{
		moveStep_kernel = cl::Kernel(program, "moveStep", &err);
		randomFill_kernel = cl::Kernel(program, "randomFill", &err);
	}
	catch (cl::Error er) {
		printf("ERROR: %s(%d)\n", er.what(), er.err());
	}

	//initialize our CPU memory arrays, send them to the device and set the kernel arguements

	///Circle circle[circlesCount];
	//circle.A = .5f;
	//circle.B = 10.0f;
	//circle.C = 3;
	/*
	circle[i].D = .01;
	circle[i].E = 1;
	*/

	printf("Creating OpenCL arrays\n");
	cl_circle = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Circle)*circlesCount, NULL, &err);
	cl_m_z = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(uint), NULL, &err);
	cl_m_w = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(uint), NULL, &err);
	cl_size = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_double2), NULL, &err);
	cl_max_speed = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(double), NULL, &err);
	cl_circlesCount = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(int), NULL, &err);
	cl_E = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(double), NULL, &err);

	printf("Pushing data to the GPU\n");
	//push our CPU arrays to the GPU

	//write the circle struct to GPU memory as a buffer
	//err = queue.enqueueWriteBuffer(cl_circle, CL_TRUE, 0, sizeof(Circle), &circle, NULL, &event);
	srand(time(NULL));
	uint m_z = rand();
	for(int i = 0; i<10000; i++);
	uint m_w = rand();
	err = queue.enqueueWriteBuffer(cl_m_z, CL_TRUE, 0, sizeof(uint), &m_z, NULL, &event);
	err = queue.enqueueWriteBuffer(cl_m_w, CL_TRUE, 0, sizeof(uint), &m_w, NULL, &event);
	err = queue.enqueueWriteBuffer(cl_size, CL_TRUE, 0, sizeof(cl_double2), &size, NULL, &event);
	err = queue.enqueueWriteBuffer(cl_max_speed, CL_TRUE, 0, sizeof(double), &max_speed, NULL, &event);
	err = queue.enqueueWriteBuffer(cl_circlesCount, CL_TRUE, 0, sizeof(int), &circlesCount, NULL, &event);
	err = queue.enqueueWriteBuffer(cl_E, CL_TRUE, 0, sizeof(double), &E, NULL, &event);
   


	//set the arguements of our kernel
	err = moveStep_kernel.setArg(0, cl_circle);
	err = moveStep_kernel.setArg(1, cl_circlesCount);
	err = moveStep_kernel.setArg(2, cl_size);
	err = moveStep_kernel.setArg(3, cl_E);
	err = randomFill_kernel.setArg(0, cl_circle);
	err = randomFill_kernel.setArg(1, cl_m_z);
	err = randomFill_kernel.setArg(2, cl_m_w);
	err = randomFill_kernel.setArg(3, cl_size);
	err = randomFill_kernel.setArg(4, cl_max_speed);

	//Wait for the command queue to finish these commands before proceeding
	queue.finish();
	
	
	while(true)
	{
		
		frameCounter++;
	}
}

char* ClTimer::file_contents(const char *filename, int *length)
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
const char* ClTimer::oclErrorString(cl_int error)
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
