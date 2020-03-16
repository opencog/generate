
Design Notes
------------
Random ruminations on how to best implement this.


=== Recursive tree-walking.
This is a "simple", direct, obvious choice.
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

Problem w/recursive tree-walking is that it works efficiently only when
the dictionary specifies only trees. If the dictionary allows cycles,
then tree-walking fails, in two distinct ways. First, any cycle allows
infinite recursion; so recursion has to be blocked by depth. Second,
the walker will walk the cycle every possible way, even when the final
resulting graph is the same. This leads to a lot of wasted CPU time,
rediscovering the same graph.

=== Desirable algorithmic properties
Based on above, some thoughts.

==== Link-type limiting features.
For example, for language-generation, we want to avoid having certain
link-types appear more than once; for example, neither a subject, nor an
object link should appear more than once in a sentence, unless that
sentence is paraphrasing: e.g. "John said that Mary is beautiful."

==== Cycle preference.
For example, for language-generation, the wall-noun, wall-verb and
noun-verb should normally form a cycle. It's a mistake to break this
cycle, and extend one of the links to a different vertex (e.g. have two
different subjects).

Do we need to have explicit "must-form-a-cycle" rules?  Can we do this
statistically?
