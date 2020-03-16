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
	: GenerateCallback(as), _stack_depth(0),
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

bool DefaultCallback::recurse(const Frame& frm)
{
	// Pick 50 as an arbitrary choice. XXX FIXME - why?
	if (_stack_depth < 50) return true;

	// Well, we could also randomly continue half the time ...!?
	// std rand uniform distribution ...
	return false;
}
