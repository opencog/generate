;
; dict-tree.scm
; See `examples/dict-tree.scm` for details.
;
; Tree test: dictionary that allows four sentences, as trees (acyclic)
;
;                        +----O-----+
;        +----W---+---S--+    +--D--+
;        |        |      |    |     |
;     LEFT-WALL John    saw   a    cat
;
; The other three substitute "Mary" for "John" and "dog" for "cat".
; All four possible sentences are generated.
;
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(define left-wall (Concept "LEFT-WALL"))

; A test dictionary of basic data.
(Section
	(Concept "LEFT-WALL")
	(ConnectorSeq
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
