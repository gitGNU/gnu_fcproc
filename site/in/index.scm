(h3 "DESCRIPTION")
(p "Filter Chains Processor (FCP) is a small command line utility that "
   "handles parallel files transformations, automatically building filter "
   "chains in order to transform each input file from its format to the "
   "desired output format.")
(p "Each filter chain is computed dinamically, using external tools (defined "
   "by the user in a configuration file) or internal functions.")

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
   (a (@ (href "http://lists.nongnu.org/mailman/listinfo/fcp-generic"))
      "mailing list page")
   " for more information on subscribing to the mailing list.")

(h3 "REPORT A BUG")
(p "If you think you have found a bug then please send as complete a report "
   "as possible to "
   "<fcp-generic AT nongnu DOT org>. "
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
   (a (@ (href "http://git.savannah.nongnu.org/gitweb/?p=fcp.git"))
      "Git repository")
   " of this project with your web browser. This gives you a good "
   "picture of the current status of the source files. You may also view "
   "the complete histories of any file in the repository as well as "
   "differences among two versions.")

(h4 "Getting a copy of the Git Repository")
(p "Anonymous checkout: "
   (p (@ class "terminal")
      (a (@ (href "http://savannah.gnu.org/maintenance/UsingGit")) "git")
      " clone git://git.savannah.nongnu.org/fcp.git"))

(h4 "Contribute")
(p "If you have time and programming skills, you can help us by developing "
   "missing features, regression tests or bug-fixing the present codebase. "
   "Subscribe to the "
   (a (@ (href "http://lists.nongnu.org/mailman/listinfo/fcp-generic"))
      "mailing list")
   ", drop us a mail and start coding. Send your code to the "
   "mailing list under the form of patches for the current revision system."
   (br)
   (br)
   "If you have time but no programming skills, you can help with "
   "documentation,  packaging, tests before releases etc ...")
