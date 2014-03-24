/** \file
 * \author Max Mertens <max.mail@dameweb.de>
 * \section LICENSE
 * Copyright (c) 2014, Max Mertens.
 * All rights reserved.
 * This file is licensed under the "BSD 3-Clause License".
 * Full license text is under the file "LICENSE" provided with this code. */

#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <QTextStream>

namespace SphereSim
{

	/** \brief Types of Format for the console output. */
	namespace Format
	{
		/** \see Format */
		enum Format
		{
			/** \brief Print console output in bold.
			 * Bold text is usually brighter than normal. */
			bold
		};
	}

	/** \brief Color used for the console output. */
	namespace Color
	{
		/** \see Color */
		enum Color
		{
			/** \brief Print console output in black. */
			black,
			/** \brief Print console output in red. */
			red,
			/** \brief Print console output in green. */
			green,
			/** \brief Print console output in yellow. */
			yellow,
			/** \brief Print console output in blue. */
			blue,
			/** \brief Print console output in magenta. */
			magenta,
			/** \brief Print console output in cyan. */
			cyan,
			/** \brief Print console output in white. */
			white
		};
	}

	/** \brief Helper class used for formatted console output. */
	class InternalConsole
	{
	private:
		/** \brief Initialize console. */
		InternalConsole();

		/** \copydoc InternalConsole
		 * \param c Console color.
		 * \param f Text formatting. */
		InternalConsole(quint16 c, quint16 f);

		/** \copydoc Color
		 * \see Color */
		quint16 color;
		/** \copydoc Format
		 * \see Format */
		quint16 font;

		/** \brief Qt console that is used internally to output text. */
		static QTextStream console;

	public:
		InternalConsole(const InternalConsole&) = delete;
		InternalConsole& operator=(const InternalConsole&) = delete;

		/** \brief Output formatted string.
		 * \param t Object to be printed to console. */
		template <typename T>
		InternalConsole& operator<<(T t)
		{
			if (color>=0 && color<=Color::white)
			{
				console<<"\x1b[3";
				console<<color;
				if ((font & 1<<Format::bold)>0)
				{
					console<<";1";
				}
				console<<"m";
				console<<t;
				console<<"\x1b[0m";
				console.flush();
			}
			else
			{
				console<<t;
			}
			return *this;
		}

		friend class Console;

	};

	/** \brief Manage formatted console output. */
	class Console
	{
	public:
		Console() = delete;
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;

		/** \brief Print out normally formatted text. */
		static InternalConsole out;
		/** \brief Print out bold and normally colored text. */
		static InternalConsole bold;

		/** \brief Print out black text. */
		static InternalConsole black;
		/** \brief Print out red text. */
		static InternalConsole red;
		/** \brief Print out green text. */
		static InternalConsole green;
		/** \brief Print out yellow text. */
		static InternalConsole yellow;
		/** \brief Print out blue text. */
		static InternalConsole blue;
		/** \brief Print out magenta text. */
		static InternalConsole magenta;
		/** \brief Print out cyan text. */
		static InternalConsole cyan;
		/** \brief Print out white text. */
		static InternalConsole white;

		/** \brief Print out bold black text. */
		static InternalConsole blackBold;
		/** \brief Print out bold red text. */
		static InternalConsole redBold;
		/** \brief Print out bold green text. */
		static InternalConsole greenBold;
		/** \brief Print out bold yellow text. */
		static InternalConsole yellowBold;
		/** \brief Print out bold blue text. */
		static InternalConsole blueBold;
		/** \brief Print out bold magenta text. */
		static InternalConsole magentaBold;
		/** \brief Print out bold cyan text. */
		static InternalConsole cyanBold;
		/** \brief Print out bold white text. */
		static InternalConsole whiteBold;

	};

}

#endif
