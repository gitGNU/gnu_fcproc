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
#include <vector>

#include "libs/misc/trace.h"
#include "libs/graph/dag.h"

using namespace Graph;

DAG::DAG(void)
{
	root_ = 0;

	TR_DBG("DAG %p created\n", this);
}

DAG::~DAG(void)
{
	if (root_) {
		delete root_;
	}

	TR_DBG("DAG %p destroyed\n", this);
}

bool DAG::add(const std::string & tag,
	      const std::string & command)
{
#if 0
	Graph::Tree * tree = new Graph::Tree(tag, command);

	if (!root_) {
		root_ = tree;
		return true;
	}

	return root_->children(tree);
#endif

	return false;
}

bool DAG::remove(const std::string & tag)
{
#if 0
	if (!root_) {
		return root_->remove(tag);
	}
#endif

	return false;
}

#if 0
std::vector<const Node &> DAG::path(const std::string & tag_in,
				    const std::string & tag_out)
{
	// Find tag-out node

	// Go back till tag-in node found, filling the path

	std::vector<const Node &> tmp;

	return tmp;
}
#endif
