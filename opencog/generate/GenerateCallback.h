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

/// Executive decision-making callbacks
///
/// At every branch-point in the traversal algorithm, a list of
/// branches to traverse must be obtained. Likwise, a priority-order
/// for these branches must be given. At any point, there must be
/// a decision to termnate, or to continue traversal. All of these
/// executive decisions are made via the callback interface defined
/// in this class.
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

	/// Given an exsiting connected section `fm_sect` and a connector
	/// `fm_con` on that section, as well as a mating `to_con`, return
	/// a section that could be attached. This allows the callback to
	/// chose sections in such a way that the highest-priority or most
	/// preferable sections are mated first.  Return null handle to
	/// discontinue mating.
	///
	/// This should be implemented so that it behaves like a 'future'
	/// or a 'promise', so that, when called, it returns the next
	/// section from a (virtual) list of eligible sections.
	virtual Handle select(const Frame&,
	                      const Handle& fm_sect, const Handle& fm_con,
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
