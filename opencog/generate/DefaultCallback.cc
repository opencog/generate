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

DefaultCallback::DefaultCallback(AtomSpace* as, const HandlePairSeq& pp)
	: GenerateCallback(as), _as(as),
	_stack_depth(0), _effort(0),
	_pole_pairs(pp)
{
}

DefaultCallback::~DefaultCallback() {}

/// add_to_lexis() - Declare a section as valid for use in assembly.
/// Basically, only those sections that have been declared to be a part
/// of the lexis will be used during assembly/aggregation.
///
void DefaultCallback::add_to_lexis(const Handle& sect)
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
		sect_list.push_back(con);
		_lexis[con] = sect_list;
	}
}

// We are storing pole_pairs in a specialized struct in this class.
// This is arguably a deasign failure ... we should be storing the
// pole pairs in the AtomSpace, as EvaluationLinks with Predicate
// "pole_pair", and then using the AtomSpace API to access...
// Just sayin...
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

static inline Handle uniform_choice(const HandleSeq& lst)
{
	static std::random_device seed;
	static std::mt19937 rangen(seed());
	std::uniform_int_distribution<> dist(0, lst.size()-1);
	int sno = dist(rangen);
	return lst[sno];
}

/// Return section containing `to_con`.
Handle DefaultCallback::select(const Frame& frame,
                               const Handle& fm_sect, const Handle& fm_con,
                               const Handle& to_con)
{
	// OK, a bunch of temp hackery here.  First, we are going to randomly
	// draw elibible sections. Two reasons: (1) the long term strategy
	// is to use some guided, weighted random draw anyway. (2) we don't
	// have any way of maintaining state, because there is no hook to
	// maintain our private state in class Frame. XXX FIXME.

	HandleSeq to_seqs = to_con->getIncomingSetByType(CONNECTOR_SEQ);
	if (0 == to_seqs.size()) return Handle::UNDEFINED;

	Handle to_seq = uniform_choice(to_seqs);
	HandleSeq to_sects = to_seq->getIncomingSetByType(SECTION);
	if (0 == to_sects.size()) return Handle::UNDEFINED;
	Handle to_sect = uniform_choice(to_sects);

#if 0
	Handle fm_point = fm_sect->getOutgoingAtom(0);
	Handle to_point = to_sect->getOutgoingAtom(0);
	Handle cpr = _as->get_link(SET_LINK, fm_point, to_point);

	// If above list doesn't exist, no connection has been made before.
	if (nullptr == cpr) return true;

	Handle linkty = fm_con->getOutgoingAtom(0);
	Handle link = _as->get_link(EVALUATION_LINK, linkty, cpr);
	if (nullptr == link) return false;
#endif

	return to_sect;
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
