;
; connect.scm
;
; Sandbox for fiddling about.

(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

; Some data
(Section
	(Concept "hello")
	(ConnectorSeq
		(Connector
			(Concept "A")
			(ConnectorDir "+"))))

(Section
	(Concept "world")
	(ConnectorSeq
		(Connector
			(Concept "A")
			(ConnectorDir "-"))))
