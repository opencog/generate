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
	_frame_stack_depth(0), _effort(0)
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
/// First try to attach to an existing open section.
/// If that fails, then pick a new section from the lexis.
Handle DefaultCallback::select(const Frame& frame,
                               const Handle& fm_sect, const Handle& fm_con,
                               const Handle& to_con)
{
	// Do we have an iterator (a future/promise) for the to-connector
	// in the current frame?  If not, try to set one up. If this fails,
	// then kick over to the dictionary.
	unsigned fit = _frameit.get(to_con, 0);
	if (0 == fit)
	{
		HandleSeq to_sects;
		for (const Handle& open_sect : frame._open_sections)
		{
			const Handle& conseq = open_sect->getOutgoingAtom(1);
			for (const Handle& con : conseq->getOutgoingSet())
			{
				if (con == to_con) to_sects.push_back(open_sect);
			}
		}

		// Start iterating over the sections that contain to_con.
		if (0 < to_sects.size())
		{
			_frameit[to_con] = 1;
			return to_sects[0];
		}
		// else fall-through here, and go to the lexis-lookup of the
		// connector.
	}
	else
	{
#if 0
		if (toit == _dict.sections(to_con).end())
		{
			// We've iterated to the end; we're done.
			_lexlit.erase(to_con);
			return Handle::UNDEFINED;
		}

		// Increment and save.
		Handle to_sect = *toit;
		toit++;
		_frameit[to_con] = toit;
		return to_sect;
#endif
	}

	const HandleSeq& to_sects = _dict.sections(to_con);

	// Do we have an iterator (a future/promise) for the to-connector?
	// If not, then set one up. Else use the one we found.  The iterator
	// that we are setting up here will point into the dictionary, i.e.
	// into the pool of allowable sections that we can pick from.
	unsigned curit = _lexlit.get(to_con, 0);
	if (0 == curit)
	{
		// Oh no, dead end!
		if (0 == to_sects.size()) return Handle::UNDEFINED;

		// Start it up.
		_lexlit[to_con] = 1;
		return to_sects[0];
	}

	if (to_sects.size() <= curit)
	{
		// We've iterated to the end; we're done.
		_lexlit.erase(to_con);
		return Handle::UNDEFINED;
	}

	// Increment and save.
	_lexlit[to_con] ++;
	return to_sects[curit];

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

/// 100% pure hackalicious junkola. XXX FIXME.
bool DefaultCallback::recurse(const Frame& frm)
{
	_effort ++;
	if (12123 < _effort) return false;

	// Pick 20 as an arbitrary choice. XXX FIXME - why?
	// Well, 2^20 is a big number, and 10^20 is even bigger. Ouch.
	if (_frame_stack_depth < 20) return true;

	// Well, we could also randomly continue half the time ...!?
	// std rand uniform distribution ...
	return false;
}

void DefaultCallback::push_frame(const Frame& frm)
{
	_frame_stack_depth++;
}

void DefaultCallback::pop_frame(const Frame& frm)
{
	_frame_stack_depth--;
}

void DefaultCallback::push_odometer(const Odometer& odo)
{
	_lexlit_stack.push(_lexlit);
	_lexlit.clear();
}

void DefaultCallback::pop_odometer(const Odometer& odo)
{
	_lexlit = _lexlit_stack.top(); _lexlit_stack.pop();
}
