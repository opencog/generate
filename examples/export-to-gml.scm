;
; export-to-gml.scm
;
; Create some simple networks, and export them to GML - Graph Modelling
; Language, see https://en.wikipedia.org/wiki/Graph_Modelling_Language
; This allows the networks to be visualized with CytoScape or Gephi.
;
(use-modules (opencog) (opencog generate))
(use-modules (ice-9 textual-ports))

(load "tests/generate/basic-network.scm")

;; XXX Caution, this is not the final API -- subjet to change.
;; This generates multiple graphs.
(define graph-set (cog-aggregate (cog-atomspace) (Concept "peep 3")))

;; Convert the Atomese representation of these networks into
;; a GML string.
(define gml-string (export-to-gml graph-set))

;; Print the string to a file.
(let ((outport (open-file "/tmp/network.gml" "w")))
   (put-string outport gml-string)
   (close outport))
