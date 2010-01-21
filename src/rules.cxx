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
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <fstream>
#include <algorithm>
#include <boost/filesystem.hpp>

#include "regex.h"

#include "debug.h"
#include "utility.h"
#include "exception.h"
#include "rules.h"
#include "file.h"

namespace fcp {

        rules::rules(const std::vector<std::string> & filenames)
        {
                if (regcomp(&re_.empty_,
                            "^[ \t]*$",
                            REG_NOSUB)) {
                        throw fcp::exception("Cannot compile empty regexp");
                }

                if (regcomp(&re_.comment_,
                            "^[ \t]*#.*$",
                            REG_NOSUB)) {
                        throw fcp::exception("Cannot compile comment regexp");
                }

                if (regcomp(&re_.include_,
                            "^[ \t]*include[ \t]+\"(.*)\"[ \t]*$",
                            REG_EXTENDED)) {
                        throw fcp::exception("Cannot compile include regexp");
                }

                if (regcomp(&re_.header_,
                            "^(.*):(.*)[ \t]*$",
                            REG_EXTENDED)) {
                        throw fcp::exception("Cannot compile header regexp");
                }

                if (regcomp(&re_.body_,
                            "^\t(.*)$",
                            REG_EXTENDED)) {
                        throw fcp::exception("Cannot compile body regexp");
                }

                std::vector<std::string>::const_iterator iter;
                for (iter  = filenames.begin();
                     iter != filenames.end();
                     iter++) {
                        parse((* iter));
                }

                regfree(&re_.body_);
                regfree(&re_.header_);
                regfree(&re_.include_);
                regfree(&re_.comment_);
                regfree(&re_.empty_);

                std::map<std::string,
                        std::map<std::string,
                        std::vector<std::string> > >::const_iterator in;
                std::map<std::string,
                        std::vector<std::string> >::const_iterator   out;

                // Check for rule-loops
                TR_DBG("Known rules:\n");
                for (in  = rules_.begin();
                     in != rules_.end();
                     in++) {
                        TR_DBG("  '%s' ->\n", in->first.c_str());
                        for (out  = in->second.begin();
                             out != in->second.end();
                             out++) {
                                // There must be no loops !
                                BUG_ON(in->first == out->first);

                                TR_DBG("    '%s'\n", out->first.c_str());
                        }
                }
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

        std::string rules::readline(std::ifstream & stream)
        {
                std::string ret;
                bool        stop;

                stop = false;
                while (!stop) {
                        std::string tmp;

                        std::getline(stream, tmp);
                        tmp = fcp::trim_right(tmp, " \t");
                        if (tmp.find_last_of("\\") != tmp.size()) {
                                stop = true;
                        }

                        ret = ret + tmp;
                }

                return ret;
        }

        void rules::parse(const std::string & filename)
        {
                // Always dump the file under examination
                TR_DBG("Parsing rules from file '%s'\n", filename.c_str());

                std::ifstream stream;

                stream.open(filename.c_str());
                if (!stream) {
                        std::string e("Cannot open file "
                                      "'" + filename + "' "
                                      "for reading");
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

                std::string              tag_in   = "";
                std::string              tag_out  = "";
                std::vector<std::string> commands;

                commands.clear();

                state = S_IDLE;
                while (!stream.eof()) {
                        if (state == S_IDLE) {
                                P_DBG("State %d, number %d, line '%s'\n",
                                      state, number, line.c_str());

                                // Read a new line
                                line = readline(stream); number++;

                                P_DBG("  line %d = '%s'\n",
                                      number, line.c_str());

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

                                        P_DBG("  Got include is '%s'\n",
                                              include.c_str());
                                        parse(include);
                                        continue;
                                }

                                // Line is not empty, start reading header
                                state = S_RULE_HEADER;
                                continue;

                        } else if (state == S_RULE_HEADER) {
                                P_DBG("State %d, number %d, line '%s'\n",
                                      state, number, line.c_str());

                                // Is this an header line ?
                                if (regexec(&re_.header_,
                                            line.c_str(),
                                            3, re_.match_, 0) != 0) {
                                        std::string e("Missing header "
                                                      "in file "
                                                      "'" + filename + "'"
                                                      " at line "
                                                      "'" +
                                                      fcp::itos(number) +
                                                      "'");
                                        throw fcp::exception(e.c_str());
                                }

                                P_DBG("  Got header\n");

                                DUMP_REGMATCHES(re_.match_);

                                tag_in = line.substr(re_.match_[1].rm_so,
                                                     re_.match_[1].rm_eo -
                                                     re_.match_[1].rm_so);
                                if (tag_in == "") {
                                        std::string e("Missing input tag "
                                                      "in file "
                                                      "'" + filename + "'"
                                                      " at line "
                                                      "'" +
                                                      fcp::itos(number) +
                                                      "'");
                                        throw fcp::exception(e.c_str());
                                }

                                tag_out = line.substr(re_.match_[2].rm_so,
                                                      re_.match_[2].rm_eo -
                                                      re_.match_[2].rm_so);
                                if (tag_out == "") {
                                        std::string e("Missing output tag "
                                                      "in file "
                                                      "'" + filename + "'"
                                                      " at line "
                                                      "'" +
                                                      fcp::itos(number) +
                                                      "'");
                                        throw fcp::exception(e.c_str());
                                }

                                BUG_ON(tag_in  == "");
                                BUG_ON(tag_out == "");

                                P_DBG("  tag in  = '%s'\n", tag_in.c_str());
                                P_DBG("  tag out = '%s'\n", tag_out.c_str());

                                // Header read, start reading body
                                state = S_RULE_BODY;
                                continue;

                        } else if (state == S_RULE_BODY) {
                                P_DBG("State %d, number %d, line '%s'\n",
                                      state, number, line.c_str());

                                // Read a new line
                                line = readline(stream); number++;

                                P_DBG("  State %d, number %d, line '%s'\n",
                                      state, number, line.c_str());

                                // Empty lines complete the body part
                                if (line.size() == 0) {
                                        if (commands.size() == 0) {
                                                std::string e("Missing body "
                                                              "in file "
                                                              "'" + filename + "'"
                                                              " at line "
                                                              "'" +
                                                              fcp::itos(number) +
                                                              "'");
                                                throw fcp::exception(e.c_str());                                        }

                                        state = S_RULE_COMPLETE;
                                        continue;
                                }

                                if (regexec(&re_.body_,
                                            line.c_str(),
                                            3, re_.match_, 0) != 0) {
                                        std::string e("Wrong body "
                                                      "in file "
                                                      "'" + filename + "'"
                                                      " at line "
                                                      "'" +
                                                      fcp::itos(number) +
                                                      "'");
                                        throw fcp::exception(e.c_str());
                                }

                                DUMP_REGMATCHES(re_.match_);

                                line = line.substr(re_.match_[1].rm_so,
                                                   re_.match_[1].rm_eo -
                                                   re_.match_[1].rm_so);

                                commands.push_back(line);
                                continue;

                        } else if (state == S_RULE_COMPLETE) {
                                P_DBG("State %d, number %d, line '%s'\n",
                                      state, number, line.c_str());

                                BUG_ON(tag_in  == "");
                                BUG_ON(tag_out == "");
                                BUG_ON(commands.size() < 1);

                                P_DBG("  %s -> %s\n",
                                      tag_in.c_str(), tag_out.c_str());

                                (rules_[tag_in])[tag_out] = commands;

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
                        (rules_[tag_in])[tag_out] = commands;
                }

                stream.close();
        }

        typedef std::pair<std::string, std::vector<std::string> > fcdata_t;

        bool rules::chain_nodes(Antiloop &            antiloop,
                                const std::string &   tag_in,
                                const std::string &   tag_out,
                                int                   depth,
                                std::vector<node_t> & data) const
        {
                BUG_ON(depth < 0); // Allow depth == 1

                TR_DBG("Walking '%s' -> '%s' (depth = %d)\n",
                       tag_in.c_str(), tag_out.c_str(), depth);

                depth--;
                if (depth < 0) {
                        TR_DBG("Max chain size exceeded\n");
                        return false;
                }

                std::map<std::string,
                        std::map<std::string,
                        std::vector<std::string> > >::const_iterator i;
                i = rules_.find(tag_in);
                if (i == rules_.end()) {
                        TR_DBG("No rules available for '%s' -> '%s'\n",
                               tag_in.c_str(), tag_out.c_str());
                        return false;
                }

                TR_DBG("Got '%s' node\n", tag_in.c_str());

                // Antiloop detection check
                if (!antiloop.insert(tag_in, tag_out)) {
                        TR_DBG("Got a loop while walking '%s' -> '%s'\n",
                               tag_in.c_str(), tag_out.c_str());
                        return false;
                }

                std::map<std::string,
                        std::vector<std::string> >::const_iterator j;
                for (j = i->second.begin(); j != i->second.end(); j++) {

                        // We have got a node
                        node_t t(j->first, j->second);

                        if (j->first == tag_out) {
                                // This node is the last node in the chain
                                TR_DBG("Got chain end ('%s' -> '%s')\n",
                                       i->first.c_str(), j->first.c_str());
                                data.push_back(t);

                                return true;
                        }

                        // This is not the last one, let us recurse in order
                        // to find our way out
                        if (chain_nodes(antiloop,
                                        j->first,
                                        tag_out,
                                        depth,
                                        data)) {
                                TR_DBG("Got chain node ('%s' -> '%s')\n",
                                       i->first.c_str(), j->first.c_str());
                                data.push_back(t);

                                return true;
                        }
                }

                return false;
        }

        std::vector<fcp::filter *>
        rules::chain(const fcp::file &               input,
                     const fcp::file &               output,
                     int                             depth,
                     const boost::filesystem::path & work) const
        {
                BUG_ON(depth <= 0);

                TR_DBG("Looking for chain '%s' -> '%s' "
                       "(max depth %d)\n",
                       input.name().c_str(),
                       output.name().c_str(),
                       depth);

                std::set<std::pair<std::string, std::string> > loop;
                std::vector<fcp::filter *>                     ret;

                // Build chain data based on extensions
                std::vector<node_t> data;
                Antiloop            antiloop;
                std::string         in_type;
                std::string         out_type;

                in_type = input.type();
                if (in_type.empty()) {
                        std::string e("Cannot detect file type for "
                                      "'" + input.name() + "'");
                        throw fcp::exception(e.c_str());
                }

                out_type = output.type();
                if (out_type.empty()) {
                        std::string e("Cannot detect file type for "
                                      "'" + output.name() + "'");
                        throw fcp::exception(e.c_str());
                }

                TR_DBG("Input type  = '%s'\n", in_type.c_str());
                TR_DBG("Output type = '%s'\n", out_type.c_str());

                if (!chain_nodes(antiloop, in_type, out_type, depth, data)) {
                        TR_DBG("No chain found ...\n");
                        data.clear();
                        return ret;
                }

                // Some basic check(s) before beginning

                // Our chains must have 1 node at least
                BUG_ON(data.size() < 1);

                std::reverse(data.begin(), data.end());
                TR_DBG("Chain found!\n");

                // We must transform the node_t sequence into a proper
                // chain. During such transformation we must tweak the
                // paths along the chain. The starting point is the input
                // path, the ending point is the output path while we must work
                // on the 'work' path on all remaining nodes

                // The starting point lives on the input path
                boost::filesystem::path src(input.path());

                std::vector<node_t>::size_type i;
                for (i = 0; i < data.size(); i++) {
                        boost::filesystem::path tmp;

                        if (i == (data.size() - 1)) {
                                // The ending point lives on the output path
                                tmp = output.path();
                        } else {
                                // All the others must live in the work path
                                tmp = work /
                                        (boost::filesystem::basename(src) +
                                         "." +
                                         data[i].first);
                        }

                        boost::filesystem::path dst(tmp);
                        fcp::filter * f = new fcp::filter(src, dst,
                                                          data[i].second);
                        ret.push_back(f);

                        src = dst;
                }

                return ret;
        }

}

std::ostream & operator<<(std::ostream &     stream,
                          const fcp::rules & rules)
{
        std::map<std::string,
                std::map<std::string,
                std::vector<std::string> > >::const_iterator i;

        for (i  = rules.rules_.begin();
             i != rules.rules_.end();
             i++) {
                std::map<std::string,
                        std::vector<std::string> >::const_iterator j;
                for (j  = i->second.begin();
                     j != i->second.end();
                     j++) {
                        stream << i->first
                               << " -> "
                               << j->first
                               << std::endl;
                }
        }

        return stream;
}
