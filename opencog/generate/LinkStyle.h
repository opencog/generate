/*
 * opencog/generate/LinkStyle.h
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

#ifndef _OPENCOG_LINK_STYLE_H
#define _OPENCOG_LINK_STYLE_H

#include <opencog/atomspace/AtomSpace.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

/// Utility class providing a basic style of linking together two
/// puzzle pieces. Other styles are possible...

class LinkStyle
{
protected:
	AtomSpace* _scratch;
	Handle _point_set;

	HandleSeq _mempoints;
	HandleSeq _inhsects;

public:
	LinkStyle(void);
	void clear(void);

	Handle create_unique_section(const Handle&);
	Handle create_undirected_link(const Handle&, const Handle&,
	                              const Handle&, const Handle&);

	size_t num_undirected_links(const Handle&, const Handle&,
	                            const Handle&);
	size_t num_any_links(const Handle&, const Handle&);

	void save_work(AtomSpace*);
};

/** @}*/
}  // namespace opencog

#endif // _OPENCOG_LINK_STYLE_H
