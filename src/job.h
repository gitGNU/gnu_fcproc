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

#ifndef JOB_H
#define JOB_H

#include "config.h"

#include <vector>
#include <string>

#include "file.h"
#include "rule.h"

namespace FCP {
	class Job {
	public:
		Job(const std::string &        id,
		    const FCP::File &          input,
		    std::vector<FCP::Rule *> & rules,
		    const FCP::File &          output);
		~Job(void);

		void run(const std::string & temp_dir);

	protected:

	private:
		std::string              id_;
		FCP::File                input_;
		std::vector<FCP::Rule *> rules_;
		FCP::File                output_;
	};
};

#endif // JOB_H
