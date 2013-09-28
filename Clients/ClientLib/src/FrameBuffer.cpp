/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#include <FrameBuffer.hpp>
#include <Console.hpp>

using namespace SphereSim;

template <typename T>
FrameBuffer<T>::FrameBuffer(quint16 bufferSize_){
	bufferSize = bufferSize_;
	elementsPerFrame = 0;
	frames = NULL;
	readIndex = 0;
	writeIndex = 0;
	elementReadIndex = 0;
	elementWriteIndex = 0;
	currentReadFrame = NULL;
	currentWriteFrame = NULL;
	skipNextFrame = false;
}

template <typename T>
FrameBuffer<T>::FrameBuffer(quint16 bufferSize_, quint16 elementsPerFrame_){
	bufferSize = bufferSize_;
	elementsPerFrame = elementsPerFrame_;
	quint32 totalElements = bufferSize*elementsPerFrame;
	frames = new T[totalElements];
	readIndex = 0;
	writeIndex = 0;
	elementReadIndex = 0;
	elementWriteIndex = 0;
	currentReadFrame = &frames[readIndex*(quint32)elementsPerFrame];
	currentWriteFrame = &frames[writeIndex*(quint32)elementsPerFrame];
	skipNextFrame = false;
}

template <typename T>
FrameBuffer<T>::~FrameBuffer(){
	if(frames != NULL){
		delete[] frames;
	}
}

template <typename T>
void FrameBuffer<T>::setElementsPerFrame(quint16 elementsPerFrame_){
	if(elementsPerFrame_ != elementsPerFrame){
		if(frames != NULL){
			delete[] frames;
		}
		elementsPerFrame = elementsPerFrame_;
		quint32 totalElements = bufferSize*elementsPerFrame;
		frames = new T[totalElements];
		readIndex = 0;
		writeIndex = 0;
		currentReadFrame = &frames[readIndex*(quint32)elementsPerFrame];
		currentWriteFrame = &frames[writeIndex*(quint32)elementsPerFrame];
		skipNextFrame = false;
	}
}

template <typename T>
void FrameBuffer<T>::pushElement(T element){
	if(skipNextFrame){
		return;
	}
	if(elementWriteIndex<elementsPerFrame){
		currentWriteFrame[elementWriteIndex++] = element;
	}
}

template <typename T>
void FrameBuffer<T>::pushFrame(){
	elementWriteIndex = 0;
	skipNextFrame = false;
	if(readIndex == ((writeIndex+1)%bufferSize)){
		skipNextFrame = true;
	}else{
		writeIndex = (writeIndex+1)%bufferSize;
		currentWriteFrame = &frames[writeIndex*(quint32)elementsPerFrame];
	}
}

template <typename T>
T FrameBuffer<T>::popElement(){
	if(elementReadIndex<elementsPerFrame){
		return currentReadFrame[elementReadIndex++];
	}else{
		return T();
	}
}

template <typename T>
void FrameBuffer<T>::popFrame(){
	if(readIndex != writeIndex){
		readIndex = (readIndex+1)%bufferSize;
		currentReadFrame = &frames[readIndex*(quint32)elementsPerFrame];
	}
	elementReadIndex = 0;
}

template <typename T>
void FrameBuffer<T>::print(){
	Console::out<<"[Framebuffer ("<<bufferSize<<" frames x "<<elementsPerFrame<<" elements)]";
}

namespace SphereSim{
	template <>
	void FrameBuffer<quint8>::print(){
		Console::out<<"[Framebuffer ("<<bufferSize<<" frames x "<<elementsPerFrame<<" elements): \n";
		for(quint16 i = 0; i<bufferSize; i++){
			for(quint16 j = 0; j<elementsPerFrame; j++){
				Console::out<<frames[i*elementsPerFrame + j]<<"\t";
			}
			Console::out<<"\n";
		}
		Console::out<<"]\n";
	}

	template class FrameBuffer<Sphere>;
	template class FrameBuffer<Vector3>;
	template class FrameBuffer<quint8>;
}
