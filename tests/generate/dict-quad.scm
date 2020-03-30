;
; dict-quad.scm
;
; Loop test: dictionary that allows four sentences, with one cycle.
;
;        +----------WV--------+----O-----+
;        +----W---+--S--+--I--+    +--D--+
;        |        |     |     |    |     |
;     LEFT-WALL Mary  could  see  the   cat
;
; The other three substitute "see/hear" and "John/Mary".
; The cycle is a quadrilateral with edges (WV, W, S, I) and vertexes
; (LEFT-WALL, Mary, could, see)
;
; As with the triangle variations, edges are undirected.
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
	(Concept "could")
	(ConnectorSeq
		(Connector (Concept "S") (ConnectorDir "-"))
		(Connector (Concept "I") (ConnectorDir "+"))))

(Section
	(Concept "see")
	(ConnectorSeq
		(Connector (Concept "I") (ConnectorDir "-"))
		(Connector (Concept "WV") (ConnectorDir "-"))
		(Connector (Concept "O") (ConnectorDir "+"))))

(Section
	(Concept "hear")
	(ConnectorSeq
		(Connector (Concept "I") (ConnectorDir "-"))
		(Connector (Concept "WV") (ConnectorDir "-"))
		(Connector (Concept "O") (ConnectorDir "+"))))

(Section
	(Concept "the")
	(ConnectorSeq
		(Connector (Concept "D") (ConnectorDir "+"))))

(Section
	(Concept "dog")
	(ConnectorSeq
		(Connector (Concept "D") (ConnectorDir "-"))
		(Connector (Concept "O") (ConnectorDir "-"))))
