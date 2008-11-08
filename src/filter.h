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

#ifndef FILTER_H
#define FILTER_H

#include "config.h"

#include <string>
#include <vector>
#include <set>
#include <map>

#include "file.h"
#include "directory.h"

namespace FCP {
	// XXX FIXME: tag_in and tag_out could be useless, please check ASAP
	class Filter {
	public:
		Filter(const std::string &              tag_in,
		       const std::string &              tag_out,
		       const std::vector<std::string> & commands);
		~Filter(void);

		const std::string &      input(void);
		const std::string &      output(void);
		void                     setup(const std::string &    id,
					       const FCP::File &      input,
					       const FCP::File &      output,
					       const FCP::Directory & tmp_dir);
		void                     run(bool dry_run);

	protected:
		// No copy allowed
		Filter(const Filter &);
		void operator =(const Filter &);

	private:
		std::string              tag_in_;
		std::string              tag_out_;

		std::vector<std::string> templates_;
		std::vector<std::string> commands_;

		std::string              mktemp(const std::string & id,
						const std::string & tmp_dir,
						size_t              index);
		std::vector<std::string> commands(const std::string & id,
						  const FCP::File &   input,
						  const FCP::File &   output,
						  const std::string & tmp_dir);

	};
}

#endif // FILTER_H
