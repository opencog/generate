/*
 * opencog/generate/BasicParameters.cc
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

#include "BasicParameters.h"

using namespace opencog;

BasicParameters::BasicParameters()
{
	// Try to close existing connectors .. most of the time.
	join_existing = 0.7;

	// Not interested in networks larger than 20 nodes.
	max_network_size = 20;

	// Max diameter of 10
	max_depth = 10;

	// Maximm number of solutions
	max_solutions = 100;

	// Don't allow self-connections. This is usually the desirable
	// default.
	allow_self_connections = false;
}

BasicParameters::~BasicParameters()
{}

static std::random_device seed;
static std::mt19937 rangen(seed());

static inline double uniform_double(void)
{
   static std::uniform_real_distribution<> dist(0.0, 1.0);
   return dist(rangen);
}

bool BasicParameters::connect_existing(const Frame& frm)
{
	return uniform_double() < join_existing;
}

bool BasicParameters::step(const Frame& frm)
{
	return (frm._linkage.size() < max_network_size)
		and (frm._nodo < max_depth);
}
