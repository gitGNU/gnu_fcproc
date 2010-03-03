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

#include "debug.h"
#include "exception.h"
#include "utility.h"
#include "tag.h"

namespace fcp {

        tag::tag(const std::string & id,
                 char                separator) :
                id_(id)
        {
                const std::string::size_type p = id_.find(separator);

                if ((p < 0) || (p > id_.size())) {
                        std::string e("Missing separator in tag " +
                                      QUOTE(id_));
                        throw fcp::exception(e.c_str());
                }

                // Work on the input part
                {
                        // Grab the input part
                        std::string tmp = id_.substr(0, p);
                        if (tmp.size() == 0) {
                                std::string e("Missing input file "
                                              "in transformation " +
                                              QUOTE(id_));
                                throw fcp::exception(e.c_str());
                        }
                        TR_DBG("Tag subpart %s (input)\n", CQUOTE(tmp));

                        // Slice it in (name, type)
                        std::string name;
                        std::string type;
                        fcp::string::slice(tmp, '%', name, type);
                        TR_DBG("  Subpart name %s\n", CQUOTE(name));
                        TR_DBG("  Subpart type %s\n", CQUOTE(type));

                        // Create the input filename
                        input_ = fcp::file(name, type);
                }

                // Work on the output part
                {
                        // Grab the output part
                        std::string tmp = id_.substr(p + 1);
                        if (tmp.size() == 0) {
                                std::string e("Missing output file "
                                              "in transformation " +
                                              QUOTE(id_));
                                throw fcp::exception(e.c_str());
                        }
                        TR_DBG("Tag subpart %s (output)\n",
                               CQUOTE(tmp));

                        // Slice it in (name, type)
                        std::string name;
                        std::string type;
                        fcp::string::slice(tmp, '%', name, type);
                        TR_DBG("  Subpart name %s\n", CQUOTE(name));
                        TR_DBG("  Subpart type %s\n", CQUOTE(type));

                        // Create the output filename
                        output_ = fcp::file(name, type);
                }
        }

        tag::~tag()
        { }

        const std::string & tag::id() const
        { return id_; }

        const fcp::file & tag::output()
        { return output_; }

        const fcp::file & tag::input()
        { return input_; }

}
