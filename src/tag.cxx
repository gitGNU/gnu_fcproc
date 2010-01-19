// -*- c++ -*-

//
// Copyright (C) 2008, 2009 Francesco Salvestrini
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

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "libs/misc/string.h"
#include "tag.h"

namespace fcp {
        tag::tag(const std::string & id,
                 char                separator) :
                id_(id)
        {
                const std::string::size_type p = id_.find(separator);

                if ((p < 0) || (p > id_.size())) {
                        throw Exception("Missing separator "
                                        "in tag "
                                        "'" + id_ + "'");
                }

                // Work on the input part
                {
                        // Grab the input part
                        std::string tmp = id_.substr(0, p);
                        if (tmp.size() == 0) {
                                throw Exception("Missing input file "
                                                "in transformation "
                                                "'" + id_ + "'");
                        }
                        TR_DBG("Tag subpart '%s' (input)\n", tmp.c_str());

                        // Slice it in (name, type)
                        std::string name;
                        std::string type;
                        String::slice(tmp, '%', name, type);
                        TR_DBG("  Subpart name '%s'\n", name.c_str());
                        TR_DBG("  Subpart type '%s'\n", type.c_str());

                        // Create the input filename
                        lhs_ = fcp::file(name, type);
                }

                // Work on the output part
                {
                        // Grab the output part
                        std::string tmp = id_.substr(p + 1);
                        if (tmp.size() == 0) {
                                throw Exception("Missing output file "
                                                "in transformation "
                                                "'" + id_ + "'");
                        }
                        TR_DBG("Tag subpart '%s' (output)\n", tmp.c_str());

                        // Slice it in (name, type)
                        std::string name;
                        std::string type;
                        String::slice(tmp, '%', name, type);
                        TR_DBG("  Subpart name '%s'\n", name.c_str());
                        TR_DBG("  Subpart type '%s'\n", type.c_str());

                        // Create the output filename
                        rhs_ = fcp::file(name, type);
                }
        }

        tag::~tag()
        { }

        const std::string & tag::id() const
        { return id_; }

        const fcp::file & tag::rhs()
        { return rhs_; }

        const fcp::file & tag::lhs()
        { return lhs_; }

};
