//
// Copyright (C) 2007, 2008 Francesco Salvestrini
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

#include "libs/misc/debug.h"
#include "libs/misc/exception.h"
#include "transformation.h"
#include "rules.h"
#include "chain.h"

namespace FCP {
	Transformation::Transformation(const std::string & tag,
				       char                separator,
				       FCP::Rules &        rules,
				       int                 mdepth) :
		tag_(tag)
	{
		std::string::size_type p;

		// Split the tag into input and output part
		p = tag_.find(separator);
		if ((p < 0) || (p > tag_.size())) {
			throw Exception("Missing separator "
					"in transformation "
					"'" + tag_ + "'");
		}

		std::string tmp;

		tmp = tag_.substr(0, p);
		if (tmp.size() == 0) {
			throw Exception("Missing input file "
					"in transformation "
					"'" + tag_ + "'");
		}
		input_ = new FCP::File(tmp);
		BUG_ON(input_ == 0);

		tmp = tag_.substr(p + 1);
		if (tmp.size() == 0) {
			throw Exception("Missing output file "
					"in transformation "
					"'" + tag_ + "'");
		}
		output_ = new FCP::File(tmp);
		BUG_ON(output_ == 0);

		// Build the filters-chain for this transformation
		std::vector<FCP::Filter *> chain;
		rules.chains(input_->extension(),
			     output_->extension(),
			     mdepth,
			     chain);
		if (chain.size() == 0) {
			throw Exception("No filters-chain available for "
					"'" + tag_ + "' "
					"transformation");
		}

		TR_DBG("Filters-chain for transformation '%s':\n",
		       tag_.c_str());
		std::vector<FCP::Filter *>::iterator iter;
		for (iter = chain.begin(); iter != chain.end(); iter++) {
			TR_DBG("  '%s' -> '%s'\n",
			       (*iter)->input().c_str(),
			       (*iter)->output().c_str());
		}

		// Finally create the filters-chain from the filters sequence
		chain_ = new FCP::Chain(tag_, input(), chain, output());
		BUG_ON(chain_ == 0);
	}

	Transformation::~Transformation(void)
	{
		delete input_;
		delete output_;
		delete chain_;
	}

	void Transformation::run(const std::string & tmp_dir,
				 bool                dry_run,
				 bool                force)
	{
		TR_DBG("Transforming '%s' -> '%s'\n",
		       input_->name().c_str(),
		       output_->name().c_str());

		chain_->run(tmp_dir, dry_run, force);
	}

	const std::string & Transformation::tag(void) const
	{
		return tag_;
	}

	const FCP::File & Transformation::input(void) const
	{
		return *input_;
	}

	const FCP::File & Transformation::output(void) const
	{
		return *output_;
	}
};
