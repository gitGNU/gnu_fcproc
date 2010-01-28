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

#include <string>
#include <boost/filesystem.hpp>

#include "debug.h"
#include "exception.h"
#include "string.h"
#include "transformation.h"
#include "rules.h"
#include "chain.h"
#include "tag.h"
#include "namespace.h"

namespace fcp {

        transformation::transformation(const std::string & tag,
                                       char                sep) :
                tag_(tag, sep)
        { }

        transformation::~transformation()
        { }

        void transformation::run(bool               dry,
                                 bool               force,
                                 const fcp::rules & rules,
                                 int                depth,
                                 const bfs::path &  work)
        {
                TR_DBG("Running transformation '%s' (%s, %s)\n",
                       tag_.id().c_str(),
                       dry   ? "dry"    : "not dry",
                       force ? "forced" : "not forced");

                fcp::file input(tag_.input());
                fcp::file output(tag_.output());

                if (bfs::exists(input.path())       &&
                    bfs::exists(output.path())      &&
                    bfs::equivalent(input.path(),
                                    output.path())) {
                        std::string e("Transformation "
                                      "'" + tag_.id() + "' "
                                      "must have different "
                                      "input and output");
                        throw fcp::exception(e.c_str());
                }

                // Build the chain for this transformation
                std::vector<fcp::filter *> rules_chain = rules.chain(input,
                                                                     output,
                                                                     depth,
                                                                     work);
                if (rules_chain.size() == 0) {
                        std::string e("No chain available for "
                                      "'" + tag_.id() + "' "
                                      "transformation");
                        throw fcp::exception(e.c_str());
                }

                // Finally create the chain from the filters sequence
                fcp::chain filters_chain(tag_.id(),
                                         input,
                                         output,
                                         rules_chain,
                                         work);

                filters_chain.run(dry, force);
        }

        const fcp::tag & transformation::tag() const
        { return tag_; }

}
