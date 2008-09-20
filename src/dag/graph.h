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

namespace DAG {

	class Rule {
	public:
		Rule(const std::string & command) :
			command_(command) {
		};
		~Rule(void) { };

		bool run(std::string & input,
			 std::string & output);

	protected:
		Rule(void);

	private:
		std::string command_;
	};

	class Node : public Rule {
	public:
		Node(const std::string & tag,
		     const std::string & command) :
			Rule(command),
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

	class DAG {
	public:
		DAG(void);
		~DAG(void);

	protected:

	private:
	};
};

#endif // GRAPH_H
