;
; gml-export.scm
;
; Quick-n-dirty hack to export networks to GML - Graph Modeling Language
; See https://en.wikipedia.org/wiki/Graph_Modelling_Language
;

(define-public (export-to-gml GRAPH-SET)
"
   export-to-gml GRAPH-SET

   Export the GRAPH-SET - a SetLink wrapping one or more graphs,
	to a UTF-8 encoded text string in GML - Graph Modeling Language
   format.
"
	(define graph-lst (cog-outgoing-set GRAPH-SET)

)
