
Examples
========
A basic understanding of the AtomSpace is required. See the
[AtomSpace](https://github.com/opencog/atomspace) git repo, and the
[examples](https://github.com/opencog/atomspace/examples) there.

In addition, at least some basic familiarity with the theory of
[Link Grammar](https://www.abisource.com/projects/link-grammar/)
is useful, as the work here generalizes the notions there.

The examples are meant to be explored in the order given.

* [basic-network](basic-network.scm) --
     Definitions for generating a simple random network.
* [export-to-gml](export-to-gml.scm) --
     Generate a random network, and export it to GML for visualztion.
* [parameters.scm](parameters.scm) --
     Some of the paramaters that control the network generation.

The next batch of examples define simple English-language grammars that
encode dependency relations. These are written in the style of Link
Grammar (LG). In LG, dependencies can consist of directed or undirected
edges; the examples all use undirected edges. Unlike the above, the
edges have different types, indicating the type of dependency (subject,
object, adjective, determiner, etc.). In LG, the parse "tree" does not
need to be strictly a tree; it may contain loops (loops, in general,
serve not only to indicate secondary, interesting relationships, but
also serve to constrain the parse itself.)

* [dict-tree](dict-tree.scm) --
     A simple grammar, with typed edges, generating a dependency tree.
* [dict-loop](dict-loop.scm) --
     A simple grammar generating a dependency graph that includes a loop.
* [dict-triquad](dict-triquad.scm) --
     A grammar generating a dependency graph that consists of three
     quadrilaterals.
* [dict-mixed](dict-mixed.scm) --
     A grammar generating a dependency graph with a mixture of loops and
     trees.
* [grammar](grammar.scm) --
     The demo itself, employing the above dictionaries to generate a
     (small) corpus of sentences.

Some general comments about the mating rules for connectors are given below:

* [trisexual](trisexual.scm) --
     Examples of mono-sexual, heterosexual and tri-sexual mating rules.
     Plus a short explanation of why this is called "sexuality"; other
     code calls this "polarity" (so, mono-polar, bipolar and tri-polar).

That's all for now!
