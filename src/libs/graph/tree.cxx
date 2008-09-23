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

#include "libs/graph/node.h"
#include "libs/graph/tree.h"
#include "libs/misc/trace.h"

using namespace Graph;

Tree::Tree(const std::string & tag,
	   const std::string & command) :
	Node(tag, command)
{
	father_ = 0;
	children_.clear();

	TR_DBG("Tree %p created\n", this);
}

Tree::~Tree(void)
{
	TR_DBG("Tree %p destroyed\n", this);

}

void Tree::father(Tree & tree)
{
}

Tree & Tree::father(void)
{
	return *father_;
}

void Tree::child(Tree & tree)
{
}

const std::vector<Tree *> & Tree::children(void)
{
	return children_;
}
