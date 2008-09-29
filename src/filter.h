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

namespace Graph {
	// Rule11: 1 input, 1 output
	class Rule11 {
	public:
		Rule11(const std::string & command);
		~Rule11(void);

		bool                run(std::string & input,
					std::string & output);
		const std::string & command(void);

	protected:
		Rule11(void);

	private:
		std::string command_;
	};

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

	class Tree : public Node {
	public:
		Tree(const std::string & tag,
		     const std::string & command);
		~Tree(void);

		void                        father(Tree * tree);
		Tree *                      father(void);
		void                        child(Tree * tree);
		const std::vector<Tree *> & children(void);

	protected:
		Tree(void);

	private:
		Tree *              father_;
		std::vector<Tree *> children_;
	};

	class DAG {
	public:
		DAG(void);
		~DAG(void);

		bool                       add(const std::string & tag,
					       const std::string & command);
		bool                       remove(const std::string & tag);

		std::vector<Graph::Node *> chain(std::string tag_from,
						 std::string tag_to);

	protected:

	private:
		Tree * root_;
	};
};

#endif // FILTER_H
