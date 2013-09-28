/**
 * \file
 * \author Max Mertens <mail@sheepstyle.comeze.com>
 * \section LICENSE
 * Copyright (c) 2013, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code.
 */

#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <QTextStream>

namespace SphereSim{
	
	namespace Format{
		enum Format{
			bold
		};
	}
	namespace Color{
		enum Color{
			black,
			red,
			green,
			yellow,
			blue,
			magenta,
			cyan,
			white
		};
	}
	
	/**
	 * \brief Helper class that manages formatted console output. 
	 */
	class InternalConsole{
	private:
		InternalConsole();
		InternalConsole(quint16 c, quint16 f);
		
		quint16 color;
		quint16 font;
		
		static QTextStream console;
		
	public:
		
		/**
		 * \brief Outputs formatted string.
		 * \param t Object to be printed to console.
		 */
		template<typename T>
		InternalConsole& operator<<(T t){
			if(color>=0 && color<=Color::white){
				console<<"\x1b[3";
				console<<color;
				if((font & 1<<Format::bold)>0){
					console<<";1";
				}
				console<<"m";
				console<<t;
				console<<"\x1b[0m";
				console.flush();
			}else{
				console<<t;
			}
			return *this;
		}
		
		friend class Console;
		
	};
	
	/**
	 * \brief Class that manages formatted console output.
	 */
	class Console{
	private:
		Console(){}
		
	public:
		static InternalConsole out;
		static InternalConsole bold;
		
		static InternalConsole black;
		static InternalConsole red;
		static InternalConsole green;
		static InternalConsole yellow;
		static InternalConsole blue;
		static InternalConsole magenta;
		static InternalConsole cyan;
		static InternalConsole white;
		
		static InternalConsole blackBold;
		static InternalConsole redBold;
		static InternalConsole greenBold;
		static InternalConsole yellowBold;
		static InternalConsole blueBold;
		static InternalConsole magentaBold;
		static InternalConsole cyanBold;
		static InternalConsole whiteBold;

	};
	
}

#endif
