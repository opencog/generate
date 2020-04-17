;
; export-to-gml.scm
;
; Create some simple networks, and export them to GML - Graph Modelling
; Language, see https://en.wikipedia.org/wiki/Graph_Modelling_Language
; This allows the networks to be visualized with CytoScape or Gephi.
;
; The PNG image `n20.gml.png` is an example random network generated
; with the code below, and visualized with CytoScape.
;
(use-modules (opencog) (opencog generate))
(use-modules (ice-9 textual-ports))

; Load the dataset we plan to demo.
(load "basic-network.scm")

; The parameters that will control the search.
(load "parameters.scm")

;; Generate a collection of random networks, using the lexis, the
;; connectable end-points and the probability weightings previously
;; defined. The result of running this will generate multiple graphs.
;;
;; Aggregation will be started on "burr-3" -- this is the seed, or
;; root from which the rest of the graph will be constructed.
;;
;; XXX Caution, this is not the final API -- subjet to change.
(define graph-set
	(cog-random-aggregate polarity-set lexis weights basic-net-params
		(Concept "burr-3")))

;; Print the number of graphs that were generated
(cog-arity graph-set)

;; Convert the Atomese representation of these networks into
;; a GML string.
(define gml-string (export-to-gml graph-set))

;; It appears that CytoScape does not really like files with multiple
;; graphs in them. Thus, we'll pick just one graph, and export that.
(define just-one (Set (gar graph-set)))
(define just-one-gml (export-to-gml just-one))

;; Print the string to a file.
(let ((outport (open-file "/tmp/basic-random-net.gml" "w")))
   (put-string outport just-one-gml)
   (close outport))

;; Visualize the resulting network.
;; -- Install and start cytoscape.
;; -- Select "File -> Import -> Network from file ..."
;; -- Select "/tmp/basic-random-net.gml"
;; -- Hit the "Apply Perfered Layout" button (the rotating arrows button)
;; Ta-dahhhh! That's it!

;; ---------------------------------------------------------------
;; OK, just for the heck of it, play around with the close-fraction
;; parameter. Then rerun the the graph generation. Setting it to zero
;; should result in a tree, containing no loops.
;;
(State (Member close-fraction basic-net-params) (Number 0))

;; May as well ask for just one tree, from the outset.
(State (Member max-solutions basic-net-params) (Number 1))
;;
;; Go!  Just cut-n-paste from above, and do it again...
;; Whoops! It hangs! Infinite loop! Yowww! What happened?
;;
;; So ... there's a trick here. Its important to understand. The node
;; weightings that were set up in the `basic-network.scm` file will
;; generate trees that are infinitely large.  At each iteration step,
;; there are more open, unconnected connectors created, than there are
;; connectors that are closed, so with each iteration, the tree only
;; gets larger. Infinite. With the current weightings, the tree gains
;; 22/49 = 0.449... new unconnected connectors per iteration.  To obtain
;; this number, note that 1 + 1/2 + 1/3 + 1/4 + 1/5 + 1/6 = 49/20 and
;; that the number of new connectors created per iteration is
;; 1/3 + 2/4 + 3/5 + 4/6 = (21/10) / (49/20) = 6/7 while the number
;; closed is 20/49, so the expected number of new nodes per iteration
;; is 6/7 - 20/49 = 22/49 = 0.449... So, an infinite tree is generated.
;;
;; That's yucky for an actual example. Instead, adjust the weights,
;; before getting trapped by an inf loop.
;;
(cog-set-value! b1 weights (FloatValue 1))
(cog-set-value! b2 weights (FloatValue (/ 1.0 4)))
(cog-set-value! b3 weights (FloatValue (/ 1.0 9)))
(cog-set-value! b4 weights (FloatValue (/ 1.0 16)))
(cog-set-value! b5 weights (FloatValue (/ 1.0 25)))
(cog-set-value! b6 weights (FloatValue (/ 1.0 36)))

;; Repeating the arithmetic above, the values are:
;; norm = 1 + 1/4 + 1/9 + 1/16 + 1/25 + 1/36 = 5369/3600
;; opened = (1/9 + 2/16 + 3/25 + 4/36) / norm = 0.3133...
;; closed = 1/norm = 3600/5369 = 0.6705...
;; net = opened - closed = -0.3572...
;; So, if we start with the arity-3 root, all three connectors will
;; close off after about 3 steps, and the trees will be quite small
;; Better to start with the arity-6 root; it provides a bigger tree.

(define one-tree
	(cog-random-aggregate polarity-set lexis weights basic-net-params
		(Concept "burr-6")))

;; Create the GML string
(define tree-gml-string (export-to-gml one-tree))

;; Print the string to a file.
(let ((outport (open-file "/tmp/basic-random-tree.gml" "w")))
   (put-string outport tree-gml-string)
   (close outport))

;; Hush printing when loading this file.
*unspecified*
