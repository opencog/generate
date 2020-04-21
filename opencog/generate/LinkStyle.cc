/*
 * opencog/generate/LinkStyle.cc
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
#include <uuid/uuid.h>

#include <opencog/util/oc_assert.h>
#include <opencog/atoms/base/Link.h>

#include "LinkStyle.h"

using namespace opencog;

LinkStyle::LinkStyle(void) : _scratch(nullptr)
{
}

/// Given a generic section, create a unique instance of it.
/// As "puzzle pieces" are assembled, each new usage represents a
/// "different location" in the puzzle, and so we create a unique
/// instance for each location.
///
/// This assumes that the section point is a node, so that we
/// generate a unique string for that node.
Handle LinkStyle::create_unique_section(const Handle& sect)
{
	uuid_t uu;
	uuid_generate(uu);
	char idstr[37];
	uuid_unparse(uu, idstr);

	Handle point = sect->getOutgoingAtom(0);
	Handle disj = sect->getOutgoingAtom(1);

	if (not point->is_node())
		throw RuntimeException(TRACE_INFO,
			"Expection a Node for the section point, got %s",
				point->to_string().c_str());

	// Create a unique point.
	Handle upoint(_scratch->add_node(point->get_type(),
	              point->get_name() + "@" + idstr));

	// Record the point location.
	if (_point_set)
		_mempoints.emplace_back(createLink(MEMBER_LINK, upoint, _point_set));

	// Create a unique instance of the section.
	Handle usect(_scratch->add_link(SECTION, upoint, disj));

	// Record it's original type.
	// _inhsects.emplace_back(createLink(INHERITANCE_LINK, upoint, sect));

	return usect;
}

/// Create an undirected edge connecting the two points `fm_pnt` and
/// `to_pnt`, using the connectors `fm_con` and `to_con`. The edge
/// is "undirected" because a SetLink is used to hold the two
/// end-points. Recall SetLinks are unordered links, so neither point
/// can be identified as head or tail.
Handle LinkStyle::create_undirected_link(const Handle& fm_con,
                                         const Handle& to_con,
                                         const Handle& fm_pnt,
                                         const Handle& to_pnt)
{
	// Create the actual link to use.
	Handle linkty = fm_con->getOutgoingAtom(0);
	Handle edg = _scratch->add_link(SET_LINK, fm_pnt, to_pnt);
	Handle lnk = _scratch->add_link(EVALUATION_LINK, linkty, edg);
	return lnk;
}

/// Return a count of the number of links, of type `link_type`,
/// connecting the two sections. Returns zero if they are not nearest
/// neighbors, otherwise return a count.
size_t LinkStyle::num_undirected_links(const Handle& fm_sect,
                                       const Handle& to_sect,
                                       const Handle& link_type)
{
	const Handle& fm_pnt = fm_sect->getOutgoingAtom(0);
	const Handle& to_pnt = to_sect->getOutgoingAtom(0);
	Handle edg = _scratch->get_link(SET_LINK, fm_pnt, to_pnt);
	if (nullptr == edg) return 0;
	Handle lnk = _scratch->get_link(EVALUATION_LINK, link_type, edg);
	if (nullptr == lnk) return 0;

	const Handle& disj = fm_sect->getOutgoingAtom(1);
	size_t count = 0;
	for (const Handle& exli : disj->getOutgoingSet())
	{
		if (*exli == *lnk) count++;
	}

	// XXX FIXME -- undef this and save some CPU...
#define SELF_TEST
#ifdef SELF_TEST
	// Count the number of links, starting with to_sect.
	// The result should be the same. This is a waste of CPU time,
	// if everything is working correctly.
	const Handle& tdis = to_sect->getOutgoingAtom(1);
	size_t tcnt = 0;
	for (const Handle& exli : tdis->getOutgoingSet())
	{
		if (*exli == *lnk) tcnt++;
	}
	OC_ASSERT(count == tcnt, "Not internally self-consistent!");
#endif

	return count;
}

/// Return a count of the number of any kind of links (no matter what
/// the type), connecting the two sections. Returns zero if they are
/// not nearest neighbors, otherwise return a count. Actually, as
/// currently written, this does assume the links are undirected.
size_t LinkStyle::num_any_links(const Handle& fm_sect,
                                const Handle& to_sect)
{
	const Handle& fm_pnt = fm_sect->getOutgoingAtom(0);
	const Handle& to_pnt = to_sect->getOutgoingAtom(0);

	const Handle& fm_disj = fm_sect->getOutgoingAtom(1);
	const Handle& to_disj = to_sect->getOutgoingAtom(1);

	size_t cnt = 0;
	for (const Handle& lnk: fm_disj->getOutgoingSet())
	{
		if (CONNECTOR == lnk->get_type()) continue;

		HandleSeq djs = lnk->getIncomingSetByType(CONNECTOR_SEQ);
		for (const Handle& tdj : djs)
		{
			if (*tdj == *to_disj) cnt++;
		}
	}

	return cnt;
}

void LinkStyle::clear(void)
{
	_mempoints.clear();
	_inhsects.clear();
}

void LinkStyle::save_work(AtomSpace* as)
{
	for (const Handle& h : _mempoints)
	{
		Handle pt(as->add_atom(h));

#if FINISH_ME_LATER
		// Link tying together the fully-connected piece, and the
		// puzzle piece it came from.
		IncomingSet iset = pt->get_incoming_by_type(SECTION, as);
		OC_ASSERT(1 == iset.size(), "Wrong number of sections");
		as
#endif
	}
}
