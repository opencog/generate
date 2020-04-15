
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

* See the unit-tests directory for more examples... look at them in the
  following order:
   + dict-helloworld.scm
   + dict-tree.scm
   + dict-loop.scm
   + dict-biloop.scm
   + dict-quad.scm
   + dict-mixed.scm
  These go through a series of lxis that generate increasingly
  sophisticated networks.

That's all for now!
