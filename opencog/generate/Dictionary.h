/*
 * opencog/generate/Dictionary.h
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

#ifndef _OPENCOG_DICTIONARY_H
#define _OPENCOG_DICTIONARY_H

#include <opencog/atomspace/AtomSpace.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

typedef std::map<Handle, HandleSeq> HandleSeqMap;

/// Dictionary (Lexis) of sections that can connect to one-another.
/// This provides several convenience data structures. These include:
///
/// * A set of pairs of connectors that can attach to one-another.
///
/// * A map from Connectors to the Sections that contain them.
///
/// Attention: In principle, these maps should not be stored in a
/// C++ struct such as this, but should instead be pulled directly
/// from the AtomSpace. However, at this time, it just seems more
/// convenient to use this struct. This may change!
class Dictionary
{
	AtomSpace* _as;

	/// Pairings of connectors that can joint to one-another.
	HandlePairSeq _pole_pairs;

	/// Map from Connectors to Sections that hold that connector.
	/// This map is set up at the start, before iteration begins.
	//
	HandleSeqMap _connectables;

	/// Map from points to Sections rooted at that point.
	/// This map is set up at the start, before iteration begins.
	HandleSeqMap _entries;

public:
	Dictionary(AtomSpace*);

	void add_pole_pair(const Handle&, const Handle&);

	HandleSeq joints(const Handle&) const;

	void add_to_lexis(const Handle&);
	void add_to_lexis(const HandleSet& lex) {
		for (const Handle& h: lex) add_to_lexis(h);
	}
	// void sort_lexis(const Handle&);

	const HandleSeq& connectables(const Handle&) const;
	const HandleSeq& entries(const Handle&) const;
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_DICTIONARY_H
