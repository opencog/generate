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

; Maximum number of odometer steps to take, when searching for a
; solution. It's not hard to specify grammars with weighting that lead
; to infinite trees, (i.e. are infinitely recursive) and so it's
; important to be able to halt exploration in such cases.
;
; CPU's of 2016 vintage run approx 1.2K steps/second (obviously,
; this will be grammar dependent.)
(define max-steps (Predicate "*-max-steps-*"))

; Maximum depth of exploration. Starting from the nucleation points,
; sections are chained on, forming a branching tree of chains. (They
; may also interlink, thus forming a network rathr than a linear
; chain or tre of chains). Ignoring the crosslinks, the max depth of
; exploration is the maximum length of the chain that will be explored.
(define max-depth (Predicate "*-max-depth-*"))

; Maximum number of points in the network. Networks having more than
; this number of points in them will not be explored.
(define max-network-size (Predicate "*-max-network-size-*"))

; When the network is generated, many individual instances of the
; network points will be generated. To get easy access to these, they
; can be tied at a well-known location -- specifically, they will
; be tied with a MemberLink to the specified anchor point.
(define point-set-anchor (Predicate "*-point-set-anchor-*"))

; --------------------------------------------------------------
; The parameters that are used for the `basic-network.scm` demo.
(define basic-net-params (Concept "Basic network demo"))

(State (Member max-solutions basic-net-params) (Number 10))
(State (Member close-fraction basic-net-params) (Number 0.5))
(State (Member max-steps basic-net-params) (Number 123123))
(State (Member point-set-anchor basic-net-params) (Anchor "Basic-Net Points"))

; Hush the output during load.
*unspecified*
