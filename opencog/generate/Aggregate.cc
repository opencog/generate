/*
 * opencog/generate/Aggregate.cc
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

#include <opencog/util/Logger.h>

#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atomspace/AtomSpace.h>

#include "Aggregate.h"
#include "GenerateCallback.h"

using namespace opencog;

// Strategy: starting from a single nucleation center (e.g. the left
// wall), recursively aggregate connections until there are no
// unconnected connectors.

Aggregate::Aggregate(AtomSpace* as)
	: _as(as)
{
	_cb = nullptr;
	_scratch = nullptr;
}

Aggregate::~Aggregate()
{
}

/// Yuck. Should not allow re-use... XXX FIXME!?
void Aggregate::clear(void)
{
	while (not _frame_stack.empty()) _frame_stack.pop();
	while (not _odo_sections.empty()) _odo_sections.pop();
	while (not _odo_stack.empty()) _odo_stack.pop();

	_frame.clear();
	_odo.clear();

	_scratch = createAtomSpace(_as);
	_cb->clear(_scratch.get());
}

/// The nuclei are the nucleation points: points that must
/// appear in sections, some section of which must be linkable.
///
void Aggregate::aggregate(const HandleSet& nuclei,
                          GenerateCallback& cb)
{
	_cb = &cb;
	clear();

	// Set it up and go.
	_cb->root_set(nuclei);
	while (true)
	{
		HandleSet starters = _cb->next_root();
		if (starters.size() == 0) break;

		push_frame();
		for (const Handle& sect : starters)
		{
			_frame._open_sections.insert(sect);
		}
		recurse();
		pop_frame();
	}
}

/// Breadth-first recursion.
/// See the README.md for an explanation.
void Aggregate::recurse(void)
{
	// Nothing to do.
	if (0 == _frame._open_sections.size()) return;

	// Halt recursion, if need be.
	if (not _cb->step(_frame))
	{
		logger().fine("Recursion halted at frame depth=%lu odo level=%lu",
			_frame_stack.size(), _odo_stack.size());
		return;
	}

	logger().fine("Enter recurse");

	// Initialize a brand-new odometer at the next recursion level.
	push_odo();
	bool more = init_odometer();
	if (not more)
	{
		pop_odo();
		return;
	}

	// Take the first step.
	_odo._step = 0;
	more = do_step();
	_odo._step = _odo._size-1;

	logger().fine("Recurse: After first step, have-more=%d", more);
	while (true)
	{
		// Odometer is exhausted; we are done.
		if (not more)
		{
			pop_odo();
			return;
		}

		// If we are here, we have a valid odo state. Explore it.
		recurse();
		logger().fine("Returned from recurse");

		// Exploration is done, step to the next state.
		more = step_odometer();
		logger().fine("Recurse: After next step, have-more=%d", more);
	}

	return; // *not-reached*
}

/// Initialize the odometer state. This creates an ordered list of
/// all as-yet unconnected connectors in the open state.
/// Returns false if initialization failed, i.e. if the current
/// open-connector state is not extendable.
bool Aggregate::init_odometer(void)
{
	// Should be empty already, but just in case...
	_odo._from_index.clear();
	_odo._to_connectors.clear();
	_odo._sections.clear();

	// Loop over all open connectors
	for (const Handle& sect: _frame._open_sections)
	{
		Handle disj = sect->getOutgoingAtom(1);
		const HandleSeq& conseq = disj->getOutgoingSet();
		for (size_t idx = 0; idx < conseq.size(); idx++)
		{
			const Handle& from_con = conseq[idx];

			// There may be fully connected links in the sequence.
			// Ignore those. We want unconnected connectors only.
			if (CONNECTOR != from_con->get_type()) continue;

			// Get a list of connectors that can be connected to.
			// If none, then this connector can never be closed.
			HandleSeq to_cons = _cb->joints(from_con);
			if (0 == to_cons.size()) return false;

			for (const Handle& to_con: to_cons)
			{
				_odo._sections.push_back(sect);
				_odo._from_index.push_back(idx);
				_odo._to_connectors.push_back(to_con);
			}
		}
	}

	_odo._size = _odo._to_connectors.size();
	if (0 == _odo._size) return false;
	_odo._step = 0;

	logger().fine("Initialized odometer of length %lu", _odo._size);
	_odo.print_odometer(_frame);

	return true;
}

bool Aggregate::do_step(void)
{
	// Erase the last connection that was made.
	if (_frame._wheel == _odo._step and
	    _frame._nodo == _odo_stack.size()) pop_frame();

	logger().fine("Step odometer wheel %lu of %lu at depth %lu",
	               _odo._step, _odo._size, _odo_stack.size());
	_odo.print_odometer(_frame);

	// Draw a new piece via callback, and attach it.
	bool did_step = false;
	for (size_t ic = _odo._step; ic < _odo._size; ic++)
	{
		Handle fm_sect = _odo._sections[ic];
		size_t offset = _odo._from_index[ic];
		const Handle& conseq = fm_sect->getOutgoingAtom(1);
		const Handle& fm_con = conseq->getOutgoingAtom(offset);
		const Handle& to_con = _odo._to_connectors[ic];

		// Is there an open connector at this location?
		if (fm_con->get_type() != CONNECTOR)
		{
			logger().fine("Wheel-con not open: %lu of %lu at depth %lu",
			               ic, _odo._size, _odo_stack.size());
			_odo.print_wheel(_frame, ic);

			if (ic == _odo._step)
			{
				// If we are here, then this wheel has "effectively"
				// rolled over. We cannot continue to the remaining
				// wheels.
				_odo._step = ic - 1;
				return false;
			}
			continue;
		}

		// ----------------------------
		// If we made it to here, then the to-connector is still free.
		// Draw a new section to connect to it.
		Handle to_sect = _cb->select(_frame, fm_sect, offset, to_con);

		if (nullptr == to_sect)
		{
			logger().fine("Rolled over wheel %lu of %lu at depth %lu",
			               ic, _odo._size, _odo_stack.size());
			_odo.print_wheel(_frame, ic);

			// If we are here, then this wheel has rolled over.
			// That means that it's time for the previous wheel
			// to take a step. Mark that wheel.
			_odo._step = ic - 1;
			return false;
		}

		did_step = true;
		push_frame();
		_frame._wheel = ic;

		// Connect it up, and get the newly-connected section.
		HandlePair hpr = connect_section(fm_sect, offset, to_sect, to_con);

		// Replace the from-section with the now-connected section.
		for (size_t in = 0; in < _odo._size; in++)
		{
			if (*_odo._sections[in] == *fm_sect) _odo._sections[in] = hpr.first;
			if (*_odo._sections[in] == *to_sect) _odo._sections[in] = hpr.second;
		}
	}

	if (not did_step)
	{
		logger().fine("Did not step wheel: %lu of %lu at depth %lu",
			               _odo._step, _odo._size, _odo_stack.size());
		if (0 < _odo._step) _odo._step --;
		return false;
	}

	// Next time, we will turn just the last wheel.
	_odo._step = _odo._size - 1;

	logger().fine("Stepped odo: open-points=%lu open-sect=%lu lkg=%lu",
		_frame._open_points.size(), _frame._open_sections.size(),
		_frame._linkage.size());

	// If we found a solution, let the callback accumulate it.
	if (0 == _frame._open_sections.size())
		_cb->solution(_frame);

	return true;
}

bool Aggregate::step_odometer(void)
{
	if (not _cb->step(_frame))
	{
		logger().fine("Odometer halted at frame depth=%lu odo stack=%lu",
			_frame_stack.size(), _odo_stack.size());
		return false;
	}

	// Total rollover
	if (_odo._size < _odo._step) return false;

	// Take a step.
	bool did_step = do_step();
	while (not did_step)
	{
		// If the stepper rolled over to minus-one, then we're done.
		if (SIZE_MAX == _odo._step)
		{
			logger().fine("Exhaused the odometer at depth %lu", _odo_stack.size());
			return false;
		}
		logger().fine("Failed to step, try wheel %lu", _odo._step);

		did_step = do_step();
	}

	return did_step;
}

#define al _as->add_link
#define an _as->add_node

/// Connect a pair of sections together, by connecting two matched
/// connectors. Two new sections will be created, with the connector
/// in each section replaced by the link.
///
/// Return the pair of newly-connected sections.
HandlePair Aggregate::connect_section(const Handle& fm_sect,
                                      size_t offset,
                                      const Handle& to_sect,
                                      const Handle& to_con)
{
	// logger().fine("Connect %s\nto %s",
	//	fm_sect->to_string().c_str(), to_sect->to_string().c_str());
	logger().fine("Connect fm-offset=%lu:", offset);
	OdoFrame::print_section(fm_sect);
	OdoFrame::print_section(to_sect);

	const Handle& fm_point = fm_sect->getOutgoingAtom(0);
	const Handle& to_point = to_sect->getOutgoingAtom(0);

	const Handle& conseq = fm_sect->getOutgoingAtom(1);
	const Handle& fm_con = conseq->getOutgoingAtom(offset);

	Handle link = _cb->make_link(fm_con, to_con, fm_point, to_point);

	// Oh dear, we need the index of the to_con in the to_sect
	// Perhaps the callback should provide this info?
	const Handle& disj = to_sect->getOutgoingAtom(1);
	const HandleSeq& tseq = disj->getOutgoingSet();
	size_t tidx = -1;
	for (size_t i=0; i<tseq.size(); i++)
	{
		if (*to_con == *tseq[i]) { tidx = i; break; }
	}

	Handle new_fm = make_link(fm_sect, offset, link);
	Handle new_to = make_link(to_sect, tidx, link);
	return HandlePair(new_fm, new_to);
}

/// Create a link.  That is, replace a connector `con` by `link` in
/// the section `sect`. Then update the aggregation state. The section
/// is removed from the set of open sections. If the new linked section
/// has no (unconnected) connectors in it, then the new section is added
/// to the linkage; the point is removed from the set of open points.
///
/// `point` should be the first atom of a section (the point)
/// `sect`  should be the section to connect
/// `index` should be the index of the connector to connect
/// `link`  should be the connecting link.
///
/// Returns the newly-created section.
Handle Aggregate::make_link(const Handle& sect, size_t index,
                            const Handle& link)
{
	const Handle& point = sect->getOutgoingAtom(0);
	const Handle& disj = sect->getOutgoingAtom(1);
	HandleSeq oset = disj->getOutgoingSet();

	// Replace the connector with the link.
	oset[index] = link;

	// Any remaining unconnected connectors?
	bool is_open = false;
	for (const Handle& fc : oset)
	{
		if (CONNECTOR == fc->get_type()) { is_open = true; break; }
	}

	// Create the now-connected linkage. Create it in the scratch
	// space, so as not to pollute the main space.
	Handle linking =
		_scratch->add_link(SECTION, point,
			_scratch->add_link(CONNECTOR_SEQ, std::move(oset)));

	// Remove the section from the open set.
	_frame._open_sections.erase(sect);

	// If the connected section has remaining unconnected connectors,
	// then add it to the unfinished set. Else we are done with it.
	if (is_open)
	{
		_frame._open_sections.insert(linking);
		_frame._open_points.insert(point);
		logger().fine("---- Open point %s", point->to_string().c_str());
	}
	else
	{
		_frame._linkage.insert(linking);
		_frame._open_points.erase(point);
		logger().fine("---- Close point %s", point->to_string().c_str());
	}

	return linking;
}

void Aggregate::push_frame(void)
{
	_cb->push_frame(_frame);
	_frame_stack.push(_frame);
	_odo_sections.push(_odo._sections);
	_frame._nodo = _odo_stack.size();
	_frame._wheel = -1;

	logger().fine("---- Push: Frame stack depth now %lu npts=%lu open=%lu lkg=%lu",
	     _frame_stack.size(), _frame._open_points.size(),
	     _frame._open_sections.size(), _frame._linkage.size());
}

void Aggregate::pop_frame(void)
{
	_cb->pop_frame(_frame);
	_frame = _frame_stack.top(); _frame_stack.pop();
	_odo._sections = _odo_sections.top(); _odo_sections.pop();

	logger().fine("---- Pop: Frame stack depth now %lu npts=%lu open=%lu lkg=%lu",
	     _frame_stack.size(), _frame._open_points.size(),
	     _frame._open_sections.size(), _frame._linkage.size());
	_frame.print();
}

/// Push the odometer state.
void Aggregate::push_odo(void)
{
	_cb->push_odometer(_odo);
	_odo_stack.push(_odo);

	logger().fine("==== Push: Odo stack depth now %lu", _odo_stack.size());

	_odo._frame_depth = _frame_stack.size();
}

void Aggregate::pop_odo(void)
{
	// Realign the frame stack to where we started.
	while (_odo._frame_depth < _frame_stack.size()) pop_frame();

	_cb->pop_odometer(_odo);
	_odo = _odo_stack.top(); _odo_stack.pop();

	logger().fine("==== Pop: Odo stack depth now %lu", _odo_stack.size());
}

// ========================== END OF FILE ==========================
