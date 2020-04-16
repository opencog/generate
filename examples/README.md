
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

* See the unit-tests directory for more examples... look at them in the
  following order:
   + dict-helloworld.scm -- Multiple edge types.
   + dict-tree.scm   -- Tree graph with edge types.
   + dict-loop.scm   -- Cyclic graph with edge types.
   + dict-biloop.scm -- Graph with two cycles (triangles).
   + dict-quad.scm   -- Graph where the cycle is a square.
   + dict-mixed.scm  -- Mixture of cycles.

These go through a series of lexis that generate increasingly
sophisticated networks. All of the examples are "linguistic",
with the linking constrainted by explit edge-types.

That's all for now!
