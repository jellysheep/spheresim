/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#include <Object.hpp>
#include <Vector.hpp>

#include <iostream>

#define generateConstructorCase(type, T)		\
		case type:								\
			data = new T();						\
			break;

#define generateCopyConstructorCase(type, T)	\
		case type:								\
			data = new T();						\
			*((T*)data) = *((T*)o.data);		\
			break;

#define generateCopyCase(type, T)				\
		case type:								\
			*((T*)data) = o.get<T>();			\
			break;

#define generateDestructorCase(type, T)			\
		case type:								\
			delete (T*)data;					\
			break;

#define generateEnumerableSetter(T)				\
	template<>									\
	bool Object::set<T>(const T& t)				\
	{											\
		bool changed = false;					\
		switch(type)							\
		{										\
		case BOOL:								\
			changed = *((bool*)data) != (bool)t;\
			*((bool*)data) = (bool)t;			\
			break;								\
		case INT:								\
			changed = *((int*)data) != (int)t;	\
			*((int*)data) = (int)t;				\
			break;								\
		case LONG:								\
			changed = *((long*)data) != (long)t;\
			*((long*)data) = (long)t;			\
			break;								\
		case DOUBLE:							\
			changed = *((double*)data) != (double)t;\
			*((double*)data) = (double)t;		\
			break;								\
		case FLOAT:								\
			changed = *((float*)data) != (float)t;\
			*((float*)data) = (float)t;			\
			break;								\
		case CHAR:								\
			changed = *((char*)data) != (char)t;\
			*((char*)data) = (char)t;			\
			break;								\
		default:								\
			std::cerr<<(char)type<<type<<"\n";	\
			throw std::exception();				\
		}										\
		return changed;							\
	}

#define generateEnumerableGetter(T)				\
	template<>									\
	T Object::get<T>() const					\
	{											\
		switch(type)							\
		{										\
		case BOOL:								\
			return (T)*((bool*)data);			\
			break;								\
		case INT:								\
			return (T)*((int*)data);			\
			break;								\
		case LONG:								\
			return (T)*((long*)data);			\
			break;								\
		case DOUBLE:							\
			return (T)*((double*)data);			\
			break;								\
		case FLOAT:								\
			return (T)*((float*)data);			\
			break;								\
		case CHAR:								\
			return (T)*((char*)data);			\
			break;								\
		default:								\
			std::cerr<<(char)type<<type<<"\n";	\
			throw std::exception();				\
		}										\
	}

#define generateGenericSetter(_type, T)			\
	template<>									\
	bool Object::set<T>(const T& t)				\
	{											\
		bool changed = false;					\
		if(type == _type)						\
		{										\
			changed = *((T*)data) != (T)t;		\
			*((T*)data) = (T)t;					\
		}										\
		else									\
		{										\
			std::cerr<<(char)type<<"!="			\
				<<(char)_type<<"\n";			\
			throw std::exception();				\
		}										\
		return changed;							\
	}

#define generateGenericGetter(_type, T)			\
	template<>									\
	T Object::get<T>() const					\
	{											\
		if(type == _type)						\
		{										\
			return *((T*)data);					\
		}										\
		else									\
		{										\
			std::cerr<<(char)type<<"!="			\
				<<(char)_type<<"\n";			\
			throw std::exception();				\
		}										\
		return T();								\
	}

#define generateDataGetterCase(type, T)			\
		case type:								\
			return getBytesFromType(*((T*)data))\
				.prepend((char)type);

#define generateDataSetterCase(type, T)			\
		case type:								\
			return set(getTypeFromBytes<T>(bytes));

namespace SphereSim
{
	Object::Object(const Type type):type(type), data(nullptr)
	{
		switch(type)
		{
			generateConstructorCase(BOOL, bool);
			generateConstructorCase(INT, int);
			generateConstructorCase(LONG, long);
			generateConstructorCase(DOUBLE, double);
			generateConstructorCase(FLOAT, float);
			generateConstructorCase(CHAR, char);
			generateConstructorCase(VECTOR3, Vector3);
			generateConstructorCase(STRING, std::string);
		default:
			std::cerr<<"No known type specified.\n";
			throw std::exception();
			break;
		}
	}

	Object::Object(const Object& o):type(o.type), data(nullptr)
	{
		switch(type)
		{
			generateCopyConstructorCase(BOOL, bool);
			generateCopyConstructorCase(INT, int);
			generateCopyConstructorCase(LONG, long);
			generateCopyConstructorCase(DOUBLE, double);
			generateCopyConstructorCase(FLOAT, float);
			generateCopyConstructorCase(CHAR, char);
			generateCopyConstructorCase(VECTOR3, Vector3);
			generateCopyConstructorCase(STRING, std::string);
		default:
			std::cerr<<"No known type specified.\n";
			throw std::exception();
			break;
		}
	}

	Object::~Object()
	{
		switch(type)
		{
			generateDestructorCase(BOOL, bool);
			generateDestructorCase(INT, int);
			generateDestructorCase(LONG, long);
			generateDestructorCase(DOUBLE, double);
			generateDestructorCase(FLOAT, float);
			generateDestructorCase(CHAR, char);
			generateDestructorCase(VECTOR3, Vector3);
			generateDestructorCase(STRING, std::string);
		default:
			std::cerr<<"No known type specified.\n";
			throw std::exception();
			break;
		}
	}

	generateEnumerableSetter(bool);
	generateEnumerableSetter(int);
	generateEnumerableSetter(long);
	generateEnumerableSetter(double);
	generateEnumerableSetter(float);
	generateEnumerableSetter(char);

	generateGenericSetter(VECTOR3, Vector3);
	generateGenericSetter(STRING, std::string);

	generateEnumerableGetter(bool);
	generateEnumerableGetter(int);
	generateEnumerableGetter(long);
	generateEnumerableGetter(double);
	generateEnumerableGetter(float);
	generateEnumerableGetter(char);

	generateGenericGetter(VECTOR3, Vector3);
	generateGenericGetter(STRING, std::string);

	Object& Object::operator=(const Object& o)
	{
		switch(type)
		{
			generateCopyCase(BOOL, bool);
			generateCopyCase(INT, int);
			generateCopyCase(LONG, long);
			generateCopyCase(DOUBLE, double);
			generateCopyCase(FLOAT, float);
			generateCopyCase(CHAR, char);
			generateCopyCase(VECTOR3, Vector3);
			generateCopyCase(STRING, std::string);
		default:
			std::cerr<<"No known type specified.\n";
			throw std::exception();
			break;
		}
		return *this;
	}

	template<>
	bool Object::getTypeFromBytes<bool>(const QByteArray &bytes)
	{
		return bytes[0]==0?false:true;
	}

	template<>
	int Object::getTypeFromBytes<int>(const QByteArray &bytes)
	{
		int l = 0;
		char* chars = (char*)&l;
		for(int i = 0; i<4; i++)
			chars[i] = bytes.at(i);
		return l;
	}

	template<>
	long Object::getTypeFromBytes<long>(const QByteArray &bytes)
	{
		long l = 0;
		char* chars = (char*)&l;
		for(int i = 0; i<8; i++)
			chars[i] = bytes.at(i);
		return l;
	}

	template<>
	double Object::getTypeFromBytes<double>(const QByteArray &bytes)
	{
		long l = getTypeFromBytes<long>(bytes);
		return *((double*)&l);
	}

	template<>
	float Object::getTypeFromBytes<float>(const QByteArray &bytes)
	{
		int i = getTypeFromBytes<int>(bytes);
		return *((float*)&i);
	}

	template<>
	char Object::getTypeFromBytes<char>(const QByteArray &bytes)
	{
		return bytes[0];
	}

	template<>
	Vector3 Object::getTypeFromBytes<Vector3>(const QByteArray &bytes)
	{
		Vector3 v;
		v(0) = getTypeFromBytes<double>(bytes.mid(0,8));
		v(1) = getTypeFromBytes<double>(bytes.mid(8,8));
		v(2) = getTypeFromBytes<double>(bytes.mid(16,8));
		return v;
	}

	template<>
	std::string Object::getTypeFromBytes<std::string>(const QByteArray &bytes)
	{
		std::string str(bytes.constData(), bytes.length());
		return str;
	}

	template<>
	QByteArray Object::getBytesFromType<bool>(const bool &t)
	{
		return QByteArray(1, t?255:0);
	}

	template<>
	QByteArray Object::getBytesFromType<int>(const int &t)
	{
		QByteArray bytes(4, 0);
		int l = t;
		char* chars = (char*)&l;
		for(int i = 0; i<4; i++)
			bytes[i] = chars[i];
		return bytes;
	}

	template<>
	QByteArray Object::getBytesFromType<long>(const long &t)
	{
		QByteArray bytes(8, 0);
		long l = t;
		char* chars = (char*)&l;
		for(int i = 0; i<8; i++)
			bytes[i] = chars[i];
		return bytes;
	}

	template<>
	QByteArray Object::getBytesFromType<double>(const double &t)
	{
		long l = *((long*)&t);
		return getBytesFromType<long>(l);
	}

	template<>
	QByteArray Object::getBytesFromType<float>(const float &t)
	{
		int i = *((int*)&t);
		return getBytesFromType<int>(i);
	}

	template<>
	QByteArray Object::getBytesFromType<char>(const char &t)
	{
		return QByteArray(1, t);
	}

	template<>
	QByteArray Object::getBytesFromType<Vector3>(const Vector3 &t)
	{
		QByteArray bytes, tmp;
		double d0 = t(0), d1 = t(1), d2 = t(2);
		tmp = getBytesFromType<double>(d0);
		bytes += tmp;
		tmp = getBytesFromType<double>(d1);
		bytes += tmp;
		tmp = getBytesFromType<double>(d2);
		bytes += tmp;
		return bytes;
	}

	template<>
	QByteArray Object::getBytesFromType<std::string>(const std::string &t)
	{
		return QByteArray(t.c_str(), t.length());
	}

	QByteArray Object::getData() const
	{
		switch(type)
		{
			generateDataGetterCase(BOOL, bool);
			generateDataGetterCase(INT, int);
			generateDataGetterCase(LONG, long);
			generateDataGetterCase(DOUBLE, double);
			generateDataGetterCase(FLOAT, float);
			generateDataGetterCase(CHAR, char);
			generateDataGetterCase(VECTOR3, Vector3);
			generateDataGetterCase(STRING, std::string);
		default:
			std::cerr<<"No known type specified.\n";
			throw std::exception();
			break;
		}
	}

	bool Object::setData(const QByteArray &bytes_)
	{
		Type t = (Type)bytes_[0];
		QByteArray bytes = bytes_.mid(1);
		switch(t)
		{
			generateDataSetterCase(BOOL, bool);
			generateDataSetterCase(INT, int);
			generateDataSetterCase(LONG, long);
			generateDataSetterCase(DOUBLE, double);
			generateDataSetterCase(FLOAT, float);
			generateDataSetterCase(CHAR, char);
			generateDataSetterCase(VECTOR3, Vector3);
			generateDataSetterCase(STRING, std::string);
		default:
			std::cerr<<"No known type specified.\n";
			throw std::exception();
			break;
		}
		return false;
	}

}
