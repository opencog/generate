/*
 * GenerateSCM.cc
 *
 * Copyright (C) 2020 Linas Vepstas
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

#include <opencog/guile/SchemeModule.h>
#include <opencog/guile/SchemePrimitive.h>

#include <opencog/generate/Aggregate.h>
#include <opencog/generate/Dictionary.h>
#include <opencog/generate/BasicParameters.h>
#include <opencog/generate/RandomCallback.h>

using namespace opencog;
namespace opencog {

/**
 * Scheme wrapper for the generation code. Quick Hack.
 * Completely wrong and terrible bad API.  XXX FIXME.
 */

class GenerateSCM : public ModuleWrap
{
protected:
	virtual void init();

	Handle do_random_aggregate(Handle, Handle, Handle, Handle);

public:
	GenerateSCM();
};

#define al as->add_link
#define an as->add_node

Handle GenerateSCM::do_random_aggregate(Handle poles,
                                        Handle lexis,
                                        Handle weight,
                                        Handle root)
{
	AtomSpace* as = SchemeSmob::ss_get_env_as("cog-random-aggregate");
	Dictionary dict(as);

	// Add the poles to the dictionary.
	HandleSeq poleset = poles->getIncomingSetByType(MEMBER_LINK);
	for (const Handle& pole_pair : poleset)
	{
		if (*pole_pair->getOutgoingAtom(1) != *poles) continue;

		const Handle& p0 = pole_pair->getOutgoingAtom(0);
		const Handle& p1 = pole_pair->getOutgoingAtom(1);
		dict.add_pole_pair(p0, p1);

		if (nameserver().isA(pole_pair->get_type(), UNORDERED_LINK)
		    and *p0 != *p1)
		{
			dict.add_pole_pair(p1, p0);
		}
	}

	// Add the sections to the dictionary.
	HandleSeq sects = lexis->getIncomingSetByType(MEMBER_LINK);
	for (const Handle& sect : sects)
	{
		if (*sect->getOutgoingAtom(1) != *lexis) continue;
		dict.add_to_lexis(sect);
	}

#if 0
logger().set_print_to_stdout_flag(true);
logger().set_timestamp_flag(false);
logger().set_level(Logger::FINE);
#endif

	BasicParameters basic;
	RandomCallback cb(as, dict, basic);
	cb.set_weight_key(weight);
	Aggregate ag(as);
	ag.aggregate({root}, cb);

	Handle result = cb.get_solutions();
	result = as->add_atom(result);
	return result;
}

} /*end of namespace opencog*/

GenerateSCM::GenerateSCM() : ModuleWrap("opencog generate") {}

/// This is called while (opencog generate) is the current module.
/// Thus, all the definitions below happen in that module.
void GenerateSCM::init(void)
{
	define_scheme_primitive("cog-random-aggregate",
		&GenerateSCM::do_random_aggregate, this, "generate");
}

extern "C" {
void opencog_generate_init(void);
};

void opencog_generate_init(void)
{
	 static GenerateSCM generate_scm;
	 generate_scm.module_init();
}
