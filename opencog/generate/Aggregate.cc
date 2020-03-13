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
#include <opencog/atomspace/AtomSpace.h>

#include "Aggregate.h"
#include "Frame.h"

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
Handle Aggregate::aggregate(const HandleSet& nuclei,
                            const HandlePairSeq& pole_pairs)
{
	_frame._open = nuclei;
	_pole_pairs = pole_pairs;

	extend();
	return Handle::UNDEFINED;
}

// Return value of true means halt, no more solutions possible.
// Return value of false means there's more.
bool Aggregate::extend(void)
{
	// If there are no more points, we are done.
	if (0 == _open.size()) return true;

	// Pick a point, any point.
	// XXX TODO replace this by a heuristic of some kind.
	Handle nucleus = *_open.begin();

	HandleSeq sections = nucleus->getIncomingSetByType(SECTION);

	// If there are no sections, then this point is not extendable.
	// This is actually an error condition, I guess?
	if (0 == sections.size())
		throw RuntimeException(TRACE_INFO, "Can't find sections!");

	// Each section is a branch point that has to be explored on
	// it's own.

	for (const Handle& sect : sections)
	{
	}

	printf("done for now\n");

	return true;
}
