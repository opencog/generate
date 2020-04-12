/*
 * opencog/generate/SolutionCallback.h
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

#ifndef _OPENCOG_SOLUTION_CALLBACK_H
#define _OPENCOG_SOLUTION_CALLBACK_H

#include <opencog/generate/GenerateCallback.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// Callback to record completed networks. Only records results;
/// does not provide any other functions.

class SolutionCallback : public GenerateCallback
{
protected:
	AtomSpace* _as;

	/// Accumulated set of fully-grounded solutions.
	std::set<HandleSet> _solutions;

public:
	SolutionCallback(AtomSpace*);
	virtual ~SolutionCallback();

	virtual void solution(const Frame&);

	std::set<HandleSet> get_solution_set(void) { return _solutions; }
	Handle get_solutions(void);
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_SOLUTION_CALLBACK_H
