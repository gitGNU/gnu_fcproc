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
#include <cstdlib>
#include <csignal>

#include "debug.h"
#include "string.h"
#include "exception.h"
#include "command.h"

namespace fcp {

        command::command(const std::string & shell) :
                shell_(shell)
        { }

        command::~command()
        { }

        void command::rollback()
        { }

        void command::run(bool dry)
        {
                int ret;

                TR_DBG("Calling system(\"%s\")\n", shell_.c_str());
                if (dry) {
                        return;
                }

                ret = system(shell_.c_str());
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
                                      "command '" + shell_ + "'");
                        throw fcp::exception(e.c_str());
                }
        }

}
