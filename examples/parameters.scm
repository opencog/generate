;
; parameters.scm
;
; Parameters that control network generation.
; Each parameter is identified by a PredicateNode with a well-known,
; fixed name. Different sets of parameters are grouped together with
; a MemberLink that names a set they belong to. That set is then passed
; to the solver on each invocation. 

; Maximum number of solutions to accept. Search is halted after this
; number of networks are generated. Integer, zero or more, defaults
; to 100.
(define max-solutions (Predicate "*-max-solutions-*"))

; Fraction (zero to one) of attempts that should be made to create
; a closed loop (cycle) by joining together two currently-open
; connectors. If set to 1.0, every attempt to make a connection will
; begin with an attempt to link a pair of currently-unconnected (open)
; connectors. This may not be possible, so this fraction does not
; strictly control the number of loops in a graph, nor their size.
; If set to 0.0, then loops will never form, and the resulting graph
; will always be a tree. Currently applies only to the random network
; generator.
(define close-fraction (Predicate "*-close-fraction-*"))

; The parameters that are used for the `basic-network.scm` demo.
(define basic-net-params (Concept "Basic network demo"))

(Member (Evaluation max-solutions (Number 10)) basic-net-params)
(Member (Evaluation close-fraction (Number 0.5)) basic-net-params)
