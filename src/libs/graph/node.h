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

#ifndef LIBS_MISC_NODE_H
#define LIBS_MISC_NODE_H

#include "config.h"

#include "libs/graph/rule.h"

namespace Graph {
	class Node : public Rule11 {
	public:
		Node(const std::string & tag,
		     const std::string & command);
		~Node(void);

		const std::string & tag(void);

	protected:
		Node(void);

	private:
		std::string tag_;
	};
};

#endif // LIBS_MISC_NODE_H
