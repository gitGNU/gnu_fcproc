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

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "debug.h"
#include "utility.h"
#include "exception.h"
#include "rules.h"
#include "transformation.h"

#define PROGRAM_NAME "fcp"

class wrong_option : public fcp::exception {
public:
        wrong_option(const char * message) :
                fcp::exception(message) { }
};

class cannot_run : public fcp::exception {
public:
        cannot_run(const char * message) :
                fcp::exception(message) { }
};

class missing_directory : public fcp::exception {
public:
        missing_directory(const char * name) :
                // XXX FIXME: Remove ASAP
                fcp::exception((std::string("Directory '") +
                                std::string(name)          +
                                std::string("' is missing")).c_str()) { }
};

class missing_file : public fcp::exception {
public:
        missing_file(const char * name) :
                // XXX FIXME: Remove ASAP
                fcp::exception((std::string("File '") +
                                std::string(name)     +
                                std::string("' is missing")).c_str()) { }
};

void run(const std::vector<std::string> & tags,
         // XXX FIXME: Change to boost::filesystem::path
         fcp::rules &                     rules,
         bool                             dump_rules,
         int                              max_depth,
         char                             separator,
         const boost::filesystem::path &  work_dir,
         bool                             force,
         bool                             dry_run)
{
        BUG_ON(rules.size() <= 0);

        std::vector<fcp::transformation *> transformations(tags.size());
        std::vector<fcp::transformation *>::iterator it;

        // Setup transformations
        {
                std::vector<std::string>::const_iterator tf =
                        tags.begin();

                for (it =  transformations.begin();
                     it != transformations.end();
                     it++, tf++) {
                        (*it) = new fcp::transformation((*tf),
                                                        separator,
                                                        rules,
                                                        max_depth,
                                                        work_dir);
                }
        }

        TR_VRB("Performing %d transformation%c\n",
               transformations.size(),
               (transformations.size() > 1) ? 's' : ' ');

        // Run all transformations
        for (it  = transformations.begin();
             it != transformations.end();
             it++) {
                (*it)->run(dry_run, force);
        }

        // Clean up everything
        for (it  = transformations.begin();
             it != transformations.end();
             it++) {
                delete (*it);
        }

        TR_VRB("Operations complete\n");
}

void version()
{
        std::cout
                << PROGRAM_NAME
                << " (" << PACKAGE_NAME  << ") "
                << PACKAGE_VERSION
                << std::endl
                << std::endl
                << "Copyright (C) 2008, 2009 Francesco Salvestrini"
                << std::endl
                << std::endl
                << "This is free software.  "
                << "You may redistribute copies of it under the terms of"
                << std::endl
                << "the GNU General Public License "
                << "<http://www.gnu.org/licenses/gpl.html>."
                << std::endl
                << "There is NO WARRANTY, to the extent permitted by law."
                << std::endl;
}

// XXX FIXME: Ugly as hell, please remove ASAP
char separator = ':';

void help(boost::program_options::options_description & options)
{
        std::cout
                << "Usage: "
                << PROGRAM_NAME
                << " [OPTION]... [TRANSFORMATION]..."
                << std::endl
                << std::endl
                << options
                << std::endl
                << "Specify TRANSFORMATION using the format:"
                << std::endl
                << std::endl
                << "  INPUTFILE[%TYPE]<SEPARATOR>OUTPUTFILE[%TYPE]"
                << std::endl
                << std::endl
                << "Default SEPARATOR is "
                << "'" << separator << "'. "
                << "INPUTFILE and OUTPUTFILE must be different."
                << std::endl
                << "File TYPE is optional and it will be guessed "
                << "if not provided."
                << std::endl
                << std::endl
                << "Report bugs to <" << PACKAGE_BUGREPORT << ">"
                << std::endl;
}

// XXX FIXME: This prototype sucks
bool handle_options(int                      argc,
                    char *                   argv[],
                    bool                     & dry_run,
                    bool                     & force,
                    bool                     & dump_rules,
                    std::vector<std::string> & transformations_tags,
                    std::vector<std::string> & rules_default,
                    std::vector<std::string> & rules_user,
                    std::string              & temp_dir_name,
                    int                      & max_depth)
{
        namespace bpo = boost::program_options;

        // Main options
        bpo::options_description main_options("Options");

        main_options.add_options()
                ("config,c",
                 bpo::value<std::string>(),
                 "use alternate configuration file")
                ("rules,r",
                 bpo::value<std::vector<std::string> >()->composing(),
                 "use alternate rules file")
                ("max-depth,m",
                 bpo::value<int>(),
                 "set max filter-chains depth")
                ("temp-dir,t",
                 bpo::value<std::string>(),
                 "set temporary directory")
                ("separator,s",
                 bpo::value<char>(),
                 "set input/output separator character")
                ("no-std-rules,q",
                 "do not load standard rules")
                ("dump-rules,b",
                 "dump rules base, then exit")
                ("dry-run,n",
                 "display commands without modifying any files")
                ("force,f",
                 "consider all files outdated")
                ("debug,d",
                 "enable debugging traces")
                ("verbose,v",
                 "verbosely report processing")
                ("help,h",
                 "print this help, then exit")
                ("version,V",
                 "print version number, then exit");

        // Command line positional/hidden options (transformations)
        bpo::options_description hidden_options("Hidden options");
        hidden_options.add_options()
                ("transformation,T",
                 bpo::value<std::vector<std::string> >(),
                 "set transformation");

        bpo::positional_options_description positional_options;
        positional_options.add("transformation", -1);

        // Setting up the options parser
        bpo::options_description all_options("All options");
        all_options.add(main_options);
        all_options.add(hidden_options);

        bpo::variables_map vm;
        bpo::store(bpo::command_line_parser(argc, argv).
                   options(all_options).
                   positional(positional_options).
                   run(),
                   vm);
        bpo::notify(vm);

        // Check options
        if (vm.count("rules")) {
                rules_user =
                        vm["rules"].as<std::vector<std::string> >();
        }
        if (vm.count("max-depth")) {
                max_depth = vm["max-depth"].as<int>();
        }
        if (max_depth < 1) {
                throw wrong_option("Wrong max-depth");
        }
        if (vm.count("temp-dir")) {
                temp_dir_name = vm["temp-dir"].as<std::string>();
        }
        if (vm.count("separator")) {
                separator = vm["separator"].as<char>();
        }
        if (vm.count("no-std-rules")) {
                rules_default.clear();
        }
        if (vm.count("dump-rules")) {
                dump_rules = true;
        }
        if (vm.count("dry-run")) {
                dry_run = true;
        }
        if (vm.count("force")) {
                force = true;
        }
        if (vm.count("transformation")) {
                transformations_tags =
                        vm["transformation"].as<std::vector<std::string> >();
        }

        if (vm.count("debug")) {
                TR_CONFIG_LVL(TR_LVL_DEBUG);
        }
        if (vm.count("verbose")) {
                TR_CONFIG_LVL(TR_LVL_VERBOSE);
        }
        if (vm.count("version")) {
                version();
                return false;
        }
        if (vm.count("help")) {
                // Passing only main options
                help(main_options);
                return false;
        }

        return true;
}

void program(int argc, char * argv[])
{
        bool                     dry_run          = false;
        bool                     force            = false;
        bool                     dump_rules       = false;
        std::vector<std::string> transformations;
        int                      max_depth        = 16;
        boost::filesystem::path  temp_dir("/tmp");
        std::vector<std::string> rules_filenames;

        // Do not pollute ...
        {
                std::vector<std::string> rules_default;
                std::vector<std::string> rules_user;
                std::string              temp_dir_name;

                // Push default rules
                rules_default.push_back(fcp::getenv("HOME") +
                                        std::string("/") +      \
                                        std::string(".") +      \
                                        std::string(PACKAGE_TARNAME) +  \
                                        std::string("/") +              \
                                        std::string("rules"));

                TR_DBG("Parsing program options\n");

                if (!handle_options(argc, argv,
                                    dry_run,
                                    force,
                                    dump_rules,
                                    transformations,
                                    rules_default,
                                    rules_user,
                                    temp_dir_name,
                                    max_depth)) {
                        TR_DBG("Clean exit now ...\n");
                        return;
                }
                TR_DBG("Option parsing complete (must continue)\n");

                // Insert default rules
                rules_filenames.insert(rules_filenames.end(),
                                       rules_default.begin(),
                                       rules_default.end());
                // Insert user rules
                rules_filenames.insert(rules_filenames.end(),
                                       rules_user.begin(),
                                       rules_user.end());

                if (!temp_dir_name.empty()) {
                        temp_dir = boost::filesystem::path(temp_dir_name);
                }
        }

        TR_VRB("Checking parameters ...\n");

        std::vector<boost::filesystem::path>
                rules_paths(rules_filenames.size());

        // From filenames to paths (to be rearranged ASAP)
        std::vector<std::string>::iterator             i;
        std::vector<boost::filesystem::path>::iterator j;
        for (i = rules_filenames.begin(), j = rules_paths.begin();
             i != rules_filenames.end();
             i++, j++) {
                (*j) = boost::filesystem::path(*i);
        }

        // Read rules file
        fcp::rules rules(rules_paths);
        if (dump_rules) {
                std::cout << rules;
                return;
        }

        // Check required parameters
        if (transformations.size() == 0) {
                throw wrong_option("Missing transformation(s)");
        }

        if (!boost::filesystem::exists(temp_dir)) {
                throw missing_directory(temp_dir.string().c_str());
        }
        BUG_ON(!boost::filesystem::exists(temp_dir));

        boost::filesystem::path work_dir;

        try {
                work_dir = boost::filesystem::path(temp_dir.string() +
                                                   std::string("/") +
                                                   (std::string(PROGRAM_NAME) +
                                                    "-" +
                                                    fcp::itos(getpid())));

                if (boost::filesystem::exists(work_dir)) {
                        // Remove our working directory left from previous run
                        boost::filesystem::remove_all(work_dir);
                }
                boost::filesystem::create_directory(work_dir);
        } catch (...) {
                throw cannot_run((std::string("Cannot create ")      +
                                  std::string("working directory '") +
                                  work_dir.string()                  +
                                  std::string("'")).c_str());
        }
        BUG_ON(!boost::filesystem::exists(work_dir));

        TR_DBG("Transformation(s) = %d\n",   transformations.size());
        TR_DBG("Rules file(s)     = %d\n",   rules_filenames.size());
        TR_DBG("Separator         = '%c'\n", separator);
        TR_DBG("Max depth         = '%d'\n", max_depth);
        TR_DBG("Working directory = '%s'\n", work_dir.string().c_str());

        BUG_ON(transformations.size() <= 0);
        BUG_ON(rules.size() < 0);
        BUG_ON(max_depth <= 0);

        bool retval = false;

        try {
                run(transformations,
                    rules,
                    dump_rules,
                    max_depth,
                    separator,
                    work_dir,
                    force,
                    dry_run);
        } catch (fcp::exception & e) {
                throw cannot_run(e.what());
        }

        try {
                boost::filesystem::remove_all(work_dir);
        } catch (...) {
                // XXX FIXME: We should only warn our user here ...
                throw cannot_run((std::string("Cannot remove ")      +
                                  std::string("working directory '") +
                                  work_dir.string()                  +
                                  std::string("'")).c_str());
        }
}

void hint(const std::string & message)
{
        BUG_ON(message.size() == 0);

        std::cout
                << message
                << std::endl
                << "Try `" << PROGRAM_NAME << " -h' "
                <<"for more information."
                << std::endl;
}

int main(int argc, char * argv[])
{
        TR_CONFIG_PFX(PROGRAM_NAME);
        TR_CONFIG_LVL(TR_LVL_DEFAULT);

        TR_DBG("Program start ...\n");

        int retval = EXIT_FAILURE;
        try {
                program(argc, argv);
                retval = EXIT_SUCCESS;
        } catch (wrong_option & e) {
                hint(e.what());
        } catch (fcp::exception & e) {
                TR_ERR("%s\n", e.what());
        } catch (...) {
                // All unhandled exceptions are bugs for me !
                BUG();
        }

        TR_DBG("Program stop (retval = %d) ...\n", retval);

        return retval;
}
