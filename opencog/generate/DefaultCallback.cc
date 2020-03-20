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

#include <opencog/atoms/base/Link.h>

#include "DefaultCallback.h"

using namespace opencog;

DefaultCallback::DefaultCallback(AtomSpace* as, const HandlePairSeq& pp)
	: GenerateCallback(as),
	_stack_depth(0), _effort(0),
	_as(as), _pole_pairs(pp)
{
}

DefaultCallback::~DefaultCallback() {}

HandleSeq DefaultCallback::joints(const Handle& from_con)
{
	HandleSeq phs;

	// Nothing to do, if not a connector.
	if (CONNECTOR != from_con->get_type()) return phs;

	// Assume only one pole per connector.
	Handle from_pole = from_con->getOutgoingAtom(1);
	Handle to_pole;
	for (const HandlePair& popr: _pole_pairs)
		if (from_pole == popr.first) { to_pole = popr.second; break; }

	// A matching pole was not found.
	if (!to_pole) return phs;

	// Link type of the desired link to make...
	Handle linkty = from_con->getOutgoingAtom(0);

	// Find appropriate connector, if it exists.
	Handle matching = _as->get_atom(createLink(CONNECTOR, linkty, to_pole));
	if (matching)
		phs.push_back(matching);

	return phs;
}

/// Return false if the connection should not be made.
/// Return true if it is OK to connect these sections.
bool DefaultCallback::connect(const Frame& frame,
                              const Handle& fm_sect, const Handle& fm_con,
                              const Handle& to_sect, const Handle& to_con)
{
	// Cycle breaking. The idea here is that any given attempted
	// assembly of pieces will in general have cycles (loops) in
	// them. A tree traversal means that these loops will be walked
	// in every possible permutation, leading to vast amounts of
	// repeated rediscoveries of previously attempted connections.
	// There's two solutions to this: (1) don't do tree traversal.
	// (2) do the cheap hack below. The cheap hack is that previous
	// connection attempts will still be sitting around in the
	// AtomSpace, so if we find that these two sections are already
	// connected (due to some earlier attempt) just return false,
	// and don't try again.

	Handle fm_point = fm_sect->getOutgoingAtom(0);
	Handle to_point = to_sect->getOutgoingAtom(0);
	Handle linkty = fm_con->getOutgoingAtom(0);
	Handle cpr = _as->get_link(LIST_LINK, linkty, fm_point, to_point);

	// If above list doesn't exist, no connection has been made before.
	if (nullptr == cpr) return true;

	Handle link = _as->get_link(EVALUATION_LINK, _cpred, cpr);
	if (nullptr == link) return false;

	return true;
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
