/*
 * opencog/generate/CollectStyle.cc
 *
 * Copyright (C) 2020 Linas Vepstas <linasvepstas@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <opencog/atoms/base/Link.h>

#include "CollectStyle.h"

using namespace opencog;

CollectStyle::CollectStyle(void)
{
}

CollectStyle::~CollectStyle() {}

/// In this "style" of recording a result, we just tack it onto
/// a C++ container holding the solutions. Other "styles" are
/// possible; we could report them elsewehre, too...
void CollectStyle::record_solution(const OdoFrame& frm)
{
	size_t nsolns = _solutions.size();
	_solutions.insert(frm._linkage);
	size_t news = _solutions.size();
	logger().fine("====================================");
	if (nsolns != news)
	{
		logger().fine("Obtained new solution %lu of size %lu:",
		       news, frm._linkage.size());
		for (const Handle& lkg : frm._linkage)
			frm.print_section(lkg);
	}
	else
	{
		logger().fine("Rediscovered solution, still have %lu size=%lu",
		        news, frm._linkage.size());
	}
	logger().fine("====================================");
}

/// XXX FIXME ... maybe should attach to a MemberLink or something?
/// This obviously fails to scale if the section is large.
/// That would be considered to be a different "style" and would
/// almost surely be better than using SetLinks...
Handle CollectStyle::get_solutions(void)
{
	HandleSeq solns;
	for (const auto& sol : _solutions)
	{
		HandleSeq sects;
		for (const Handle& sect : sol)
			sects.push_back(sect);

		solns.push_back(createLink(std::move(sects), SET_LINK));
	}
	return createLink(std::move(solns), SET_LINK);
}
