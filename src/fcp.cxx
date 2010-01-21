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

#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"
#include "utility.h"
#include "exception.h"
#include "rules.h"
#include "transformation.h"

#define PROGRAM_NAME "fcp"

void version()
{
        std::cout
                << PROGRAM_NAME << " (" << PACKAGE_NAME  << ") " << PACKAGE_VERSION <<
                std::endl
                <<
                std::endl
                << "Copyright (C) 2008, 2009 Francesco Salvestrini" <<
                std::endl
                <<
                std::endl
                << "This is free software.  You may redistribute copies of it under the terms of" <<
                std::endl
                << "the GNU General Public License <http://www.gnu.org/licenses/gpl.html>." <<
                std::endl
                << "There is NO WARRANTY, to the extent permitted by law." <<
                std::endl;
}

#define HARM_MY_FILESYSTEM 0

char          separator          = ':';
#define       DFLT_RULES                        \
        (fcp::getenv("HOME") +                  \
         std::string("/") +                     \
         std::string(".") +                     \
         std::string(PACKAGE_TARNAME) +         \
         std::string("/") +                     \
         std::string("rules"))
int           max_depth          = 16;

void help(boost::program_options::options_description & options)
{
        std::cout
                << "Usage: "
                << PROGRAM_NAME
                << " [OPTION]... [TRANSFORMATION]..." <<
                std::endl
                <<
                std::endl
                <<
                options
                <<
                std::endl
                << "Specify TRANSFORMATION using the format:" <<
                std::endl
                <<
                std::endl
                << "  INPUTFILE[%TYPE]<SEPARATOR>OUTPUTFILE[%TYPE]" <<
                std::endl
                <<
                std::endl
                << "Default SEPARATOR is "
                << "'" << separator << "'. "
                << "INPUTFILE and OUTPUTFILE must be different." <<
                std::endl
                << "File TYPE is optional and it will be guessed "
                << "if not provided." <<
                std::endl
                <<
                std::endl
                << "Report bugs to <" << PACKAGE_BUGREPORT << ">" <<
                std::endl;
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

class cant_run : public fcp::exception {
public:
        cant_run(const char * message) :
                fcp::exception(message) { }
};

class wrong_opt : public fcp::exception {
public:
        wrong_opt(const char * message) :
                fcp::exception(message) { }
};

void run(const std::vector<std::string> & tags,
         // XXX FIXME: Change to boost::filesystem::path
         const std::vector<std::string> & rules_filenames,
         bool                             dump_rules,
         int                              max_depth,
         char                             separator,
         const boost::filesystem::path &  work_dir,
         bool                             force,
         bool                             dry_run)
{
        // Read rules file
        fcp::rules rules(rules_filenames);
        if (dump_rules) {
                TR_VRB("Rules:\n");
                std::cout << rules;
                return;
        }

        if (tags.size() == 0) {
                throw cant_run("Missing transformation(s)");
        }

#if 0
        TR_VRB("Read %d rule%c\n",
               rules.size(),
               (rules.size() > 1) ? 's' : ' ');
#endif

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

int program(int argc, char * argv[])
{
        namespace bpo = boost::program_options;

        int retval = 1;

        try {
                bool                     dry_run       = false;
                bool                     force         = false;
                bool                     dump_rules    = false;

                std::vector<std::string> transformations_tags;
                std::vector<std::string> rules_all;
                std::vector<std::string> rules_default;
                std::vector<std::string> rules_user;

                std::string              temp_dir_name;

                // Push default rules
                rules_default.push_back(DFLT_RULES);

                TR_DBG("Parsing program options\n");

                // Main options
                bpo::options_description main_options("Options");

                main_options.add_options()
                        ("config,c",
                         bpo::value<std::string>(),
                         "use alternate configuration file")
                        ("rules,r",
                         bpo::value<std::string>(),
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
                try {
                        bpo::store(bpo::command_line_parser(argc, argv).options(all_options).positional(positional_options).run(),
                                                      vm);
                } catch (bpo::error & e) {
                        throw wrong_opt(e.what());
                }
                bpo::notify(vm);

                // Check options
                if (vm.count("rules")) {
                        rules_user.push_back(vm["rules"].as<std::string>());
                }

                if (vm.count("max-depth")) {
                        max_depth = vm["max-depth"].as<int>();
                }
                if (max_depth < 1) {
                        hint("Wrong max-depth");
                        return 1;
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

                if (vm.count("debug")) {
                        TR_CONFIG_LVL(TR_LVL_DEBUG);
                }

                if (vm.count("verbose")) {
                        TR_CONFIG_LVL(TR_LVL_VERBOSE);
                }

                if (vm.count("version")) {
                        version();
                        return 0;
                }
                if (vm.count("help")) {
                        // Passing only main options
                        help(main_options);
                        return 0;
                }

                if (vm.count("transformation")) {
                        transformations_tags = vm["transformation"].as<std::vector<std::string> >();
                }

                TR_DBG("Option parsing complete\n");

                TR_DBG("Tags      '%d'\n", transformations_tags.size());
                TR_DBG("Separator '%c'\n", separator);
                TR_DBG("Max depth '%d'\n", max_depth);
                BUG_ON(max_depth <= 0);

                // Insert default rules
                rules_all.insert(rules_all.end(),
                                 rules_default.begin(),
                                 rules_default.end());
                // Insert user rules
                rules_all.insert(rules_all.end(),
                                 rules_user.begin(),
                                 rules_user.end());

                if (rules_all.size() == 0) {
                        // Hmmmm is this a real error ?
                        hint("No rules available");
                        return 1;
                }

                TR_DBG("You have %d rule%c\n",
                       rules_all.size(),
                       (rules_all.size() > 1 ? 's' : ' '));
                BUG_ON(rules_all.size() == 0);

                // Setup temporary directory
                if (temp_dir_name.empty()) {
                        temp_dir_name = std::string("/tmp/");
                }
                BUG_ON(temp_dir_name.empty());

                boost::filesystem::path temp_dir(temp_dir_name +
                                                 std::string("/") +
                                                 (std::string(PROGRAM_NAME) +
                                                  "-" +
                                                  fcp::itos(getpid())));

                bool remove_temp_dir = false;
                if (!boost::filesystem::exists(temp_dir)) {
                        boost::filesystem::create_directory(temp_dir);
                        remove_temp_dir = true;
                }
                BUG_ON(!boost::filesystem::exists(temp_dir));

                TR_DBG("Temporary dir '%s' (%s)\n",
                       temp_dir.string().c_str(),
                       remove_temp_dir ? "remove" : "keep");

                // Setup working directory (inside temp directory)
                boost::filesystem::path work_dir(temp_dir_name +
                                                 std::string("/") +
                                                 (std::string(PROGRAM_NAME) +
                                                  "-" +
                                                  fcp::itos(getpid())));
                if (boost::filesystem::exists(work_dir)) {
                        // Remove our working directory left from previous run
#if HARM_MY_FILESYSTEM
                        boost::filesystem::remove_all(work_dir);
#else
                        TR_DBG("Avoiding '%s' directory removal\n",
                               work_dir.string().c_str());
#endif
                }
                boost::filesystem::create_directory(work_dir);
                BUG_ON(!boost::filesystem::exists(work_dir));
                TR_DBG("Working dir '%s' (%s)\n", work_dir.string().c_str());

                run(transformations_tags,
                    rules_all,
                    dump_rules,
                    max_depth,
                    separator,
                    work_dir,
                    force,
                    dry_run);

                // Remove our directories
#if HARM_MY_FILESYSTEM
                boost::filesystem::remove_all(work_dir);
#else
                TR_DBG("Avoiding '%s' directory removal\n",
                       work_dir.string().c_str());
#endif
                if (remove_temp_dir) {
#if HARM_MY_FILESYSTEM
                        boost::filesystem::remove_all(temp_dir);
#else
                        TR_DBG("Avoiding '%s' directory removal\n",
                               temp_dir.string().c_str());
#endif
                }

                // Everything went smoothly ...
                retval = 0;
        } catch (cant_run & e) {
                hint(e.what());
        } catch (wrong_opt & e) {
                hint(e.what());
        } catch (fcp::exception & e) {
                TR_ERR("%s\n", e.what());
        }

        return retval;
}

int main(int argc, char * argv[])
{
        TR_CONFIG_PFX(PROGRAM_NAME);
        TR_CONFIG_LVL(TR_LVL_DEFAULT);

        try {
                return program(argc, argv);
        } catch (...) {
                // All unhandled exceptions are bugs for me !
                BUG();
        }

        return 1; // useless
}
