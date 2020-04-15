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
)

(include-from-path "opencog/generate/gml-export.scm")

; Documentation for the functions implemented as C++ code
(set-procedure-property! cog-random-aggregate 'documentation
"
  cog-random-aggregate POLES LEXIS WEIGHT ROOT

    Aggregate a random network around ROOT, using the sections defined
    in the LEXIS, selected with weighting given by WEIGHT. The
    connectable enpoints are given by POLES.

    See the example `basic-network.scm` for more details.
")
