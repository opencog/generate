/*
 * opencog/generate/Frame.h
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

#ifndef _OPENCOG_FRAME_H
#define _OPENCOG_FRAME_H

#include <opencog/atomspace/AtomSpace.h>

namespace opencog
{
/** \addtogroup grp_generate
 *  @{
 */

class Frame
	: public std::enable_shared_from_this<Frame>
{
private:
	AtomSpace* _as;

	// Sections with unconnected connectors.
	HandleSet _open;

	// Completed links.
	HandleSet _closed;

	void extend_one(const Handle&);

public:
	Frame(AtomSpace*);
	~Frame();

	void add(const Handle&);
	bool extend(void);
};

typedef std::shared_ptr<Frame> FramePtr;
#define createFrame std::make_shared<Frame>

/** @}*/
}  // namespace opencog

#endif // _OPENCOG_FRAME_H
