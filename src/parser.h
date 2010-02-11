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

#ifndef FCP_PARSER_H
#define FCP_PARSER_H

#include "config.h"

#include <string>
#include <vector>
#include <iostream>
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/cregex.hpp>

#include "rules.h"
#include "utility.h"
#include "namespace.h"

namespace fcp {

        class parser : public boost::noncopyable {
        public:
                parser();
                ~parser();

                void parse(const bfs::path & file,
                           const bfs::path & base,
                           void (* feeder)(const std::string &              i,
                                           const std::string &              o,
                                           const std::vector<std::string> & c));

        private:
                struct {
                        boost::regmatch_t match_[3];
                        boost::regex_t    empty_;
                        boost::regex_t    comment_;
                        boost::regex_t    include_;
                        boost::regex_t    header_;
                        boost::regex_t    body_;
                } re_;

                std::string readline(std::ifstream & stream);
        };

}

#endif
