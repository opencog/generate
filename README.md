# Network Generation
Generate graphical networks (sheaf sections) from graph germs.
Currently focused on natural language generation.

Knowledge, including sentences in natural language, can be represented
as graphs. Such graphs can be generated from grammars (syntax rules).
One interesting way of representing rules is as "germs": a (labelled,
typed) vertex, with associated (labelled, typed) "half-edges" or
"connectors". These can be visualized as jigsaw-puzzle pieces, which
can only be assembled into larger sections (sheaf sections) when the
connectors can be mated. They occur naturally in theories of natural
language, such as the
[Link Grammar](https://www.abisource.com/projects/link-grammar/)
natural langauge parser, where they are explicitly refered to as
jigsaw-puzzle pieces in the foundational papers.

Germs are extremely generic: key examples are tensors, where the
half-edges or connectors are the tensor indexes. Tensors are deeply
related to Cartesian categories, or any categories with a tensor
product. Examples of such categories include the Lambda Calculus,
and are thus tied to computing, in general. For example, in
computing (in functional programming langauges) one has the concept of
"beta-reduction": plugging values into an expression with variables
in it, replacing each variable with a value. The corresponding notion
for germs is mating two connectors together, plugging one into the
other. Unlike the variable/value distinction, thinking in terms of
connectors makes beta-reduction more obviously symmetric. That is,
one usually thinks of "variables" as being completely unlike "values",
living in a conceptually different world.  Thinking of them as a place
to plug something in, and something to plug into it emphasizes the
duality between the two. Connectors provide a simpler, more natural
representation for joining typed classes of things together.

To summarize: the concept of joining together connectors underpins
the Curry-Howard correspondance, and the extremely broad generalizations
thereof that have been discovered in modern times.

## Overview


# Version 0.0.1
There is nothing here, yet.
