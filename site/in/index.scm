(h3 "DESCRIPTION")
(p "Filter Chains Processor (FCP) is a command line utility that "
   "handles multiple files transformations, automatically building filter "
   "chains in order to transform each input file from its format to the "
   "desired output format.")
(p "Each filter chain is computed dinamically, using external tools (defined "
   "by the user in a configuration file).")

(h3 "USAGE")
(h4 "Installation")
(p "FCP uses the autoconf/automake facilities. The simplest way to install "
   "the package in your system is:")
(ul
 (li (kbd "cd") " to the directory containing the package's source code "
     "and type " (kbd "./configure") " to configure the package for your "
     "system."
     "Running " (kbd "configure") " might take a while. While running, "
     "it prints some messages telling which features it is checking for")
 (li "Type " (kbd "make") " to compile the package")
 (li "Optionally, type " (kbd "make check") " to run any self-tests that "
     "come with the package")
 (li "Type " (kbd "make install") " to install the programs and any data "
     "files and documentation"))

(p "Use " (kbd "./configure --help") " to get a brief help of all "
   "configuration tweaks available."
   (br)
   (br)
   "You can remove the program binaries and object files from the "
   "source code directory by typing " (kbd "make clean") ". "
   "To also remove the files that " (samp "configure") " created (so you "
   "can compile the package for a different architecture), type "
   (kbd "make distclean") "."
   (br)
   (br)
   "You can also type " (kbd "make uninstall") " to remove the installed "
   "files."
   (br)
   (br)
   "Note that the " (samp "--prefix") " option allows installation using "
   "different paths than the default ones, this option enables sandbox "
   "installations.")

(h4 "Use")
(p "")

(pre (@ class "terminal")
     "

	Usage: fcp [OPTION]... [TRANSFORMATION]...

	Options:
	  -r, --rules=FILE	  use alternate rules file
				  [default $HOME/.fcp/rules]
	  -m, --max-depth=NUM	  use NUM as max filter-chains depth
				  [default 16]
	  -t, --temp-dir=DIR	  use DIR as temporary directory
				  [default $HOME/.fcp/cache]
	  -s, --separator=CHAR	  use CHAR as INPUTFILE/OUTPUTFILE separator
				  [default `:']
	  -q, --no-rules	  do not load initial rules
	  -n, --dry-run		  display commands without modifying any files
	  -d, --debug		  enable debugging traces
	  -v, --verbose		  verbosely report processing
	  -h, --help		  print this help, then exit
	  -V, --version		  print version number, then exit

")

(h5 "Options:")
(p "Although some (maybe) sane default values are hardwired into the "
   "program, the following options allow overriding:")

(ul
 (li (kbd "-r") ", " (kbd "--rules")       ": "
     "Specify a different rules file")
 (li (kbd "-m") ", " (kbd "--max-depth")   ": "
     "Specify maximum filter chains length")
 (li (kbd "-r") ", " (kbd "--temp-dir")    ": "
     "Specify a temporary directory")
 (li (kbd "-s") ", " (kbd "--separator")   ": "
     "Overrides transformation separator")
 (li (kbd "-q") ", " (kbd "--no-rules")   ": "
     "Do not load initial rules")
 (li (kbd "-n") ", " (kbd "--dry-run")     ": "
     "Run the program without modifyng any files")
 )

(h4 "Rules file format:")
(p "")

(h3 "COPYING")
(p "FCP is licensed under the "
   (a (@ (href "http://www.gnu.org/licenses/licenses.html"))
      "GNU General Public License, version 2"))

(h3 "MAINTAINERS")
(p "Francesco Salvestrini <salvestrini AT gmail DOT com>")

(h3 "AUTHORS")
(p "Francesco Salvestrini <salvestrini AT gmail DOT com>")

(h3 "RELEASES")
(p "Sorry, no public release available at the moment.")

(h3 "MAILING LISTS")
(p "The project has a single moderated mailing list, with an archive. "
   "In order to post a message to the mailing list you must be subscribed. "
   "Please consult the "
   (a (@ (href "http://lists.nongnu.org/mailman/listinfo/fcproc-generic"))
      "mailing list page")
   " for more information on subscribing to the mailing list.")

(h3 "REPORT A BUG")
(p "If you think you have found a bug then please send as complete a report "
   "as possible to "
   "<fcproc-generic AT nongnu DOT org>. "
   "An easy way to collect all the required information, such as platform and "
   "compiler, is to include in your report the config.log file available at "
   "the end of the configuration procedure. "
   (br)
   (br)
   "If you have a patch for a bug that hasn't yet been fixed in "
   "the latest repository sources, please be so kind to create it using the "
   "repository sources, not the release sources.")

(h3 "DEVELOPMENT")
(h4 "Browsing sources")
(p "You can browse the "
   (a (@ (href "http://git.savannah.nongnu.org/gitweb/?p=fcproc.git"))
      "Git repository")
   " of this project with your web browser. This gives you a good "
   "picture of the current status of the source files. You may also view "
   "the complete histories of any file in the repository as well as "
   "differences among two versions.")

(h4 "Getting a copy of the Git Repository")
(p "Anonymous checkout: ")
(pre (@ class "terminal")
     (a (@ (href "http://savannah.gnu.org/maintenance/UsingGit")) "git")
     " clone git://git.savannah.nongnu.org/fcproc.git")

(h4 "Contribute")
(p "If you have time and programming skills, you can help us by developing "
   "missing features, regression tests or bug-fixing the present codebase. "
   "Subscribe to the "
   (a (@ (href "http://lists.nongnu.org/mailman/listinfo/fcproc-generic"))
      "mailing list")
   ", drop us a mail and start coding. Send your code to the "
   "mailing list under the form of patches for the current revision system."
   (br)
   (br)
   "If you have time but no programming skills, you can help with "
   "documentation,  packaging, tests before releases etc ...")
