
Covid-19 Demo
--------------
Demo of a random social network with a SEIR Covid-19 simulation.

 SEIR == "susceptible", "exposed", "infected", and "recovered".

This is a demo, only! It is demonstrating several different AtomSpace
programming techniques, one of which is the brand-new (version 0.1)
random network generator. Another is Value Flows: the use of Values
for holding mutable state, and flowing that state through a network.
It is assumed that the reader is already familiar with the basics of
the OpenCog AtomSpace! This demo *could* be enhanced to do actual
scientific exploration; see "making this useful", below.

This demo generates a random "social network", consisting of two types
of relationships: "friends" and "strangers". It adopts a simple SEIR
model of disease transmission and progression along this network.
The model itself is a (simple) hand-coded state-transition machine.

Making this Useful
------------------
This is a demo, not a final finished product.

There's no whizzy visualization of the disease progression, or the
resulting stats. You can visualize snapshots of the network with
CytoScape or Gephi, but it won't be animated as the disease spreads.

There's no statistical analysis performed, and no graphs or curves are
drawn. The demo shows how to generate the raw data, not how to analyze
raw data.

Altering the disease progression model is not particularly easy. It is
written in Atomese, which is rather verbose, and programming in
Atomese is, in many ways, like programming in assembly code.  This is
perhaps the most important thing to understand about this demo.
Atomese is a graphical programming language, and it was designed for
automation. This means that the graphs, and the Atomese code, is meant
to be easy for other algorithms to manipulate. It's designed for other
machines, not for humans :-!

Consider, for example, a graphical editor - something where you can
drag-n-drop, hand-draw interacting bubble diagrams. Something easy
to use - something a medical professional could get the hang of in
half an hour - something where a disease model could be sketched out
with bubbles and lines and flow-charts. The goal of Atomese is that
it becomes easy -- really easy -- to convert such diagrams into
executable code. That's what Atomese is designed to do.

Another possibility is that of an automatic model explorer: a system
that automatically creates a variety of different models, and then
explores how well each model works. The automatic generator could,
for example, mutate the best models and see if even better-fitting
models can be obtained, searching for a best-fit.  The goal of Atomese
is that it becomes very easy for such models to be generated and run.
The verboseness of Atomese is no longer a problem; the simple
programming language it implements is easy for other tools to
manipulate and control. That's what it's good for.

Thus, in reading the below, keep in mind that there is nothing special
about the SEIR model; its a stand-in for what could be a generic,
arbitrary state transition machine. Such machines can be hand-coded,
of course, but the interesting application is when they are generated
from other sources.

Last but not least: keep in mind that the network generator is
currently at version 0.1, and is not yet as versatile, flexible and
powerful as it could be. The parameter settings below generate
networks that are long and thin. One such network can be seen in
the included image.
