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

#include "debug.h"
#include "string.h"
#include "exception.h"
#include "chain.h"
#include "filter.h"
#include "file.h"

namespace fcp {

        chain::chain(const std::string &             id,
                     const fcp::file &               input,
                     const fcp::file &               output,
                     std::vector<fcp::filter *> &    filters,
                     const boost::filesystem::path & work) :
                id_(id),
                input_(input),
                output_(output),
                filters_(filters)
        {
                TR_DBG("Creating chain '%s'\n",
                       id.c_str());
                TR_DBG("  Working directory '%s'\n",
                       work.string().c_str());
                TR_DBG("  Chain input file  '%s'\n",
                       input_.name().c_str());
                TR_DBG("  Chain output file '%s'\n",
                       output_.name().c_str());

                std::vector<fcp::filter *>::iterator i;
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        BUG_ON((*i) == 0);
                        (*i)->setup(id_, work);
                }

                TR_DBG("Chain for transformation '%s':\n",
                       id_.c_str());
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        TR_DBG("  '%s' -> '%s'\n",
                               (*i)->input().string().c_str(),
                               (*i)->output().string().c_str());
                }
        }

        bool chain::is_spurious()
        {
                TR_DBG("Checking for spurious rebuild\n");

                // Check the output file presence and its last
                // modification time in order to avoid a rebuild
                // if it is up-to-date

                BUG_ON(!boost::filesystem::exists(input_.path()));

                if (!boost::filesystem::exists(output_.path())) {
                        TR_DBG("Output file does not exists\n");
                        return false;
                }

                TR_DBG("Output file exists\n");

                if (boost::filesystem::last_write_time(input_.path()) <=
                    boost::filesystem::last_write_time(output_.path())) {
                        TR_DBG("Output file '%s' is up-to-date\n",
                               output_.name().c_str());
                        return true;
                }

                TR_DBG("Output file is outdated\n");

                return false;
        }

        void chain::run(bool dry,
                        bool force)
        {
                TR_DBG("Running chain '%s'\n", id_.c_str());

                TR_DBG("Checking dry/run/spurious\n");

                if (!dry && !force) {
                        TR_DBG("Checking input file '%s' existance\n",
                               input_.name().c_str());

                        if (!boost::filesystem::exists(input_.path())) {
                                std::string e("Missing input file "
                                              "for chain "
                                              "'" + input_.name() + "'");
                                throw fcp::exception(e.c_str());
                        }

                        if (is_spurious()) {
                                TR_DBG("Avoiding a spurious rebuild\n");
                                return;
                        }
                }

                TR_DBG("Working on filters\n");

                std::vector<fcp::filter *>::iterator i;
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        BUG_ON((*i) == 0);
                        (*i)->run(dry);
                }
        }

}
