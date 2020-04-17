;
; OpenCog Generation module
;

(define-module (opencog generate))

(use-modules (opencog))
(use-modules (opencog generate-config))
(load-extension
	(string-append opencog-ext-path-generate "libguile-generate")
	"opencog_generate_init")

(export
	cog-random-aggregate
	cog-simple-aggregate
)

(include-from-path "opencog/generate/gml-export.scm")

; ----------------------------------------------------------
; Documentation for the functions implemented as C++ code
(set-procedure-property! cog-random-aggregate 'documentation
"
  cog-random-aggregate POLES LEXIS WEIGHT PARAMS ROOT

    Aggregate a random network around ROOT, using the sections defined
    in the LEXIS, selected with weighting given by WEIGHT. The
    connectable enpoints are given by POLES. Some parameters
    controlling the search are in PARAMS.

    See the example `basic-network.scm` for more details.
")

(set-procedure-property! cog-simple-aggregate 'documentation
"
  cog-simple-aggregate POLES LEXIS PARAMS ROOT

    Aggregate a simple network around ROOT, using the sections defined
    in the LEXIS, and the connectable enpoints given by POLES. Some
    parameters controlling the search are in PARAMS.

    See the examples `dict-tree.scm` and `dict-loop.scm` for more details.
")
