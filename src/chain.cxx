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
#include "utility.h"
#include "namespace.h"

namespace fcp {

        chain::chain(const std::string &          id,
                     const fcp::file &            input,
                     const fcp::file &            output,
                     std::vector<fcp::filter *> & filters,
                     const bfs::path &            work) :
                id_(id),
                input_(input),
                output_(output),
                filters_(filters)
        {
                TR_DBG("Creating chain %s\n",      CQUOTE(id));
                TR_DBG("  Working directory %s\n", CQUOTE(work));
                TR_DBG("  Chain input file  %s\n", CQUOTE(input_.name()));
                TR_DBG("  Chain output file %s\n", CQUOTE(output_.name()));

                std::vector<fcp::filter *>::iterator i;
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        BUG_ON((*i) == 0);
                        (*i)->setup(id_, work);
                }

                TR_DBG("Chain for transformation %s:\n",CQUOTE(id_));
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        TR_DBG("  %s -> %s\n",
                               CQUOTE((*i)->input()),
                               CQUOTE((*i)->output()));
                }
        }

        bool chain::is_spurious()
        {
                TR_DBG("Checking for spurious rebuild\n");

                // Check the output file presence and its last
                // modification time in order to avoid a rebuild
                // if it is up-to-date

                BUG_ON(!bfs::exists(input_.path()));

                if (!bfs::exists(output_.path())) {
                        TR_DBG("Output file does not exists\n");
                        return false;
                }

                time_t diff = (bfs::last_write_time(output_.path()) -
                               bfs::last_write_time(input_.path()));
                if (diff >= 0) {
                        TR_DBG("Output file %s is up-to-date (delta = %ld)\n",
                               CQUOTE(output_.name()), diff);
                        return true;
                }

                TR_DBG("Output file is outdated\n");

                return false;
        }

        void chain::run(bool dry,
                        bool force)
        {
                TR_DBG("Running chain %s\n", CQUOTE(id_));

                if (!dry) {
                        TR_DBG("Checking input file %s\n",
                               CQUOTE(input_.name()));

                        if (!bfs::exists(input_.path())) {
                                std::string e("Missing input file for chain " +
                                              QUOTE(input_.name()));
                                throw fcp::exception(e.c_str());
                        }

                        if (!force) {
                                if (is_spurious()) {
                                        TR_DBG("Avoiding spurious rebuild\n");
                                        return;
                                }
                        }
                }

                std::vector<fcp::filter *>::iterator i;
                for (i = filters_.begin(); i != filters_.end(); i++) {
                        BUG_ON((*i) == 0);
                        (*i)->run(dry);
                }
        }

}
