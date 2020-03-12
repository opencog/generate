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

// Extend unconnected connectors by one step.
// Return false if not done (more work remains)
// Else return true if done.
bool Frame::extend(void)
{
	if (0 == _open.size()) return true;

	for (const Handle& section : _open)
	{
		extend_one(section);
	}

	return (0 == _open.size());
}

void Frame::extend_one(const Handle& section)
{
	printf("duude extend =%s\n", section->to_string().c_str());
}
