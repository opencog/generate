/*
 * opencog/generate/CollectStyle.h
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

#ifndef _OPENCOG_COLLECT_STYLE_H
#define _OPENCOG_COLLECT_STYLE_H

#include <opencog/generate/Odometer.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// Record completed networks. This is one possible "style" of
/// recording results; other styles are possible.

class CollectStyle
{
protected:
	/// Accumulated set of fully-grounded solutions.
	std::set<HandleSet> _solutions;

public:
	CollectStyle(void);
	~CollectStyle();

	void clear(void) { _solutions.clear(); }
	void record_solution(const OdoFrame&);

	size_t num_solutions(void) { return _solutions.size(); }
	std::set<HandleSet> get_solution_set(void) { return _solutions; }
	Handle get_solutions(void);
};


/** @}*/
}  // namespace opencog

#endif // _OPENCOG_COLLECT_STYLE_H
