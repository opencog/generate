;
; seir.scm
;
; Random network demo featuring a SEIR Covid-19 simulation.
;
; SEIR == "susceptible", "exposed", "infected", and "recovered".
;
; Model with a state-transition machine. The state will be kept
; in a specific value.

(use-modules (opencog) (opencog exec))

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
				(Equal (ValueOf (Variable "$A") seir-state) susceptible))
				(Equal (ValueOf (Variable "$B") seir-state) infected)))
			(SetValue (Variable "$A") seir-state exposed))
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
			; number generator. RandomLink generates a uniform
			; distribution.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "exposed"))
				(GreaterThan
					(Times
						(ValueOf (Variable "$A") susceptibility)
						(Random (Number 0) (Number 1)))
					(Number 0.5)))
			(SetValue (Variable "$A") seir-state infected)

			; If A is infected, then A has a chance of dying,
			; depending on the infirmity.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "infected"))
				(GreaterThan
					(Times
						(ValueOf (Variable "$A") infirmity)
						(Random (Number 0) (Number 1)))
					(Number 0.5)))
			(SetValue (Variable "$A") seir-state died)

			; If A is infected, then A has a chance of recovering,
			; depending on the infirmity.
			(And
				(Equal (ValueOf (Variable "$A") seir-state) (Concept "infected"))
				(GreaterThan
					(Times
						(ValueOf (Variable "$A") recovery)
						(Random (Number 0) (Number 1)))
					(Number 0.5)))
			(SetValue (Variable "$A") seir-state recovered)
	))
)

; ---------------------------------------------------------------------
; A grammar that defines a random network.

; ---------------------------------------------------------------------
(define person-a (Concept "person a"))
(define person-b (Concept "person b"))

(cog-execute!  (SetValue person-a seir-state susceptible))
(cog-execute!  (SetValue person-b seir-state susceptible))
(cog-execute!  (SetValue person-b seir-state infected))

(cog-set-value! person-a infirm-state (FloatValue 0.6))

(cog-execute! (Put trans (List person-a person-b)))
; ---------------------------------------------------------------------
