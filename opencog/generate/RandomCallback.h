/*
 * opencog/generate/RandomCallback.h
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

#ifndef _OPENCOG_RANDOM_CALLBACK_H
#define _OPENCOG_RANDOM_CALLBACK_H

#include <opencog/generate/Dictionary.h>
#include <opencog/generate/GenerateCallback.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// Callback to provide sections (aka "puzzle pieces") to extend the
/// current assembly. Under construction. Meant to draw pieces using
/// a stochastic, random selection process.
///

class RandomCallback : public GenerateCallback
{
protected:
	AtomSpace* _as;

private:
	Dictionary _dict;

	// -------------------------------------------
	// Lexical selection
	virtual Handle select_from_lexis(const Frame&,
	                                 const Handle&, const Handle&,
	                                 const Handle&);

	// Iterator, pointing from a to-connector, to a list of
	// all sections in the dictionary that contain this to-connector.
	// Used by `select()` to return the next attachable section.
	HandleUCounter _lexlit;

	// Stack of iterators into the lists of sections.
	std::stack<HandleUCounter> _lexlit_stack;

	// -------------------------------------------
	virtual Handle select_from_open(const Frame&,
	                                const Handle&, const Handle&,
	                                const Handle&);
	struct OpenSelections
	{
		HandleSeqMap _opensect;

		// Iterator, pointing from a to-connector, to a list of
		// all open sections in the current frame.
		HandleUCounter _openit;
	};

	OpenSelections _opensel;
	std::stack<OpenSelections> _opensel_stack;
	// -------------------------------------------

public:
	RandomCallback(AtomSpace*, const Dictionary&);
	virtual ~RandomCallback();

	virtual HandleSeq joints(const Handle& con) {
		return _dict.joints(con);
	}
	virtual Handle select(const Frame&,
	                      const Handle&, const Handle&,
	                      const Handle&);

	virtual Handle make_link(const Handle&, const Handle&,
	                         const Handle&, const Handle&);
	virtual void push_frame(const Frame&);
	virtual void pop_frame(const Frame&);
	virtual void push_odometer(const Odometer&);
	virtual void pop_odometer(const Odometer&);
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_RANDOM_CALLBACK_H
