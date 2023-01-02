/*
 * opencog/generate/RandomCallback.cc
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

#include <random>
#include <uuid/uuid.h>

#include <opencog/atoms/base/Link.h>

#include "RandomCallback.h"

using namespace opencog;

RandomCallback::RandomCallback(AtomSpace* as, const Dictionary& dict,
                               RandomParameters& parms) :
	GenerateCallback(as), _dict(dict), _parms(&parms)
{
	_steps_taken = 0;

	max_solutions = 100;

	// Not interested in networks larger than 20 nodes.
	max_network_size = 20;

	// Max diameter of 10
	max_depth = 10;
}

RandomCallback::~RandomCallback() {}

static std::random_device seed;
static std::mt19937 rangen(seed());

void RandomCallback::clear(AtomSpace* scratch)
{
	while (not _opensel_stack.empty()) _opensel_stack.pop();
	_opensel._opensect.clear();
	_opensel._opendi.clear();

	_root_sections.clear();
	_root_dist.clear();
	_distmap.clear();
	_steps_taken = 0;
	CollectStyle::clear();
	LinkStyle::clear();
	LinkStyle::_point_set = point_set;
	LinkStyle::_scratch = scratch;
}

void RandomCallback::root_set(const HandleSet& roots)
{
	for (const Handle& point: roots)
	{
		HandleSeq sects(_dict.entries(point));
		if (0 == sects.size())
			throw RuntimeException(TRACE_INFO,
				"No dictionary entry for root=%s", point->to_string().c_str());

		_root_sections.push_back(sects);

		// Create a discrete distribution. This will randomly pick
		// an index into the `root_sections` array. The weight of
		// each index is given by the pdf aka "probability distribution
		// function".  The pdf is just given by the weighting-key
		// hanging off the section (in a FloatValue).
		std::vector<double> pdf;
		for (const Handle& sect: sects)
		{
			FloatValuePtr fvp(FloatValueCast(sect->getValue(_weight_key)));
			if (fvp)
				pdf.push_back(fvp->value()[0]);
			else
				pdf.push_back(0.0);
		}
		std::discrete_distribution<size_t> dist(pdf.begin(), pdf.end());
		_root_dist.push_back(dist);
	}
}

/// Perform a random draw of root sections.
HandleSet RandomCallback::next_root(void)
{
	static HandleSet empty_set;
	size_t len = _root_sections.size();
	if (len == 0) return empty_set;

	// Stop iterating if limits have been reached.
	if (max_steps < _steps_taken) return empty_set;
	if (max_solutions <= num_solutions()) return empty_set;

	// Random drawing.
	HandleSet starters;
	for (size_t i=0; i<len; i++)
	{
		size_t idx = _root_dist[i](rangen);
		Handle root(_root_sections[i][idx]);
		starters.insert(create_unique_section(root));
	}

	return starters;
}

/// Return a section containing `to_con`.
/// Pick a new section from the lexis.
Handle RandomCallback::select_from_lexis(const OdoFrame& frame,
                               const Handle& fm_sect, size_t offset,
                               const Handle& to_con)
{
	const HandleSeq& to_sects = _dict.connectables(to_con);

	// Oh no, dead end!
	if (0 == to_sects.size()) return Handle::UNDEFINED;

	// Do we have a chooser for the to-connector?
	// If so, then use to pick a section, randomly.
	auto curit = _distmap.find(to_con);
	if (_distmap.end() != curit)
	{
		auto dist = curit->second;
		return create_unique_section(to_sects[dist(rangen)]);
	}

	// Create a discrete distribution. This will randomly pick an
	// index into the `to_sects` array. The weight of each index
	// is given by the pdf aka "probability distribution function".
	// The pdf is just given by the weighting-key hanging off the
	// section (in a FloatValue).
	std::vector<double> pdf;
	for (const Handle& sect: to_sects)
	{
		FloatValuePtr fvp(FloatValueCast(sect->getValue(_weight_key)));
		if (fvp)
			pdf.push_back(fvp->value()[0]);
		else
			pdf.push_back(0.0);
	}
	std::discrete_distribution<size_t> dist(pdf.begin(), pdf.end());
	_distmap.emplace(std::make_pair(to_con, dist));

	return create_unique_section(to_sects[dist(rangen)]);
}

/// Return a section containing `to_con`, from the set of currently
/// unconnected sections.
///
/// Examine the set of currently-unconnected connectors. If any of
/// them are connectable to `to_con`, then randomly pick one of the
/// sections, and return that. Otherwise return the undefined handle.
///
/// This disallows self-connections (the from and to-sections being the
/// same) unless the parameters allow it.
//
// This just sets up a list of candidates; the actual picking is done
// by `do_select_one()`.
Handle RandomCallback::select_from_open(const OdoFrame& frame,
                               const Handle& fm_sect, size_t offset,
                               const Handle& to_con)
{
	// XXX FIXME -- this implements a cache of distributions
	// ... which are kept on a stack ... this could be RAM intensive
	// ... and also CPU intensive. It might be faster to just
	// choose on the fly, yeah? Don't know... needs investigation.

	// Are there any attachable connectors?
	auto tosit = _opensel._opensect.find(to_con);
	if (_opensel._opensect.end() != tosit and tosit->second.size() == 0)
		return Handle::UNDEFINED;

	// If there's only one, pick it.
	const HandleSeq& to_seclist = tosit->second;
	if (to_seclist.size() == 1)
		return to_seclist[0];

	// Do we have a chooser for the to-connector in the current frame?
	// If so, then use it.
	auto curit = _opensel._opendi.find(to_con);
	if (_opensel._opendi.end() != curit)
	{
		auto dist = curit->second;
		return to_seclist[dist(rangen)];
	}

	// Create a list of connectable sections
	const Handle& linkty = to_con->getOutgoingAtom(0);
	HandleSeq to_sects;
	for (const Handle& open_sect : frame._open_sections)
	{
		if (not allow_self_connections and open_sect == fm_sect)
			continue;

		const Handle& conseq = open_sect->getOutgoingAtom(1);
		for (const Handle& con : conseq->getOutgoingSet())
		{
			if (*con == *to_con)
			{
				// Wait, are these already connected?
				if (pair_any_links <= num_any_links(fm_sect, open_sect))
					continue;
				if (1 < pair_any_links and
				    pair_typed_links <= num_undirected_links(fm_sect,
				                                  open_sect, linkty))
					continue;
				to_sects.push_back(open_sect);
			}
		}
	}

	// Save it...
	_opensel._opensect[to_con] = to_sects;

	// Oh no, dead end!
	if (0 == to_sects.size()) return Handle::UNDEFINED;

	// There's only one. Pick it.
	if (1 == to_sects.size()) return to_sects[0];

	// Create a discrete distribution.
	// Argh ... XXX FIXME ... the aggregator does NOT copy
	// values onto the assembled linkage, and so these will
	// always fail to have the weight key on them. That's OK,
	// for now, because copying the values it probably wasteful.
	// but still ... this results in a uniform distribution,
	// and so its ... ick. Imperfect. Perfect enemy of the good.
#if 0
	std::vector<double> pdf;
	for (const Handle& sect: to_sects)
	{
		FloatValuePtr fvp(FloatValueCast(sect->getValue(_weight_key)));
		if (fvp) pdf.push_back(fvp->value()[0]);
		else pdf.push_back(1.0);
	}
#else
	std::vector<double> pdf(to_sects.size(), 1.0);
#endif
	std::discrete_distribution<size_t> dist(pdf.begin(), pdf.end());
	_opensel._opendi.emplace(std::make_pair(to_con, dist));

	return to_sects[dist(rangen)];
}

/// Return a section containing `to_con`.
/// First try to attach to an existing open section.
/// If that fails, then pick a new section from the lexis.
Handle RandomCallback::select(const OdoFrame& frame,
                              const Handle& fm_sect, size_t offset,
                              const Handle& to_con)
{
	// See if we can find other open connectors to connect to.
	if (_parms->connect_existing(frame))
	{
		Handle open_sect = select_from_open(frame, fm_sect, offset, to_con);
		if (open_sect) return open_sect;
	}

	// Select from the dictionary...
	return select_from_lexis(frame, fm_sect, offset, to_con);
}

/// Create an undirected edge connecting the two points `fm_pnt` and
/// `to_pnt`, using the connectors `fm_con` and `to_con`. The edge
/// is "undirected" because a SetLink is used to hold the two
/// end-points. Recall SetLinks are unordered links, so neither point
/// can be identified as head or tail.
Handle RandomCallback::make_link(const Handle& fm_con,
                                 const Handle& to_con,
                                 const Handle& fm_pnt,
                                 const Handle& to_pnt)
{
	return create_undirected_link(fm_con, to_con, fm_pnt, to_pnt);
}

size_t RandomCallback::num_links(const Handle& fm_sect,
                                 const Handle& to_sect,
                                 const Handle& link_type)
{
	return num_undirected_links(fm_sect, to_sect, link_type);
}

void RandomCallback::push_frame(const OdoFrame& frm)
{
	_opensel_stack.push(_opensel);
	_opensel._opensect.clear();
	_opensel._opendi.clear();
}

void RandomCallback::pop_frame(const OdoFrame& frm)
{
	_opensel = _opensel_stack.top(); _opensel_stack.pop();
}

bool RandomCallback::step(const OdoFrame& frm)
{
	_steps_taken ++;
	if (max_steps < _steps_taken) return false;
	if (max_solutions <= num_solutions()) return false;
	if (max_network_size < frm._linkage.size()) return false;
	if (max_depth < frm._nodo) return false;
	return true;
	// return _parms->step(frm);
}

void RandomCallback::solution(const OdoFrame& frm)
{
	record_solution(frm);
}

Handle RandomCallback::get_solutions(void)
{
	Handle results = CollectStyle::get_solutions();

	// Populate the atomspace, only if there are results to report.
	if (0 < results->get_arity()) LinkStyle::save_work(_as);
	return results;
}
