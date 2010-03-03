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

#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/cregex.hpp>
#include <boost/version.hpp>

#include "debug.h"
#include "exception.h"
#include "parser.h"
#include "utility.h"

namespace fcp {

        class parse_error : public fcp::exception {
        public:
                parse_error(const char *      message,
                            const bfs::path & path,
                            size_t            line) :
                        fcp::exception((std::string(message)  +
                                        std::string(" in ")   +
                                        path.string()         +
                                        std::string(":")      +
                                        fcp::string::itos(line)).c_str()) { }
        };

        parser::parser()
        {
                if (boost::regcomp(&re_.empty_,
                                   "^[ \t]*$",
                                   boost::REG_NOSUB)) {
                        throw fcp::exception("Cannot compile empty regexp");
                }

                if (boost::regcomp(&re_.comment_,
                                   "^[ \t]*#.*$",
                                   boost::REG_NOSUB)) {
                        throw fcp::exception("Cannot compile comment regexp");
                }

                if (boost::regcomp(&re_.include_,
                                   "^[ \t]*include[ \t]+\"(.*)\"[ \t]*$",
                                   boost::REG_EXTENDED)) {
                        throw fcp::exception("Cannot compile include regexp");
                }

                if (boost::regcomp(&re_.header_,
                                   "^(.*):(.*)[ \t]*$",
                                   boost::REG_EXTENDED)) {
                        throw fcp::exception("Cannot compile header regexp");
                }

                if (boost::regcomp(&re_.body_,
                                   "^\t(.*)$",
                                   boost::REG_EXTENDED)) {
                        throw fcp::exception("Cannot compile body regexp");
                }
        }

        parser::~parser()
        {
                boost::regfree(&re_.body_);
                boost::regfree(&re_.header_);
                boost::regfree(&re_.include_);
                boost::regfree(&re_.comment_);
                boost::regfree(&re_.empty_);
        }

#define PARSER_DEBUGS 0
#if PARSER_DEBUGS
#define P_DBG(FMT,ARGS...) TR_DBG(FMT, ##ARGS);
#else
#define P_DBG(FMT,ARGS...)
#endif

#define DUMP_REGMATCH(X) {                                              \
                P_DBG("    regexmatch_t.rm_so = %d\n", (X).rm_so);      \
                P_DBG("    regexmatch_t.rm_eo = %d\n", (X).rm_eo);      \
        }

#if 0
#define DUMP_REGMATCHES(X) {                    \
                DUMP_REGMATCH((X)[0]);          \
                DUMP_REGMATCH((X)[1]);          \
                DUMP_REGMATCH((X)[2]);          \
        }
#else
#define DUMP_REGMATCHES(X)
#endif

        std::string parser::readline(std::ifstream & stream)
        {
                std::string ret;
                bool        stop;

                stop = false;
                while (!stop) {
                        std::string tmp;

                        std::getline(stream, tmp);
                        tmp = fcp::string::trim_right(tmp, " \t");
                        if (tmp.find_last_of("\\") != tmp.size()) {
                                stop = true;
                        }

                        ret = ret + tmp;
                }

                return ret;
        }

        void parser::parse(const bfs::path & file,
                           const bfs::path & base,
                           void (* feeder)(const std::string &              i,
                                           const std::string &              o,
                                           const std::vector<std::string> & c))
        {
                bfs::path file_path;

                if (file.root_directory() != "") {
                        file_path = file;
                } else {
                        file_path = base / file;
                }

                if (!bfs::exists(file_path)) {
                        std::string e(std::string("File ")       +
                                      QUOTE(file_path)           +
                                      std::string(" is missing"));
                        fcp::exception(e.c_str());
                }

                bfs::path base_path;

#if (BOOST_VERSION >= 103600)
                base_path = file_path.parent_path();
#else
                base_path = file_path.branch_path();
#endif

                // Always dump the file under examination
                TR_DBG("Parsing rules from file %s\n",
                       CQUOTE(file_path));

                std::ifstream stream;
                stream.open(file_path.string().c_str());
                if (!stream) {
                        std::string e("Cannot open "   +
                                      QUOTE(file_path) +
                                      " for reading");
                        throw fcp::exception(e.c_str());
                }

                std::string line   = "";
                size_t      number = 0;
                enum {
                        S_IDLE,
                        S_RULE_HEADER,
                        S_RULE_BODY,
                        S_RULE_COMPLETE,
                } state;

                std::string              tag_in;
                std::string              tag_out;
                std::vector<std::string> commands;

                commands.clear();

                state = S_IDLE;
                while (!stream.eof()) {
                        if (state == S_IDLE) {
                                P_DBG("State %d, number %d, line %s\n",
                                      state, number, CQUOTE(line));

                                // Read a new line
                                line = readline(stream); number++;

                                P_DBG("  line %d = %s\n",
                                      number, CQUOTE(line));

                                // Is this an empty line ?
                                if (regexec(&re_.empty_,
                                            line.c_str(),
                                            3, re_.match_, 0) == 0) {
                                        P_DBG("  Got empty line\n");
                                        continue;
                                }

                                // Is this a comment line ?
                                if (regexec(&re_.comment_,
                                            line.c_str(),
                                            3, re_.match_, 0) == 0) {
                                        P_DBG("  Got comment line\n");
                                        continue;
                                }

                                // Is this an include line ?
                                if (regexec(&re_.include_,
                                            line.c_str(),
                                            3, re_.match_, 0) == 0) {

                                        DUMP_REGMATCHES(re_.match_);

                                        std::string include;
                                        include =
                                                line.substr(re_.match_[1].
                                                            rm_so,
                                                            re_.match_[1].
                                                            rm_eo -
                                                            re_.match_[1].
                                                            rm_so);

                                        P_DBG("  Got include is %s\n",
                                              CQUOTE(include));
                                        parse(bfs::path(include),
                                              base_path,
                                              feeder);
                                        continue;
                                }

                                // Line is not empty, start reading header
                                state = S_RULE_HEADER;
                                continue;

                        } else if (state == S_RULE_HEADER) {
                                P_DBG("State %d, number %d, line %s\n",
                                      state, number, CQUOTE(line));

                                // Is this an header line ?
                                if (regexec(&re_.header_,
                                            line.c_str(),
                                            3, re_.match_, 0) != 0) {
                                        throw parse_error("Missing header",
                                                          file,
                                                          number);
                                }

                                P_DBG("  Got header\n");

                                DUMP_REGMATCHES(re_.match_);

                                tag_in = line.substr(re_.match_[1].rm_so,
                                                     re_.match_[1].rm_eo -
                                                     re_.match_[1].rm_so);
                                if (tag_in == "") {
                                        throw parse_error("Missing input tag",
                                                          file,
                                                          number);
                                }

                                tag_out = line.substr(re_.match_[2].rm_so,
                                                      re_.match_[2].rm_eo -
                                                      re_.match_[2].rm_so);
                                if (tag_out == "") {
                                        throw parse_error("Missing output tag",
                                                          file,
                                                          number);
                                }

                                BUG_ON(tag_in  == "");
                                BUG_ON(tag_out == "");

                                P_DBG("  tag in  = %s\n",
                                      CQUOTE(tag_in));
                                P_DBG("  tag out = %s\n",
                                      CQUOTE(tag_out));

                                // Header read, start reading body
                                state = S_RULE_BODY;
                                continue;

                        } else if (state == S_RULE_BODY) {
                                P_DBG("State %d, number %d, line %s\n",
                                      state, number, CQUOTE(line));

                                // Read a new line
                                line = readline(stream); number++;

                                P_DBG("  State %d, number %d, line %s\n",
                                      state, number, CQUOTE(line));

                                // Empty lines complete the body part
                                if (line.size() == 0) {
                                        if (commands.size() != 0) {
                                                state = S_RULE_COMPLETE;
                                                continue;
                                        }

                                        throw parse_error("Missing body",
                                                          file,
                                                          number);
                                }

                                if (regexec(&re_.body_,
                                            line.c_str(),
                                            3, re_.match_, 0) != 0) {
                                        throw parse_error("Wrong body",
                                                          file,
                                                          number);
                                }

                                DUMP_REGMATCHES(re_.match_);

                                line = line.substr(re_.match_[1].rm_so,
                                                   re_.match_[1].rm_eo -
                                                   re_.match_[1].rm_so);

                                commands.push_back(line);
                                continue;

                        } else if (state == S_RULE_COMPLETE) {
                                P_DBG("State %d, number %d, line %s\n",
                                      state, number, CQUOTE(line));

                                BUG_ON(tag_in  == "");
                                BUG_ON(tag_out == "");
                                BUG_ON(commands.size() < 1);

                                P_DBG("  %s -> %s\n",
                                      tag_in.c_str(), tag_out.c_str());

                                feeder(tag_in, tag_out, commands);

                                P_DBG("  Added rule\n");

                                tag_in   = "";
                                tag_out  = "";
                                commands.clear();

                                state = S_IDLE;
                                continue;

                        } else {
                                BUG();
                        }

                        BUG();
                }

                // Do we need to push the last command ?
                if (commands.size() && tag_in != "" && tag_out != "") {
                        // Yes we do
                        feeder(tag_in, tag_out, commands);
                }

                stream.close();
        }

}
