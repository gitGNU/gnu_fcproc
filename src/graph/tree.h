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

#ifndef TREE_H
#define TREE_H

#include "config.h"

#include <vector>

#include "graph/node.h"
#include "graph/tree.h"

namespace Graph {
	class Tree : public Node {
	public:
		Tree(const std::string & tag,
		     const std::string & command);
		~Tree(void);

		void                        father(Tree & tree);
		Tree &                      father(void);
		void                        child(Tree & tree);
		const std::vector<Tree *> & children(void);

	protected:
		Tree(void);

	private:
		Tree *              father_;
		std::vector<Tree *> children_;
	};
};

#endif // TREE_H
