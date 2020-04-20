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
; the location (key) under which the state-value will be stored.
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
					(RandomNumber (Number 0) (Number 1))
					(ValueOf (Variable "$A") susceptibility)))
			(SetValue (Variable "$A") seir-state infected)

			; If A is exposed, but has not become infected, then
			; A has a chance of washing off the exposure, and returning
			; to being susceptible but not infected. Basically, one
			; can't stay "exposed" forever; one either gets ill, or
			; one returns to original, unexposed state.
			; Use one minus the probability of getting infected.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "exposed"))
				(GreaterThan
					(RandomNumber (Number 0) (Number 1))
					(Minus (Number 1)
						(ValueOf (Variable "$A") susceptibility))))
			(SetValue (Variable "$A") seir-state susceptible)

			; If A is infected, then A has a chance of dying,
			; depending on the infirmity.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "infected"))
				(GreaterThan
					(RandomNumber (Number 0) (Number 1))
					(ValueOf (Variable "$A") infirmity)))
			(SetValue (Variable "$A") seir-state died)

			; If A is infected, then A has a chance of recovering,
			; depending on the infirmity.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "infected"))
				(GreaterThan
					(RandomNumber (Number 0) (Number 1))
					(ValueOf (Variable "$A") recovery)))
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
; two relationship types: "friend" and "stranger", the idea that links
; to friends are more likely to transmit disease, than links to
; strangers, as the individual is more likely to mingle and linger with
; friends, than with strangers.
;
; The code here just automates the generation of the grammar. The
; setting of susceptibilities and infirmities will be done after the
; network has been generated.

; This is the key under which the likelihood of creating this kind of
; network node will be stored. This is used to select nodes during the
; random network generation.
(define node-weight (Predicate "node likelihood"))

; This is the anchor-point to which the grammar will be attached.
; The grammar consists of a number of prototype individuals, having
; different numbers of friend and stranger encounters, occurring with
; different probability in the network.
(define prototypes (Concept "Prototype Individual Anchor Point"))

; Define a simple function that will create a "person prototype".
; This is a prototypical person, having a social network that
; consists of some number of friends, and some number of strangers
; that are regularly encountered. The network-generation code will
; take this "prototype person", and instantiate a number of
; individuals of this type, and join them into a social network.
(define (make-person-type Nfriends Nstrangers)
	; Give each prototype person a unique label.
	; This is not required, but its convenient.
	(define label (format #f "person-~D-~D" Nfriends Nstrangers))

	(Section
		(Concept label)
		(ConnectorSeq
			(make-list Nfriends
				(Connector (Concept "friend") (ConnectorDir "*")))
			(make-list Nstrangers
				(Connector (Concept "stranger") (ConnectorDir "*"))))))

; A doubly-nested loop to create network points (not yet people!)
; having some number of friends, and some number of strangers
; encountered during their daily lives. These points only become
; "actual individual people" when the network gets created. Here,
; the points have the form of "personality prototypes", not yet
; having been instantiated as individuals.
;
(for-each (lambda (num-friends)
	(for-each (lambda (num-strangers)
			(define person-type (make-person-type num-friends num-strangers))

			; The probability that an individual, with this number of
			; friend and stranger relationships, will appear in some
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
				(* (+ num-friends num-strangers) num-friends num-strangers)))
			(cog-set-value! person-type node-weight (FloatValue weight))
			(Member person-type prototypes)
		)

		; Allow for 3 to 11 strangers in the network.
		(iota 8 3)))

	; Allow for 1 to 7 friends in the network.
	(iota 6 1))

; Both friendships and stranger relationships are symmetrical,
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
(define point-set-anchor (Predicate "*-point-set-anchor-*"))

(define params (Concept "Simple Covid net parameters"))

; Generate a dozen random networks.
(State (Member max-solutions params) (Number 12))

; Nahh. Make that just one.
(State (Member max-solutions params) (Number 1))

; Always try to close off new unconnected connectors.
(State (Member close-fraction params) (Number 1.0))

; Avoid infinite recursion by quitting after some number of steps.
; iteration stops if the number of desired nets is found, or this
; number of steps is taken, whichever comes first.
; In the current implementation (this is subjecct to change!)
; either a solution is found within about 300 steps, or not at all.
; Thus, limit the search to just a bit more than 300, and try again,
; if no solution is found. (About half of all attempts succeed.
; Again, this is subject to change as the code is updated.)
(State (Member max-steps params) (Number 345))

; Allow large networks.
(State (Member max-depth params) (Number 100))
(State (Member max-network-size params) (Number 2000))

; Record all the individials that are created. This will be handy to
; have around, later.
(define anchor (Anchor "Covid Sim Individuals"))
(State (Member point-set-anchor params) anchor)

; An initial nucleation point, from which to grow the network.
; Multiple nucleation points can be used, but we use only one here.
; In this case, a person who encounters 2 friends and 3 strangers.
(define seed (gar (make-person-type 2 3)))

; Now, create the network.
(format #t "Start creating the network!\n")
(define start-time (get-internal-real-time))

(define (try-to-make-network)
	(define net
		(cog-random-aggregate pole-set prototypes node-weight params seed))
	(if (< 0 (cog-arity net)) net
		(try-to-make-network)))

(define network-set (try-to-make-network))

(define end-time (get-internal-real-time))
(format #t "Created ~D networks in ~6F seconds\n"
	(cog-arity network-set)
	(* 1.0e-9 (- end-time start-time)))

; Dump the first network found to a file, for visualization.
(define just-one (Set (gar network-set)))
(define just-one-gml (export-to-gml just-one))

;; Print the string to a file.
(let ((outport (open-file "/tmp/social-network.gml" "w")))
	(put-string outport just-one-gml)
	(close outport))

(format #t "Found a network of size ~D\n" (cog-arity (gar just-one)))

; ---------------------------------------------------------------------
; Assign initial suceptibility and infirmity values to individuals
; in the network. These will be randomly generated numbers. But first,
; we need a way of iterating over all of the individuals that were
; created for the random network.

; Create a list of all the prototype-persons in the dictionary.
(define proto-list
	(map gar (cog-incoming-by-type prototypes 'MemberLink)))

; Each individual is linked back to the prototype that it came from,
; so gather up a list of individuals.
(define individual-list
	(map gaar (append-map
		(lambda (proto) (cog-incoming-by-type proto 'InheritanceLink))
		proto-list)))

(format #t "The social network consists of ~D individuals\n"
	(length individual-list))

; Assign to each individual the state of "initially healthy", and
; some random numbers for susceptibility.
(for-each
	(lambda (individual)
		(cog-set-value! individual seir-state susceptible)
		(cog-set-value! individual susceptibility
			(cog-execute! (RandomNumber (Number 0.1) (Number 1))))
		(cog-set-value! individual infirmity
			(cog-execute! (RandomNumber (Number 0.2) (Number 0.8))))
		(cog-set-value! individual recovery
			(cog-execute! (RandomNumber (Number 0.3) (Number 0.7)))))
	individual-list)

; Pick one person, and make them infected
(cog-set-value! (car individual-list) seir-state infected)

; ---------------------------------------------------------------------
; Obtain a list of all friendship relations, and all encounters between
; strangers. We don't really need to have these (they can always be
; generated on the fly) but they're handy to look at and have around.

(define all-relations (remove-duplicate-atoms (append-map
	(lambda (individual) (cog-incoming-by-type individual 'SetLink))
	individual-list)))

(format #t "The social network consists of ~D pair-wise relationships\n"
	(length all-relations))

(define (get-relations-of-type RELATION)
	(filter-map (lambda (relpair)
		(equal? RELATION (gar (car
			(cog-incoming-by-type relpair 'EvaluationLink)))))
		all-relations))

(define friend-relations (get-relations-of-type (Concept "friend")))
(format #t "The social network consists of ~D freind-pairs\n"
	(length friend-relations))

(define stranger-relations (get-relations-of-type (Concept "stranger")))
(format #t "The social network consists of ~D stranger-pairs\n"
	(length stranger-relations))

; ---------------------------------------------------------------------
; Start applying state transition rules to the network.
; This will be done "by hand" for the first few rounds, just to show how
; it works. Several differnt styles will be shown. One is by directly
; running scheme code. A second, better way is by applying Atomese
; search-and-update rules directly to the atomspace contents. The
; second way is ultimately simpler, and provides better automation,
; but both ways are illustrated, for comparison.

; ------ But first, a handy utility:
; Execute some atomese. Assume that it returns a Setlink.
; Unwrap the SetLink, discard it, return the contents.
(define (exec-unwrap ATOMESE)
	(define set-link (cog-execute! ATOMESE))
	(define contents (cog-outgoing-set set-link))
	(cog-delete set-link)
	contents)

; ---------
; A function that runs the transmission step once. This is written
; using a scheme `for-each` loop to iterate over all encounters.
; It requires, as input, the list of all relations created above.
; As such, it is "static", depending on a static relationship list.
; Later in the demo, we'll replace this loop by a search over the
; Atomspace.
(define (run-one-transmission-step)

	; The transmission rule is directed, from first to second individual.
	; But the relation-pairs are undirected, so we have to run the rule
	; twice, swapping the individuals the second time.
	(for-each
		(lambda (relation)
			(exec-unwrap (Put (DefinedSchema "transmission")
				(List (gar relation) (gdr relation)))))
		all-relations)

	; And now swapped ...
	(for-each
		(lambda (relation)
			(exec-unwrap (Put (DefinedSchema "transmission")
				(List (gdr relation) (gar relation)))))
		all-relations)
)

; Obtain a list of individuals in a given state.
(define (get-individuals-in-state STATE)
	(exec-unwrap
		(Get
			(TypedVariable (Variable "$indiv") (Type "ConceptNode"))
			(Equal (ValueOf (Variable "$indiv") seir-state) STATE))))

; A function that reports the current stats on the population.
(define (report-stats)
	(format #t
		"Exposed: ~D    Infected: ~D   Recovered: ~D  Died: ~D  of Total: ~D\n"
		(length (get-individuals-in-state exposed))
		(length (get-individuals-in-state infected))
		(length (get-individuals-in-state recovered))
		(length (get-individuals-in-state died))
		(length individual-list))
	*unspecified*)

; A function that rolls the dice, and updates state: looping over
; all individuals, to see if they got infected, recovered, or died.
; As before, this uses a `for-each` loop in scheme to loop over all
; individuals.
(define (run-one-state-transition)
	(for-each
		(lambda (individual)
			(exec-unwrap (Put (DefinedSchema "state transition")
				individual)))
		individual-list))

; Now, actually run a few iterations, and see what happens.
(run-one-transmission-step)
(report-stats)
(run-one-state-transition)
(report-stats)

(run-one-transmission-step)
(report-stats)
(run-one-state-transition)
(report-stats)

; ---------------------------------------------------------------------
; State transitions, applied as rewrite rules, run over the AtomSpace.
; Instead of iterating over scheme lists of relations and individuals,
; this searches the AtomSpace for the relevant relations/individuals.

; Search for all pairs, and apply the transmission rule.
(define (do-transmission)
	(exec-unwrap
		(Bind
			(VariableList
				(TypedVariable (Variable "$pers-a") (Type "ConceptNode"))
				(TypedVariable (Variable "$pers-b") (Type "ConceptNode")))
			(Present
				(Set (Variable "$pers-a") (Variable "$pers-b")))
			(Put (DefinedSchema "transmission")
				(List (Variable "$pers-a") (Variable "$pers-b")))))
	*unspecified*)

; Search for individuals, and apply the state transition rule.
; Make use of the anchor point (defined above) to which all of the
; individuals are attached. Doing this greatly simplifies finding
; all of them.
(define (do-state-transition)
	(exec-unwrap
		(Bind
			(TypedVariable (Variable "$person") (Type "ConceptNode"))
			(Present (Member (Variable "$person") anchor))
			(Put (DefinedSchema "state transition")
				(Variable "$person"))))
	*unspecified*)

(do-transmission)     (report-stats)
(do-state-transition) (report-stats)

; Place the above in a loop that terminates only when the number
; of exposed and infected individuals drops to zero.
(define (loop)
	(do-transmission)     (report-stats)
	(do-state-transition) (report-stats)
	(if (and
		(= 0 (length (get-individuals-in-state exposed)))
		(= 0 (length (get-individuals-in-state infected))))
		(format #t "Finished simulation\n")
		(loop)))

; Run the loop.  This may take a while...
; (loop)

(display "Now say `(loop)` to run the rest of the simulation automatically\n")

; ---------------------------------------------------------------------
; The end.

*unspecified*
; ---------------------------------------------------------------------
