;
; basic-network.scm
;
; Basic test: Generate random network. Dictionary contains one word,
; with arbitrary numbers of connectors, all of the same type.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(Section
	(Concept "peep 1")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))))

(Section
	(Concept "peep 2")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))))

(Section
	(Concept "peep 3")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))))

(Section
	(Concept "peep 4")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))))

(Section
	(Concept "peep 5")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))))

(Section
	(Concept "peep 6")
	(ConnectorSeq
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))
		(Connector (Concept "E") (ConnectorDir "*"))))
