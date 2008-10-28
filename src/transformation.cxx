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

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "transformation.h"

namespace FCP {
	Transformation::Transformation(const std::string & tag,
				       char                separator) :
		tag_(tag),
		input_(0),
		output_(0)
	{
		std::string::size_type p;

		p = tag_.find(separator);
		if ((p < 0) || (p > tag_.size())) {
			throw Exception("Missing separator "
					"in transformation "
					"'" + tag_ + "'");
		}

		std::string tmp;

		tmp = tag_.substr(0, p);
		if (tmp.size() == 0) {
			throw Exception("Missing input file "
					"in transformation "
					"'" + tag_ + "'");
		}
		BUG_ON(tmp == "");
		input_ = new FCP::File(tmp);

		tmp = tag_.substr(p + 1);
		if (tmp.size() == 0) {
			throw Exception("Missing output file "
					"in transformation "
					"'" + tag_ + "'");
		}
		BUG_ON(tmp == "");
		output_ = new FCP::File(tmp);
	}

	Transformation::~Transformation(void)
	{
		BUG_ON(input_ == 0);
		delete input_;

		BUG_ON(output_ == 0);
		delete output_;
	}

	const std::string & Transformation::tag(void) const
	{
		return tag_;
	}

	const FCP::File & Transformation::input(void) const
	{
		BUG_ON(input_ == 0);
		return *input_;
	}

	const FCP::File & Transformation::output(void) const
	{
		BUG_ON(output_ == 0);
		return *output_;
	}
};
