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
		      const FS::File &             output,
		      std::vector<FCP::Filter *> & filters,
		      const FS::Directory &        work);
		~Chain(void);

		const std::string & id(void);
		void                run(bool dry,
					bool force);

	protected:
		// No copy allowed
		Chain(const Chain &);
		void operator =(const Chain &);

	private:
		std::string                id_;
		const FS::File &           input_;
		const FS::File &           output_;
		std::vector<FCP::Filter *> filters_;
	};
}

#endif // CHAIN_H
