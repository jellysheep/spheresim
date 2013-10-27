/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _TWODIMARRAY_HPP_
#define _TWODIMARRAY_HPP_

#include <exception>

namespace SphereSim
{
	class TwoDimArrayException : public std::exception
	{
		const char* what() const noexcept
		{
			return "TwoDimArray exception.";
		}
	};
	
	template <typename T, bool extremeSpeed=false, bool throwExceptions=true>
	class TwoDimArray
	{
	private:
		TwoDimArray(const TwoDimArray<T>&);
		
		TwoDimArray& operator=(const TwoDimArray<T>&);
		
		T** data;
		
		unsigned int* counter;
		
		const unsigned int constInnerSize;
		
		unsigned int outerSize;
		
		void deleteAll()
		{
			if(data != NULL)
			{
				for(unsigned int i = 0; i<outerSize; i++)
				{
					if(data[i] != NULL)
					{
						delete[] data[i];
						data[i] = NULL;
					}
				}
				delete[] data;
				data = NULL;
			}
			if(counter != NULL)
			{
				delete[] counter;
				counter = NULL;
			}
		}
		
		void initArrays()
		{
			data = new T*[outerSize];
			counter = new unsigned int[outerSize];
			for(unsigned int i = 0; i<outerSize; i++)
			{
				data[i] = new T[constInnerSize];
				counter[i] = 0;
			}
		}
		
	public:
		TwoDimArray(const unsigned int size)
			:constInnerSize(size), outerSize(0), data(NULL), counter(NULL)
		{
		}
		
		TwoDimArray(const unsigned int size, const unsigned int size2)
			:constInnerSize(size), outerSize(size2), data(NULL), counter(NULL)
		{
			if(size2 > 0)
				initArrays();
		}
		
		~TwoDimArray()
		{
			deleteAll();
		}
		
		void changeSize(const unsigned int size)
		{
			if(size != outerSize && size > 0)
			{
				deleteAll();
				outerSize = size;
				initArrays();
			}
		}
		
		inline T* operator[](const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"TwoDimArray::operator[] error.";
					if(throwExceptions)
						throw TwoDimArrayException();
					return NULL;
				}
			}
			return data[index];
		}
		
		inline const T* operator[](const unsigned int index) const
		{
			if(!extremeSpeed)
			{
				if(data == NULL)
				{
					qDebug()<<"TwoDimArray::operator[] const error.";
					if(throwExceptions)
						throw TwoDimArrayException();
					return NULL;
				}
			}
			return data[index];
		}
		
		inline void addElement(const unsigned int index, T& element)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || data == NULL || index >= outerSize || data[index] == NULL || counter[index] >= constInnerSize)
				{
					qDebug()<<"TwoDimArray::addElement error.";
					if(throwExceptions)
						throw TwoDimArrayException();
					return;
				}
			}
			if(counter[index] < constInnerSize)
				data[index][counter[index]++] = element;
			else
			{
				qDebug()<<"TwoDimArray::addElement error: full."<<counter[index]<<constInnerSize;
				if(throwExceptions)
					throw TwoDimArrayException();
			}
		}
		
		inline bool addElementIfNotContained(const unsigned int index, T& element)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || data == NULL || index >= outerSize || data[index] == NULL || counter[index] >= constInnerSize)
				{
					qDebug()<<"TwoDimArray::addElementIfNotContained error.";
					if(throwExceptions)
						throw TwoDimArrayException();
					return false;
				}
			}
			if(counter[index] < constInnerSize)
			{
				for(unsigned int i = 0; i<counter[index]; ++i)
				{
					if(data[index][i] == element)
					{
						return false;
					}
				}
				data[index][counter[index]++] = element;
				return true;
			}
			qDebug()<<"TwoDimArray::addElementIfNotContained error: full.";
			if(throwExceptions)
				throw TwoDimArrayException();
			return false;
		}
		
		inline unsigned int getCount(const unsigned int index)
		{
			
			if(!extremeSpeed)
			{
				if(counter == NULL || index >= outerSize)
				{
					qDebug()<<"TwoDimArray::getCount error.";
					if(throwExceptions)
						throw TwoDimArrayException();
					return 0;
				}
			}
			return counter[index];
		}
		
		inline void resetCounter(const unsigned int index)
		{
			if(!extremeSpeed)
			{
				if(counter == NULL || index >= outerSize)
				{
					qDebug()<<"TwoDimArray::resetCounter error.";
					if(throwExceptions)
						throw TwoDimArrayException();
					return;
				}
			}
			counter[index] = 0;
		}
			
	};
}

#endif
