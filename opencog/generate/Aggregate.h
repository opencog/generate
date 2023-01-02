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
#include <opencog/generate/Odometer.h>
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
	AtomSpacePtr _scratch;

	/// Decision-maker
	GenerateCallback* _cb;

	/// Current traversal state
	OdoFrame _frame;
	Odometer _odo;

	std::stack<OdoFrame> _frame_stack;
	std::stack<HandleSeq> _odo_sections;
	void push_frame();
	void pop_frame();

	std::stack<Odometer> _odo_stack;
	void push_odo();
	void pop_odo();

	void clear(void);

	bool init_odometer(void);
	bool step_odometer(void);
	bool do_step(void);

	void recurse(void);

	HandlePair connect_section(const Handle&, size_t,
	                           const Handle&, const Handle&);
	Handle make_link(const Handle&, size_t, const Handle&);

public:
	Aggregate(AtomSpace*);
	~Aggregate();

	void aggregate(const HandleSet&, GenerateCallback&);

};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_AGGREGATE_H
