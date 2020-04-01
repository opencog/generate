
=Theory of Operation
Below are the details of the data structures in use, and the algorithms
used to perform the breadth-first search.

The breadth-first algo starts when it is given a single puzzle-piece.
It creates an ordered list of the (unconnected) connectors on the puzzle
piece: this is called the "odometer".  It gets this name because each
connector can be attached to several other puzzle-pieces; as each one is
tried, one must iterate over the remaining untried pieces. The odometer
guarantees that each attempted combination is tried once. See below for
a more precise description of the odometer.


== Odometer details




= Callback-specific structures.
The below describes data structures that are not part of the general
implementation, but are instead used by specific callback to acheive
specific functions.

== Dictionary
The dictionary is the "box" of unconnected puzzle pieces.

