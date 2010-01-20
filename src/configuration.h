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

#ifndef FCP_CONFIGURATION_H
#define FCP_CONFIGURATION_H

#include "config.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "utility.h"

namespace fcp {
                class configuration {
                public:
                        configuration(std::string delimiter = "=",
                             std::string comment   = "#",
                             std::string whites    = " \n\t\v\r\f");

                        // Search for key and read value or optional
                        // default value
                        template<class T>
                        bool get(T &                 var,
                                 const std::string & key) const;
                        template<class T>
                        bool get(T &                 var,
                                 const std::string & key,
                                 const T &           value) const;

                        template<class T>
                        void set(std::string key,
                                 const T &   value);

                        void remove(const std::string & key);
                        bool exists(const std::string & key) const;

                        friend std::ostream &
                        operator <<(std::ostream & os,
                                    const configuration &   cf);

                        friend std::istream &
                        operator >>(std::istream & is,
                                    configuration &         cf);

                protected:

                private:
                        std::string            delimiter_;
                        std::string            comment_;
                        std::string            whites_;
                        std::map<std::string,
                                 std::string>  tuples_;

                        template<class T>
                        static std::string     as_string(const T & t);
                        template<class T>
                        static T               to_T(const std::string & s);
                };

                template<class T>
                std::string configuration::as_string(const T & t)
                {
                        std::ostringstream o;

                        o << t;

                        return o.str();
                }

                template<class T>
                T configuration::to_T(const std::string & s)
                {
                        T                  t;
                        std::istringstream i(s);

                        i >> t;

                        return t;
                }

                template<class T>
                bool configuration::get(T &                 var,
                               const std::string & key) const
                {
                        std::map<std::string, std::string>::const_iterator p;

                        p = tuples_.find(key);
                        if (p != tuples_.end()) {
                                var = to_T<T>(p->second);
                                return true;
                        }

                        return false;
                }

                template<class T>
                bool configuration::get(T &                 var,
                               const std::string & key,
                               const T &          value) const
                {
                        std::map<std::string, std::string>::const_iterator p;

                        p = tuples_.find(key);
                        if (p != tuples_.end()) {
                                var = to_T<T>(p->second);
                                return true;
                        }

                        var = value;

                        return false;
                }

                template<class T>
                void configuration::set(std::string key,
                               const T &   value)
                {
                        tuples_[fcp::trim_both(key, whites_)] =
                                fcp::trim_both(as_string(value), whites_);
                }
        }

#endif
