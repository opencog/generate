;
; dict-tree.scm
;
; This demo shows how to construct a lexis that will generate (acyclic)
; trees with typed edges. The example uses a simple English-language
; grammar to generate four "sentences". The all have the form:
;
;                        +----O-----+
;        +----W---+---S--+    +--D--+
;        |        |      |    |     |
;     LEFT-WALL John    saw   a    cat
;
; The other three substitute "Mary" for "John" and "dog" for "cat".
; All four possible sentences are generated.
;
; The dictionary defines nine "jigsaw-puzzle pieces" in it.  These
; assemble into the above trees. It should be obvious by inspection how
; these should be assembled. ... and, if its not ... then:
;
; The connector ID's must match, and the conector directions must be
; polar opposites. The edges connecting the vertexes are undirected
; edges; the ConnectorDir polarities instead indicate "to the left" and
; "to the right".  So, for example, "cat" has two "-" ConnectorDir's
; so that it will directionally link to the left for both words. This
; differs from what one might expect in a dependency grammar, where
; the O (object) link would indicate a head-dependent arrow from
; verb to object, and the D (determiner) link would indicate a
; head-dependent arrow from noun to the determiner. Although the above
; describes the dependency relations as arrows, in this particular
; example, the actual edges are undirected.
;
; Each "word" is a point in the section. Each section indicates the
; kinds of connectors it can attach to.
;
(use-modules (srfi srfi-1))
(use-modules (opencog))

; Each word in the dictionary is added by means of a MemberLink.
; The name of the dictionary here is `(Concept "dict-tree")`.
(define dict-tree (Concept "dict-tree"))

; To keep the example simple and readable, the membership is done
; in a giant-sized for-loop.
(for-each
	(lambda (sect) (Member sect dict-tree))
	(list

		;; All sentences start with the "left wall" -- this is a
		;; "word" at the start of the sentence, which serves as the
		;; root to which subsequent words will be attached.
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

)) ; End of the for-each loop.
