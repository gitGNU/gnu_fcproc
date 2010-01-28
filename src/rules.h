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

#ifndef FCP_RULES_H
#define FCP_RULES_H

#include "config.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/cregex.hpp>

#include "filter.h"
#include "file.h"
#include "namespace.h"

namespace fcp {
        class rules;
}

std::ostream & operator<<(std::ostream & stream, const fcp::rules & rules);

namespace fcp {
        class rules : public boost::noncopyable {
        public:
                friend std::ostream & ::operator<<(std::ostream & stream,
                                                   const rules &  rules);

                rules(const std::vector<bfs::path> & files);

                // Builds a filters-chain
                std::vector<fcp::filter *>
                chain(const fcp::file & input,
                      const fcp::file & output,
                      int               mdepth,
                      const bfs::path & work) const;

                size_t size();
                bool   empty();

        protected:

        private:
                struct {
                        boost::regmatch_t match_[3];
                        boost::regex_t    empty_;
                        boost::regex_t    comment_;
                        boost::regex_t    include_;
                        boost::regex_t    header_;
                        boost::regex_t    body_;
                } re_;

                // rules = (input-tag, output-tag, commands)
                std::map<std::string,
                        std::map<std::string,
                        std::vector<std::string> > > rules_;

                std::string readline(std::ifstream & stream);
                void        parse(const bfs::path & file);

                typedef std::pair<std::string,
                                  std::vector<std::string> > node_t;

                class Antiloop {
                private:
                        std::set<std::pair<std::string, std::string> > set_;

                public:
                        bool insert(const std::string & in,
                                    const std::string & out) {
                                std::pair<std::string, std::string> t(in,out);

                                if (set_.find(t) != set_.end()) {
                                        // The node is already present, we have
                                        // got a loop ...
                                        return false;
                                }
                                set_.insert(t);
                                return true;
                        }
                };

                bool chain_nodes(Antiloop &            antiloop,
                                 const std::string &   tag_in,
                                 const std::string &   tag_out,
                                 int                   mdepth,
                                 std::vector<node_t> & data) const;
        };
}

#endif
