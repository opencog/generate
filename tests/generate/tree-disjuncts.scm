;
; Tree test: dictionary that allows four setences, as trees (acyclic)
;
;                        +----O-----+
;        +--->W---+---S--+    +--D--+
;        |        |      |    |     |
;     LEFT-WALL John    saw   a    cat
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(define left-wall (Concept "LEFT-WALL"))

; A test dictionary of basic data.
(Section
	(Concept "LEFT-WALL")
	(ConnectorSeq
		(Connector
			(Concept "W")
			(ConnectorDir "+"))))

(Section
	(Concept "John")
	(ConnectorSeq
		(Connector
			(Concept "W")
			(ConnectorDir "-"))
		(Connector
			(Concept "S")
			(ConnectorDir "+"))))

(Section
	(Concept "Sue")
	(ConnectorSeq
		(Connector
			(Concept "W")
			(ConnectorDir "-"))
		(Connector
			(Concept "S")
			(ConnectorDir "+"))))

(Section
	(Concept "saw")
	(ConnectorSeq
		(Connector
			(Concept "S")
			(ConnectorDir "-"))
		(Connector
			(Concept "O")
			(ConnectorDir "+"))))

(Section
	(Concept "a")
	(ConnectorSeq
		(Connector
			(Concept "D")
			(ConnectorDir "+"))))

(Section
	(Concept "cat")
	(ConnectorSeq
		(Connector
			(Concept "D")
			(ConnectorDir "-"))
		(Connector
			(Concept "O")
			(ConnectorDir "-"))))

(Section
	(Concept "dog")
	(ConnectorSeq
		(Connector
			(Concept "D")
			(ConnectorDir "-"))
		(Connector
			(Concept "O")
			(ConnectorDir "-"))))
