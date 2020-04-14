;
; gml-export.scm
;
; Quick-n-dirty hack to export networks to GML - Graph Modeling Language
; See https://en.wikipedia.org/wiki/Graph_Modelling_Language
;

(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog uuid))

(define (graph-to-nodes GRAPH)
	; A list of just the points
	(define pt-list
		(map (lambda (sect) (gar sect))
			(cog-outgoing-set GRAPH)))

	(fold
		(lambda (point str)
			(string-concatenate (list
				"\tnode [\n"
				"\t\tid "
				(format #f "~D" (cog-assign-uuid point))
				"\n"
				"\t\tlabel \""
				(cog-name point)
				"\"\n"
				"\t]\n"
				str
			)))
		""
		; A list of the unique points in the set.
		(delete-dup-atoms pt-list)
	)
)

(define (graph-to-edges GRAPH)

	; The set of edges we've seen so far...
	(define is-duplicate? (make-atom-set))

	; Filter - remove edges we've seen already, but keep the duplicates.
	(define (filter-dupe elist result)
		(if (eq? elist '())
			result
			(filter-dupe
				(keep-duplicate-atoms elist)
				(append
					(filter-map
						(lambda (edge) (not (is-duplicate? edge)))
						elist)
					result))))

	; A list of just the edges. The `(gdr sect)` is the disjunct.
	; We do two fancy things here: we de-duplicate edges reported
	; in other sections; however, if a given section has the same
	; edge several times, we allow that.
	(define edge-list
		(append-map
			(lambda (sect)
				; Remove unwanted edges from the list.
				(filter-dupe (cog-outgoing-set (gdr sect))))
			(cog-outgoing-set GRAPH)))

	(fold
		(lambda (edge str)
			(string-concatenate (list
				"\tedge [\n"
				"\t\tsource "
				(format #f "~D" (cog-assign-uuid (gadr edge)))
				"\n\t\ttarget "
				(format #f "~D" (cog-assign-uuid (gddr edge)))
				"\n\t\tlabel \""
				(cog-name (gar edge))
				"\"\n\t]\n"
				str
			)))
		""
		; A list of the edges in the set.
		edge-list
	)
)

(define-public (export-to-gml GRAPH-SET)
"
   export-to-gml GRAPH-SET

   Export the GRAPH-SET - a SetLink wrapping one or more graphs,
	to a UTF-8 encoded text string in GML - Graph Modeling Language
   format.
"
	(define graph-id 0)
	(fold
		(lambda (grph str)
			(set! graph-id (+ 1 graph-id))
			(string-concatenate (list
				"graph [\n"
				"\tcomment \"Created by opencog generate\"\n"
				"\tdirected 1\n"
				(format #f "\tlabel \"placeholder ~D\"\n" graph-id)
				(format #f "\tid ~D\n" graph-id)
				(graph-to-nodes grph)
				(graph-to-edges grph)
				"]\n"
				str)))
		""
		(cog-outgoing-set GRAPH-SET))
)
