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
				"\t\t\tid "
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
	; A list of just the edges
	(define edge-list
		(append-map (lambda (sect) (cog-outgoing-set (gdr sect)))
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
		; A list of the unique points in the set.
		; This deletes half, since undirected edges show up twice...
		(delete-dup-atoms edge-list)
	)
)

(define-public (export-to-gml GRAPH-SET)
"
   export-to-gml GRAPH-SET

   Export the GRAPH-SET - a SetLink wrapping one or more graphs,
	to a UTF-8 encoded text string in GML - Graph Modeling Language
   format.
"
	(fold
		(lambda (grph str)
			(string-concatenate (list
				"graph [\n"
				"\tlabel \"placeholder label\"\n"
				(graph-to-nodes grph)
				(graph-to-edges grph)
				"]\n"
				str)))
		""
		(cog-outgoing-set GRAPH-SET))
)
