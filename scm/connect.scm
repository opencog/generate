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

; ------------------------------------------------------
;
(define (get-connectable-sections CONNECTOR POL-L POL-R)
"
  Return a set of section-pairs that could be connected together.
  CONNECTOR is the atom in the connectors to join together
  POL-L and POL-R are the polarities to join together.
"
	(define getter
		(Bind
			(VariableList
				(Variable "$latom") (Glob "$lfront") (Glob "$lback")
				(Variable "$ratom") (Glob "$rfront") (Glob "$rback"))
			(And
				(Section
					(Variable "$latom")
					(ConnectorSeq
						(Glob "$lfront")
						(Connector CONNECTOR POL-L)
						(Glob "$lback")))
				(Section
					(Variable "$ratom")
					(ConnectorSeq
						(Glob "$rfront")
						(Connector CONNECTOR POL-R)
						(Glob "$rback"))))

			; Just repeat what was joined.
			(List
				(Section
					(Variable "$latom")
					(ConnectorSeq
						(Glob "$lfront")
						(Connector CONNECTOR POL-L)
						(Glob "$lback")))
				(Section
					(Variable "$ratom")
					(ConnectorSeq
						(Glob "$rfront")
						(Connector CONNECTOR POL-R)
						(Glob "$rback"))))
	))

	(cog-execute! getter)
)

; ------------------------------------------------------
;
(get-connectable-sections
	(Concept "A") (ConnectorDir "+") (ConnectorDir "-"))
