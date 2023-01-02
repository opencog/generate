/*
 * opencog/generate/BasicParameters.h
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

#ifndef _OPENCOG_BASIC_PARAMETERS_H
#define _OPENCOG_BASIC_PARAMETERS_H

#include <random>
#include <opencog/generate/RandomParameters.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// The BasicParameters class is used to provide configuration info
/// to the RandomCallback class, constraining the ranges and
/// distributions that get used. Its currently a mish-mash of policy.
/// Under construction.

class BasicParameters : public RandomParameters
{
public:
	BasicParameters();
	virtual ~BasicParameters();

	virtual bool connect_existing(const OdoFrame&);
	virtual bool step(const OdoFrame&);

	/// Fraction of the time that an attempt should be made to join
	/// together two existing open connectors, if that is possible.
	/// When two existing connectors are joined together, the size
	/// of the network does not increase, and the number of existing
	/// unconnected connectorsdrops by two. Otherwise, a new puzzle
	/// piece is selected from the lexis (thus necessarily enlarging
	/// the network.)
	double close_fraction;
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_BASIC_PARAMETERS_H
