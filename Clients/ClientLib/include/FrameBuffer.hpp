/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _FRAMEBUFFER_HPP_
#define _FRAMEBUFFER_HPP_

#include "Sphere.hpp"

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
        unsigned short bufferSize;

        /** \brief Number of elements per frame. */
        unsigned short elementsPerFrame;

        /** \brief Index of the currently read frame. */
        unsigned short readIndex;

        /** \brief Index of the currently written frame. */
        unsigned short writeIndex;

        /** \brief Index of the currently read element. */
        unsigned short elementReadIndex;

        /** \brief Index of the currently written element. */
        unsigned short elementWriteIndex;

        /** \brief Percentage level of the buffer. */
        unsigned char percentageLevel;

        /** \brief Storage of the last push or pop action. */
        enum FrameBufferAction { push, pop } lastFrameBufferAction;

        /** \brief Current ActionSender, used to send signals. */
        ActionSender* actionSender;

        /** \brief Print out the whole buffer. */
        void print();

        /** \brief Update percentage level of the buffer.
         * \param greaterThanHysteresis More than one frame was added or removed.
         * \see percentageLevel */
        void updatePercentageLevel(bool greaterThanHysteresis = true);

    public:
        /** \brief Initialize frame buffer.
         * \param bufferSize Number of frames in the buffer. */
        FrameBuffer(unsigned short bufferSize);

        /** \copydoc FrameBuffer
         * \copydetails updateElementsPerFrame */
        FrameBuffer(unsigned short bufferSize, unsigned short elementsPerFrame);

        /** \brief Clear up member variables. */
        ~FrameBuffer();

        FrameBuffer() = delete;
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;

        /** \brief Update the number of elements per frame.
         * \param elementsPerFrame Number of elements per frame. */
        void updateElementsPerFrame(unsigned short elementsPerFrame);

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

        /** \brief Set current ActionSender.
         * \see actionSender */
        void setActionSender(ActionSender* actionSender);

        /** \brief Get number of frames in the buffer. */
        unsigned short getFrameCount();
    };
}

#endif /*_FRAMEBUFFER_HPP_*/
