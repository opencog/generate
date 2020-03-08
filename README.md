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
Algorithms that learn network structure, that learn grammars, need to be
evaluated for accuracy. One way to do this is to generate random
grammars, use them to generate a corpus of "sentences" (graphs) and
see how well the learned grammar corresponds to the input grammar.
This promises to be a much better strategy than evaluating against
a curated corpus, e.g. of English-language sentences, as one can more
finely control and measure different grammatical behaviors.

Generation is also a cornerstone to algorithms that can express
themselves, i.e. to move from deep syntax to suraface structure. For
example, given the abstract concepts "Ben", "pizza" and "to eat", and
the grammar for the English language, a key task is generating a
grammatically valid sentence with those three concepts embedded in it.

The goal of the code here is to work with grammars, expressed as "germs"
(or as link-grammar-style "disjuncts"), and generate graphs, usually
sequential sentences of words. It is hoped that the design will be
generic enough that the system can also be used to generate proofs
given either Hilbert-style axioms or given natural dedcution; i.e. that
the system will be broad enough to be a proof system. In particular,
it is hoped that the system will be sufficiently general to host
uncertain inference, thus providing an alternative implementation for
[PLN](https://github.com/opencog/pln) (Probabilistic Logic Networks),
the [URE](https://github.com/opencog/ure) (Unified Rule Engine) and
the OpenPsi rule scheduling system.

In the short term, it is expected that the generative abilities will
be used to evaluated the accuray of the learning algorithms in the
OpenCog [language learning subsystem](https://github.com/opencog/learn).

# Version 0.0.1
There is nothing here, yet.
