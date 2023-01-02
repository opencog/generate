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
	// Try to close existing connectors .. sometimes.
	close_fraction = 0.3;
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

bool BasicParameters::connect_existing(const OdoFrame& frm)
{
	return uniform_double() < close_fraction;
}

bool BasicParameters::step(const OdoFrame& frm)
{
	return true;
}
