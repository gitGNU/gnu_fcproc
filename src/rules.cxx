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
#include <set>
#include <map>
#include <fstream>
#include <algorithm>

#include "debug.h"
#include "utility.h"
#include "exception.h"
#include "rules.h"
#include "file.h"
#include "namespace.h"

namespace fcp {

        rules::~rules()
        { }

        rules::rules()
        { }

        bool rules::is_valid()
        {
                std::map<std::string,
                        std::map<std::string,
                        std::vector<std::string> > >::const_iterator in;
                std::map<std::string,
                        std::vector<std::string> >::const_iterator   out;

                for (in  = rules_.begin();
                     in != rules_.end();
                     in++) {
                        for (out  = in->second.begin();
                             out != in->second.end();
                             out++) {
                                if (in->first == out->first) {
                                        // Rules short-circuit detected
                                        return false;
                                }
                        }
                }

                return true;
        }

        void rules::add(const std::string &              tag_in,
                        const std::string &              tag_out,
                        const std::vector<std::string> & commands)
        { rules_[tag_in][tag_out] = commands; }

        size_t rules::size()
        { return rules_.size(); }

        bool rules::empty()
        { return ((rules_.size() == 0) ? true : false); }

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
        rules::chain(const fcp::file & input,
                     const fcp::file & output,
                     int               depth,
                     const bfs::path & work) const
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
                bfs::path src(input.path());

                std::vector<node_t>::size_type i;
                for (i = 0; i < data.size(); i++) {
                        bfs::path tmp;

                        if (i == (data.size() - 1)) {
                                // The ending point lives on the output path
                                tmp = output.path();
                        } else {
                                // All the others must live in the work path
                                tmp = work /
                                        (bfs::basename(src) +
                                         "." +
                                         data[i].first);
                        }

                        bfs::path dst(tmp);
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

        TR_VRB("Rules:\n");

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
