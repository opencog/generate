/*
 * opencog/generate/GenerateCallback.h
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

#ifndef _OPENCOG_GENERATE_CALLBACK_H
#define _OPENCOG_GENERATE_CALLBACK_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/generate/Frame.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// Aggregation selection callbacks. As an assembly is being created,
/// that assembly will have unconnected, open connectors on it.
/// Aggregation proceeds by attaching sections ("puzzle pieces") to
/// each open connector, until there are none left. But what to connect
/// to what? The master aggregation algorithm defers that choice to this
/// callback API.  Different algos can offer up different connections to
/// try out. The master aggregation algo manages the overall process of
/// connecting things up; the callbacks suggest what to connect next.
///
/// The master aggregation algo is stack-based, and potentially
/// combinatoric-explosive, as each the current set of open connectors
/// depends on the history of what was previously attached. Because
/// the algo is breadt-first (see other descriptions) two stacks are
/// maintained: one for each "row" (odometer) and one for each
/// odometer-wheel.
///
class GenerateCallback
{
public:
	GenerateCallback(AtomSpace* as) {}
	virtual ~GenerateCallback() {}

	/// Given a connector, return a set of matching connectors
	/// that this particular connector could connect to. This
	/// set may be empty, or may contain more than one match.
	virtual HandleSeq joints(const Handle&) = 0;

	/// Given an existing connected section `fm_sect` and a connector
	/// `fm_con` on that section, as well as a mating `to_con`, return
	/// a section that could be attached. This allows the callback to
	/// chose sections in such a way that the highest-priority or most
	/// preferable sections are mated first.  Return null handle to
	/// discontinue mating.
	///
	/// The `fm_con` is the connector located at `offset` in the
	/// `fm_sect`.
	///
	/// This should be implemented so that it behaves like a 'future'
	/// or a 'promise', so that, when called, it returns the next
	/// section from a (virtual) list of eligible sections.
	virtual Handle select(const Frame&,
	                      const Handle& fm_sect, size_t offset,
	                      const Handle& to_sect) = 0;

	/// Create a link from connector `fm_con` to connector `to_con`,
	/// which will connect `fm_pnt` to `to_pnt`.
	virtual Handle make_link(const Handle& fm_con, const Handle& to_con,
	                         const Handle& fm_pnt, const Handle& to_pnt) = 0;

	virtual void push_frame(const Frame&) {}
	virtual void pop_frame(const Frame&) {}

	virtual void push_odometer(const Odometer&) {}
	virtual void pop_odometer(const Odometer&) {}

	/// Called before the next tree-walking recursive step is taken.
	/// Should return `true` to continue recursion, else false.
	/// The default below allows infinite recursion.
	virtual bool recurse(const Frame&) { return true; }
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_GENERATE_CALLBACK_H
