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
protected:
	Handle _cpred;

public:
	GenerateCallback(AtomSpace* as)
	{
		// Must be same as in class Aggregate ...
		_cpred = as->add_node(PREDICATE_NODE, "connection");
	}
	virtual ~GenerateCallback() {}

	/// Given a connector, return a set of matching connectors
	/// that this particular connector could connect to. This
	/// set may be empty, or may contain more than one match.
	virtual HandleSeq joints(const Handle&) = 0;

	/// Given an exsiting connected section `fm_sect` and a connector
	/// `fm_con` on that section, propose a connection to the section
	/// `to_sect` and connector `to_con`. If this returns true, then
	/// the connection will proceed. Otherwise, this connection will
	/// not be made.
	virtual bool connect(const Frame&,
	                     const Handle& fm_sect, const Handle& fm_con,
	                     const Handle& to_sect, const Handle& to_con) = 0;

	virtual void push(const Frame&) {}
	virtual void pop(const Frame&) {}

	/// Called before the next tree-walking recursive step is taken.
	/// Should return `true` to continue recursion, else false.
	/// The default below allows infinite recursion.
	virtual bool recurse(const Frame&) { return true; }
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_GENERATE_CALLBACK_H
