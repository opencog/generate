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
	_cpred = _as->add_node(PREDICATE_NODE, "connection");
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
		push();
		_frame._open_sections.insert(sect);
		extend();
		pop();
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

// Return value of true means that the extension worked, and there's
// more to explore. False means halt, no more solutions possible
// along this path.
bool Aggregate::extend(void)
{
	logger().fine("------------------------------------");
	if (not _cb->recurse(_frame))
	{
		logger().fine("recursion halted at depth %lu",
			_link_stack.size());
		return false;
	}

	logger().fine("Begin recursion: open-points=%lu open-sect=%lu lkg=%lu",
		_frame._open_points.size(), _frame._open_sections.size(), 
		_frame._linkage.size());

	// If there are no more sections, we are done.
	if (0 == _frame._open_sections.size())
	{
		logger().fine("====================================");
		logger().fine("Obtained solution: %s", oc_to_string(_frame._linkage).c_str());
		logger().fine("====================================");
		_solutions.insert(_frame._linkage);
		return false;
	}

	// Each section is a branch point that has to be explored on
	// it's own. Halt, if the section is not extendable any more.
	HandleSet sects = _frame._open_sections;
	for (const Handle& sect : sects)
	{
		push();
		bool keep_going = extend_section(sect);
		pop();

		if (not keep_going) return false;
	}

	return false;
}

#define al _as->add_link
#define an _as->add_node

/// Attempt to connect every connector in a section.
/// Return true if every every connector on the section was
/// successfully extended. Return false if the section is not
/// extendable (if there is at least one connector that cannot
/// be connected to something.)
bool Aggregate::extend_section(const Handle& section)
{
	logger().fine("Extend section=%s\n", section->to_string().c_str());

	// Unpack the section; the point and the sequence of connectors.
	Handle from_point = section->getOutgoingAtom(0);
	Handle from_seq = section->getOutgoingAtom(1);

	for (const Handle& from_con : from_seq->getOutgoingSet())
	{
		// There may be fully connected links in th sequence.
		// Ignore those. We want unconnected connectors only.
		if (CONNECTOR != from_con->get_type()) continue;

		// Get a list of connectors that can be connected to.
		// If none, then this connector can never be closed.
		HandleSeq to_cons = _cb->joints(from_con);
		if (0 == to_cons.size()) return false;

		// Link type of the desired link to make...
		Handle linkty = from_con->getOutgoingAtom(0);

		// XXX assume one matching connector. XXX this needs to be a loop.
		Handle matching = to_cons[0];

		// Find all ConnectorSeq with the matching connector in it.
		HandleSeq to_seqs = matching->getIncomingSetByType(CONNECTOR_SEQ);
		bool found_internal = false;
		for (const Handle& to_seq : to_seqs)
		{
			logger().fine("Connect from %s\nto %s",
				from_con->to_string().c_str(), to_seq->to_string().c_str());

			HandleSeq to_sects = to_seq->getIncomingSetByType(SECTION);

			for (const Handle& to_sect : to_sects)
			{
				// If none of the available sections are something
				// we want to hook up, then there's nothing to do.
				if (_frame._open_sections.end() == _frame._open_sections.find(to_sect)) continue;

				// Have be previously connected these pieces together?
				// if so, then don't try it again.
				Handle to_point = to_sect->getOutgoingAtom(0);
				Handle cpr = _as->add_link(LIST_LINK, linkty, from_point, to_point);
				Handle link = _as->get_link(EVALUATION_LINK, _cpred, cpr);
				if (nullptr != link) continue;

				link = _as->add_link(EVALUATION_LINK, _cpred, cpr);

				found_internal = true;
				push();
				connect_section(section, from_con, to_sect, matching, link);

				// And now, recurse...
				extend();
				pop();
			}
		}

logger().fine("found_internal = %d", found_internal);
		if (found_internal) continue;

		// Hack-alicious
		for (const Handle& to_seq : to_seqs)
		{
			logger().fine("Connect from %s\nto %s",
				from_con->to_string().c_str(), to_seq->to_string().c_str());

			HandleSeq to_sects = to_seq->getIncomingSetByType(SECTION);

			for (const Handle& to_sect : to_sects)
			{
				// Just like above, but the opposite sense.
				if (_frame._open_sections.end() != _frame._open_sections.find(to_sect)) continue;

// XXX All this needs to be refactored to match the above.
				Handle to_point = to_sect->getOutgoingAtom(0);
				Handle link = _as->add_link(EVALUATION_LINK, _cpred,
					_as->add_link(LIST_LINK, linkty, from_point, to_point));

				push();
				connect_section(section, from_con, to_sect, matching, link);

				// And now, recurse...
				extend();
				pop();
			}
		}
	}
	return true;
}

/// Connect a pair of sections together, by connecting two matched
/// connectors. Two new sections will be created, with the connector
// in each section replaced by the link.
void Aggregate::connect_section(const Handle& from_sect,
                                const Handle& from_con,
                                const Handle& to_sect,
                                const Handle& to_con,
                                const Handle& link)
{
	logger().fine("Connect %s\nto %s",
		from_sect->to_string().c_str(), to_sect->to_string().c_str());

	make_link(from_sect, from_con, link);
	make_link(to_sect, to_con, link);
}

/// Create a link.  That is, replace a connector `con` by `link` in
/// the section `sect`. Then update the aggregation state. The section
/// is removed from the set of open sections. If the new linked section
/// has no (unconnected) connector in it, then the new section is added
/// to the linkage; the point is removed from the set of open points.
///
/// `point` should be the first atom of a section (the point)
/// `sect` should be the section to connect
/// `con` should be the connector to connect
/// `link` should be the connecting link.
///
/// Returns true if the new link is not fully connected.
bool Aggregate::make_link(const Handle& sect,
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

	// Remove the section from the opn set.
	_frame._open_sections.erase(sect);

	// If it has remaining unconnected connectors, then
	// add it to the unfinished set. Else we are done with it.
	if (is_open)
	{
		_frame._open_sections.insert(linking);
		_frame._open_points.insert(point);
	}
	else
	{
		_frame._linkage.insert(linking);
		_frame._open_points.erase(point);
	}

	return is_open;
}

void Aggregate::push(void)
{
	_cb->push(_frame);
	_point_stack.push(_frame._open_points);
	_open_stack.push(_frame._open_sections);
	_link_stack.push(_frame._linkage);

	logger().fine("---- Push: Stack depth now %lu", _link_stack.size());
}

void Aggregate::pop(void)
{
	_cb->pop(_frame);
	_frame._open_points = _point_stack.top(); _point_stack.pop();
	_frame._open_sections = _open_stack.top(); _open_stack.pop();
	_frame._linkage = _link_stack.top(); _link_stack.pop();

	logger().fine("---- Pop: Stack depth now %lu", _link_stack.size());
}
