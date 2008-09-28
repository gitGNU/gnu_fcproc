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
	Transformation(const std::string & input_filename,
		       char                separator,
		       const std::string & output_filename);
	~Transformation(void);

	std::string  input(void);
	std::string  output(void);
	const char * c_str(void);

protected:
	Transformation(void);

private:
	std::string input_;
	char        separator_;
	std::string output_;

	std::string id_;
};

#endif // TRANSFORMATION_H
