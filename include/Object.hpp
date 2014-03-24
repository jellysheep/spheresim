/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

#include <QByteArray>
#include <string>

namespace SphereSim
{

    template <typename T>
    class Visitor;

    class Object
    {
    public:
        enum Type
        {
            BOOL    = 'b',
            INT     = 'i',
        #if USE_DOUBLE
            SCALAR  = 'd',
        #else
            SCALAR  = 'f',
        #endif
            DOUBLE  = 'd',
            FLOAT   = 'f',
            VECTOR3 = 'v',
            STRING  = 's'
        };

        enum Flags
        {
            CLIENT_WRITABLE = 1<<0,
            SERVER_WRITABLE = 1<<1
        };

    private:
        const Type type;
        void* data;

        template <typename T>
        T* getPtr() const
        {
            return (T*)data;
        }

        template <typename T>
        T applyVisitor(Visitor<T>&& visitor) const;

    public:
        Object() = delete;
        Object(const Object& o) = delete;
        Object& operator=(const Object&) = delete;

        Object(const Type type);
        Object(Object&& o);

        template <typename T>
        Object(const Type type, const T& t):Object(type)
        {
            set<T>(t);
        }

        ~Object();

        template <typename T>
        bool set(const T& t);

        template <typename T>
        T get() const;

        template <typename T>
        T& getRef() const
        {
            return *getPtr<T>();
        }

        QByteArray getData() const;

        bool setData(const QByteArray &bytes);

        Type getType() const
        {
            return type;
        }

    };

}

#endif
