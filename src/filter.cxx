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
#include <csignal>

#include <boost/filesystem.hpp>

#include "debug.h"
#include "string.h"
#include "exception.h"
#include "filter.h"
#include "utility.h"
#include "namespace.h"

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
                        fcp::itos(index);
        }

        // XXX FIXME: Remove id parameter ASAP
        void filter::setup(const std::string & id,
                           const bfs::path &   work_dir)
        {
                TR_DBG("Filter '%s' setup:\n",
                       id.c_str());
                TR_DBG("  Input       '%s\n",
                       input_.string().c_str());
                TR_DBG("  Output      '%s'\n",
                       output_.string().c_str());
                TR_DBG("  working dir '%s'\n",
                       work_dir.string().c_str());

                commands_ = templates_;

                std::map<std::string, std::string> temps;
                size_t                             count;

                count = 0;

                //TR_DBG("Replacing variables\n")
                std::vector<std::string>::iterator ic;
                for (ic  = commands_.begin(); ic != commands_.end(); ic++) {
                        std::string command = (*ic);
                        command = fcp::replace(command,"$I",input_.string());
                        command = fcp::replace(command,"$O",output_.string());

                        //TR_DBG("  Command '%s'\n", command.c_str());

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

                                //TR_DBG("      v = '%s'\n", v.c_str());

                                std::string t = temps[v];
                                if (t == "") {
                                        t = mktemp(id,
                                                   work_dir.string().c_str(),
                                                   count);
                                        count++;
                                        temps[v] = t;
                                }

                                command = fcp::replace(command, v, t);

                                //TR_DBG("    Replaced '%s' with '%s'\n",
                                //       v.c_str(), t.c_str());
                        }

                        //TR_DBG("    Command is now '%s'\n", command.c_str());

                        (*ic) = command;
                }

                BUG_ON(commands_.size() != templates_.size());
        }

        void filter::run(bool dry)
        {
                // XXX FIXME: Place a more specific trace ...
                TR_DBG("Running filter commands\n");

                std::vector<std::string>::iterator i;
                for (i  = commands_.begin(); i != commands_.end(); i++) {
                        if (dry) {
                                TR_VRB("%s\n", (*i).c_str());
                                continue;
                        }

                        int ret;

                        BUG_ON((*i).size() == 0);

                        TR_DBG("  Calling system(\"%s\")\n", (*i).c_str());

                        // XXX FIXME: Use gnulib system()
                        ret = system((*i).c_str());
                        if (ret == -1) {
                                throw fcp::exception("Got fork() failure");
                        }
                        if (WIFSIGNALED(ret) &&
                            (WTERMSIG(ret) == SIGINT ||
                             WTERMSIG(ret) == SIGQUIT)) {
                                throw fcp::exception("Interrupted");
                        }
                        if (WEXITSTATUS(ret) != 0) {
                                std::string e("Got problems running "
                                              "command '" + (*i) + "'");
                                throw fcp::exception(e.c_str());
                        }
                }
        }

}
