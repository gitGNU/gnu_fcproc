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

#ifndef FCP_TRANSFORMATION_H
#define FCP_TRANSFORMATION_H

#include "config.h"

#include <string>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

#include "tag.h"
#include "rules.h"
#include "chain.h"
#include "file.h"

namespace fcp {
        class transformation : public boost::noncopyable {
        public:
                transformation(const std::string &             tag,
                               char                            separator,
                               const fcp::rules &              rules,
                               int                             depth,
                               const boost::filesystem::path & work);
                ~transformation();

                void             run(bool dry,
                                     bool force);

                const fcp::tag & tag() const;

        protected:

        private:
                void slice(char                separator,
                           const std::string & tag,
                           std::string &       name,
                           std::string &       type);

                fcp::tag     tag_;
                fcp::chain * chain_;
        };
}

#endif
