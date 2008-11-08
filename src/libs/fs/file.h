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

#ifndef LIBS_FS_FILE_H
#define LIBS_FS_FILE_H

#include "config.h"

#include <string>

namespace FS {
	class File {
	public:
		File(const std::string & name);
		~File(void);

		const std::string & name(void)      const;
		const std::string & dirname(void)   const;
		const std::string & basename(void)  const;
		const std::string & extension(void) const;
		time_t              mtime(void)     const;
		bool                exists(void)    const;

	protected:
		File(void);

	private:
		std::string name_;
		std::string dirname_;
		std::string basename_;
		std::string extension_;
	};
}

#endif // LIBS_FS_FILE_H
