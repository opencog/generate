/*
 * opencog/generate/Aggregate.h
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

#ifndef _OPENCOG_AGGREGATE_H
#define _OPENCOG_AGGREGATE_H

#include <set>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/generate/Frame.h>
#include <opencog/generate/GenerateCallback.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

class Aggregate
{
private:
	AtomSpace* _as;
	Handle _cpred;

	// Decision-maker
	GenerateCallback* _cb;

	// Current state
	Frame _frame;

	std::stack<HandleSet> _point_stack;
	std::stack<HandleSet> _open_stack;
	std::stack<HandleSet> _link_stack;
	void push();
	void pop();

	std::set<HandleSet> _solutions;

	bool extend(void);
	bool extend_section(const Handle&);
	void connect_section(const Handle&, const Handle&,
	                     const Handle&, const Handle&,
	                     const Handle&);
	bool make_link(const Handle&, const Handle&,
	               const Handle&);

public:
	Aggregate(AtomSpace*);
	~Aggregate();

	Handle aggregate(const HandleSet&, GenerateCallback&);

};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_AGGREGATE_H
