/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include "Sphere.hpp"
#include "SphereTransmit.hpp"

using namespace SphereSim;

void SphereSim::writeChar(std::ostream& stream, unsigned char c)
{
    stream.write((char*)&c, 1);
}

char SphereSim::readChar(std::istream& stream)
{
    unsigned char c;
    stream.read((char*)&c, 1);
    return c;
}

void SphereSim::writeShort(std::ostream& stream, unsigned short s)
{
    stream.write((char*)&s, 2);
}

unsigned short SphereSim::readShort(std::istream& stream)
{
    unsigned short s;
    stream.read((char*)&s, 2);
    return s;
}

void SphereSim::writeInt(std::ostream& stream, unsigned int i)
{
    stream.write((char*)&i, 4);
}

unsigned int SphereSim::readInt(std::istream& stream)
{
    unsigned int i;
    stream.read((char*)&i, 4);
    return i;
}

void SphereSim::writeScalar(std::ostream& stream, double d)
{
    stream.write((char*)&d, 8);
}

double SphereSim::readScalar(std::istream& stream)
{
    double d;
    stream.read((char*)&d, 8);
    return d;
}

void SphereSim::writeBasicSphereData(std::ostream& stream, Sphere& s)
{
    writeScalar(stream, s.radius);
    writeScalar(stream, s.pos(0));
    writeScalar(stream, s.pos(1));
    writeScalar(stream, s.pos(2));
}

void SphereSim::readBasicSphereData(std::istream& stream, Sphere& s)
{
    s.radius = readScalar(stream);
    s.pos(0) = readScalar(stream);
    s.pos(1) = readScalar(stream);
    s.pos(2) = readScalar(stream);
}

void SphereSim::writeAllSphereData(std::ostream& stream, Sphere& s)
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

void SphereSim::readAllSphereData(std::istream& stream, Sphere& s)
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
