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

#include "config.h"

#include <string>

#include "transformation.h"

Transformation::Transformation(const std::string & input_filename,
			       char                separator,
			       const std::string & output_filename) :
	input_(input_filename),
	separator_(separator),
	output_(output_filename)
{
	char tmp[2];
	
	tmp[0] = separator_;
	tmp[1] = 0;

	id_ = input_ + std::string(tmp) + output_;
}

Transformation::~Transformation(void)
{
}

std::string Transformation::input(void)
{
	return input_;
}

std::string Transformation::output(void)
{
	return output_;
}

const char * Transformation::c_str(void)
{
	return id_.c_str();
}
