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

	class Object
	{
	public:
		enum Type
		{
			BOOL	= 'b',
			INT		= 'i',
			LONG	= 'l',
		#if USE_DOUBLE
			SCALAR	= 'd',
		#else
			SCALAR	= 'f',
		#endif
			DOUBLE	= 'd',
			FLOAT	= 'f',
			CHAR	= 'c',
			VECTOR3	= 'v',
			STRING	= 's'
		};

		enum Flags
		{
			CLIENT_WRITABLE = 1<<0,
			SERVER_WRITABLE = 1<<1
		};

	private:
		const Type type;
		void* data;

		template<class T>
		T* getPtr() const
		{
			return (T*)data;
		}

		template<class T>
		static T getTypeFromBytes(const QByteArray &bytes);

		template<class T>
		static QByteArray getBytesFromType(const T &t);

	public:
		Object() = delete;

		Object(const Type _type);

		Object(const Object& o);

		Object& operator=(const Object& o);

		template<class T>
		Object(const Type _type, const T& t):Object(_type)
		{
			set<T>(t);
		}

		~Object();

		template<class T>
		bool set(const T& t);

		template<class T>
		T get() const;

		template<class T>
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
