/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _ARRAY_HPP_
#define _ARRAY_HPP_

#include <TwoDimArray.hpp>

#include <exception>

namespace SphereSim
{
	class ArrayException : public std::exception
	{
		const char* what() const noexcept
		{
			return "Array exception.";
		}
	};
}

#define USE_STL_VECTOR 0
#define USE_QT_VECTOR 0

#if USE_STL_VECTOR
	#include <vector>
	#define Array std::vector
#elif USE_QT_VECTOR
	#include <QVector>
	#define Array QVector
#else

#include <QDebug>

namespace SphereSim
{
	template <typename T, bool extremeSpeed, bool throwExceptions>
	class TwoDimArray;
	
	template <typename T, bool extremeSpeed=true, bool throwExceptions=true>
	class Array
	{
	private:
		Array(const Array<T>&);
		
		Array& operator=(const Array<T>&);
		
		T* values;
		
		unsigned int counter;
		
		unsigned int count;
		
		void deleteAll()
		{
			if(values != NULL)
			{
				delete[] values;
				values = NULL;
			}
			counter = 0;
		}
		
		void initArrays()
		{
			values = new T[count];
			counter = 0;
		}
		
	public:
		Array(const unsigned int _count=0)
			:count(_count), values(NULL), counter(0)
		{
			if(count>0)
				initArrays();
		}
		
		~Array()
		{
			deleteAll();
		}
		
		void resize(const unsigned int _count)
		{
			if(_count != count && _count > 0)
			{
				T* newValues = new T[_count];
				unsigned int commonSize = (count<_count?count:_count);
				memcpy(newValues, values, commonSize*sizeof(T));
				if(_count > count)
					for(unsigned int i = count; i<_count; i++)
						newValues[i] = T();
				T* oldValues = values;
				if(_count < count)
				{
					count = _count;
					values = newValues;
				}
				else
				{
					values = newValues;
					count = _count;
				}
				delete[] oldValues;
				oldValues = NULL;
			}
		}
		
		inline T& operator[](const unsigned int index)
		{
			//if(!extremeSpeed)
			{
				if(values == NULL)
				{
					qDebug()<<"Array::operator[] error.";
					if(throwExceptions)
						throw ArrayException();
				}
			}
			return values[index];
		}
		
		inline const T& operator[](const unsigned int index) const
		{
			if(!extremeSpeed)
			{
				if(values == NULL)
				{
					qDebug()<<"Array::operator[] const error.";
					if(throwExceptions)
						throw ArrayException();
				}
			}
			return values[index];
		}
		
		inline void addElement(T& element)
		{
			if(!extremeSpeed)
			{
				if(values == NULL)
				{
					qDebug()<<"Array::addElement error.";
					if(throwExceptions)
						throw ArrayException();
					return;
				}
			}
			if(counter < count)
				values[counter++] = element;
			else
			{
				qDebug()<<"Array::addElement error: full."<<counter<<count;
				if(throwExceptions)
					throw ArrayException();
			}
		}
		
		inline bool addElementIfNotContained(T& element)
		{
			if(!extremeSpeed)
			{
				if(values == NULL)
				{
					qDebug()<<"Array::addElement error.";
					if(throwExceptions)
						throw ArrayException();
					return false;
				}
			}
			if(counter < count)
			{
				for(unsigned int i = 0; i<counter; ++i)
				{
					if(values[i] == element)
						return false;
				}
				values[counter++] = element;
				return true;
			}
			qDebug()<<"Array::addElementIfNotContained error: full.";
			if(throwExceptions)
				throw ArrayException();
			return false;
		}
		
		inline unsigned int size()
		{
			return count;
		}
		
		inline unsigned int getCounter()
		{
			return counter;
		}
		
		inline void resetCounter()
		{
			counter = 0;
		}
		
		void push_back(T t)
		{
			resize(count+1);
			values[count-1] = t;
		}
		
		void erase(unsigned int index)
		{
			if(index >= count)
				return;
			T* newValues = new T[count-1];
			if(index > 0)
				memcpy(newValues, values, index*sizeof(T));
			if(count-index > 1)
				memcpy(&newValues[index], &values[index+1], (count-index-1)*sizeof(T));
			T* oldValues = values;
			count--;
			values = newValues;
			delete[] oldValues;
			oldValues = NULL;
		}
		
		inline T* data()
		{
			return values;
		}
		
		inline const T* constData() const
		{
			return values;
		}
		
		inline const unsigned int begin() const
		{
			return 0;
		}
		
		friend class TwoDimArray<T, extremeSpeed, throwExceptions>;
			
	};
}

#endif

#endif
