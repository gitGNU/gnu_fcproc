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
#include <sstream>
#include <cstdlib>

#include "string.h"
#include "debug.h"
#include "utility.h"

namespace fcp {

        namespace string {

                std::string trim_left(const std::string & s,
                                      const std::string & t)
                {
                        static std::string tmp;

                        tmp = s;

                        return tmp.erase(0, tmp.find_first_not_of(t));
                }

                std::string trim_right(const std::string & s,
                                       const std::string & t)
                {
                        static std::string tmp;

                        tmp = s;

                        return tmp.erase(tmp.find_last_not_of(t) + 1);
                }

                std::string trim_both(const std::string & s,
                                      const std::string & t)
                {
                        return trim_left(trim_right(s, t), t);
                }

                std::string replace(std::string &       in,
                                    const std::string & from,
                                    const std::string & to)
                {
                        BUG_ON(from.size() == 0);

                        //TR_DBG("Replacing in '%s': '%s' -> '%s'\n",
                        //       in.c_str(), from.c_str(), to.c_str());

                        std::string            t;
                        std::string::size_type p;
                        std::string::size_type q;

                        t = in;
                        p = 0;
                        q = from.size();
                        for (;;) {
                                p = t.find(from, p);
                                if (p == t.npos) {
                                        break;
                                }

                                t.replace(p, q, to);
                                p += to.size();
                        }

                        //TR_DBG("String is now '%s'\n", t.c_str());

                        return t;
                }

                std::string itos(int value)
                {
                        std::stringstream tmp;

                        tmp << value;

                        return tmp.str();
                }

                void slice(const std::string & in,
                           char                separator,
                           std::string &       out1,
                           std::string &       out2)
                {
                        std::string::size_type p;

                        p = in.find(separator);
                        if ((p < 0) || (p > in.size())) {
                                out1 = in;
                                out2 = "";
                                return;
                        }

                        out1 = in.substr(0, p);
                        out2 = in.substr(p + 1);
                }

                std::string quote(const std::string & s)
                { return std::string("`") + s + std::string("'"); }

                std::string quote(const bfs::path & p)
                { return quote(p.string()); }

        }

        namespace environment {

                std::string get(const char * key)
                {
                        BUG_ON(key == 0);

                        return std::string(::getenv(key));
                }

                std::string get(const std::string & key)
                {
                        BUG_ON(key.size() == 0);

                        return fcp::environment::get(key.c_str());
                }

                bool set(const std::string & key,
                         const std::string & value)
                {
                        BUG_ON(key.size() == 0);

                        if (::setenv(key.c_str(), value.c_str(), 1) != 0) {
                                return false;
                        }

                        return true;
                }

        }

}
