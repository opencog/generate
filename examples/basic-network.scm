;
; basic-network.scm
;
; Example of a lexis that can be used to generate random networks.
; The lexis contains only one node type and one edge type, and so
; the generated network will consist of only these two. Six sections
; are defined, having one through six connectors on them. These are
; given a weighting, so that when the network is assembled, the
; weights determine the probability that a node will be used.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

; Six burrs with arities one through six.
(define b1
(Section
	(Concept "burr-1")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*")))))

(define b2
(Section
	(Concept "burr-2")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define b3
(Section
	(Concept "burr-3")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define b4
(Section
	(Concept "burr-4")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define b5
(Section
	(Concept "burr-5")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

(define b6
(Section
	(Concept "burr-6")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*")))))

; Give each of them a weight; this will be the probaility of
; their being drawn (selected) when assempling the graph.
(define weights (PredicateNode "weights"))

(cog-set-value! b1 weights (FloatValue 1))
(cog-set-value! b2 weights (FloatValue (/ 1.0 2)))
(cog-set-value! b3 weights (FloatValue (/ 1.0 3)))
(cog-set-value! b4 weights (FloatValue (/ 1.0 4)))
(cog-set-value! b5 weights (FloatValue (/ 1.0 5)))
(cog-set-value! b6 weights (FloatValue (/ 1.0 6)))

; Place the six burrs into a common lexis
(define lexis (Concept "six burrs"))
(Member b1 lexis)
(Member b2 lexis)
(Member b3 lexis)
(Member b4 lexis)
(Member b5 lexis)
(Member b6 lexis)

; Define the connectable directions (poles). In this case, the
; connectors are all the same, and the direction (or "pole")
; connects to itself (rather than having opposite polarity).
; Furthermore, the connections are symmetric (creating undirected
; edges.) Thus, the pole-pair set is very simple.
(define polarity-set (Concept "any to any"))
(Member (Set (ConnectorDir "*") (ConnectorDir "*")) polarity-set)

; Be silent when loading this file.
*unspecified*
