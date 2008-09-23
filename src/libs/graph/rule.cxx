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

#include "libs/misc/trace.h"
#include "libs/graph/rule.h"

using namespace Graph;

Rule11::Rule11(const std::string & command) :
	command_(command)
{
}

Rule11::~Rule11(void)
{
}

bool Rule11::run(std::string & input,
		 std::string & output)
{
	TR_DBG("Running command '%s' with input '%s' and output '%s'\n",
	       command_.c_str(), input.c_str(), output.c_str());

	return true;
}
