// -*- c++ -*-

//
// Copyright (C) 2008, 2009 Francesco Salvestrini
//                          Alessandro Massignan
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

#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

#include "debug.h"
#include "utility.h"
#include "exception.h"
#include "rules.h"
#include "parser.h"
#include "namespace.h"

#define PROGRAM_NAME "fcp2dot"

class wrong_option : public fcp::exception {
public:
        wrong_option(const char * message) :
                fcp::exception(message) { }
};

void run(std::vector<std::string> & dot,
         fcp::rules &               rules,
         char &                     separator)
{
        // Trace processed tags in order to
        // skip multiple declarations
        std::map<std::string, bool> processed_tags;

        // Setting up iterators
        fcp::rules::iterator                                            in;
        std::map<std::string,std::vector<std::string> >::const_iterator out;

        // Setting up special characters protection
        boost::regex pattern;
        try {
                pattern.assign("([\"\'\\$])",
                               boost::regex_constants::match_posix |
                               boost::regex_constants::match_any);
        } catch(boost::regex_error & e) {
                throw fcp::exception(e.what());
        }

        std::string  replace;
        replace = "\\\\\\1";

        dot.push_back("digraph {");

        for (in  = rules.begin();
             in != rules.end();
             in++) {

                if (processed_tags.find(in->first) ==
                    processed_tags.end()) {
                        processed_tags[in->first] = true;

                        dot.push_back("\t"      +
                                      in->first +
                                      ";");
                }
                for (out  = in->second.begin();
                     out != in->second.end();
                     out++) {

                        if (processed_tags.find(out->first) ==
                            processed_tags.end()) {
                                processed_tags[out->first] = true;
                                dot.push_back("\t"       +
                                              out->first +
                                              ";");
                        }
                        std::string cmd;
                        std::string cmd_dot;

                        cmd = in->first + "_" + out->first;

                        cmd_dot = "\t" + cmd + "[shape=box, label=\"";

                        std::vector<std::string>::const_iterator i;

                        for (i  = out->second.begin();
                             i != out->second.end();
                             i++) {
                                std::string tmp;
                                tmp = boost::regex_replace(*i,
                                                           pattern,
                                                           replace);

                                if ((out->second.end() - i) > 1) {
                                        cmd_dot += tmp + "\\n";
                                } else {
                                        cmd_dot += tmp;
                                }
                        }
                        cmd_dot += "\",fontnames=\"mono\",fontsize=8];";
                        dot.push_back(cmd_dot);

                        dot.push_back("\t"      +
                                      in->first +
                                      " -> "    +
                                      cmd       +
                                      ";");
                        dot.push_back("\t"       +
                                      cmd        +
                                      " -> "     +
                                      out->first +
                                      ";");
                }
                dot.push_back("");
        }
        dot.push_back("}");
}

#define DFLT_SEPARATOR ':'

// XXX FIXME: This prototype sucks
bool handle_options(int           argc,
                    char *        argv[],
                    std::string & input_file,
                    std::string & output_file,
                    bool &        force,
                    char &        separator)
{

        // Main options
        bpo::options_description main_options("Options");
        main_options.add_options()
                ("input,i",
                 bpo::value<std::string>(),
                 "input filename")
                ("output,o",
                 bpo::value<std::string>(),
                 "output filename")
                ("force,f",
                 "output file will be overwritten if exists")
                ("separator,s",
                 bpo::value<char>(&separator)->default_value(DFLT_SEPARATOR),
                 "set input/output separator character")
                ("debug,d",
                 "enable debugging traces")
                ("verbose,v",
                 "verbosely report processing")
                ("help,h",
                 "print this help, then exit")
                ("version,V",
                 "print version number, then exit");

        // Setting up the options parser
        bpo::options_description all_options("All options");
        all_options.add(main_options);

        bpo::variables_map vm;
        bpo::store(bpo::command_line_parser(argc, argv).
                   options(all_options).
                   run(),
                   vm);
        bpo::notify(vm);

        // Check options
        if (vm.count("input")) {
                input_file = vm["input"].as<std::string>();
        }
        if (vm.count("output")) {
                output_file = vm["output"].as<std::string>();
        }
        if (vm.count("force")) {
                force = true;
        }

        if (vm.count("debug")) {
                TR_CONFIG_LVL(TR_LVL_DEBUG, true);
        }
        if (vm.count("verbose")) {
                TR_CONFIG_LVL(TR_LVL_VERBOSE, true);
        }
        if (vm.count("version")) {
                std::vector<std::string> authors;

                authors.push_back("Francesco Salvestrini");
                authors.push_back("Alessandro Massignan");

                fcp::program::version(PROGRAM_NAME,
                                      PACKAGE_NAME,
                                      PACKAGE_VERSION,
                                      authors,
                                      std::cout);
                return false;
        }
        if (vm.count("help")) {
                // Passing only main options
                fcp::program::help(PROGRAM_NAME,
                                   "[OPTION]... [TRANSFORMATION]...",
                                   PACKAGE_BUGREPORT,
                                   main_options,
                                   std::cout);
                return false;
        }

        return true;
}

// XXX FIXME: This is a shame, please remove ASAP
fcp::rules rules;

void feeder(const std::string &              i,
            const std::string &              o,
            const std::vector<std::string> & c)
{ rules.add(i, o, c); }

void program(int argc, char * argv[])
{
        std::string input_file;
        std::string output_file;
        bool        force;
        char        separator;

        force = false;

        // Do not pollute ...
        {
                TR_DBG("Parsing program options\n");

                try {
                        if (!handle_options(argc, argv,
                                            input_file,
                                            output_file,
                                            force,
                                            separator)) {
                                TR_DBG("Clean exit now ...\n");
                                return;
                        }
                } catch (bpo::error & e) {
                        // Boost program_options error handler
                        throw fcp::exception(e.what());
                }

                TR_DBG("Option parsing complete (must continue)\n");
        }

        TR_VRB("Checking parameters ...\n");

        if (input_file.empty()) {
                throw wrong_option("Missing input file");
        }
        if (input_file.empty()) {
                throw wrong_option("Missing output file");
        }

        TR_DBG("Input file  = %s\n",   CQUOTE(input_file));
        TR_DBG("Output file = %s\n",   CQUOTE(output_file));
        TR_DBG("Separator   = '%c'\n", separator);

        bfs::path rules_file(input_file);
        bfs::path dot_file(output_file);

        if (!force && bfs::exists(dot_file)) {
                std::string e("Could not overwrite file " +
                              QUOTE(dot_file)             +
                              ", use force to override");
                throw fcp::exception(e.c_str());
        }

        TR_VRB("Parsing rules...\n");

        fcp::parser parser;

        parser.parse(rules_file,
                     bfs::initial_path<bfs::path>(),
                     feeder);
        if (!rules.is_valid()) {
                std::string e =
                        std::string("Invalid rules set "
                                    "detected while "
                                    "parsing ") +
                        input_file;

                throw fcp::exception(e.c_str());
        }
        BUG_ON(!rules.is_valid());

        TR_VRB("Processing rules...\n");

        std::vector<std::string> dot;

        run(dot, rules, separator);
        BUG_ON(dot.empty());

        TR_VRB("Writing to output file...\n");

        std::ofstream s(output_file.c_str());
        BUG_ON(!s.is_open());

        std::vector<std::string>::const_iterator i;
        for (i  = dot.begin(); i != dot.end(); i++) {
                s << *i << std::endl;
        }

        s.close();
}

int main(int argc, char * argv[])
{
        TR_CONFIG_LVL(TR_LVL_CRITICAL, true);
        TR_CONFIG_LVL(TR_LVL_ERROR,    true);
        TR_CONFIG_LVL(TR_LVL_WARNING,  true);
        TR_CONFIG_LVL(TR_LVL_NOTICE,   true);
        TR_CONFIG_LVL(TR_LVL_VERBOSE,  false);
        TR_CONFIG_LVL(TR_LVL_DEBUG,    false);

        TR_DBG("Program start ...\n");

        int retval = EXIT_FAILURE;
        try {
                program(argc, argv);
                retval = EXIT_SUCCESS;
        } catch (wrong_option & e) {
                fcp::program::hint(PROGRAM_NAME, e.what(), "-h", std::cout);
        } catch (fcp::exception & e) {
                TR_ERR("%s\n", e.what());
        } catch (...) {
                // All unhandled exceptions are bugs for us !
                BUG();
        }

        TR_DBG("Program stop (retval = %d) ...\n", retval);

        return retval;
}
