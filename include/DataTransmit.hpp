/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _SPHERETRANSMIT_HPP_
#define _SPHERETRANSMIT_HPP_

#include "Vector.hpp"

#include <iostream>
#include <string>

namespace SphereSim
{
    class Sphere;

    void writeBool(std::ostream& stream, bool b);
    bool readBool(std::istream& stream);

    void writeChar(std::ostream& stream, unsigned char c);
    char readChar(std::istream& stream);

    void writeShort(std::ostream& stream, unsigned short s);
    unsigned short readShort(std::istream& stream);

    void writeInt(std::ostream& stream, unsigned int i);
    unsigned int readInt(std::istream& stream);

    void writeFloat(std::ostream& stream, float f);
    float readFloat(std::istream& stream);

    void writeDouble(std::ostream& stream, double d);
    double readDouble(std::istream& stream);

    void writeVector3(std::ostream& stream, Vector3 v);
    Vector3 readVector3(std::istream& stream);

    void writeString(std::ostream& stream, std::string s);
    std::string readString(std::istream& stream);

    /** \brief Write basic data of a sphere to a stream.
     * \param stream Stream to write to.
     * \param s Sphere to send. */
    void writeBasicSphereData(std::ostream& stream, Sphere& s);
    /** \brief Read and update basic data of a sphere from a stream.
     * \param stream Stream to read from.
     * \param s Sphere to update. */
    void readBasicSphereData(std::istream& stream, Sphere& s);

    /** \brief Write all data of a sphere to a stream.
     * \param stream Stream to write to.
     * \param s Sphere to send. */
    void writeAllSphereData(std::ostream& stream, Sphere& s);
    /** \brief Read and update all data of a sphere from a stream.
     * \param stream Stream to read from.
     * \param s Sphere to update. */
    void readAllSphereData(std::istream& stream, Sphere& s);

}

#endif
