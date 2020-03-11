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

Handle Aggregate::aggregate(const HandleSet& nuclei)
{
	if (1 != nuclei.size())
		throw RuntimeException(TRACE_INFO, "Not implemented!!");

	Handle nucleus = *nuclei.begin();

	return nucleus;
}
