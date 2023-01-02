/*
 * opencog/generate/Odometer.h
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

#ifndef _OPENCOG_ODOMETER_H
#define _OPENCOG_ODOMETER_H

#include <opencog/atomspace/AtomSpace.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// Current traversal state
struct OdoFrame
{
	/// Points that are unconnected
	HandleSet _open_points;

	/// Sections with unconnected connectors.
	HandleSet _open_sections;

	/// Completed links.
	HandleSet _linkage;

	/// The depth of the odometer stack, and the odometer wheel
	/// that this frame is isolating. State earlier than this is
	/// in earlier frames, and later state is in later frames.
	size_t _nodo;
	size_t _wheel;

	void clear(void);
	void print(void) const;

	static void print_section(const Handle&);
};

/// Odometer for breadth-wise aggregation
struct Odometer
{
	/// Size of odometer. All vectors below are of this size.
	size_t _size;

	/// Ordered list of from-sections which have unconnected connectors
	// that are being explored as a part of this odometer.
	/// from-connectors.
	HandleSeq _sections;

	/// Index into the corresponding section, pointing at the open
	/// connector (the "from-connector").
	std::vector<size_t> _from_index;

	/// List of valid connectors that each from-connector can mate to.
	/// (That can be legally joined to a from-connector) Note there
	/// may be multiple to-connectors for each from-connector.
	HandleSeq _to_connectors;

	/// The next wheel to be stepped. This is an index into the
	/// above sequences.
	size_t _step;

	/// The correspoding frame-stack depth, when this odo was created.
	size_t _frame_depth;

	void clear(void);
	void print_odometer(const OdoFrame&) const;
	void print_wheel(const OdoFrame&, size_t) const;
};

/** @}*/
}  // namespace opencog

#endif // _OPENCOG_ODOMETER_H
