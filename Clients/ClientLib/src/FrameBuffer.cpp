/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <FrameBuffer.hpp>
#include <Console.hpp>
#include <ActionSender.hpp>

using namespace SphereSim;

template <typename T>
FrameBuffer<T>::FrameBuffer(quint16 bufferSize_)
{
	bufferSize = bufferSize_;
	elementsPerFrame = 0;
	frames = NULL;
	currentReadFrame = NULL;
	currentWriteFrame = NULL;
	readIndex = 0;
	writeIndex = 0;
	elementReadIndex = 0;
	elementWriteIndex = 0;
	actionSender = NULL;
	updatePercentageLevel();
	lastFrameBufferAction = pop;
}

template <typename T>
FrameBuffer<T>::FrameBuffer(quint16 bufferSize_, quint16 elementsPerFrame_):FrameBuffer(bufferSize_)
{
	elementsPerFrame = elementsPerFrame_;
	quint32 totalElements = bufferSize*elementsPerFrame;
	frames = new T[totalElements];
	currentReadFrame = &frames[readIndex*(quint32)elementsPerFrame];
	currentWriteFrame = &frames[writeIndex*(quint32)elementsPerFrame];
}

template <typename T>
FrameBuffer<T>::~FrameBuffer()
{
	if(frames != NULL)
	{
		delete[] frames;
	}
}

template <typename T>
void FrameBuffer<T>::updateElementsPerFrame(quint16 elementsPerFrame_)
{
	if(elementsPerFrame_ != elementsPerFrame)
	{
		if(frames != NULL)
		{
			delete[] frames;
		}
		elementsPerFrame = elementsPerFrame_;
		quint32 totalElements = bufferSize*elementsPerFrame;
		frames = new T[totalElements];
		readIndex = 0;
		writeIndex = 0;
		currentReadFrame = &frames[readIndex*(quint32)elementsPerFrame];
		currentWriteFrame = &frames[writeIndex*(quint32)elementsPerFrame];
		qDebug()<<"updateElementsPerFrame"<<elementsPerFrame;
	}
}

template <typename T>
void FrameBuffer<T>::pushElement(T element)
{
	if(elementWriteIndex<elementsPerFrame)
	{
		currentWriteFrame[elementWriteIndex++] = element;
	}
}

template <typename T>
void FrameBuffer<T>::pushFrame()
{
	elementWriteIndex = 0;
	if(readIndex == ((writeIndex+1)%bufferSize))
	{
		qDebug()<<"FrameBuffer: buffer full!";
		readIndex = (readIndex+1)%bufferSize;
		currentReadFrame = &frames[readIndex*(quint32)elementsPerFrame];
	}
	writeIndex = (writeIndex+1)%bufferSize;
	currentWriteFrame = &frames[writeIndex*(quint32)elementsPerFrame];
	updatePercentageLevel(lastFrameBufferAction == push);
	lastFrameBufferAction = push;
}

template <typename T>
T FrameBuffer<T>::popElement()
{
	if(elementReadIndex<elementsPerFrame)
	{
		return currentReadFrame[elementReadIndex++];
	}else{
		return T();
	}
}

template <typename T>
void FrameBuffer<T>::popFrame()
{
	if(writeIndex != ((readIndex+1)%bufferSize) && writeIndex != readIndex)
	{
		readIndex = (readIndex+1)%bufferSize;
		currentReadFrame = &frames[readIndex*(quint32)elementsPerFrame];
		updatePercentageLevel(lastFrameBufferAction == pop);
		lastFrameBufferAction = pop;
	}else{
		qDebug()<<"FrameBuffer: buffer empty!";
	}
	elementReadIndex = 0;
}

template <typename T>
void FrameBuffer<T>::print()
{
	Console::out<<"[Framebuffer ("<<bufferSize<<" frames x "<<elementsPerFrame<<" elements)]";
}

template <typename T>
bool FrameBuffer<T>::hasElements()
{
	return (elementReadIndex<elementsPerFrame);
}

template <typename T>
quint16 FrameBuffer<T>::getFrameCount()
{
	int frameCount = writeIndex-readIndex;
	if(frameCount<0)
		frameCount += bufferSize;
	return (quint16) frameCount;
}

template <typename T>
void FrameBuffer<T>::updatePercentageLevel(bool greaterThanHysteresis)
{
	if(actionSender == NULL)
		return;
	
	if(bufferSize == 0)
	{
		percentageLevel = 0;
	}
	else
	{
		percentageLevel = (quint8)(getFrameCount()*100/bufferSize);
	}
	if(greaterThanHysteresis)
		emit actionSender->greatFrameBufferPercentageLevelUpdate(percentageLevel);
	emit actionSender->frameBufferPercentageLevelUpdate(percentageLevel);
}

template <typename T>
void FrameBuffer<T>::setActionSender(ActionSender* actSend)
{
	actionSender = actSend;
}

namespace SphereSim{
	template <>
	void FrameBuffer<quint8>::print()
	{
		Console::out<<"[Framebuffer ("<<bufferSize<<" frames x "<<elementsPerFrame<<" elements): \n";
		for(quint16 i = 0; i<bufferSize; i++)
		{
			for(quint16 j = 0; j<elementsPerFrame; j++)
			{
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
