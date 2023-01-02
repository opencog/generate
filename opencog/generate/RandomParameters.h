/*
 * opencog/generate/RandomParameters.h
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

#ifndef _OPENCOG_RANDOM_PARAMETERS_H
#define _OPENCOG_RANDOM_PARAMETERS_H

#include <opencog/generate/Odometer.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// The RandomParameters class is used to provide configuration info
/// to the RandomCallback class, constraining the ranges and
/// distributions that get used.
///

class RandomParameters
{
public:
	RandomParameters() {}
	virtual ~RandomParameters() {}

	/// Return true attempt connecting a pair of existing open sections,
	/// else return false to fish for a new, fresh puzzle-piece out of
	/// the lexis. Consistently returning true will maximally close off
	/// any open connectors without enlarging the network. Rturning false
	/// will always increase the size of the network.
	virtual bool connect_existing(const OdoFrame&) = 0;

	/// Return true to continue stepping the odometer. Called before
	/// taking an odometer step.  Returning false will abort the
	/// current odometer.
	virtual bool step(const OdoFrame&) = 0;
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_RANDOM_PARAMETERS_H
