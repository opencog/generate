;
; grammar.scm
;
; This demo shows how some small, simple English-language grammars can
; be specified, and used to generate simple English-language sentences.
; The grammars are all very small, and can therefore be exhaustively
; explored -- that is, all possible sentences are generated. Thus, this
; shows the deterministic explorer at work.
;
; As before, the demo concludes with the resulting graphs exported to
; GML; the graphs can be with CytoScape or Gephi.
;
(use-modules (opencog) (opencog generate))
(use-modules (ice-9 textual-ports))

; --------
; There are several different grammars that will be used in this
; example. Load all of them.  Each one defines a diffferent dictionary.
(load "dict-tree.scm")
(load "dict-loop.scm")
(load "dict-triquad.scm")
(load "dict-mixed.scm")

; --------
;; All sentences start with the "left wall" -- this is a "word" at the
;; start of the sentence, which serves as the root to which subsequent
;; words will be attached.
(define left-wall (Concept "LEFT-WALL"))

; Define the connectable directions (poles). In this case, there
; are two direction types: "+" and "-" denoting "to the right" and
; "to the left". The connector mating rules are that "+" can connect
; only to "-", and vice-versa. The resulting edges are NOT directed
; edges -- they are still undirected edges; the poles are only used
; to indicate word-order in the generated sentence. This convention of
; plus and minus for word-order is in keeping with the representation
; used in the Link Grammar parser. The notation used here should look
; very familiar, when compared to the notaton there. This is not
; accidental.
;
; All of the example dictionaries use this same set of direction rules.
; Its very simple: only "+" and "-" connections are allowed.
;
(define dir-set (Concept "left-right connectors"))
(Member (Set (ConnectorDir "+") (ConnectorDir "-")) dir-set)

; None of the examples require any parameters to control generation.
; The defaults are used. Still, a set needs to be provided - it will be
; the empty set.
(define no-params (Concept "this is an empty set of parameters"))

; --------
;; Convenience function to convert a graph to GML, and write it to
;; a file. It appears that CytoScape does not like files with multiple
;; graphs in them. Thus, we'll pick just one graph, and export that.
;;
;; The resulting graph can be visualized as follows:
;; -- Install and start cytoscape.
;; -- Select "File -> Import -> Network from file ..."
;; -- Choose the appropriate file.
;; -- Hit the "Apply Perfered Layout" button (the rotating arrows button)
(define (export-to-file GRAPH-SET FILENAME)

	(define just-one (Set (gar GRAPH-SET)))
	(define just-one-gml (export-to-gml just-one))

	;; Print the string to a file.
	(let ((outport (open-file FILENAME "w")))
   	(put-string outport just-one-gml)
   	(close outport))

	*unspecified*)

; --------
;; First example: use the "tree" dictionary to generate four sentences,
;; each of the sentences having a dependency relationship that is a
;; strict tree, having no loops. No generation paramters are used,
;; that is, the defaults are accepted.
;;
;; XXX Caution, this is not the final API -- subjet to change.
(define tree-set
	(cog-simple-aggregate dir-set dict-tree no-params left-wall))

;; Print the number of graphs that were generated.
(cog-arity tree-set)

; Write the corpus of sentences out to a file (in GML format).
(export-to-file tree-set "/tmp/corpus-tree.gml")

; --------
; Do it again, for the other dictionaries.
(export-to-file
	(cog-simple-aggregate dir-set dict-loop no-params left-wall)
	"/tmp/corpus-loop.gml")

(export-to-file
	(cog-simple-aggregate dir-set dict-triquad no-params left-wall)
	"/tmp/corpus-triquad.gml")

(export-to-file
	(cog-simple-aggregate dir-set dict-mixed no-params left-wall)
	"/tmp/corpus-mixed.gml")

;; Hush printing when loading this file.
*unspecified*
