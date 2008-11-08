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

#ifndef CHAIN_H
#define CHAIN_H

#include "config.h"

#include <vector>
#include <string>
#include <map>

#include "libs/fs/file.h"
#include "libs/fs/directory.h"
#include "filter.h"

namespace FCP {
	class Chain {
	public:
		Chain(const std::string &          id,
		      const FS::File &             input,
		      std::vector<FCP::Filter *> & filters,
		      const FS::File &             output);
		~Chain(void);

		const std::string & id(void);
		void                run(const FS::Directory & tmp_dir,
					bool                  dry_run,
					bool                  force);

	protected:
		// No copy allowed
		Chain(const Chain &);
		void operator =(const Chain &);

	private:
		std::string                id_;
		FS::File                   input_;
		FS::File                   output_;
		std::vector<FCP::Filter *> filters_;
	};
}

#endif // CHAIN_H
