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

#include <boost/filesystem.hpp>

#include "debug.h"
#include "string.h"
#include "exception.h"
#include "filter.h"
#include "utility.h"
#include "namespace.h"
#include "command.h"

namespace fcp {

        filter::filter(const bfs::path &                input,
                       const bfs::path &                output,
                       const std::vector<std::string> & commands) :
                input_(input),
                output_(output),
                templates_(commands)
        {
                commands_.clear(); // useless
        }

        bfs::path filter::input()
        { return input_; }

        bfs::path filter::output()
        { return output_; }

        std::string filter::mktemp(const std::string & id,
                                   const std::string & dir,
                                   size_t              index)
        {
                return dir + std::string("/") + id + std::string("-") +
                        fcp::string::itos(index);
        }

        // XXX FIXME: Remove id parameter ASAP
        void filter::setup(const std::string & id,
                           const bfs::path &   work_dir)
        {
                TR_DBG("Filter %s setup:\n", CQUOTE(id));
                TR_DBG("  Input       %s\n", CQUOTE(input_));
                TR_DBG("  Output      %s\n", CQUOTE(output_));
                TR_DBG("  working dir %s\n", CQUOTE(work_dir));

                commands_ = templates_;

                std::map<std::string, std::string> temps;
                size_t                             count;

                count = 0;

                //TR_DBG("Replacing variables\n")
                for (std::vector<std::string>::iterator ic =
                             commands_.begin();
                     ic != commands_.end();
                     ic++) {
                        std::string command = (*ic);
                        command = fcp::string::replace(command,
                                                       "$I", input_.string());
                        command = fcp::string::replace(command,
                                                       "$O", output_.string());

                        //TR_DBG("  Command %s\n", CQUOTE(command));

                        // Ugly
                        for (;;) {
                                //TR_DBG("    Replace in progress\n");

                                //
                                // XXX FIXME:
                                //     Add environment variable substitution
                                //

                                std::string::size_type s =
                                        command.find("$T");
                                //TR_DBG("      s = %d, e = %d\n", s, e);
                                if ((s == 0) || (s == std::string::npos)) {
                                        break;
                                }

                                std::string::size_type e =
                                        command.find_first_not_of("0123456789",
                                                                  s + 1);
                                //TR_DBG("      s = %d, e = %d\n", s, e);
                                if ((e == 0) || (e == std::string::npos)) {
                                        break;
                                }

                                std::string v = command.substr(s, e - s + 2);
                                BUG_ON(v.size() == 0);

                                //TR_DBG("      v = %s\n", CQUOTE(v));

                                std::string t = temps[v];
                                if (t == "") {
                                        t = mktemp(id,
                                                   work_dir.string().c_str(),
                                                   count);
                                        count++;
                                        temps[v] = t;
                                }

                                command = fcp::string::replace(command, v, t);

                                //TR_DBG("    Replaced %s with %s\n",
                                //       CQUOTE(v),
                                //       CQUOTE(t));
                        }

                        //TR_DBG("    Command is now %s\n",
                        //       CQUOTE(command));

                        (*ic) = command;
                }

                BUG_ON(commands_.size() != templates_.size());
        }

        void filter::run(bool dry)
        {
                // XXX FIXME: Place a more specific trace ...
                TR_DBG("Running filter commands\n");

                for (std::vector<std::string>::iterator i =
                             commands_.begin();
                     i != commands_.end();
                     i++) {
                        if (dry) {
                                TR_VRB("%s\n", (*i).c_str());
                                continue;
                        }

                        fcp::command c(*i);

                        c.run(dry);
                }
        }

}
