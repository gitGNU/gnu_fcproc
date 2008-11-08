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

#include "libs/fs/file.h"
#include "libs/fs/directory.h"
#include "rules.h"
#include "chain.h"

namespace FCP {
	class Transformation {
	public:
		Transformation(const std::string & tag,
			       char                separator,
			       FCP::Rules &        rules,
			       int                 mdepth);
		~Transformation(void);

		void                run(const FS::Directory & tmp_dir,
					bool                  dry_run,
					bool                  force);

		const std::string & tag(void)    const;
		const FS::File &    input(void)  const;
		const FS::File &    output(void) const;

	protected:
		// No copy allowed
		Transformation(const Transformation &);
		void operator =(const Transformation &);

	private:
		std::string  tag_;
		FS::File *   input_;
		FS::File *   output_;
		FCP::Chain * chain_;
	};
}

#endif // TRANSFORMATION_H
