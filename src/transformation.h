//
// Copyright (C) 2007, 2008 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "config.h"

#include <string>

class Transformation {
public:
	class Exception : public std::exception {
	public:
	Exception(const std::string & description) :
		description_(description) {
		};
		~Exception(void) throw() { };

		virtual const char * what(void) const throw() {
			return description_.c_str();
		};

	protected:
		Exception(void);

	private:
		std::string description_;
	};

	Transformation(const std::string & tag,
		       char                separator);
	~Transformation(void);

	const std::string & input(void);
	const std::string & output(void);
	const std::string & tag(void);

	bool                execute(void);

protected:
	Transformation(void);

	// No copy
	Transformation(const Transformation & t);
	Transformation & operator =(const Transformation & t);

private:
	std::string tag_;
	char        separator_;

	std::string input_;
	std::string output_;
};

#endif // TRANSFORMATION_H
