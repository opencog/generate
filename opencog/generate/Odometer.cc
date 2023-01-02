/*
 * opencog/generate/Odometer.cc
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

#include <stdio.h>

#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atomspace/AtomSpace.h>

#include "Odometer.h"

using namespace opencog;

void OdoFrame::clear(void)
{
	_open_points.clear();
	_open_sections.clear();
	_linkage.clear();
	_nodo = -1;
	_wheel = -1;
}

void Odometer::clear(void)
{
	_sections.clear();
	_from_index.clear();
	_to_connectors.clear();
	_size = 0;
	_step = -1;
	_frame_depth = 0;
}

// =================================================================
// Debug printing utilities.
// Current printing format makes assumptions about connectors
// which will be invalid, in general. XXX FIMXE, someday.

void OdoFrame::print_section(const Handle& section)
{
	logger().fine("    %s:",
		section->getOutgoingAtom(0)->get_name().c_str());
	const HandleSeq& conseq = section->getOutgoingAtom(1)->getOutgoingSet();
	for (const Handle& con : conseq)
	{
		if (CONNECTOR == con->get_type())
			logger().fine("      %s%s",
				con->getOutgoingAtom(0)->get_name().c_str(),
				con->getOutgoingAtom(1)->get_name().c_str());
		else
			logger().fine("      %s - %s - %s",
				con->getOutgoingAtom(1)->getOutgoingAtom(0)->get_name().c_str(),
				con->getOutgoingAtom(0)->get_name().c_str(),
				con->getOutgoingAtom(1)->getOutgoingAtom(1)->get_name().c_str());
	}
}

void OdoFrame::print(void) const
{
	logger().fine("OdoFrame:");
	std::string pts;
	for (const Handle& pt : _open_points)
		pts += pt->get_name() + ", ";
	logger().fine("    pts: %s", pts.c_str());

	logger().fine("  Open:");
	for (const Handle& open : _open_sections)
		print_section(open);

	logger().fine("  Closed:");
	for (const Handle& lkg : _linkage)
		print_section(lkg);
}

// =================================================================

void Odometer::print_wheel(const OdoFrame& frm, size_t i) const
{
	bool sect_open = true;
	const Handle& fm_sect = _sections[i];
	if (frm._open_sections.find(fm_sect) == frm._open_sections.end())
		sect_open = false;

	const Handle& disj = fm_sect->getOutgoingAtom(1);
	const Handle& fm_con = disj->getOutgoingAtom(_from_index[i]);

	bool conn_open = (fm_con->get_type() == CONNECTOR);
	if (conn_open)
		logger().fine("    wheel %lu: %s : %s\t: %s -> %s (sect %s; conn %s)",
			i,
			_sections[i]->getOutgoingAtom(0)->get_name().c_str(),
			fm_con->getOutgoingAtom(0)->get_name().c_str(),
			fm_con->getOutgoingAtom(1)->get_name().c_str(),
			_to_connectors[i]->getOutgoingAtom(1)->get_name().c_str(),
			sect_open ? "open" : "closed",
			conn_open ? "open" : "closed"
		);
	else
	{
		const Handle& lnkset = fm_con->getOutgoingAtom(1);
		logger().fine("    wheel %lu: %s : %s\t: %s -> %s (sect %s; conn %s)",
			i,
			_sections[i]->getOutgoingAtom(0)->get_name().c_str(),
			fm_con->getOutgoingAtom(0)->get_name().c_str(),
			lnkset->getOutgoingAtom(0)->get_name().c_str(),
			lnkset->getOutgoingAtom(1)->get_name().c_str(),
			sect_open ? "open" : "closed",
			conn_open ? "open" : "closed"
		);
	}
}

void Odometer::print_odometer(const OdoFrame& frm) const
{
	logger().fine("Odometer State: length %lu", _size);
	for (size_t i=0; i<_size; i++)
		print_wheel(frm, i);
}

// ========================== END OF FILE ==========================
