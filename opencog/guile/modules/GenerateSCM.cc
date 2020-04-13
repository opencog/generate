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

	Handle do_aggregate(AtomSpace* as, Handle);

public:
	GenerateSCM();
};

#define al as->add_link
#define an as->add_node

Handle GenerateSCM::do_aggregate(AtomSpace* as, Handle root)
{
	// Everything below here is WRONG!!!
	Dictionary dict(as);
	Handle any = an(CONNECTOR_DIR_NODE, "*");
	dict.add_pole_pair(any, any);

	HandleSet lex;
	as->get_handleset_by_type(lex, SECTION);
	dict.add_to_lexis(lex);

#if 0
logger().set_print_to_stdout_flag(true);
logger().set_timestamp_flag(false);
logger().set_level(Logger::FINE);
#endif

	Handle weights = an(PREDICATE_NODE, "weights");
	BasicParameters basic;
	RandomCallback cb(as, dict, basic);
	cb.set_weight_key(weights);
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
	define_scheme_primitive("cog-aggregate",
		&GenerateSCM::do_aggregate, this, "generate");
}

extern "C" {
void opencog_generate_init(void);
};

void opencog_generate_init(void)
{
	 static GenerateSCM generate_scm;
	 generate_scm.module_init();
}
