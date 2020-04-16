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

#include <opencog/atoms/core/NumberNode.h>
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

	Handle do_random_aggregate(Handle, Handle, Handle, Handle, Handle);

public:
	GenerateSCM();
};

#define al as->add_link
#define an as->add_node

/// Decode paramters. A bit ad-hoc, right now.
void decode_param(const Handle& evli,
                  RandomCallback& cb,
                  BasicParameters& basic)
{
	// We expect and EvaluationLink. Ah heck, any ordered link
	// will do...
	if (not evli->is_link())
		throw InvalidParamException(TRACE_INFO,
			"Expecting an ordered link, got %s",
			evli->to_short_string());

	const Handle& pname = evli->getOutgoingAtom(0);
	const Handle& pval = evli->getOutgoingAtom(1);

	// We expect the paramater name in a PredicateNode. Ah heck,
	// any node will do ...
	if (not pname->is_node())
		throw InvalidParamException(TRACE_INFO,
			"Expecting a parameter name, got %s",
			pname->to_short_string());
	const std::string& sname = pname->get_name();

	// All parameters below here expect a NumberNode
	if (not nameserver().isA(pval->get_type(), NUMBER_NODE))
		throw InvalidParamException(TRACE_INFO,
			"Expecting a numerical value, got %s",
			pval->to_short_string());
	double dval = NumberNodeCast(pval)->get_value();

	if (0 == sname.compare("*-max-solutions-*"))
		cb.max_solutions = dval;

	else if (0 == sname.compare("*-close-fraction-*"))
		basic.close_fraction = dval;
}

/// C++ implementation of teh scheme function.
Handle GenerateSCM::do_random_aggregate(Handle poles,
                                        Handle lexis,
                                        Handle weight,
                                        Handle params,
                                        Handle root)
{
	AtomSpace* as = SchemeSmob::ss_get_env_as("cog-random-aggregate");
	Dictionary dict(as);

	// Add the poles to the dictionary.
	HandleSeq poleset = poles->getIncomingSetByType(MEMBER_LINK);
	for (const Handle& membli : poleset)
	{
		if (*membli->getOutgoingAtom(1) != *poles) continue;

		const Handle& pole_pair = membli->getOutgoingAtom(0);
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
	for (const Handle& membli : sects)
	{
		if (*membli->getOutgoingAtom(1) != *lexis) continue;
		dict.add_to_lexis(membli->getOutgoingAtom(0));
	}

	BasicParameters basic;
	RandomCallback cb(as, dict, basic);
	cb.set_weight_key(weight);

	// Decode the parameters. This is .. tedious.
	HandleSeq memps = params->getIncomingSetByType(MEMBER_LINK);
	for (const Handle& membli : memps)
	{
		if (*membli->getOutgoingAtom(1) != *params) continue;
		decode_param(membli->getOutgoingAtom(0), cb, basic);
	}

#if 0
logger().set_print_to_stdout_flag(true);
logger().set_timestamp_flag(false);
logger().set_level(Logger::FINE);
#endif

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
