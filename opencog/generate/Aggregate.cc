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
#include "DefaultCallback.h"

using namespace opencog;

// Strategy: starting from a single nucleation center (e.g. the left
// wall), recursively aggregate connections until there are no
// unconnected connectors.

Aggregate::Aggregate(AtomSpace* as)
	: _as(as)
{
	_cb = nullptr;
}

Aggregate::~Aggregate()
{
}

/// The nuclei are the nucleation points: points that must
/// appear in sections, some section of which must be linkable.
///
/// pol_pairs is a list of polarization pairs, i.e.
/// match pairs of ConnectorDir pairs (from, to) which
/// are to be connected.
Handle Aggregate::aggregate(const HandleSet& nuclei,
                            GenerateCallback& cb)
{
	_cb = &cb;

	_frame._open_points = nuclei;

	// Pick a point, any point.
	// XXX TODO replace this by a heuristic of some kind.
	Handle nucleus = *_frame._open_points.begin();

	HandleSeq sections = nucleus->getIncomingSetByType(SECTION);

	for (const Handle& sect : sections)
	{
		push_frame();
		_frame._open_sections.insert(sect);
		recurse();
		pop_frame();
	}

	logger().fine("Finished; found %lu solutions\n", _solutions.size());

	// Ugh. This is kind-of unpleasant, but for now we will use SetLink
	// to return results. This obviously fails to scale if the section is
	// large.
	HandleSeq solns;
	for (const auto& sol : _solutions)
	{
		HandleSeq sects;
		for (const Handle& sect : sol)
			sects.push_back(sect);

		solns.push_back(createLink(std::move(sects), SET_LINK));
	}
	return createLink(std::move(solns), SET_LINK);
}

/// Breadth-first recursion.
/// See the README.md for an explanation.
bool Aggregate::recurse(void)
{
	// Nothing to do.
	if (0 == _frame._open_sections.size()) return false;

	logger().fine("Enter recurse");

	// Initialize a brand-new odometer at the next recursion level.
	push_odo();
	bool more = init_odometer();
	if (not more)
	{
		pop_odo();
		return false;
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
			return false;
		}

		// If we are here, we have a valid odo state. Explore it.
		recurse();
		logger().fine("Returned from recurse");

		// Exploration is done, step to the next state.
		more = step_odometer();
		logger().fine("Recurse: After next step, have-more=%d", more);
	}

	return false; // *not-reached*
}

/// Initialize the odometer state. This creates an ordered list of
/// all as-yet unconnected connectors in the open state.
/// Returns false if initialization failed, i.e. if the current
/// open-connector state is not extendable.
bool Aggregate::init_odometer(void)
{
	// Should be empty already, but just in case...
	_odo._from_connectors.clear();
	_odo._to_connectors.clear();
	_odo._sections.clear();

	// Loop over all open connectors
	for (const Handle& sect: _frame._open_sections)
	{
		Handle disj = sect->getOutgoingAtom(1);
		const HandleSeq& conseq = disj->getOutgoingSet();
		for (const Handle& from_con: conseq)
		{
			// There may be fully connected links in the sequence.
			// Ignore those. We want unconnected connectors only.
			if (CONNECTOR != from_con->get_type()) continue;

			// Get a list of connectors that can be connected to.
			// If none, then this connector can never be closed.
			HandleSeq to_cons = _cb->joints(from_con);
			if (0 == to_cons.size()) return false;

			for (const Handle& to_con: to_cons)
			{
				_odo._from_connectors.push_back(from_con);
				_odo._to_connectors.push_back(to_con);
				_odo._sections.push_back(sect);
			}
		}
	}

	_odo._size = _odo._to_connectors.size();
	if (0 == _odo._size) return false;
	_odo._step = 0;

	logger().fine("Initialized odometer of length %lu", _odo._size);
	print_odometer();

	return true;
}

bool Aggregate::do_step(void)
{
	// Erase the last connection that was made.
	if (_frame._wheel == _odo._step and
	    _frame._nodo == _odo_stack.size()) pop_frame();

	logger().fine("Step odometer wheel %lu of %lu at depth %lu",
	               _odo._step, _odo._size, _odo_stack.size());
	print_odometer();

	// Draw a new piece via callback, and attach it.
	bool did_step = false;
	for (size_t ic = _odo._step; ic < _odo._size; ic++)
	{
		Handle fm_sect = _odo._sections[ic];
		const Handle& fm_con = _odo._from_connectors[ic];
		const Handle& to_con = _odo._to_connectors[ic];

		// It's possible that the section is no longer open,
		// because the connector got connected to it. So check.
		if (_frame._open_sections.find(fm_sect) ==
		    _frame._open_sections.end())
		{
			logger().fine("Wheel-sect not open: %lu of %lu at depth %lu",
			               ic, _odo._size, _odo_stack.size());
			print_wheel(ic);
			continue;
		}

		// Section is still open, but the connector might not be.
		bool still_open = false;
		Handle disj = fm_sect->getOutgoingAtom(1);
		for (const Handle& fco: disj->getOutgoingSet())
		{
			if (fco == fm_con) { still_open = true; break; }
		}
		if (not still_open)
		{
			logger().fine("Wheel-con not open: %lu of %lu at depth %lu",
			               ic, _odo._size, _odo_stack.size());
			print_wheel(ic);

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
		Handle to_sect = _cb->select(_frame, fm_sect, fm_con, to_con);

		if (nullptr == to_sect)
		{
			logger().fine("Rolled over wheel %lu of %lu at depth %lu",
			               ic, _odo._size, _odo_stack.size());
			print_wheel(ic);

			// If we are here, then this wheel has rolled over.
			// That means that its time for the next wheel to take
			// a step. Mark that wheel.
			_odo._step = ic - 1;
			return false;
		}

		did_step = true;
		push_frame();
		_frame._wheel = ic;

		// Connect it up, and get the newly-connected section.
		HandlePair hpr = connect_section(fm_sect, fm_con, to_sect, to_con);

		// Replace the from-section with the now-connected section.
		for (size_t in = 0; in < _odo._size; in++)
		{
			if (_odo._sections[in] == fm_sect) _odo._sections[in] = hpr.first;
			if (_odo._sections[in] == to_sect) _odo._sections[in] = hpr.second;
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

	check_for_solution();

	return true;
}

bool Aggregate::step_odometer(void)
{
	// Total rollover
	if (_odo._size < _odo._step) return false;

	// Take a step.
	bool did_step = do_step();
	while (not did_step)
	{
		// If the stepper rollwed over to minus-one, then we're done.
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

// False means halt, no more solutions possible along this path.
bool Aggregate::check_for_solution(void)
{
	logger().fine("--------- Check for solution -------------");
	if (not _cb->recurse(_frame))
	{
		logger().fine("Solution seeking halted at depth %lu",
			_frame_stack.size());
		return false;
	}

	logger().fine("Current state: open-points=%lu open-sect=%lu lkg=%lu",
		_frame._open_points.size(), _frame._open_sections.size(),
		_frame._linkage.size());

	// If there are no more sections, we are done.
	if (0 == _frame._open_sections.size())
	{
		size_t nsolns = _solutions.size();
		_solutions.insert(_frame._linkage);
		size_t news = _solutions.size();
		logger().fine("====================================");
		if (nsolns != news)
		{
			logger().fine("Obtained new solution %lu of size %lu:",
			       news, _frame._linkage.size());
			for (const Handle& lkg : _frame._linkage)
				print_section(lkg);
		}
		else
		{
			logger().fine("Rediscovered solution, still have %lu size=%lu",
			        news, _frame._linkage.size());
		}
		logger().fine("====================================");
		return false;
	}
	return true;
}

#define al _as->add_link
#define an _as->add_node

/// Connect a pair of sections together, by connecting two matched
/// connectors. Two new sections will be created, with the connector
/// in each section replaced by the link.
///
/// Return the pair of newly-connected sections.
HandlePair Aggregate::connect_section(const Handle& fm_sect,
                                      const Handle& fm_con,
                                      const Handle& to_sect,
                                      const Handle& to_con)
{
	// logger().fine("Connect %s\nto %s",
	//	fm_sect->to_string().c_str(), to_sect->to_string().c_str());
	logger().fine("Connect:");
	print_section(fm_sect);
	print_section(to_sect);

	Handle fm_point = fm_sect->getOutgoingAtom(0);
	Handle to_point = to_sect->getOutgoingAtom(0);

	Handle link = _cb->make_link(fm_con, to_con, fm_point, to_point);

	Handle new_fm = make_link(fm_sect, fm_con, link);
	Handle new_to = make_link(to_sect, to_con, link);
	return HandlePair(new_fm, new_to);
}

/// Create a link.  That is, replace a connector `con` by `link` in
/// the section `sect`. Then update the aggregation state. The section
/// is removed from the set of open sections. If the new linked section
/// has no (unconnected) connectors in it, then the new section is added
/// to the linkage; the point is removed from the set of open points.
///
/// `point` should be the first atom of a section (the point)
/// `sect` should be the section to connect
/// `con` should be the connector to connect
/// `link` should be the connecting link.
///
/// Returns the newly-created section.
Handle Aggregate::make_link(const Handle& sect,
                            const Handle& con, const Handle& link)
{
	bool is_open = false;
	HandleSeq oset;
	Handle point = sect->getOutgoingAtom(0);
	Handle seq = sect->getOutgoingAtom(1);
	for (const Handle& fc: seq->getOutgoingSet())
	{
		// If it's not the relevant connector, then just copy.
		if (fc != con)
		{
			oset.push_back(fc);
			if (CONNECTOR == fc->get_type()) is_open = true;
		}
		else
			oset.push_back(link);
	}

	Handle linking =
		_as->add_link(SECTION, point,
			_as->add_link(CONNECTOR_SEQ, std::move(oset)));

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
	print_frame(_frame);
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
	size_t restore_depth = _odo._frame_depth;

	_cb->pop_odometer(_odo);
	_odo = _odo_stack.top(); _odo_stack.pop();

	logger().fine("==== Pop: Odo stack depth now %lu", _odo_stack.size());

	// Realign the frame stack to where we started.
	while (restore_depth < _frame_stack.size()) pop_frame();
}

// =================================================================
// Debug printing utilities.
// Current printing format makes assumptions about connectors
// which will be invalid, in general. XXX FIMXE, someday.

void Aggregate::print_section(const Handle& section) const
{
	logger().fine("    %s:",
		section->getOutgoingAtom(0)->get_name().c_str());
	const HandleSeq& conseq = section->getOutgoingAtom(1)->getOutgoingSet();
	for (const Handle& con : conseq)
	{
		if (CONNECTOR == con->get_type())
			logger().fine("      %s%s",
				con->getOutgoingAtom(0)->get_name().c_str(),
				con->getOutgoingAtom(1)->get_name().c_str());
		else
			logger().fine("      %s - %s - %s",
				con->getOutgoingAtom(1)->getOutgoingAtom(0)->get_name().c_str(),
				con->getOutgoingAtom(0)->get_name().c_str(),
				con->getOutgoingAtom(1)->getOutgoingAtom(1)->get_name().c_str());
	}
}

void Aggregate::print_frame(const Frame& frm) const
{
	logger().fine("Frame:");
	std::string pts;
	for (const Handle& pt : frm._open_points)
		pts += pt->get_name() + ", ";
	logger().fine("    pts: %s", pts.c_str());

	logger().fine("  Open:");
	for (const Handle& open : frm._open_sections)
		print_section(open);

	logger().fine("  Closed:");
	for (const Handle& lkg : frm._linkage)
		print_section(lkg);
}

void Aggregate::print_wheel(size_t i) const
{
	bool sect_open = true;
	const Handle& fm_sect = _odo._sections[i];
	if (_frame._open_sections.find(fm_sect) == _frame._open_sections.end())
		sect_open = false;

	bool conn_open = false;
	const Handle& fm_con = _odo._from_connectors[i];
	Handle disj = fm_sect->getOutgoingAtom(1);
	for (const Handle& fco: disj->getOutgoingSet())
	{
		if (fco == fm_con) { conn_open = true; break; }
	}

	logger().fine("    wheel %lu: %s : %s\t: %s -> %s (sect %s; conn %s)",
		i,
		_odo._sections[i]->getOutgoingAtom(0)->get_name().c_str(),
		_odo._from_connectors[i]->getOutgoingAtom(0)->get_name().c_str(),
		_odo._from_connectors[i]->getOutgoingAtom(1)->get_name().c_str(),
		_odo._to_connectors[i]->getOutgoingAtom(1)->get_name().c_str(),
		sect_open ? "open" : "closed",
		conn_open ? "open" : "closed"
	);
}

void Aggregate::print_odometer() const
{
	logger().fine("Odometer State: length %lu at depth %lu",
		_odo._size, _odo_stack.size());
	for (size_t i=0; i<_odo._size; i++)
		print_wheel(i);
}

// ========================== END OF FILE ==========================
