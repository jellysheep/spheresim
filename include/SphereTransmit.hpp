/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SPHERETRANSMIT_HPP_
#define _SPHERETRANSMIT_HPP_

#include "Sphere.hpp"

#include <QDataStream>

namespace SphereSim
{
    namespace
    {
        template <typename streamtype>
        void writeChar(streamtype& stream, unsigned char c)
        {
            stream.write((char*)&c, 1);
        }

        template <typename streamtype>
        char readChar(streamtype& stream)
        {
            unsigned char c;
            stream.read((char*)&c, 1);
            return c;
        }

        template <typename streamtype>
        void writeShort(streamtype& stream, unsigned short s)
        {
            stream.write((char*)&s, 2);
        }

        template <typename streamtype>
        unsigned short readShort(streamtype& stream)
        {
            unsigned short s;
            stream.read((char*)&s, 2);
            return s;
        }

        template <typename streamtype>
        void writeInt(streamtype& stream, unsigned int i)
        {
            stream.write((char*)&i, 4);
        }

        template <typename streamtype>
        unsigned int readInt(streamtype& stream)
        {
            unsigned int i;
            stream.read((char*)&i, 4);
            return i;
        }

        template <typename streamtype>
        void writeScalar(streamtype& stream, double d)
        {
            stream.write((char*)&d, 8);
        }

        template <typename streamtype>
        double readScalar(streamtype& stream)
        {
            double d;
            stream.read((char*)&d, 8);
            return d;
        }

        /** \brief Write basic data of a sphere to a stream.
         * \param stream Stream to write to.
         * \param s Sphere to send. */
        template <typename streamtype>
        void writeBasicSphereData(streamtype& stream, Sphere& s)
        {
            writeScalar(stream, s.radius);
            writeScalar(stream, s.pos(0));
            writeScalar(stream, s.pos(1));
            writeScalar(stream, s.pos(2));
        }

        /** \brief Read and update basic data of a sphere from a stream.
         * \param stream Stream to read from.
         * \param s Sphere to update. */
        template <typename streamtype>
        void readBasicSphereData(streamtype& stream, Sphere& s)
        {
            s.radius = readScalar(stream);
            s.pos(0) = readScalar(stream);
            s.pos(1) = readScalar(stream);
            s.pos(2) = readScalar(stream);
        }

        /** \brief Write all data of a sphere to a stream.
         * \param stream Stream to write to.
         * \param s Sphere to send. */
        template <typename streamtype>
        void writeAllSphereData(streamtype& stream, Sphere& s)
        {
            writeScalar(stream, s.radius);
            writeScalar(stream, s.mass);
            writeScalar(stream, s.pos(0));
            writeScalar(stream, s.speed(0));
            writeScalar(stream, s.acc(0));
            writeScalar(stream, s.pos(1));
            writeScalar(stream, s.speed(1));
            writeScalar(stream, s.acc(1));
            writeScalar(stream, s.pos(2));
            writeScalar(stream, s.speed(2));
            writeScalar(stream, s.acc(2));
        }

        /** \brief Read and update all data of a sphere from a stream.
         * \param stream Stream to read from.
         * \param s Sphere to update. */
        template <typename streamtype>
        void readAllSphereData(streamtype& stream, Sphere& s)
        {
            s.radius = readScalar(stream);
            s.mass = readScalar(stream);
            s.pos(0) = readScalar(stream);
            s.speed(0) = readScalar(stream);
            s.acc(0) = readScalar(stream);
            s.pos(1) = readScalar(stream);
            s.speed(1) = readScalar(stream);
            s.acc(1) = readScalar(stream);
            s.pos(2) = readScalar(stream);
            s.speed(2) = readScalar(stream);
            s.acc(2) = readScalar(stream);
        }
    };
}

#endif
