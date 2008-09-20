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

#ifndef GRAPH_H
#define GRAPH_H

#include "config.h"

#include <string>
#include <vector>

namespace DAG {

	// Rule11: 1 input, 1 output
	class Rule11 {
	public:
		Rule11(const std::string & command) :
			command_(command) {
		};
		~Rule11(void) { };

		bool run(std::string & input,
			 std::string & output);

	protected:
		Rule11(void);

	private:
		std::string command_;
	};

	class Node : public Rule11 {
	public:
		Node(const std::string & tag,
		     const std::string & command) :
			Rule11(command),
			tag_(tag) { };
		~Node(void) { };

		const std::string & tag(void) {
			return tag_;
		}

	protected:
		Node(void);

	private:
		std::string tag_;
	};

	class Tree {
	public:
		Tree(const Node & node) :
			node_(node) { };
		~Tree(void) { };

	protected:
		Node node_;

	private:
	};

	class DAG {
	public:
		DAG(void);
		~DAG(void);

		void                      add(Node * node);
		void                      remove(Node * node);
		std::vector<const Node &> path(const std::string & tag_in,
					       const std::string & tag_out);
	protected:

	private:
		Tree * root_;
	};
};

#endif // GRAPH_H
