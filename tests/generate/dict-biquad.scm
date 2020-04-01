;
; dict-biquad.scm
;
; Double-Loop test: dictionary that allows four sentences, with two
; cycles.
;
;       +----------WV------+--------CV-------+
;       +---W---+--S-+--I--+--Ce-+--S--+--I--+
;       |       |    |     |     |     |     |
;   LEFT-WALL Mary might think John  could fall
;
; The other sentences swap "might" for "could".  The two cycles are both
; quads; one with edges (W, S, I, WV), vertexes (LEFT-WALL, Mary,
; might, think). The other has edges (Ce, S, I, CV) and vertexes (think,
; John, could, fall). The two quads share a single common vertex: "think".
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
	(Concept "Mary")
	(ConnectorSeq
		(Connector (Concept "W") (ConnectorDir "-"))
		(Connector (Concept "S") (ConnectorDir "+"))))

(Section
	(Concept "John")
	(ConnectorSeq
		(Connector (Concept "Ce") (ConnectorDir "-"))
		(Connector (Concept "S") (ConnectorDir "+"))))

(Section
	(Concept "think")
	(ConnectorSeq
		(Connector (Concept "I") (ConnectorDir "-"))
		(Connector (Concept "WV") (ConnectorDir "-"))
		(Connector (Concept "CV") (ConnectorDir "+"))
		(Connector (Concept "Ce") (ConnectorDir "+"))))

(Section
	(Concept "fall")
	(ConnectorSeq
		(Connector (Concept "I") (ConnectorDir "-"))
		(Connector (Concept "CV") (ConnectorDir "-"))))

(Section
	(Concept "might")
	(ConnectorSeq
		(Connector (Concept "S") (ConnectorDir "-"))
		(Connector (Concept "I") (ConnectorDir "+"))))

(Section
	(Concept "could")
	(ConnectorSeq
		(Connector (Concept "S") (ConnectorDir "-"))
		(Connector (Concept "I") (ConnectorDir "+"))))
