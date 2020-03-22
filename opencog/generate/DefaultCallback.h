/*
 * opencog/generate/DefaultCallback.h
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

#ifndef _OPENCOG_DEFAULT_CALLBACK_H
#define _OPENCOG_DEFAULT_CALLBACK_H

#include <opencog/generate/GenerateCallback.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

class DefaultCallback : public GenerateCallback
{
protected:
	AtomSpace* _as;

private:
	size_t _stack_depth;
	size_t _effort;
	HandlePairSeq _pole_pairs;

	// Map from Connectors to Sections that hold that connector.
	std::map<Handle, HandleSeq> _lexis;

	std::map<Handle, HandleSeq::const_iterator> _lexlit;

	// Stack of iterators into the
	// std::stack<

public:
	DefaultCallback(AtomSpace*, const HandlePairSeq&);
	virtual ~DefaultCallback();

	void add_to_lexis(const Handle&);

	virtual HandleSeq joints(const Handle&);
	virtual Handle select(const Frame&,
	                      const Handle&, const Handle&,
	                      const Handle&);

	virtual Handle make_link(const Handle&, const Handle&,
	                         const Handle&, const Handle&);
	virtual void push(const Frame&) { _stack_depth++; }
	virtual void pop(const Frame&) { _stack_depth--; }
	virtual bool recurse(const Frame&);
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_DEFAULT_CALLBACK_H
