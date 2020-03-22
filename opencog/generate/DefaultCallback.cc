/*
 * opencog/generate/DefaultCallback.cc
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

#include <random>

#include <opencog/atoms/base/Link.h>

#include "DefaultCallback.h"

using namespace opencog;

DefaultCallback::DefaultCallback(AtomSpace* as, const Dictionary& dict)
	: GenerateCallback(as), _as(as), _dict(dict),
	_stack_depth(0), _effort(0)
{
}

DefaultCallback::~DefaultCallback() {}

static inline Handle uniform_choice(const HandleSeq& lst)
{
	static std::random_device seed;
	static std::mt19937 rangen(seed());
	std::uniform_int_distribution<> dist(0, lst.size()-1);
	int sno = dist(rangen);
	return lst[sno];
}

/// Return a section containing `to_con`.
Handle DefaultCallback::select(const Frame& frame,
                               const Handle& fm_sect, const Handle& fm_con,
                               const Handle& to_con)
{
#if 0
	// If `close_cycle` is true, then attempt to connect to
	// an existing open section (thus potentially creating a
	// cycle or loop).
	if (close_cycle)
	{
		if (_frame._open_sections.end() ==
		    _frame._open_sections.find(to_sect)) continue;
	}
	else
	{
		if (_frame._open_sections.end() !=
		    _frame._open_sections.find(to_sect)) continue;
	}
#endif

	const auto& curit = _lexlit.find(to_con);
	if (curit != _lexlit.end())
	{
		// The iterator is point somewhere into _lexis[to_con]
		HandleSeq::const_iterator toit = curit->second;
		if (toit != _dict.sections(to_con).end())
		{
			// Increment and save.
			Handle to_sect = *toit;
			toit++;
			_lexlit[to_con] = toit;
			return to_sect;
		}
		else
		{
			// We've iterated to the end; we're done.
			_lexlit.erase(to_con);
			return Handle::UNDEFINED;
		}
	}

	const HandleSeq& to_sects = _dict.sections(to_con);
	if (0 == to_sects.size()) return Handle::UNDEFINED;

	// Start iterating over the sections that contain to_con.
	HandleSeq::const_iterator toit = to_sects.begin();

	// Increment and save.
	Handle to_sect = *toit;
	toit++;
	_lexlit[to_con] = toit;
	return to_sect;

#if 0
	// Randomly draw elegible sections. The long term strategy
	// is to use some guided, weighted random draw anyway.
	Handle to_sect = uniform_choice(its->second);
#endif

#if 0
	// Avoid retrying previously-tried connections...
	Handle fm_point = fm_sect->getOutgoingAtom(0);
	Handle to_point = to_sect->getOutgoingAtom(0);
	Handle cpr = _as->get_link(SET_LINK, fm_point, to_point);

	// If above list doesn't exist, no connection has been made before.
	if (nullptr == cpr) return true;

	Handle linkty = fm_con->getOutgoingAtom(0);
	Handle link = _as->get_link(EVALUATION_LINK, linkty, cpr);
	if (nullptr == link) return false;
#endif
}

/// Create an undirected edge connecting the two points `fm_pnt` and
/// `to_pnt`, using the connectors `fm_con` and `to_con`.
Handle DefaultCallback::make_link(const Handle& fm_con,
                                  const Handle& to_con,
                                  const Handle& fm_pnt,
                                  const Handle& to_pnt)
{
	Handle linkty = fm_con->getOutgoingAtom(0);
	Handle edg = _as->add_link(SET_LINK, fm_pnt, to_pnt);
	Handle lnk = _as->add_link(EVALUATION_LINK, linkty, edg);
	return lnk;
}

bool DefaultCallback::recurse(const Frame& frm)
{
	_effort ++;
	if (12123 < _effort) return false;

	// Pick 20 as an arbitrary choice. XXX FIXME - why?
	// Well, 2^20 is a big number, and 10^20 is even bigger. Ouch.
	if (_stack_depth < 20) return true;

	// Well, we could also randomly continue half the time ...!?
	// std rand uniform distribution ...
	return false;
}
