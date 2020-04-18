;
; seir.scm
;
; Demo of random networks to encode a SEIR Covid-19 simulation.
;
; SEIR == "susceptible", "exposed", "infected", and "recovered".
;
; Model with a state-transition machine. The state will be kept
; in a specific value.

(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))
(use-modules (opencog generate))
(use-modules (ice-9 textual-ports))

; ---------------------------------------------------------------------
; Scheme short-hand for five possible states. This serves no particular
; purpose, other than to be slightly less verbose when writing later
; expressions.
(define susceptible (Concept "susceptible"))
(define exposed (Concept "exposed"))
(define infected (Concept "infected"))
(define recovered (Concept "recovered"))
(define died (Concept "died"))

; Predicate under which the SEIR state will be stored. This marks
; the location (key) under which the state-value willl be stored.
(define seir-state (Predicate "SEIR state"))

; Predicate under which a susceptibility weight will be stored.
; This weight, a floating point number between zero and one,
; modulates the probability of becoming infected upon exposure.
(define susceptibility (Predicate "Susceptibility weight"))

; Predicate under which an infirmity weight will be stored.
; This weight, a floating point number between zero and one,
; modulates the probability of dying after infection.
(define infirmity (Predicate "Infirmity weight"))

; Predicate under which a recovery weight will be stored.
; This weight, a floating point number between zero and one,
; modulates the probability of recovering after infection.
(define recovery (Predicate "Recovery weight"))

; ---------------------------------------------------------------------
; A function that implements the transmission of the disease.
; It takes two arguments: two people, A and B, with A being
; exposed if B is infected. It is unidirectional.
(Define
	(DefinedSchema "transmission")
	(Lambda
		(VariableList (Variable "$A") (Variable "$B"))
		(Cond
			; If A is susceptible and B is infected, then A is exposed.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) susceptible)
				(Equal (ValueOf (Variable "$B") seir-state) infected))
			(SetValue (Variable "$A") seir-state exposed)
		)))

; A function that implements state transitions of an individual.
; It takes one argument: a person, and then walks that person
; through various states, with a probabilistic change of state.
(Define
	(DefinedSchema "state transition")
	(Lambda
		(Variable "$A")
		(Cond
			; If A is exposed, and is infirm, then A is infected.
			; The probability of infection is controlled by a random
			; number generator. RandomNumberLink generates a uniform
			; distribution.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "exposed"))
				(GreaterThan
					(Times
						(ValueOf (Variable "$A") susceptibility)
						(RandomNumber (Number 0) (Number 1)))
					(Number 0.5)))
			(SetValue (Variable "$A") seir-state infected)

			; If A is infected, then A has a chance of dying,
			; depending on the infirmity.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "infected"))
				(GreaterThan
					(Times
						(ValueOf (Variable "$A") infirmity)
						(RandomNumber (Number 0) (Number 1)))
					(Number 0.5)))
			(SetValue (Variable "$A") seir-state died)

			; If A is infected, then A has a chance of recovering,
			; depending on the infirmity.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "infected"))
				(GreaterThan
					(Times
						(ValueOf (Variable "$A") recovery)
						(RandomNumber (Number 0) (Number 1)))
					(Number 0.5)))
			(SetValue (Variable "$A") seir-state recovered)
	))
)

; ---------------------------------------------------------------------
; A grammar that defines a random network. This grammar will be used
; to generate fixed, static networks of individuals. Why use a static
; network? To better model geographical isolation. Thus, for example,
; a person who mostly stays home is likely to encounter only three
; people: other family members. A person who is highly social might
; meet dozens of people - now, this does NOT mean that the meetings
; happen every day - they may be very infrequent. But its usually
; the same dozen people, from the same neighborhood - even, maybe
; random strangers in the grocery store.  The static network captures
; this locality, and the general invariance of the potential network.
;
; The grammar used here is quite simple: it is just a minor twist on
; the grammar used in the `basic-network.scm` example. It introduces
; two relatinoship types: "freind" and "stranger", the idea that links
; to freinds are more likely to transmit disease, than links to
; strangers, as the individual is more likely to mingle and linger with
; freinds, than with strangers.
;
; The code here just automates the generation of the grammar. The
; setting of susceptibilities and infirmities will be done after the
; network has been generated.

; This is the key under which the liklihood of creating this kind of
; network node will be stored. This is used to selet nodes during the
; random network generation.
(define node-weight (Predicate "node liklihood"))

; This is the anchor-point to which the grammar will be attached.
; The grammar consists of a number of prototype individuals, having
; different numbers of freind and stranger encounters, occuring with
; different probability in the network.
(define prototypes (Concept "Prototype Individual Anchor Point"))

; A doubly-nested loop to create network points (not yet people!)
; having some number of freinds, and some number of strangers
; encountered during their daily lives. These points only become
; "actual individual people" when the network gets created. Here,
; the points have the form of "personality prototypes", not yet
; having been instantiated as individuals.
;
(for-each (lambda (num-freinds)
	(for-each (lambda (num-strangers)
			; Give each prototype person a unique label.
			; This is not required, but its convenient.
			(define label (format #f "person-~D-~D"
				num-freinds num-strangers))
			(define person-type
				(Section
					(Concept label)
					(ConnectorSeq
						(make-list num-freinds
							(Connector (Concept "freind") (ConnectorDir "*")))
						(make-list num-strangers
							(Connector (Concept "stranger") (ConnectorDir "*"))))))

			; The probability that an individual, with this number of
			; freind and stranger relationships, will appear in some
			; typical random network. This is just a cheesy hack, for
			; now. Its not even the expectation,value, not really, its
			; just a weighting controlling the likelihood of such
			; prototype being selected, during network generation. The
			; weighting chosen here punishes prototypes with many
			; relationships. At this time, this is needed to force the
			; search to terminate reasonably quickly, as otherwise, the
			; current search algos will generate unboundedly large
			; incomplete networks, without closing off open connections.
			; Future work should improve the situation. Recall, we are
			; at version 0.1 of the network generation code, right now.
			(define weight (/ 1.0
				(* (+ num-freinds num-strangers) num-freinds num-strangers)))
			(cog-set-value! person-type node-weight (FloatValue weight))
			(Member person-type prototypes)
		)

		; Allow for 3 to 11 strangers in the network.
		(iota 8 3)))

	; Allow for 1 to 7 freinds in the network.
	(iota 6 1))

; Both freindships and stranger relationships are symmetrical,
; any-to-any.
(define pole-set (Concept "any to any"))
(Member (Set (ConnectorDir "*") (ConnectorDir "*")) pole-set)

; The below-defined names MUST be defined as given.
; See the file `examples/parameters.scm` for details.
(define max-solutions (Predicate "*-max-solutions-*"))
(define close-fraction (Predicate "*-close-fraction-*"))
(define max-steps (Predicate "*-max-steps-*"))
(define max-depth (Predicate "*-max-depth-*"))
(define max-network-size (Predicate "*-max-network-size-*"))

(define params (Concept "Simple Covid net paramaters"))

; Generate a dozen random networks.
(State (Member max-solutions params) (Number 12))

; Always try to close off new unconnected connectors.
(State (Member close-fraction params) (Number 1.0))

; Avoid infinite recursion by quiting after some number of steps.
; iteration stopps if the number of desired nets is found, or this
; number of steps is taken, whichever comes first.
(State (Member max-steps params) (Number 523123))

; Allow large networks.
(State (Member max-depth params) (Number 100))
(State (Member max-network-size params) (Number 2000))

; An initial nucleation point, from which to grow the network.
; Multiple nucleation points can be used, but we use only one here.
; In this case, a person who encounters 2 freinds and 3 strangers.
(define seed (Concept "person-2-3"))

; Now, create the network.
(format #t "Start creating the network!\n")
(define network-set
	(cog-random-aggregate pole-set prototypes node-weight params seed))

(format #t "Created ~D networks\n" (cog-arity network-set))
(format #t "The network sizes are ~A\n"
	(map cog-arity (cog-outgoing-set network-set)))

; Dump the first network found to a file, for visualization.
(define just-one (Set (gar network-set)))
(define just-one-gml (export-to-gml just-one))

;; Print the string to a file.
(let ((outport (open-file "/tmp/social-network.gml" "w")))
	(put-string outport just-one-gml)
	(close outport))


; ---------------------------------------------------------------------
; Some validation and debugging tools.
(define person-a (Concept "person a"))
(define person-b (Concept "person b"))

(cog-execute!  (SetValue person-a seir-state susceptible))
(cog-execute!  (SetValue person-b seir-state susceptible))
(cog-execute!  (SetValue person-b seir-state infected))

(cog-set-value! person-a infirmity (FloatValue 0.6))

; (cog-execute! (Put trans (List person-a person-b)))

*unspecified*
; ---------------------------------------------------------------------
