;
; seir.scm
;
; Random network demo featuring a SEIR Covid-19 simulation.
;
; SEIR == "susceptible," "exposed," "infected," and "recovered." 
; Model with a state-transition machine. The state will be kept
; in a specific value.

(use-modules (opencog) (opencog exec))

(define person-a (Concept "person a"))
(define person-b (Concept "person b"))
(define seir-state (Predicate "SEIR state"))
(define infirm-state (Predicate "Infirmity state"))

(cog-execute!
	(SetValue person-a seir-state (Concept "susceptible")))
(cog-execute!
	(SetValue person-b seir-state (Concept "susceptible")))
(cog-execute!
	(SetValue person-b seir-state (Concept "infected")))

(cog-set-value! person-a infirm-state (FloatValue 0.6))

(define trans
(Lambda
	(VariableList (Variable "$A") (Variable "$B"))
	(Cond
		; If A is suscptible and B is infected, then A is exposed.
		(And
			(Equal (ValueOf (Variable "$A") seir-state) (Concept "susceptible"))
			(Equal (ValueOf (Variable "$B") seir-state) (Concept "infected")))
		(SetValue (Variable "$A") seir-state (Concept "exposed"))

		; If A is exposed, and is infirm, then move to state infected.
		(And
			(Equal (ValueOf (Variable "$A") seir-state) (Concept "exposed"))
			(GreaterThan (ValueOf (Variable "$A") infirm-state) (Number 0.5)))
		(SetValue (Variable "$A") seir-state (Concept "infected"))
	))
)

(cog-execute! (Put trans (List person-a person-b)))
