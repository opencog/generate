;
; dict-loop.scm
;
; Loop test: dictionary that allows four sentences, with one cycle.
;
;        +-------WV------+----O-----+
;        +----W---+---S--+    +--D--+
;        |        |      |    |     |
;     LEFT-WALL John    saw   a    cat
;
; The other three substitute "Mary" for "John" and "dog" for "cat".
; The cycle is a triangle with edges (WV, S, W) and vertexes
; (LEFT-WALL, John, saw)
;
; The dictionary defines nine "jigsaw-puzzle pieces" in it.
; These assemble into the above graphs. It should be obvious from
; inspection how these are should be assembled. ... and, if its not
; ... the connector ID's must match, and the conector directions must
; be polar opposites. The edges connecting the vertexes are undirected
; edges; the ConnectorDir polarities instead indicate "to the left"
; and "to the right". In particular, all three edges in the triangle
; are undirected.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(define left-wall (Concept "LEFT-WALL"))

; A test dictionary of basic data.
(Section
	(Concept "LEFT-WALL")
	(ConnectorSeq
		(Connector (Concept "WV") (ConnectorDir "+"))
		(Connector (Concept "W") (ConnectorDir "+"))))

(Section
	(Concept "John")
	(ConnectorSeq
		(Connector (Concept "W") (ConnectorDir "-"))
		(Connector (Concept "S") (ConnectorDir "+"))))

(Section
	(Concept "Mary")
	(ConnectorSeq
		(Connector (Concept "W") (ConnectorDir "-"))
		(Connector (Concept "S") (ConnectorDir "+"))))

(Section
	(Concept "saw")
	(ConnectorSeq
		(Connector (Concept "S") (ConnectorDir "-"))
		(Connector (Concept "WV") (ConnectorDir "-"))
		(Connector (Concept "O") (ConnectorDir "+"))))

(Section
	(Concept "a")
	(ConnectorSeq
		(Connector (Concept "D") (ConnectorDir "+"))))

(Section
	(Concept "cat")
	(ConnectorSeq
		(Connector (Concept "D") (ConnectorDir "-"))
		(Connector (Concept "O") (ConnectorDir "-"))))

(Section
	(Concept "dog")
	(ConnectorSeq
		(Connector (Concept "D") (ConnectorDir "-"))
		(Connector (Concept "O") (ConnectorDir "-"))))
