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

	Handle do_aggregate(Handle);

public:
	GenerateSCM();
};


Handle GenerateSCM::do_aggregate(Handle root)
{
	return root;
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
