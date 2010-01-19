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

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

#include "libs/misc/debug.h"
#include "libs/misc/string.h"
#include "libs/misc/exception.h"
#include "chain.h"
#include "filter.h"
#include "file.h"

namespace FCP {
        Chain::Chain(const std::string &             id,
                     const fcp::file &               input,
                     const fcp::file &               output,
                     std::vector<FCP::Filter *> &    filters,
                     const boost::filesystem::path & work) :
                id_(id),
                input_(input),
                output_(output)
        {
                TR_DBG("Creating chain '%s' (working directory '%s')\n",
                       id.c_str(), work.string().c_str());

                filters_ = filters;

                std::vector<FCP::Filter *>::iterator i;
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        BUG_ON((*i) == 0);
                        (*i)->setup(id_, work);
                }

#if 0
                TR_DBG("Filters-chain for transformation '%s':\n",
                       tag_.id().c_str());
                std::vector<FCP::Filter *>::iterator iter;
                for (iter = chain.begin(); iter != chain.end(); iter++) {
                        TR_DBG("  '%s' -> '%s'\n",
                               (*iter)->input().path().string().c_str(),
                               (*iter)->output().path().string().c_str());
                }
#endif
        }

        void Chain::run(bool dry,
                        bool force)
        {
                TR_DBG("Running filters-chain '%s'\n", id_.c_str());

                if (!dry && !force) {
                        // Check the output file presence and its last
                        // modification time in order to avoid a rebuild
                        // if it is up-to-date

                        if (!boost::filesystem::exists(input_.path())) {
                                throw Exception("Missing input file "
                                                "'" + input_.path().string() + "'");
                        }

                        if (boost::filesystem::exists(output_.path()) &&
                            (boost::filesystem::last_write_time(input_.path()) >=
                             boost::filesystem::last_write_time(output_.path()))) {
                                TR_DBG("Output file '%s' is up-to-date\n",
                                       output_.path().string().c_str());
                                return;
                        }
                }

                std::vector<FCP::Filter *>::iterator i;
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        BUG_ON((*i) == 0);
                        (*i)->run(dry);
                }
        }
};
