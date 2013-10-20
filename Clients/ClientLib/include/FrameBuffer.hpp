/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _FRAMEBUFFER_HPP_
#define _FRAMEBUFFER_HPP_

#include <Sphere.hpp>

#include <QtGlobal>

namespace SphereSim
{
	class ActionSender;
	
	/** \brief Ring buffer to store frames. */
	template <typename T>
	class FrameBuffer
	{
	private:
		/** \brief Array containing all frame elements. */
		T *frames;
		
		/** \brief Pointer to the first element of the currently read frame. */
		T *currentReadFrame;
		
		/** \brief Pointer to the first element of the currently written frame. */
		T *currentWriteFrame;
		
		/** \brief Number of frames in the buffer. */
		quint16 bufferSize;
		
		/** \brief Number of elements per frame. */
		quint16 elementsPerFrame;
		
		/** \brief Index of the currently read frame. */
		quint16 readIndex;
		
		/** \brief Index of the currently written frame. */
		quint16 writeIndex;
		
		/** \brief Index of the currently read element. */
		quint16 elementReadIndex;
		
		/** \brief Index of the currently written element. */
		quint16 elementWriteIndex;
		
		/** \brief Flag for buffer fullness. If true, next pushed frame will be skipped. */
		bool skipNextFrame;
		
		/** \brief Print out the whole buffer. */
		void print();
		
		void updatePercentageLevel(bool greaterThanHysteresis = true);
		
		quint8 percentageLevel;
		
		ActionSender* actionSender;
		
		enum FrameBufferAction { push, pop } lastFrameBufferAction;
		
	public:
		/** \brief Initialize frame buffer.
		 * \param bufferSize Number of frames in the buffer. */
		FrameBuffer(quint16 bufferSize);
		
		/** \copydoc FrameBuffer
		 * \copydetails updateElementsPerFrame */
		FrameBuffer(quint16 bufferSize, quint16 elementsPerFrame);
		
		/** \brief Clear up member variables. */
		~FrameBuffer();
		
		/** \brief Update the number of elements per frame.
		 * \param elementsPerFrame Number of elements per frame. */
		void updateElementsPerFrame(quint16 elementsPerFrame);
		
		/** \brief Add a new element to the current frame.
		 * \param element Element to add. */
		void pushElement(T element);
		
		/** \brief Add the currently filled frame to the buffer. */
		void pushFrame();
		
		/** \brief Get next element from the current frame. 
		 * \return Element from the current frame. */
		T popElement();
		
		/** \brief Remove the currently read frame from the buffer. */
		void popFrame();
		
		/** \brief Flag for emptyness of current frame elements. */
		bool hasElements();
		
		void setActionSender(ActionSender* actionSender);
		
		quint16 getFrameCount();
	};
}

#endif