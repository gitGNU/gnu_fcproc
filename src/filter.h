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

#ifndef FCP_FILTER_H
#define FCP_FILTER_H

#include "config.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

#include "namespace.h"

namespace fcp {
        class filter : public boost::noncopyable {
        public:
                filter(const bfs::path &                input,
                       const bfs::path &                output,
                       const std::vector<std::string> & commands);

                const bfs::path & input();
                const bfs::path & output();

                void             setup(const std::string & id,
                                       const bfs::path &   tmp);
                void             run(bool dry);

        protected:

        private:
                bfs::path  input_;
                bfs::path  output_;
                std::vector<std::string> templates_;
                std::vector<std::string> commands_;

                std::string
                mktemp(const std::string & id,
                       const std::string & tmp_dir,
                       size_t              index);

                std::vector<std::string>
                commands(const std::string &  id,
                         const bfs::path &    input,
                         const bfs::path &    output,
                         const std::string &  work_dir);
        };
}

#endif
