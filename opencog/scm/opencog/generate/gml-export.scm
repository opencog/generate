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

; Export the edge list in the graph.
; This is trickier than it seems, for several reasons. First, every
; edge occurs twice: once in the from-section, and once in the
; to-section. Thus we have to de-duplicate these, as we don't want to
; draw two edges when there is actually only one.  Second, it can
; happen that there are two (or more) parallel edges between two
; vertexes. Thus, we want to deduplicate those also, while correctly
; exposing the parallelism. Thus, we have to maintain a list of
; de-duplicators: one that knows how to de-dupe single edges, and
; a second one that knows how to de-dupe double-edges, a third that
; will dedupe triple-edges, and so on. The code below works, but
; might be hard to understand because of this.
(define (graph-to-edges GRAPH)

	; The list of de-duplication sets.
	; The n'th item in this list is a function that knows how to
	; deduplicate n parallel edges.
	(define dupe-sets '())
	(define is-duplicate? (make-atom-set))

	; Filter - remove edges we've seen already,
	; but keep the duplicates. This is tail-recursive.
	(define (filter-dupe elist result duper-list)

		; Find the de-duplicator for n edges. It will be
		; the first one in the passed list, if any.
		(define duper?
			(if (null? duper-list) (make-atom-set) (car duper-list)))

		; The rest of the de-duplicator list.
		(define duper-rest
			(if (null? duper-list) '() (cdr duper-list)))

		; Oh dear, if there wasn't one, then save it.
		; Append, so that its in the n'th place.
		(if (null? duper-list)
			(set! dupe-sets (append dupe-sets (list duper?))))

		; At last -- the tail recursion.
		(if (null? elist)
			result
			(filter-dupe
				(keep-duplicate-atoms elist)
				(append
					(filter
						(lambda (edge) (not (duper? edge)))
						elist)
					result)
				duper-rest)))

	; A list of just the edges. The `(gdr sect)` is the disjunct.
	; We do two fancy things here: we de-duplicate edges reported
	; in other sections; however, if a given section has the same
	; edge several times, we allow that.
	(define edge-list
		(append-map
			(lambda (sect)
				; Remove unwanted edges from the list.
				(filter-dupe
					(cog-outgoing-set (gdr sect))
					'()
					dupe-sets))
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
		; A list of the edges we want to draw.
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
