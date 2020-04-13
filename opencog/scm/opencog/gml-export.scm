;
; gml-export.scm
;
; Quick-n-dirty hack to export networks to GML - Graph Modeling Language
; See https://en.wikipedia.org/wiki/Graph_Modelling_Language
;

(define (graph-to-nodes GRAPH)
	; A list of just the points
	(define pt-list
		(map (lambda (sect) (cog-outgoing-atom sect 0))
			(cog-outgoing-set GRAPH)))

	(fold
		(lambda (point str)
			(string-concatenate (list
				"\tnode [\n"
				"\t\tid xxx\n"
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
				"]\n"
				str)))
		""
		(cog-outgoing-set GRAPH-SET))
)
