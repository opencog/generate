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

#include <opencog/atoms/base/Link.h>

#include "RandomCallback.h"

using namespace opencog;

RandomCallback::RandomCallback(AtomSpace* as, const Dictionary& dict,
                               RandomParameters& parms)
	: GenerateCallback(as), _as(as), _dict(dict), _parms(&parms)
{
}

RandomCallback::~RandomCallback() {}

static std::random_device seed;
static std::mt19937 rangen(seed());

// Pick one out of the sequence.
static inline Handle uniform_choice(const HandleSeq& lst)
{
	std::uniform_int_distribution<> dist(0, lst.size()-1);
	int sno = dist(rangen);
	return lst[sno];
}


/// Return a section containing `to_con`.
/// Pick a new section from the lexis.
Handle RandomCallback::select_from_lexis(const Frame& frame,
                               const Handle& fm_sect, size_t offset,
                               const Handle& to_con)
{
	const HandleSeq& to_sects = _dict.sections(to_con);

	// Oh no, dead end!
	if (0 == to_sects.size()) return Handle::UNDEFINED;

	// Do we have a chooser for the to-connector?
	// If so, then use to pick a section, randomly.
	auto curit = _distmap.find(to_con);
	if (_distmap.end() != curit)
	{
		auto dist = curit->second;
		return to_sects[dist(rangen)];
	}

	// Create a discrete distribution. This will randomly pick an
	// index into the `to_sects` array. The weight of each index
	// is given by the pdf aka "probability distribution function".
	// The pdf is just given by the weighting-key haning off the
	// section (in a FloatValue).
	std::vector<double> pdf;
	for (const Handle& sect: to_sects)
	{
		FloatValuePtr fvp(FloatValueCast(sect->getValue(_weight_key)));
		pdf.push_back(fvp->value()[0]);
	}
	std::discrete_distribution<size_t> dist(pdf.begin(), pdf.end());
	_distmap.emplace(std::make_pair(to_con, dist));

	return to_sects[dist(rangen)];
}

/// Return a section containing `to_con`.
/// Try to attach to an existing open section.
Handle RandomCallback::select_from_open(const Frame& frame,
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

	// Do we have a chooser for the to-connector in the current frame?
	// If so, then use it.
	auto curit = _opensel._opendi.find(to_con);
	if (_opensel._opendi.end() != curit)
	{
		const HandleSeq& to_sects = tosit->second;
		auto dist = curit->second;
		return to_sects[dist(rangen)];
	}

	// Create a list of connectable sections
	HandleSeq to_sects;
	for (const Handle& open_sect : frame._open_sections)
	{
		const Handle& conseq = open_sect->getOutgoingAtom(1);
		for (const Handle& con : conseq->getOutgoingSet())
		{
			if (con == to_con) to_sects.push_back(open_sect);
		}
	}

	// Save it...
	_opensel._opensect[to_con] = to_sects;

	// Oh no, dead end!
	if (0 == to_sects.size()) return Handle::UNDEFINED;

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
Handle RandomCallback::select(const Frame& frame,
                               const Handle& fm_sect, size_t offset,
                               const Handle& to_con)
{
	// See if we can find other open connectors to connect to.
	if (_parms->connect_to_open(frame))
	{
		Handle open_sect = select_from_open(frame, fm_sect, offset, to_con);
		if (open_sect) return open_sect;

		// If this is non-empty, the the odometer rolled over.
		if (_opensel._opensect.find(to_con) != _opensel._opensect.end())
			return Handle::UNDEFINED;
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
	// Create the actual link to use.
	Handle linkty = fm_con->getOutgoingAtom(0);
	Handle edg = _as->add_link(SET_LINK, fm_pnt, to_pnt);
	Handle lnk = _as->add_link(EVALUATION_LINK, linkty, edg);
	return lnk;
}

void RandomCallback::push_frame(const Frame& frm)
{
	_opensel_stack.push(_opensel);
	_opensel._opensect.clear();
	_opensel._opendi.clear();
}

void RandomCallback::pop_frame(const Frame& frm)
{
	_opensel = _opensel_stack.top(); _opensel_stack.pop();
}

bool RandomCallback::step(const Frame& frm)
{
	return true;
}

void RandomCallback::solution(const Frame& frm)
{
	record_solution(frm);
}
