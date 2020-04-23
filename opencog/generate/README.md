
# Theory of Operation
Below are the details of the data structures in use, and the algorithms
used to perform the breadth-first search.

## Terminology
Some terms used in the code.

* Each "jigsaw-puzzle piece" is called a "section". This is a poor
  choice of terminology, since several connected pieces are also a
  section (per formal definition of "a section" in mathematics).

* The label on each puzzle-piece is called a "point" or "vertex".
  All pieces are labeled.

* "Aggregation" means attaching new pieces to the existing assembly
  of pieces.

Other terminology is introduced below.

## Breadth-first Aggregation
The breadth-first algo starts when it is given a single puzzle-piece.
It creates an ordered list of the (unconnected) connectors on the puzzle
piece: this is called the "odometer".  It gets this name because each
connector can be attached to several other puzzle-pieces; as each one is
tried, one must iterate over the remaining untried pieces. Each
unconnected connector corresponds to a "wheel" in the odometer; the
number of markings on each wheel corresponds to the number of puzzle-
pieces that could attach to that connector. The odometer is "stepped"
as expected: the right-most wheel must revolve once, before the next
wheel can take a single step, and so-on.  This guarantees that each
combination of potential connections is attempted once.

When all of the initial list of connectors have been extended by new
attachments, the algorithm pushes the odometer state onto a stack,
and then constructs a brand-new odometer at the "next level". A new
list is created, of all of the unconnected connectors. This forms the
spine of the new odometer. Again, each of these must be extended;
when this is done, the algo repeats: it pushes the odometer state,
and creates a new odometer.

The new odometer may be the empty set if there are no more unconnected
connectors. In this case, a "solution" is deemed to have been found:
the jigsaw puzzle is complete. If it it is impossible to obtain a
connection for some open connector, then it is impossible to continue
at this level, and the algorithm backtracks to the previous level.
The unextendable odometer is popped off the stack and discarded.
Resuming at the previous level, the odometer is stepped once, and the
attachment process is restarted.  This continues until all solutions
have been found, or until a termination condition triggers.

This is breadth-first aggregation, in that all of the connectors in the
odometer get a connection, before moving to the next level.

If a solution is found, or if it is impossible to proceed, then the
odometer must be stepped to the next step.  Conceptually, this requires
detaching the previously-connected piece at the given location, then
selecting a new piece, and attaching it. To facilitate this attachment-
detachment process, there is a second stake, the "frame stack".  The
frame is a set of the currently-assembled pieces and the set of pieces
that are not yet fully connected. A new frame is created just before
attaching a puzzle-piece; thus, returning to the previous unconnected
state is as easy as popping the frame-stack.

The frame-stack could, but does not march in synchrony with the
extension at each level.  This is because the stepping of the odometer
is faced with several complications.  Foremost is that a connector
at a given level might connect with another connector at that same
level. When this happens, this effectively knocks out one of the
odometer wheels for that level: it cannot get a new connection because
it is already connected.  During stepping, those wheels cannot be
stepped.  As a result, the frame-stack is pushed only when a new
connection is possible. This saves a few CPU-cycles of un-necessary
frame pushes.  Each frame is marked with the corresponding odometer
and odometer-wheel to allow management of the frame-pop operation.

### A curious limitation!?
The current implementation of the above is such that only one link
is generated to connect one puzzle piece to another. Thus, although
a pair of pieces may have multiple connectors that can be connected at
once, only one of these is connected, and the possibility of connecting
the others is not explored.

## User-defined callbacks
The above description suggests that each odometer wheel exists as a
finite list of connectable pieces. This is **NOT** the case! Instead,
each wheel is implemented as a future or promise, such that, when asked
upon, a "user-defined" callback will provide a connectable piece.
This allows the code to be cleanly split into two parts: the aggregation
algorithm described above, and the specifics of managing the pool of
available puzzle-pieces and the priority-order in which they are tried.

In particular, the callback mechanism allows the following tricks to
be applied:

* Puzzle pieces can be drawn at random, or in deterministic order, from
  a finite or infinite pool.
* Zero matches can be returned to force early termination of the search.
* Pieces can be given a weighting, probabilistic or otherwise, so that
  a lowest-cost aggregation path can be pursued. This allows local
  hill-climbing algos, simulated annealing, Bayesian or Markov nets,
  etc.

The callback is called to create the linkage between two pieces. This
allows the user to create either directed or undirected edges, or have
any other arbitrarily-complex markup in the linkage.

Although not part of the callback, the definition of matching connectors
must be provided on startup.  This is a set of "polar pairs" -- polar
opposites that can connect to one-another.  There is no restriction on
what these may be -- the aggregation algorithm explores all valid
connection-types.

## The `SimpleCallback`
This callback provides a minimalistic basic operation, suitable for
exhaustive searches over grammars that generate a strictly finite
number of graphs.

This callback does the following things (subject to change):

* If it is possible to perform a connection to an existing connector,
  this is given the highest (and only) priority. This prevents
  infinite recursion in graphs that contain cycles -- otherwise any
  cycle could be "unwound" by drawing new pieces, and attaching them,
  ad infinitum.

* When a cyclic connection is not possible, new pieces are drawn from
  a "dictionary" or "lexis" -- the "box" of unconnected pieces.
  Thus, users of this callback need only specify the pieces in this
  lexis.

The callback is deterministic, but the order is not specified. There
is no weighting or ranking involved in the drawing of pieces.

## The `RandomCallback`

Provides ranking. Provides random weighted draws. Need writeup here
describing it.

## Alternatives to Aggregation
There are other ways of creating network graphs. The aggregation
algorithm is an implementation of the idea that networks can be
"assembled", and that the assembly process is very much like assembling
a jogsaw puzzle: start with an edge-piece, and keep adding more and more
pieces, until one is done. But there are other possibilities.

* Changing priorities during aggregation.  So during early stages of
  aggregation, if one consistntly picks high-arity pieces, one risks
  combinatoric explosion and an infinite graph. To avoid this, the
  probabilities could be adjusted to pick only lower-arity pieces,
  as the number of unconnected connectors gets too large.

* Swapping out pieces during aggregation. If there are too many
  unconnected connectors, one could hunt for pieces that are identical,
  in all respects, except that they have fewer connectors. These can
  then be swaped in place of the bigger connector.

* Like the above, but making the network more sparse, by activly cutting
  links.

* Growing a network. Inverse of the above: given an existing connected
  edge, break it and insert a new piece in its place. This is
  interesting because it is more "botanical" in it's process - its
  actual growth.

* Growth could be governed by explicit choices of substitution rules
  (e.g. Lindenmeyer systems) and very specifically, the wealth of work
  from Przemyslaw Prusinkiewicz at http://algorithmicbotany.org/papers/

* The Prusinkiewicz work can be thought of as work with structure
  grammars (e.g. BNF forms, classic Chomsky context-free grammars)
  whereas the work here is with dependency grammars.  These two grammar
  types are "in principle" interconvertible, but behave quite
  differently. A wealk of possibilities open up by stealing the
  Prusinkiewicz ideas and applying them to dependency grammars.

## To-do

* Add weights to the polar-pairs list.
* Allow pieces to be drawn at most N times, for any given piece.
* Force planar graphs
* Control recursion when there are degenerate link-types.
  (See issue #6)

THE END. Thanks for paying attention!
