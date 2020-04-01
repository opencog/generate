
=Theory of Operation
Below are the details of the data structures in use, and the algorithms
used to perform the breadth-first search.

The breadth-first algo starts when it is given a single puzzle-piece.
It creates an ordered list of the (unconnected) connectors on the puzzle
piece: this is called the "odometer".  It gets this name because each
connector can be attached to several other puzzle-pieces; as each one is
tried, one must iterate over the remaining untried pieces. Each
unconnected connector corresponds to a "wheel" in the odometer; the
number of markings on each wheel corrsponds to the number of puzzle-
pieces that could attach to that connector. The odometer is "stepped"
as expected: the right-most wheel must revolve once, before the next
wheel can take a single step, and so-on.  This guarantees that each
combination of potential connections is attempted once.

When all of the initial list of connectors have been extended by new
attachments, the algorithm pushes the odometer state onto a stack,
and then constructs a brand-new odometer at the "next level". A new
list is created, of all of the unconnected conectors. This forms the
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
odometer get a connection, before a step to the next level is taken.

If a solution is found, or if it is impossible to proceed, then the
odometer must be stepped to the next step.  Conceptually, this requires
detaching 

When it is possible to connect a puzzle-piece to an existing unconnected
conector


== Odometer details




= Callback-specific structures.
The below describes data structures that are not part of the general
implementation, but are instead used by specific callback to acheive
specific functions.

== Dictionary
The dictionary is the "box" of unconnected puzzle pieces.

