
Design Notes
------------
Random ruminations on how to best implement this.

* Recursive tree-walking. This is a "simple", direct, obvious choice.
Starting with a single nucleation point, pick one section, look at it's
endpoints, and attach sections to each endpoint. Repeat until done.
This is straight-forward to do recursively: each choice of section is
a branch-point; each choice of section pushes the stack. That there's a
resulting combinatorial explosion is obvious.

Its hard to imagine how to improve on the above when there is a single
starter nucleation point. But LG provides the counter-example, when
there are many nucleation points (i.e. fixed words in sentence, each
word has some number of associated sections). One can then use pruning
to discard sections that cannot possibly match; this is a multi-pass
algo that runs over the "global" set of sections.
