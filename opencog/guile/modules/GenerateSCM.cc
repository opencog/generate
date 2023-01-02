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
#include <opencog/atoms/core/StateLink.h>
#include <opencog/guile/SchemeModule.h>
#include <opencog/guile/SchemePrimitive.h>

#include <opencog/generate/Aggregate.h>
#include <opencog/generate/Dictionary.h>
#include <opencog/generate/BasicParameters.h>
#include <opencog/generate/RandomCallback.h>
#include <opencog/generate/SimpleCallback.h>

using namespace opencog;
namespace opencog {

/**
 * Scheme wrapper for the generation code. Quick Hack.
 * Mediocre, ugly-ish API.  XXX FIXME.
 */

class GenerateSCM : public ModuleWrap
{
protected:
	virtual void init();

	Handle do_random_aggregate(Handle, Handle, Handle, Handle, Handle);
	Handle do_simple_aggregate(Handle, Handle, Handle, Handle);

public:
	GenerateSCM();
};

// ----------------------------------------------------------------
/// Decode parameters. A bit ad-hoc, right now.
///
/// The expected encoding for a paramter is
///    (StateLink
///       (MemberLink (PredicateNode "param") (ConceptNode "class"))
///       (Atom "value"))
/// where "param" is a well-known parameter, "class" is the particular
/// grouping of paramters we care about, and `(Atom "value")` is the
/// value for that parameter.
///
void decode_param(const Handle& membli,
                  GenerateCallback& cb,
                  BasicParameters& basic)
{
	Handle statli = StateLink::get_link(membli);
	if (nullptr == statli) return;

	const Handle& pname = membli->getOutgoingAtom(0);
	const Handle& pval = statli->getOutgoingAtom(1);

	// We expect the parameter name in a PredicateNode. Ah heck,
	// any node will do ...
	if (not pname->is_node())
		throw InvalidParamException(TRACE_INFO,
			"Expecting a parameter name, got %s",
			pname->to_short_string());
	const std::string& sname = pname->get_name();

	// We expect a node. Well, any Atom, so we don't check.
	if (0 == sname.compare("*-point-set-anchor-*"))
	{
		cb.point_set = pval;
		return;
	}

	// All parameters below here expect a NumberNode
	if (not nameserver().isA(pval->get_type(), NUMBER_NODE))
		throw InvalidParamException(TRACE_INFO,
			"Expecting a numerical value, got %s",
			pval->to_short_string());
	double dval = NumberNodeCast(pval)->get_value();

	if (0 == sname.compare("*-max-solutions-*"))
		cb.max_solutions = dval;

	else if (0 == sname.compare("*-max-steps-*"))
		cb.max_steps = dval;

	else if (0 == sname.compare("*-max-depth-*"))
		cb.max_depth = dval;

	else if(0 == sname.compare("*-max-network-size-*"))
		cb.max_network_size = dval;

	else if (0 == sname.compare("*-close-fraction-*"))
		basic.close_fraction = dval;
}

/// Decode all parameters attached to an anchor point.
/// See `decode_param()` above. This is just a loop.
void decode_params(const Handle& param_anchor,
                   GenerateCallback& cb,
                   BasicParameters& basic)
{
	// Decode the parameters. One at a time.
	HandleSeq memps = param_anchor->getIncomingSetByType(MEMBER_LINK);
	for (const Handle& membli : memps)
	{
		if (*membli->getOutgoingAtom(1) != *param_anchor) continue;
		decode_param(membli, cb, basic);
	}
}

// ----------------------------------------------------------------
/// Pull the lexis out of the atomspace.
Dictionary decode_lexis(AtomSpace* as, Handle poles, Handle lexis)
{
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
	return dict;
}

// ----------------------------------------------------------------
/// C++ implementation of the scheme function.
Handle GenerateSCM::do_random_aggregate(Handle poles,
                                        Handle lexis,
                                        Handle weight,
                                        Handle params,
                                        Handle root)
{
	AtomSpacePtr asp = SchemeSmob::ss_get_env_as("cog-random-aggregate");
	AtomSpace* as = asp.get();

	Dictionary dict(decode_lexis(as, poles, lexis));

	BasicParameters basic;
	RandomCallback cb(as, dict, basic);
	cb.set_weight_key(weight);

	// Decode the parameters.
	decode_params(params, cb, basic);

	Aggregate ag(as);
	ag.aggregate({root}, cb);

	Handle result = cb.get_solutions();
	result = as->add_atom(result);
	return result;
}

// ----------------------------------------------------------------
/// C++ implementation of the scheme function.
Handle GenerateSCM::do_simple_aggregate(Handle poles,
                                        Handle lexis,
                                        Handle params,
                                        Handle root)
{
	AtomSpacePtr asp = SchemeSmob::ss_get_env_as("cog-simple-aggregate");
	AtomSpace* as = asp.get();

	Dictionary dict(decode_lexis(as, poles, lexis));

	BasicParameters basic;
	SimpleCallback cb(as, dict);
	decode_params(params, cb, basic);

	Aggregate ag(as);
	ag.aggregate({root}, cb);

	Handle result = cb.get_solutions();
	result = as->add_atom(result);
	return result;
}

// ----------------------------------------------------------------
} /*end of namespace opencog*/

GenerateSCM::GenerateSCM() : ModuleWrap("opencog generate") {}

/// This is called while (opencog generate) is the current module.
/// Thus, all the definitions below happen in that module.
void GenerateSCM::init(void)
{
	define_scheme_primitive("cog-random-aggregate",
		&GenerateSCM::do_random_aggregate, this, "generate");
	define_scheme_primitive("cog-simple-aggregate",
		&GenerateSCM::do_simple_aggregate, this, "generate");
}

extern "C" {
void opencog_generate_init(void);
};

void opencog_generate_init(void)
{
	 static GenerateSCM generate_scm;
	 generate_scm.module_init();
}

// --------------------- END OF FILE -----------------------
