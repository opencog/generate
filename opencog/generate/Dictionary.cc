/*
 * opencog/generate/Dictionary.cc
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

#include "Dictionary.h"

using namespace opencog;

Dictionary::Dictionary(AtomSpace* as)
	: _as(as)
{}

void Dictionary::add_pole_pair(const Handle& fm_pole,
                               const Handle& to_pole)
{
	_pole_pairs.push_back({fm_pole, to_pole});
}

/// add_to_lexis() - Declare a section as valid for use in assembly.
/// Basically, only those sections that have been declared to be a part
/// of the lexis will be used during assembly/aggregation.
///
void Dictionary::add_to_lexis(const Handle& sect)
{
	// We are storing lexical entries in a specialized struct in this
	// class.  This is arguably a deasign failure ... we should be
	// storing the lexical entries  in the AtomSpace, as EvaluationLinks
	// with Predicate "lexis", and then using the AtomSpace API to
	// access... Just sayin...
	//
	// Anyway, this is a lookup table: given a connector, we can lookup
	// a list of sections that have that connector in it. We're using
	// a sequence, not a set, for two reasons: (1) fast random lookup,
	// the sequence can be ordered in priority order (e.g. Zipfian)
	// which is not done here, but could be. XXX FIXME.
	//
	Handle con_seq = sect->getOutgoingAtom(1);
	for (const Handle& con : con_seq->getOutgoingSet())
	{
		HandleSeq sect_list = _lexis[con];
		sect_list.push_back(sect);
		_lexis[con] = sect_list;
	}
}

/// Given the Connector `from_con`, return a list of Connectors
/// that it can attach to.
HandleSeq Dictionary::joints(const Handle& from_con)
{
	HandleSeq phs;

	// Nothing to do, if not a connector.
	// XXX FIXME. I think we should be asserting here.
	// Or at least throwing.
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
