//
// Copyright (C) 2008, 2009 Francesco Salvestrini
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

#ifndef FILTER_H
#define FILTER_H

#include "config.h"

#include <string>
#include <vector>
#include <set>
#include <map>

#include "libs/fs/file.h"
#include "libs/fs/directory.h"

namespace FCP {
	class Filter {
	public:
		Filter(const FS::File &                 input,
		       const FS::File &                 output,
		       const std::vector<std::string> & commands);
		~Filter(void);

		const FS::File & input(void);
		const FS::File & output(void);
		void             setup(const std::string &   id,
				       const FS::Directory & tmp);
		void             run(bool dry);

	protected:
		// No copy allowed
		Filter(const Filter &);
		void operator =(const Filter &);

	private:
		FS::File                 input_;
		FS::File                 output_;
		std::vector<std::string> templates_;
		std::vector<std::string> commands_;

		std::string              mktemp(const std::string & id,
						const std::string & tmp_dir,
						size_t              index);
		std::vector<std::string> commands(const std::string & id,
						  const FS::File &    input,
						  const FS::File &    output,
						  const std::string & work_dir);
	};
}

#endif // FILTER_H
