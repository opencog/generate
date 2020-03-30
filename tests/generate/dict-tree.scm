;
; dict-tree.scm
;
; Tree test: dictionary that allows four sentences, as trees (acyclic)
;
;                        +----O-----+
;        +----W---+---S--+    +--D--+
;        |        |      |    |     |
;     LEFT-WALL John    saw   a    cat
;
; The other three substitute "Mary" for "John" and "dog" for "cat".
; All four possible sentences are generated.
;
; The dictionary defines nine "jigsaw-puzzle pieces" in it.
; These assemble into the above trees. It should be obvious from
; inspection how these are should be assembled. ... and, if its not
; ... the connector ID's must match, and the conector directions must
; be polar opposites. The edges connecting the vertexes are undirected
; edges; the ConnectorDir polarities instead indicate "to the left" and
; "to the right".  So, for example, "cat" has two "-" ConnectorDir's
; so that it will directionally link to the left for both words. This
; differs from what one might expect in a dependency grammar, where
; the O (object) link would indicate a head-dependent arrow from
; verb to object, and the D (determiner) link would indicate a
; head-dependent arrow from noun to the determiner. None of this
; matters for this particular example; here, all edges are undirected.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(define left-wall (Concept "LEFT-WALL"))

; A test dictionary of basic data.
(Section
	(Concept "LEFT-WALL")
	(ConnectorSeq
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
