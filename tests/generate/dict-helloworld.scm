;
; dict-helloworld.scm
;
; Basic test: dictionary that allows one sentence: "Hello world".
;
; A test dictionary with three "jigsaw-puzzle pieces" in it.
; These assemble in a linear order, from left to right.
; It should be obvious from inspection how these are should
; be assembled. ... and, if its not ... the connector ID's must
; match, and the conector directions must be polar opposites.
; The edges connecting the vertexes are undirected edges; the
; ConnectorDir polarities instead indicate "to the left" and
; "to the right".
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(define left-wall (Concept "LEFT-WALL"))

(Section
	(Concept "LEFT-WALL")
	(ConnectorSeq
		(Connector (Concept "W") (ConnectorDir "+"))))

(Section
	(Concept "hello")
	(ConnectorSeq
		(Connector (Concept "W") (ConnectorDir "-"))
		(Connector (Concept "OH") (ConnectorDir "+"))))

(Section
	(Concept "world")
	(ConnectorSeq
		(Connector (Concept "OH") (ConnectorDir "-"))))
