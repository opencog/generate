# Network Generation
Generate graphical networks (sheaf sections) from graph germs.
Currently focused on natural language generation.

Knowledge, including sentences in natural language, can be represented
as graphs. Such graphs can be generated from grammars (syntax rules).
One interesting way of representing rules is as "germs": a (labeled,
typed) vertex, with associated (labeled, typed) "half-edges" or
"connectors". These can be visualized as jigsaw-puzzle pieces, which
can only be assembled into larger sections (sheaf sections) when the
connectors can be mated. They occur naturally in theories of natural
language, such as the
[Link Grammar](https://www.abisource.com/projects/link-grammar/)
natural language parser, where they are explicitly referred to as
jigsaw-puzzle pieces in the foundational papers.

Germs are extremely generic: key examples are tensors, where the
half-edges or connectors are the tensor indexes. Tensors are deeply
related to Cartesian categories, or any categories with a tensor
product. Examples of such categories include the Lambda Calculus,
and are thus tied to computing, in general. For example, in
computing (in functional programming languages) one has the concept of
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
the Curry-Howard correspondence, and the extremely broad generalizations
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
themselves, i.e. to move from deep syntax to surface structure. For
example, given the abstract concepts "Ben", "pizza" and "to eat", and
the grammar for the English language, a key task is generating a
grammatically valid sentence with those three concepts embedded in it.

The goal of the code here is to work with grammars, expressed as "germs"
(or as link-grammar-style "disjuncts"), and generate graphs, usually
sequential sentences of words. It is hoped that the design will be
generic enough that the system can also be used to generate proofs
given either Hilbert-style axioms or given natural deduction; i.e. that
the system will be broad enough to be a proof system. In particular,
it is hoped that the system will be sufficiently general to host
uncertain inference, thus providing an alternative implementation for
[PLN](https://github.com/opencog/pln) (Probabilistic Logic Networks),
the [URE](https://github.com/opencog/ure) (Unified Rule Engine) and
the OpenPsi rule scheduling system.

In the short term, it is expected that the generative abilities will
be used to evaluated the accuracy of the learning algorithms in the
OpenCog [language learning subsystem](https://github.com/opencog/learn).

# Architecture
In what follows, the terms "section", "germ" and "disjunct" will be
used somewhat interchangeably. The differences are as follows:
* A "germ" is just one item, with a list of connectors on it.
* Commonly, an object will have several germs associated with it;
  these are disjoined from one-another, and are thus called "disjuncts".
  The disjoin is in the sense of a menu-entry: you can pick one,
  but only one.
* A "section" is a collection of multiple germs, connected to
  one-another. Such a collection will typically have unconnected
  connectors, and thus behaves much like one giant-sized germ.
  It's like a partially-assembled jigsaw-puzzle: the assembled
  portions behave much like single giant-sized pieces. The term
  "section" refers to a "section of a sheaf": starting from one
  particular germ, one can extend outwards "horizontally" in a
  coherent fashion. Coherency requires that all of the gluing
  axioms of a sheaf must be satisfied.

## Germs
With this in mind, germs are represented as OpenCog Atoms as
follows:
```
   Section
       Atom "foo"
       ConnectorSeq
           Connector
               Atom "bar"
               ConnectorDir "+"
           Connector
               ...
```
Here, `Atom "foo"` and `Atom "bar"` can be any Atomese Atom; for
natural language, these will typically be either `WordNode`s for
individual words, or `WordClassNode`s for collections of words
(nouns, verbs, etc.). The user is free to choose what kind of Atoms
to use here; there is no pre-established convention (except for
words, when dealing with linguistic graphs).

Two germs can be connected together if and only if they have
matching connectors of opposite polarity. Thus,
```
    Connector
        Atom "bar"
        ConnectorDir "+"
```
can only connect to
```
    Connector
        Atom "bar"
        ConnectorDir "-"
```
The polarity is indicated with `ConnectorDir`. Several different kinds
of polarity are possible, and are in common use. By convention, the +/-
polarities indicate linear order: something must come to the left/right
of the other.  The h/d polarities indicate head/dependent relationships:
a directed edge. The v/g polarities indicate variable/ground
relationships: a variable that can be beta-reduced with a particular
value.  The polarity pairs are by convention only; they are not
mandated by the system.



# Version 0.0.1
There is nothing here, yet.
