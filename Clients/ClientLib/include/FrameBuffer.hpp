/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License". 
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _FRAMEBUFFER_HPP_
#define _FRAMEBUFFER_HPP_

#include <Sphere.hpp>

#include <QtGlobal>

namespace SphereSim{
	/**
	 * \brief A class that stores frames in a ring buffer.
	 */
	template <typename T>
	class FrameBuffer{
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
		
		/** \brief Defines if the buffer is full and the next pushed frame will be skipped. */
		bool skipNextFrame;
		
		/** \brief Prints out the whole buffer. */
		void print();
		
	public:
		/**
		 * \brief Initializes the frame buffer.
		 * \param bufferSize Number of frames in the buffer.
		 */
		FrameBuffer(quint16 bufferSize);
		
		/**
		 * \brief Initializes the frame buffer.
		 * \param bufferSize Number of frames in the buffer.
		 * \param elementsPerFrame Number of elements per frame.
		 */
		FrameBuffer(quint16 bufferSize, quint16 elementsPerFrame);
		
		~FrameBuffer();
		
		/**
		 * \brief Defines the number of elements per frame.
		 * \param elementsPerFrame Number of elements per frame.
		 */
		void setElementsPerFrame(quint16 elementsPerFrame);
		
		/**
		 * \brief Adds a new element to the current frame.
		 * \param element Element to add.
		 */
		void pushElement(T element);
		
		/**
		 * \brief Adds the new filled frame to the buffer.
		 */
		void pushFrame();
		
		/**
		 * \brief Gets a new element from the current frame.
		 * \return Element from the frame.
		 */
		T popElement();
		
		/**
		 * \brief Removes the current frame from the buffer.
		 */
		void popFrame();
	};
}

#endif
