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
#include <boost/filesystem.hpp>

#include "debug.h"
#include "file.h"
#include "namespace.h"

namespace fcp {

        file::file()
        { }

        file & file::operator=(const file & rhs)
        {
                if (this == &rhs) {
                        return *this;
                }

                this->path_ = rhs.path_;
                this->type_ = rhs.type_;

                return *this;
        }

        file::file(const bfs::path &   path,
                   const std::string & type) :
                path_(path),
                type_(type)
        {
                if (type.empty()) {
                        std::string tmp = bfs::extension(path);
                        type_ = tmp.substr(1);
                }

                TR_DBG("File: name = '%s' / type = '%s'\n",
                       path_.string().c_str(),
                       type_.c_str());
        }

        file::~file()
        { }

        const bfs::path & file::path() const
        { return path_; }

        const std::string & file::name() const
        { return path_.string(); }

        const std::string & file::type() const
        { return type_; }

        bool file::exists() const
        { return bfs::exists(path_); }

};
