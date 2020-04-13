;
; basic-network.scm
;
; Basic test: Generate random network. Dictionary contains one word,
; with arbitrary numbers of connectors, all of the same type.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

; Six burrs with different arities
(define v1
(Section
	(Concept "peep 1")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*")))))

(define v2
(Section
	(Concept "peep 2")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define v3
(Section
	(Concept "peep 3")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define v4
(Section
	(Concept "peep 4")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define v5
(Section
	(Concept "peep 5")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define v6
(Section
	(Concept "peep 6")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

; Weightings
(define weights (PredicateNode "weights"))

(cog-set-value! v1 weights (FloatValue 1))
(cog-set-value! v2 weights (FloatValue (/ 1.0 2)))
(cog-set-value! v3 weights (FloatValue (/ 1.0 3)))
(cog-set-value! v4 weights (FloatValue (/ 1.0 4)))
(cog-set-value! v5 weights (FloatValue (/ 1.0 5)))
(cog-set-value! v6 weights (FloatValue (/ 1.0 6)))

*unspecified*
