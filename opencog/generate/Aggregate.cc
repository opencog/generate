/*
 * opencog/generate/Aggregate.cc
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

#include <stdio.h>

#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atomspace/AtomSpace.h>

#include "Aggregate.h"

using namespace opencog;

// Strategy: starting from a single nucleation center (e.g. the left
// wall), recursively aggregate connections until there are no
// unconnected connectors.

Aggregate::Aggregate(AtomSpace* as)
	: _as(as)
{
}

Aggregate::~Aggregate()
{
}

/// The nuclei are the nucleation points: points that must
/// appear in sections, some section of which must be linkable.
///
/// pol_pairs is a list of polarization pairs, i.e.
/// match pairs of ConnectorDir pairs (from, to) which
/// are to be connected.
Handle Aggregate::aggregate(const HandleSet& nuclei,
                            const HandlePairSeq& pole_pairs)
{
	_open_points = nuclei;
	_pole_pairs = pole_pairs;

	extend_point();
	return Handle::UNDEFINED;
}

// Return value of true means halt, no more solutions possible.
// Return value of false means there's more.
bool Aggregate::extend_point(void)
{
	// If there are no more points, we are done.
	if (0 == _open_points.size()) return true;

	// Pick a point, any point.
	// XXX TODO replace this by a heuristic of some kind.
	Handle nucleus = *_open_points.begin();

	HandleSeq sections = nucleus->getIncomingSetByType(SECTION);

	// If there are no sections, then this point is not extendable.
	// This is actually an error condition, I guess?
	if (0 == sections.size())
		throw RuntimeException(TRACE_INFO, "Can't find sections!");

	// Each section is a branch point that has to be explored on
	// it's own.
	for (const Handle& sect : sections)
	{
		extend_section(sect);
	}

	printf("done for now\n");

	return true;
}

#define al _as->add_link
#define an _as->add_node

//// Attempt to connect every connector in a section.
void Aggregate::extend_section(const Handle& section)
{
	printf("duude extend =%s\n", section->to_string().c_str());

	// Connector seq is always second in the outset.
	Handle conseq = section->getOutgoingAtom(1);

	for (const Handle& con : conseq->getOutgoingSet())
	{
		// Nothing to do, if not a connector.
		if (CONNECTOR != con->get_type()) continue;

		// For now, assume only one pole per connector.
		Handle from_pole = con->getOutgoingAtom(1);
		Handle to_pole;
		for (const HandlePair& popr: _pole_pairs)
			if (from_pole == popr.first) { to_pole = popr.second; break; }

		// A matching pole was not found.
		if (!to_pole) continue;

printf("duude connect =%s\n%s\n", to_pole->to_string().c_str(), con->to_string().c_str());

		// Link type of the desired link to make...
		Handle linkty = con->getOutgoingAtom(0);

		// Find appropriate connector, if it exists
		Handle matching = _as->get_atom(createLink(CONNECTOR, linkty, to_pole));
		if (!matching) continue;

		// Find all ConnectorSeq with the matching connector in it.
		HandleSeq to_seqs = matching->getIncomingSetByType(CONNECTOR_SEQ);
		for (const Handle& to_seq : to_seqs)
		{
printf("duude found seq %s\n", to_seq->to_string().c_str());
			HandleSeq to_sects = to_seq->getIncomingSetByType(SECTION);
			for (const Handle& to_sect : to_sects)
			{
				connect_section(section, con, to_sect, matching, linkty);
			}
		}
	}
}

void Aggregate::connect_section(const Handle& from_sect,
                                const Handle& from_con,
                                const Handle& to_sect,
                                const Handle& to_con,
                                const Handle& linkty)
{
printf("duude connect =%s\nto %s\n", from_sect->to_string().c_str(), to_sect->to_string().c_str());

}
