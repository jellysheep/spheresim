/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "FrameBuffer.hpp"
#include "Console.hpp"
#include "ActionSender.hpp"

#include <iomanip>

using namespace SphereSim;

template <typename T>
FrameBuffer<T>::FrameBuffer(unsigned short bufferSize)
    :frames(nullptr), currentReadFrame(nullptr), currentWriteFrame(nullptr),
    bufferSize(bufferSize), elementsPerFrame(0), readIndex(0), writeIndex(0),
    elementReadIndex(0), elementWriteIndex(0), percentageLevel(0),
    lastFrameBufferAction(pop), actionSender(nullptr)
{
    updatePercentageLevel();
}

template <typename T>
FrameBuffer<T>::FrameBuffer(unsigned short bufferSize_, unsigned short elementsPerFrame_)
    :FrameBuffer(bufferSize_)
{
    elementsPerFrame = elementsPerFrame_;
    unsigned int totalElements = bufferSize*elementsPerFrame;
    frames = new T[totalElements];
    currentReadFrame = &frames[readIndex*(unsigned int)elementsPerFrame];
    currentWriteFrame = &frames[writeIndex*(unsigned int)elementsPerFrame];
}

template <typename T>
FrameBuffer<T>::~FrameBuffer()
{
    if (frames != nullptr)
    {
        delete[] frames;
    }
}

template <typename T>
void FrameBuffer<T>::updateElementsPerFrame(unsigned short elementsPerFrame_)
{
    if (elementsPerFrame_ != elementsPerFrame)
    {
        if (frames != nullptr)
        {
            delete[] frames;
        }
        elementsPerFrame = elementsPerFrame_;
        unsigned int totalElements = bufferSize*elementsPerFrame;
        frames = new T[totalElements];
        readIndex = 0;
        writeIndex = 0;
        currentReadFrame = &frames[readIndex*(unsigned int)elementsPerFrame];
        currentWriteFrame = &frames[writeIndex*(unsigned int)elementsPerFrame];
    }
}

template <typename T>
void FrameBuffer<T>::pushElement(T element)
{
    if (elementWriteIndex<elementsPerFrame)
    {
        currentWriteFrame[elementWriteIndex++] = element;
    }
}

template <typename T>
void FrameBuffer<T>::pushFrame()
{
    elementWriteIndex = 0;
    if (readIndex == ((writeIndex+1)%bufferSize))
    {
        readIndex = (readIndex+1)%bufferSize;
        currentReadFrame = &frames[readIndex*(unsigned int)elementsPerFrame];
    }
    writeIndex = (writeIndex+1)%bufferSize;
    currentWriteFrame = &frames[writeIndex*(unsigned int)elementsPerFrame];
    updatePercentageLevel(lastFrameBufferAction == push);
    lastFrameBufferAction = push;
}

template <typename T>
T FrameBuffer<T>::popElement()
{
    if (elementReadIndex<elementsPerFrame)
    {
        return currentReadFrame[elementReadIndex++];
    }
    else
    {
        return T();
    }
}

template <typename T>
void FrameBuffer<T>::popFrame()
{
    if (writeIndex != ((readIndex+1)%bufferSize) && writeIndex != readIndex)
    {
        readIndex = (readIndex+1)%bufferSize;
        currentReadFrame = &frames[readIndex*(unsigned int)elementsPerFrame];
        updatePercentageLevel(lastFrameBufferAction == pop);
        lastFrameBufferAction = pop;
    }
    elementReadIndex = 0;
}

template <typename T>
void FrameBuffer<T>::print()
{
    Console()<<"[Framebuffer ("<<bufferSize<<" frames x "
        <<elementsPerFrame<<" elements)]\n";
}

template <typename T>
bool FrameBuffer<T>::hasElements()
{
    return (elementReadIndex<elementsPerFrame);
}

template <typename T>
unsigned short FrameBuffer<T>::getFrameCount()
{
    int frameCount = writeIndex-readIndex;
    if (frameCount<0)
    {
        frameCount += bufferSize;
    }
    return (unsigned short) frameCount;
}

template <typename T>
void FrameBuffer<T>::updatePercentageLevel(bool greaterThanHysteresis)
{
    if (actionSender == nullptr)
    {
        return;
    }

    if (bufferSize == 0)
    {
        percentageLevel = 0;
    }
    else
    {
        percentageLevel = (unsigned char)((getFrameCount()-1)*100/(bufferSize-2));
    }
    if (greaterThanHysteresis)
    {
        emit actionSender->greatFrameBufferPercentageLevelUpdate(percentageLevel);
    }
    emit actionSender->frameBufferPercentageLevelUpdate(percentageLevel);
}

template <typename T>
void FrameBuffer<T>::setActionSender(ActionSender* actSend)
{
    actionSender = actSend;
}

namespace SphereSim
{
    template <>
    void FrameBuffer<unsigned char>::print()
    {
        Console console;
        console<<"[Framebuffer ("<<bufferSize<<" frames x "
            <<elementsPerFrame<<" elements): \n";
        for (unsigned short i = 0; i<bufferSize; i++)
        {
            for (unsigned short j = 0; j<elementsPerFrame; j++)
            {
                console<<std::setw(2)<<frames[i*elementsPerFrame + j]<<' ';
            }
            console<<'\n';
        }
        console<<"]\n";
    }

    template class FrameBuffer<Sphere>;
    template class FrameBuffer<Vector3>;
    template class FrameBuffer<unsigned char>;
}
