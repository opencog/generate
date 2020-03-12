/*
 * opencog/generate/Frame.cc
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

#include "Frame.h"

using namespace opencog;

Frame::Frame(AtomSpace* as)
	: _as(as)
{
}

Frame::~Frame()
{
}

void Frame::add(const Handle& section)
{
	_open.insert(section);
}

/// Extend unconnected connectors by one step.
/// Return false if not done (more work remains)
/// Else return true if done.
///
/// pol_pairs is a list of polarization pairs, i.e.
/// match pairs of ConnectorDir pairs (from, to) which
/// are to be connected.
bool Frame::extend(const HandlePairSeq& pole_pairs)
{
	if (0 == _open.size()) return true;

	for (const Handle& section : _open)
	{
		extend_one(section, pole_pairs);
	}

	return (0 == _open.size());
}

void Frame::extend_one(const Handle& section,
                       const HandlePairSeq& pole_pairs)
{
	printf("duude extend =%s\n", section->to_string().c_str());

	// Connector seq is always second in the outset.
	Handle conseq = section->getOutgoingAtom(1);

	for (const Handle& con : conseq->getOutgoingSet())
	{
		printf("duude connect =%s\n", con->to_string().c_str());
	}
}
