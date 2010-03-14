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

#ifndef FCP_UTILITY_H
#define FCP_UTILITY_H

#include "config.h"

#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

#include "namespace.h"

namespace fcp {

        namespace string {

                std::string trim_left(const std::string & s,
                                      const std::string & t);
                std::string trim_right(const std::string & s,
                                       const std::string & t);
                std::string trim_both(const std::string & s,
                                      const std::string & t);
                std::string replace(std::string &       in,
                                    const std::string & from,
                                    const std::string & to);
                std::string itos(int value);
                void        slice(const std::string & in,
                                  char                separator,
                                  std::string &       out1,
                                  std::string &       out2);

                std::string quote(const std::string & s);
                std::string quote(const bfs::path & p);

        }

        namespace environment {

                std::string get(const char *        key);
                std::string get(const std::string & key);
                bool        set(const std::string & key,
                                const std::string & value);

        }

        namespace program {

                void hint(const char *   program_name,
                          const char *   message,
                          const char *   help_option,
                          std::ostream & stream);
                void help(const char *               program_name,
                          const char *               usage,
                          const char *               bug_report_email,
                          bpo::options_description & options,
                          std::ostream &             stream);
                void version(const char *               program_name,
                             const char *               package,
                             const char *               version,
                             std::vector<std::string> & authors,
                             std::ostream & stream);

        }

}

#define QUOTE(S)  fcp::string::quote(S)
#define CQUOTE(S) (QUOTE(S)).c_str()

#endif
