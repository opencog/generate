;
; trisexual.scm
;
; This demo provides a very short discussion of how connector directions
; can be specified. Three examples are given: mono-sexual, heterosexual
; and tri-sexual. The word "sexual" is used because of the obvious
; (hetero) "sexuality" of real-world cardboard jigsaw puzzle connectors. 
;
; Other parts of the code call this "polarity", and so the three examples
; could be called mono-polar, bipolar and tri-polar. The concept of "mating"
; applies for either vocabulary. "Joining" and "linking" are synonymous
; ideas. Different blocks of code use these words interchangeably.
;
(use-modules (opencog))
;
; --------------------------------------
; The mono-sexual example occurs in the `basic-network.scm` example.
; Please review that example. The relevant section is cut-n-pasted
; below:
;
; Define the connectable directions (poles). In this case, the
; connectors are all the same, and the direction (or "pole")
; connects to itself (rather than having opposite polarity).
; Furthermore, the connections are symmetric (creating undirected
; edges.) Thus, the pole-pair set is very simple.
(define polarity-set (Concept "any to any"))
(Member (Set (ConnectorDir "*") (ConnectorDir "*")) polarity-set)

; --------------------------------------
; The hetero-sexual example occurs in the `dict-*.scm` examples, and is
; given in the file `grammar.scm` near the middle. The relevant section
; is cut-n-pasted below:
;
; Define the connectable directions (poles). In this case, there
; are two direction types: "+" and "-" denoting "to the right" and
; "to the left". The connector mating rules are that "+" can connect
; only to "-", and vice-versa. The resulting edges are NOT directed
; edges -- they are still undirected edges; the poles are only used
; to indicate word-order in the generated sentence. This convention of
; plus and minus for word-order is in keeping with the representation
; used in the Link Grammar parser. The notation used here should look
; very familiar, when compared to the notaton there. This is not
; accidental.
;
; All of the example dictionaries use this same set of direction rules.
; Its very simple: only "+" and "-" connections are allowed.
;
(define dir-set (Concept "left-right connectors"))
(Member (Set (ConnectorDir "+") (ConnectorDir "-")) dir-set)

; Please keep in mind that a `Set` in Atomese is an unordered set. Thus,
; the expression `(Set (ConnectorDir "+") (ConnectorDir "-"))` and also
; `(Set (ConnectorDir "-") (ConnectorDir "+"))` are exactly the same set.
; There is no need to specify both orderings.
;
; The above just says: "opposites attract".

; --------------------------------------
; The tri-sexual example follows below. It is not currently used in any
; demos; the reader is encouraged to construct one.
;
; The example here is that there are three sexes, `a`, `b` and `c`. The
; mating rules are such that sex `a` can mate with `b`, but nothing else,
; so `a` cannot mate to other `a`'s nor to `c`. The sex `c` can also mate
; with `b`, but not to itself. Finally, `b` cannot mate with itself. This
; idea is encoded in the following rules:

(define sexuality-set (Concept "abc example"))
(Member (Set (ConnectorDir "a") (ConnectorDir "b")) sexuality-set)
(Member (Set (ConnectorDir "c") (ConnectorDir "b")) sexuality-set)

; --------------------------------------
; All three examples above are quite simple; fairly general mating rules
; can be encoded following the above pattern.
;
; That's all, folks!
