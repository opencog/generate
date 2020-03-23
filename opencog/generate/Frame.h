/*
 * opencog/generate/Frame.h
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

#ifndef _OPENCOG_FRAME_H
#define _OPENCOG_FRAME_H

#include <opencog/atomspace/AtomSpace.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// Odometer for breadth-wise aggregation
struct Odometer
{
	/// Ordered list of open connectors. Each from-connector is a
	/// connector on an open section.  Each to-connector is a
	/// connector that can be legally joined to a from-connector.
	HandleSeq _from_connectors;
	HandleSeq _to_connectors;

	/// Ordered list of sections which hold the above from-connectors.
	/// These are the "from-sections".
	HandleSeq _sections;

	/// Ordered list of odometer state.
	/// True == take-a-step; False == do not step;
	/// Each bit corresponds to each to-connector.
	std::vector<bool> _stepper;
};

/// Current traversal state
struct Frame
{
	/// Points that are unconnected
	HandleSet _open_points;
	
	/// Sections with unconnected connectors.
	HandleSet _open_sections;

	/// Completed links.
	HandleSet _linkage;
};

/** @}*/
}  // namespace opencog

#endif // _OPENCOG_FRAME_H
