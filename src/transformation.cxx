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

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "libs/misc/string.h"
#include "transformation.h"
#include "rules.h"
#include "chain.h"
#include "tag.h"

namespace fcp {
        transformation::transformation(const std::string &             tag,
                                       char                            sep, 
                                       const fcp::rules &              rules,
                                       int                             depth,
                                       const boost::filesystem::path & work) :
                tag_(tag, sep)
        {
                fcp::file input(tag_.lhs());
                fcp::file output(tag_.rhs());

                if (boost::filesystem::exists(input.path())   &&
                    boost::filesystem::exists(output.path())  &&
                    boost::filesystem::equivalent(input.path(),
                                                  output.path())) {
                        throw Exception("Transformation "
                                        "'" + tag_.id() + "' "
                                        "must have different "
                                        "input and output "
                                        "file");
                }

                // Build the filters-chain for this transformation
                std::vector<fcp::filter *> chain;
                chain = rules.chain(input, output, depth, work);
                if (chain.size() == 0) {
                        throw Exception("No filters-chain available for "
                                        "'" + tag_.id() + "' "
                                        "transformation");
                }

                // Finally create the filters-chain from the filters sequence
                chain_ = new fcp::chain(tag_.id(),
                                        input,
                                        output,
                                        chain,
                                        work);
                assert(chain_ != 0);
        }

        transformation::~transformation()
        { }

        void transformation::run(bool dry,
                                 bool force)
        {
                TR_DBG("Running chain '%s' (%s, %s)\n",
                       tag_.id().c_str(),
                       dry   ? "dry"    : "not dry",
                       force ? "forced" : "not forced");

                chain_->run(dry, force);
        }

        const fcp::tag & transformation::tag() const
        { return tag_; }
};
