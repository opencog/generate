;
; dict-mixed.scm
;
; Mixture of trees and cycles, sharing vertexes and edges.
;
;
;        +---------------------------Xp--------------+
;        +--------------------WV---------------+     |
;        |            +------------S-----------+     |
;        |            +------MXs------+        |     |
;        |            |  +------Xd----+        |     |
;        +-----W------+  | +---Ds-----+        |     |
;        |      +--D--+  | |    +--A--+-Xc-+   +--Xf-+
;        |      |     |  | |    |     |    |   |     |
;    LEFT-WALL the   dog , a black   lab   , barked  .
;
; The other sentences swap "dog/cat" "black/white", "barked/purred".
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(define left-wall (Concept "LEFT-WALL"))
(define black (Concept "black"))
(define dog (Concept "dog"))
(define bark (Concept "barked"))

; A test dictionary of basic data.
(Section
	(Concept "LEFT-WALL")
	(ConnectorSeq
		(Connector (Concept "Xp") (ConnectorDir "+"))
		(Connector (Concept "WV") (ConnectorDir "+"))
		(Connector (Concept "W") (ConnectorDir "+"))))

(Section
	(Concept "the")
	(ConnectorSeq
		(Connector (Concept "D") (ConnectorDir "+"))))

(Section
	(Concept "a")
	(ConnectorSeq
		(Connector (Concept "Ds") (ConnectorDir "+"))))

(Section
	(Concept "dog")
	(ConnectorSeq
		(Connector (Concept "W") (ConnectorDir "-"))
		(Connector (Concept "D") (ConnectorDir "-"))
		(Connector (Concept "MXs") (ConnectorDir "+"))
		(Connector (Concept "S") (ConnectorDir "+"))))

(Section
	(Concept "cat")
	(ConnectorSeq
		(Connector (Concept "W") (ConnectorDir "-"))
		(Connector (Concept "D") (ConnectorDir "-"))
		(Connector (Concept "MXs") (ConnectorDir "+"))
		(Connector (Concept "S") (ConnectorDir "+"))))

(Section
	(Concept ",")
	(ConnectorSeq
		(Connector (Concept "Xd") (ConnectorDir "+"))))

(Section
	(Concept ",")
	(ConnectorSeq
		(Connector (Concept "Xc") (ConnectorDir "-"))))

(Section
	(Concept ".")
	(ConnectorSeq
		(Connector (Concept "Xp") (ConnectorDir "-"))
		(Connector (Concept "Xf") (ConnectorDir "-"))))

(Section
	(Concept "black")
	(ConnectorSeq
		(Connector (Concept "A") (ConnectorDir "+"))))

(Section
	(Concept "white")
	(ConnectorSeq
		(Connector (Concept "A") (ConnectorDir "+"))))

(Section
	(Concept "lab")
	(ConnectorSeq
		(Connector (Concept "A") (ConnectorDir "-"))
		(Connector (Concept "Ds") (ConnectorDir "-"))
		(Connector (Concept "Xd") (ConnectorDir "-"))
		(Connector (Concept "MXs") (ConnectorDir "-"))
		(Connector (Concept "Xc") (ConnectorDir "+"))))

(Section
	(Concept "barked")
	(ConnectorSeq
		(Connector (Concept "WV") (ConnectorDir "-"))
		(Connector (Concept "S") (ConnectorDir "-"))
		(Connector (Concept "Xf") (ConnectorDir "+"))))

(Section
	(Concept "purred")
	(ConnectorSeq
		(Connector (Concept "WV") (ConnectorDir "-"))
		(Connector (Concept "S") (ConnectorDir "-"))
		(Connector (Concept "Xf") (ConnectorDir "+"))))
