;
; seir.scm
;
; Demo of random networks to encode a SEIR Covid-19 simulation.
;
; SEIR == "susceptible", "exposed", "infected", and "recovered".
;
; This is a demo, only! It demonstrates several different AtomSpace
; programming techniques, one of which is the brand-new (version 0.1)
; random network generator. Another is Value Flows: the use of Values
; for holding mutable state, and flowing that state through a network.
; It is assumed that the reader is already familiar with the basics of
; the OpenCog AtomSpace! This demo *could* be enhanced to do actual
; scientific exploration; see "making this useful", below.
;
; The demo generates a random "social network", consisting of two types
; of relationships: "friends" and "strangers". It adopts a simple SEIR
; model of disease transmission and progression along this network.
; The model itself is a (simple) hand-coded state-transition machine.
;
; There's a blog post here:
;
;    https://blog.opencog.org/2020/04/22/covid-19-modelling-and-random-social-networks/
;
; The blog post touches on the high-lites of the demo, avoiding all of
; the grunge needed to get actual, working code. Thus, reading it is
; recommended before diving into the code below.
;
; If you're lazy and just want to run the demo, then just start the
; guile shell, load this file, and ... the demo will run.
;
; Making this Useful
; ------------------
; This is a demo, not a final finished product.
;
; There's no whizzy visualization of the disease progression, or the
; resulting stats. You can visualize snapshots of the network with
; CytoScape or Gephi, but it won't be animated as the disease spreads.
;
; There's no statistical analysis performed, and no graphs or curves are
; drawn. The demo shows how to generate the raw data, not how to analyze
; raw data. There are plenty of tools designed for statistical analysis.
; Use those.
;
; A few notes about scheme, python and Atomese. This demo would appear
; to be written ins scheme. It could have just as eassily been written
; in python, without changing it's basic form (the AtomSpace has python
; bindings.) It may seem hard to beleive, but the choice of language
; doesn't really much matter. That's because all of the hard work is
; done in Atomese.
;
; Atomese is a graphical programming language, and it was designed for
; automation. Atomese expressions are graphs; they live as graphs in the
; AtomSpace graph database. Atomese encodes a type of "Abstract Syntax
; Tree" (see https://en.wikipedia.org/wiki/Abstract_syntax_tree) and
; such a representation is useful for all of the reasons that the
; Wikipedia article says they are. But it also means that "coding" in
; Atomese can sometimes feel unusual. Like: "Why can't I just code in
; python? It would be so much easier!" -- Well, but that misses the
; point. And the point is, again: Atomese was designed for automation.
;
; What this really means is that the graphs are meant to be easy for other
; algorithms to manipulate. It's designed for ease-of-use by machines,
; not for humans! The style is more barren, unadorned, without
; short-cuts. Its a bit verbose at times. That's OK, machines don't mind
; verbosity and tedium.
;
; Consider, for example, a graphical editor - something where you can
; drag-n-drop, hand-draw interacting bubble diagrams. Something easy
; to use - something a medical professional could get the hang of in
; a few hours - something where a disease model could be sketched out
; with bubbles and lines and flow-charts. The goal of Atomese is that
; it becomes easy -- really easy -- to convert such diagrams into
; executable code. That's what Atomese is designed to do.
;
; In this demo, the disease progression model is written in Atomese.
; This is the model that a graphical GUI WYSIWYG whizz-bang system
; would generate.
;
; Atomese enables other, more abstract approaches. Consider an automatic
; model explorer: a system that automatically creates a variety of
; different disease progression models, and then explores how each
; model works. The model generator might try to fit an existing dataset.
; Alternately, it could mutate models so as to obtain certain
; characteristics.  This moves beyond just adjusting some parameters
; in some hand-created, hypothesized model. This is not just some
; monte-carlo search or machine-learning hill-climbing for parameter
; tuning.  This is a whole-sale creation of previously non-existent
; code. Atomese allows software to read, understand, mutate, modify
; and write programs.
;
; Thus, in reading the below, keep in mind that there is nothing special
; about the SEIR model; its a stand-in for what could be a generic,
; arbitrary state transition machine. Such machines can be hand-coded,
; of course, but the interesting application is when they are generated
; from other sources.
;
; Last but not least: keep in mind that the network generator is
; currently at version 0.1, and is not yet as versatile, flexible and
; powerful as it could be. The parameter settings below generate
; networks that are long and thin. One such network can be seen in
; the included image.
; ---------------------------------------------------------------------

; Import needed modules
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))
(use-modules (opencog generate))
(use-modules (ice-9 textual-ports))

; ---------------------------------------------------------------------
; Scheme short-hand for five possible disease states.
(define susceptible (Concept "susceptible"))
(define exposed (Concept "exposed"))
(define infected (Concept "infected"))
(define recovered (Concept "recovered"))
(define died (Concept "died"))

; State will be stored as Atomese Values, placed at certain locations
; (keys). Every Atom is a key-value database; the below defines the keys
; where the Values (attributes) will be stored.
;
; Predicate under which the SEIR state will be stored.
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
; It takes two arguments: two people, A and B, with A becoming
; exposed if B is infected. It is unidirectional.  This is a
; deterministic state change: there's no "probability of exposure",
; one simply is, or isn't. The probabilities are in the next step.
(Define
	(DefinedSchema "transmission")
	(Lambda
		(VariableList (Variable "$A") (Variable "$B") (Variable "$REL"))
		(Cond
			; If A is susceptible and B is infected, then maybe
			; A is exposed, if A gets close enough/spends enough
			; time with B. A passing stranger is unlikely to
			; lead to exposure; but friends are harder to avoid
			; and are thus more likely to lead to exposure.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) susceptible)
				(Equal (ValueOf (Variable "$B") seir-state) infected)
				(Or
					(And
						(Equal (Variable "$REL") (Concept "friend"))
						(GreaterThan
							(RandomNumber (Number 0) (Number 1))
							(Number 0.3)))
					(And
						(Equal (Variable "$REL") (Concept "stranger"))
						(GreaterThan
							(RandomNumber (Number 0) (Number 1))
							(Number 0.7)))))
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
			; distribution between the two indicated bounds.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) exposed)
				(GreaterThan
					(ValueOf (Variable "$A") susceptibility)
					(RandomNumber (Number 0) (Number 1))))
			(SetValue (Variable "$A") seir-state infected)

			; If A is exposed, but has not become infected, then
			; A has a chance of washing off the exposure, and returning
			; to being susceptible but not infected. Basically, one
			; can't stay "exposed" forever; one either gets ill, or
			; one returns to original, unexposed state.
			; Use one minus the probability of getting infected.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) exposed)
				(GreaterThan
					(Minus (Number 1)
						(ValueOf (Variable "$A") susceptibility))
					(RandomNumber (Number 0) (Number 1))))
			(SetValue (Variable "$A") seir-state susceptible)

			; If A is infected, then A has a chance of dying,
			; depending on the infirmity.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) infected)
				(GreaterThan
					(ValueOf (Variable "$A") infirmity)
					(RandomNumber (Number 0) (Number 1))))
			(SetValue (Variable "$A") seir-state died)

			; If A is infected, then A has a chance of recovering,
			; depending on the recovery-health strength.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) infected)
				(GreaterThan
					(ValueOf (Variable "$A") recovery)
					(RandomNumber (Number 0) (Number 1))))
			(SetValue (Variable "$A") seir-state recovered)
	))
)

; ---------------------------------------------------------------------
; The above state-transmission model was verbose.  Just a tiny-bit of
; script-fu can clean it up and make it easier to read. Lets do that.
;
; Define a wrapper for each of the state-transition clauses
(define (condition CURRENT-STATE NEXT-STATE DISTRIBUTION)
	(list
		(And
			(Equal (ValueOf (Variable "$A") seir-state) CURRENT-STATE)
			(GreaterThan
				(ValueOf (Variable "$A") DISTRIBUTION)
				(RandomNumber (Number 0) (Number 1))))
		(SetValue (Variable "$A") seir-state NEXT-STATE)))

; Define a wrapper, just like above, but instead using (1-probability)
(define (inverted CURRENT-STATE NEXT-STATE DISTRIBUTION)
	(list
		(And
			(Equal (ValueOf (Variable "$A") seir-state) CURRENT-STATE)
			(GreaterThan
				(Minus (Number 1) (ValueOf (Variable "$A") DISTRIBUTION))
				(RandomNumber (Number 0) (Number 1))))
		(SetValue (Variable "$A") seir-state NEXT-STATE)))

; Here's the easier-to-read version. So Atomese is verbose, but
; that's not hard to hide behind some wrappers.
(Define
	(DefinedSchema "alt version of state transition")
	(Lambda
		(Variable "$A")
		(Cond
			(condition exposed  infected    susceptibility)
			(inverted  exposed  susceptible susceptibility)
			(condition infected died        infirmity)
			(condition infected recovered   recovery))))

; ---------------------------------------------------------------------
; Generate a random social network. This starts by defining a grammar
; that will express the network.
;
; The generated network will be a fixed, static networks of individuals.
; Why use a static network? To better model geographical isolation.
; Thus, for example, a person who mostly stays home is likely to
; encounter only three people: other family members. A person who is
; highly social might meet dozens of people - now, this does NOT mean
; that the meetings happen every day - they may be very infrequent.
; But its usually the same dozen people, from the same neighborhood
; - even, maybe the same set of random strangers in the grocery store.
; The static network captures this geographical locality, and the
; general invariance of the potential network.
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
;
; ----
; Network Generation parameters.

; The key identifying the likelihood of creating a particular kind of
; network node.  This weighting controls the selection of the particular
; node-type.
(define node-weight (Predicate "node likelihood"))

; The anchor-point to which the grammar will be attached. The grammar
; consists of a number of prototype individuals, having different
; numbers of friend and stranger encounters, occurring with different
; probability in the network.
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
			; now. Its not even the expectation value, not really, its
			; just a weight controlling the likelihood of such a
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
; mutually reciprocal.
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

; Always try to close off new, unconnected connectors.
(State (Member close-fraction params) (Number 1.0))

; Avoid infinite recursion by quitting after some number of steps.
; Iteration stops if the number of desired nets is found, or this
; number of steps is taken, whichever comes first.
; In the current implementation (this is subject to change!)
; either a solution is found within about 300 steps, or not at all.
; Thus, limit the search to just a bit more than 300, and try again,
; if no solution is found. (About half of all attempts succeed.
; Again, this is subject to change as the code is updated.)
(State (Member max-steps params) (Number 345))

; Allow large networks.
(State (Member max-depth params) (Number 100))
(State (Member max-network-size params) (Number 2000))

; Record all the individuals that are created. This will be handy to
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

(format #t "Found a network of ~D individuals\n" (cog-arity (gar just-one)))

; ---------------------------------------------------------------------
; A handy utility.  We need this for the rest of the demo.

; Execute some Atomese. Assume that it returns a SetLink.
; Unwrap the SetLink, discard it, return the contents.
(define (exec-unwrap ATOMESE)
	(define set-link (cog-execute! ATOMESE))
	(define contents (cog-outgoing-set set-link))
	(cog-delete set-link)
	contents)

; ---------------------------------------------------------------------
; Initialize the network.
;
; Assign to each individual the state of "initially healthy".
; Choose some random numbers for the overall population health:
; some random susceptibility (the probability of becoming ill after
; being exposed), the infirmity (probability of illness leading to
; death) and a probability of recovering from illness.
;
; The initialization is done with a graph-query and graph-rewrite
; run over the AtomSpace. The query is simple: find all individuals
; attached with a `MemberLink` to an anchor node. The rewrite
; consists of the 3rd and subsequent clauses: all of the SetValues.

(define (initialize-state)
	(exec-unwrap
		(Bind
			(TypedVariable (Variable "$person") (Type "ConceptNode"))
			(Present (Member (Variable "$person") anchor))
			(SetValue (Variable "$person") seir-state susceptible)
			(SetValue (Variable "$person") susceptibility
				(RandomNumber (Number 0.2) (Number 0.8)))
			(SetValue (Variable "$person") infirmity
				(RandomNumber (Number 0.01) (Number 0.55)))
			(SetValue (Variable "$person") recovery
				(RandomNumber (Number 0.6) (Number 0.95)))
			))
	*unspecified*)

; Run the above.
(initialize-state)

; ---------------------------------------------------------------------
; Reporting statistics.
;
; Two reports are provided. The first describes the structure of the
; generated network: the number of friend and stranger relationships
; in the network. The second describes the progression of the disease
; state.
;
; Obtain a list of pairs of interacting individuals, who interact either
; through friendship or stranger encounters.
(define (get-relations-of-type RELATION)
	(exec-unwrap
		(Get
			(TypedVariable (Variable "$pair") (Type "SetLink"))
			(Present (Evaluation RELATION (Variable "$pair"))))))

(format #t "The social network consists of ~D friend-pairs\n"
	(length (get-relations-of-type (Concept "friend"))))

(format #t "The social network consists of ~D stranger-pairs\n"
	(length (get-relations-of-type (Concept "stranger"))))

; ---------
; Obtain a list of individuals in a given state.
(define (get-individuals-in-state STATE)
	(exec-unwrap
		(Get
			(TypedVariable (Variable "$indiv") (Type "ConceptNode"))
			(And
				(Present (Member (Variable "$indiv") anchor))
				(Equal (ValueOf (Variable "$indiv") seir-state) STATE)))))

; Initially, all individuals are unexposed. Just get a list of them.
(define all-individuals (get-individuals-in-state susceptible))

; A function that reports the current stats on the population.
(define (report-stats)
	(format #t
		"Exposed: ~D    Infected: ~D   Recovered: ~D  Died: ~D  of Total: ~D\n"
		(length (get-individuals-in-state exposed))
		(length (get-individuals-in-state infected))
		(length (get-individuals-in-state recovered))
		(length (get-individuals-in-state died))
		(length all-individuals))
	*unspecified*)

; ---------------------------------------------------------------------
; Apply state transition rules to the network.
;
; The state transition rules drive the actual simulation. These rules
; are applied as graph rewrite rules, running over the AtomSpace.
; There are two rules to apply. One rule that runs over all pairs,
; transmitting disease from one individual to another. The other rule
; updates the individual state, from exposed to infected to resolved.

; Search for all pairs, and apply the transmission rule.
(define (do-transmission)
	(exec-unwrap
		(Bind
			(VariableList
				(TypedVariable (Variable "$pers-a") (Type "ConceptNode"))
				(TypedVariable (Variable "$pers-b") (Type "ConceptNode"))
				(TypedVariable (Variable "$relation") (Type "ConceptNode")))
			(Present
				(Evaluation
					(Variable "$relation")
					(Set (Variable "$pers-a") (Variable "$pers-b"))))
			(Put (DefinedSchema "transmission")
				(List
					(Variable "$pers-a")
					(Variable "$pers-b")
					(Variable "$relation")))))
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

; Oh, but first, pick one person, and make them infected!
(define everyone (get-individuals-in-state susceptible))
(define one-person (first everyone))
(cog-execute! (SetValue one-person seir-state infected))

; Start the simulation. Run several steps by hand, to get the
; hang of it.
(do-transmission)     (report-stats)
(do-state-transition) (report-stats)

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
