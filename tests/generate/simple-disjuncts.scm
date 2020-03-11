;
; Test dictionary of basic data.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

; A test dictionary of basic data.
(Section
	(Concept "LEFT-WALL")
	(ConnectorSeq
		(Connector
			(Concept "W")
			(ConnectorDir "+"))))

(Section
	(Concept "hello")
	(ConnectorSeq
		(Connector
			(Concept "W")
			(ConnectorDir "-"))
		(Connector
			(Concept "OH")
			(ConnectorDir "+"))))

(Section
	(Concept "world")
	(ConnectorSeq
		(Connector
			(Concept "OH")
			(ConnectorDir "-"))))
