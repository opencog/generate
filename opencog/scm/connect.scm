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

	; Argh. By default, glob matches one or more; we want zero or more.
	(define (glob STR)
		(TypedVariable (Glob STR) (Interval (Number 0) (Number -1))))

	(define connection (Predicate "connection"))

	(define getter
		(Bind
			(VariableList
				(Variable "$latom") (glob "$lfront") (glob "$lback")
				(Variable "$ratom") (glob "$rfront") (glob "$rback"))
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

			; Replace the matched connectors with a connection
			(Section
				(Variable "$latom")
				(ConnectorSeq
					(Glob "$lfront")
					(Evaluation connection
						(List CONNECTOR (Variable "$latom") (Variable "$ratom")))
					(Glob "$lback")))
			(Section
				(Variable "$ratom")
				(ConnectorSeq
					(Glob "$rfront")
					(Evaluation connection
						(List CONNECTOR (Variable "$latom") (Variable "$ratom")))
					(Glob "$rback")))
	))

	(cog-execute! getter)
)

; ------------------------------------------------------
;
(get-connectable-sections
	(Concept "A") (ConnectorDir "+") (ConnectorDir "-"))
