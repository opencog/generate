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
#include <opencog/generate/Odometer.h>

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
protected:
	AtomSpace* _as;
public:
	GenerateCallback(AtomSpace* as) : _as(as) {}
	virtual ~GenerateCallback() {}

	/// Callbacks are allowed to keep dynamical state. Callbacks are
	/// allowed to be reused for multiple generations. This gives the
	/// callback the opportunity to clear dynamical state. In addition,
	/// this provides a scratch space where temporary results can be
	/// stored for the duration.
	virtual void clear(AtomSpace*) = 0;

	/// Declare a set of initial starting (nucleation) points.  The
	/// generated graph will grow outwards from these points.
	virtual void root_set(const HandleSet& points) = 0;

	/// Iterator, returning the next untried initial starting (nucleation)
	/// sections. These are meant to be a set of sections, containing
	/// the nucleation points, from which the graph generation can grow.
	/// The nucleation points were previously declated by `root_set()`.
	/// Returns the emtpy set, when there is no more to be done.
	virtual HandleSet next_root(void) = 0;

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
	virtual Handle select(const OdoFrame&,
	                      const Handle& fm_sect, size_t offset,
	                      const Handle& to_con) = 0;

	/// Create a link from connector `fm_con` to connector `to_con`,
	/// which will connect `fm_pnt` to `to_pnt`.
	virtual Handle make_link(const Handle& fm_con, const Handle& to_con,
	                         const Handle& fm_pnt, const Handle& to_pnt) = 0;

	/// Return a count of the number of links between `fm_sect` and
	/// `to_sect` of type `link_type`. This should return zero if these
	/// two sections are not nearest neighbors.  Return one, if there
	/// is exactly one link. Return two or more, if there are "parallel"
	/// links in place -- if the two sections are multiply-connected.
	virtual size_t num_links(const Handle& fm_sect, const Handle& to_sect,
	                         const Handle& link_type) = 0;

	virtual void push_frame(const OdoFrame&) {}
	virtual void pop_frame(const OdoFrame&) {}

	virtual void push_odometer(const Odometer&) {}
	virtual void pop_odometer(const Odometer&) {}

	/// Called before taking a step of the odometer.
	/// Return `true` to take the step, else false.
	/// Returning false will abort the current odometer.
	/// Traversal will resume at an earlier level.
	///
	/// The default below allows infinite recursion.
	virtual bool step(const OdoFrame&) { return true; }

	/// Called when a solution is found. A solution is a linkage,
	/// with no open connectors.
	virtual void solution(const OdoFrame&) = 0;

	/// Return a SetLink with all of the solutions that were
	/// reported via the above callback. Perform any other AtomSpace
	/// maintanence pertaining to reporting the solutions.
	virtual Handle get_solutions(void) = 0;

	// ---------------------------------------------------------------
	/// Generic Parameters
	/// These are parameters that all callback systems might reasonably
	/// want to consult, when determining behavior.
	// (Should these be moved to thier own class???)

	/// Maximum number of solutions to accept. Search is halted after
	/// this number is reached.
	size_t max_solutions = -1;

	/// Allow connectors on an open section to connect back onto
	/// themselves (if the other mating rules allow the two connectors
	/// to connect).
	bool allow_self_connections = false;

	/// The maximum number of any kind of link allowed between a pair
	/// of sections. By default, it is one, as "most" "typical" graphs
	/// make sense when only one edge connects a pair of vertexes.
	size_t pair_any_links = 1;

	/// The maximum number of each specific type of link allowed between
	/// a pair of sections. For example, if this is set to two, then two
	/// edges of different types can connect the same pair of points.
	/// This is ignored if `pair_any_links` (above) is 1.
	size_t pair_typed_links = 1;

	/// Maximum size of the generated network. Exploration of networks
	/// larger than this will not be attempted.
	size_t max_network_size = -1;

	/// Maximum depth to explore from the starting point. This is
	/// counted in terms of the maximum depth of the stack of
	/// odometers. This is maximum diameter of the network, as measured
	/// from the starting point.
	size_t max_depth = -1;

	/// Maximum number of odometer steps to take. This needs to be
	/// capped, as infinite loops are possible, especially with the
	/// random selector, which, with approrpaite weightings, can
	/// explosively create infinite networks. (Its potentially
	/// possible to determine, in advance, if a given weighting scheme
	/// will create infinite trees, mon average. But this does not seem
	/// like a high priority task, right now.
	/// (2016 vintage CPU run at approx 1.2K steps/second).
	size_t max_steps = 25101;

	/// A location to which all point instances will be anchored.
	/// Thus, all points can be found by following the MemberLink
	/// from this anchor point.
	Handle point_set = Handle::UNDEFINED;
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_GENERATE_CALLBACK_H
