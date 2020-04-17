;
; dict-loop.scm
;
; This defined a dictionary that generates four sentences, containing
; one cycle (loop). The loop occurs because there is an edge from the
; root to the leading verb, and an edge to the leading noun (the
; subject), as well as a link between the subject and the head-verb.
; Thus, these edges form a triangle.
;
; All four sentences have the form:
;
;        +-------WV------+----O-----+
;        +----W---+---S--+    +--D--+
;        |        |      |    |     |
;     LEFT-WALL John    saw   a    cat
;
; The other three substitute "Mary" for "John" and "dog" for "cat".
; The cycle is a triangle with edges (WV, S, W) and vertexes
; (LEFT-WALL, John, saw)
;
; The dictionary defines nine "jigsaw-puzzle pieces" in it. These
; assemble into the above graphs. It should be obvious from inspection
; how these are should be assembled. ... and, if its not ...
; The connector ID's must match, and the conector directions must
; be polar opposites. The edges connecting the vertexes are undirected
; edges; the ConnectorDir polarities instead indicate "to the left"
; and "to the right". In particular, all three edges in the triangle
; are undirected.
;
(use-modules (srfi srfi-1))
(use-modules (opencog))

; Each word in the dictionary is added by means of a MemberLink.
; The name of the dictionary here is `(Concept "dict-loop")`.
(define dict-loop (Concept "dict-loop"))

; To keep the example simple and readable, the membership is done
; in a giant-sized for-loop.
(for-each
   (lambda (sect) (Member sect dict-loop))
   (list

		(Section
			(Concept "LEFT-WALL")
			(ConnectorSeq
				(Connector (Concept "WV") (ConnectorDir "+"))
				(Connector (Concept "W") (ConnectorDir "+"))))

		(Section
			(Concept "John")
			(ConnectorSeq
				(Connector (Concept "W") (ConnectorDir "-"))
				(Connector (Concept "S") (ConnectorDir "+"))))

		(Section
			(Concept "Mary")
			(ConnectorSeq
				(Connector (Concept "W") (ConnectorDir "-"))
				(Connector (Concept "S") (ConnectorDir "+"))))

		(Section
			(Concept "saw")
			(ConnectorSeq
				(Connector (Concept "S") (ConnectorDir "-"))
				(Connector (Concept "WV") (ConnectorDir "-"))
				(Connector (Concept "O") (ConnectorDir "+"))))

		(Section
			(Concept "a")
			(ConnectorSeq
				(Connector (Concept "D") (ConnectorDir "+"))))

		(Section
			(Concept "cat")
			(ConnectorSeq
				(Connector (Concept "D") (ConnectorDir "-"))
				(Connector (Concept "O") (ConnectorDir "-"))))

		(Section
			(Concept "dog")
			(ConnectorSeq
				(Connector (Concept "D") (ConnectorDir "-"))
				(Connector (Concept "O") (ConnectorDir "-"))))

)) ; End of the for-each loop.
