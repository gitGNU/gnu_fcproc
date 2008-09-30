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
#include "libs/misc/debug.h"
#include "filter.h"

using namespace Graph;

Rule11::Rule11(const std::string & command) :
	command_(command)
{
}

Rule11::~Rule11(void)
{
}

bool Rule11::run(std::string & input,
		 std::string & output)
{
	TR_DBG("Running command '%s' with input '%s' and output '%s'\n",
	       command_.c_str(), input.c_str(), output.c_str());

	return true;
}

const std::string & Rule11::command(void)
{
	return command_;
}

Node::Node(const std::string & tag,
	   const std::string & command) :
	Rule11(command),
	tag_(tag)
{
	TR_DBG("Node %p created\n", this);
}

Node::~Node(void)
{
	TR_DBG("Node %p destroyed\n", this);
}

const std::string & Node::tag(void)
{
	return tag_;
}

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

void Tree::parent(Tree * tree)
{
	father_ = tree;
}

const Tree * Tree::parent(void)
{
	return father_;
}

void Tree::child(Tree * tree)
{
}

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

void DAG::add(const std::string & tag_from,
	      const std::string & tag_to,
	      const std::string & command)
{
	TR_DBG("Adding filter '%s' -> '%s'\n",
	       tag_from.c_str(), tag_to.c_str());

#if 0
	Graph::Tree * tree = new Graph::Tree(tag, command);

	if (!root_) {
		root_ = tree;
		return true;
	}

	return root_->children(tree);
#endif
}

void DAG::remove(const std::string & tag_from,
		 const std::string & tag_to)
{
	TR_DBG("Removing filter '%s' -> '%s'\n",
	       tag_from.c_str(), tag_to.c_str());

#if 0
	if (!root_) {
		return root_->remove(tag);
	}
#endif
}

std::vector<Graph::Node *> DAG::chain(std::string tag_from,
				      std::string tag_to)
{
	TR_DBG("Extracting filter chain from '%s' to '%s'\n",
	       tag_from.c_str(), tag_to.c_str());

	BUG_ON(tag_from.size() == 0);
	BUG_ON(tag_to.size() == 0);

	//	Graph::Node *              node;
	std::vector<Graph::Node *> filters;
	//	node = dag.find(tag_out);


	return filters;
}
