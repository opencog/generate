
Design Notes
------------
Random ruminations on how to best implement this.


### Recursive tree-walking.
This is a "simple", direct, obvious choice.
Starting with a single nucleation point, pick one section, look at it's
endpoints, and attach sections to each endpoint. Repeat until done.
This is straight-forward to do recursively: each choice of section is
a branch-point; each choice of section pushes the stack. That there's a
resulting combinatorial explosion is obvious.

Problem w/recursive tree-walking is that it works efficiently only when
the dictionary specifies only trees. If the dictionary allows cycles,
then tree-walking fails, in two distinct ways. First, any cycle allows
infinite recursion; so recursion has to be blocked some way, either by
depth or by cycle-detection. Second, even when made finite in this way,
the walker will walk the cycle every possible way, even when the final
resulting graph is the same. This leads to a lot of wasted CPU time,
rediscovering the same graph.

Depth-first recursion can be visualized as follows. Imagine a small
tree or even a single point. A branch-tip is selected - just one -
and it is grown by aggregation, until conclusion. Here "aggregation"
means joining new puzzle-pieces to existing unconnected connectors.
In depth-first aggregation, if the original seed has a second connector,
it is completely ignored until all growths from the first have been
fully explored.  This is why breadth-first aggregation is intiutively
more appealing: in breadth-first aggregation, all growing tips are
explored in parallel.

The last working version of depth-recursive tree-walking is located at
```
   git commit 88b63ec3a7406929ed5ed2dd77400452c8e51596
   Sat Mar 21 13:37:17 2020 -0500
```
Although this "works" for simple cases, its a toy; important features
have been added since then.

### Breadth-first aggregation
Breadth-first aggregation is done in "parallel", with all open
connectors being extended just one step, per time-step. Visually,
this can be imagined as a wave or a boundary that is expanding at
constant velocity: one attachement per connector per time step.
Its like a wave propagating outwards; all points on boundaries are
explored simultaneously.

Managing choices for breadth-first search is harder, and more memory
intensive.

Continuing with the visualization: in general, the length of the
boundary increases exponentially, and so, if visualized as a surface,
that surface is hyperbolic. Back-tracking requires undoing each boundary
step by one, before other possible connections can be explored. As a
result, the inner-most connectors, close to the seed, are effectively
frozen into place, as very deep backtracking would be required to
explore alternative connections for those connectors. If a poor choice
is made for the first few sections connecting to a seed, it can be a
very long time before there is enough back-tracking to where those
"deep alternatives" can be explored.

These last thoughts suggest that a better, more balanced sampling of
the search space is to maintain a population of extensions grown from
a seed, and explore each one distinctly, thus allowing "deep
alternatives" to be fully sampled, even as getting boggged down at
the edges of each growth.

### Pruning
Its hard to imagine how to improve on the above when there is a single
starter nucleation point. But LG provides the counter-example, when
there are many nucleation points (i.e. fixed words in sentence, each
word has some number of associated sections). One can then use pruning
to discard sections that cannot possibly match; this is a multi-pass
algo that runs over the "global" set of sections.

### Desirable algorithmic properties
Based on above, some thoughts.

#### Link-type limiting features.
For example, for language-generation, we want to avoid having certain
link-types appear more than once; for example, neither a subject, nor an
object link should appear more than once in a sentence, unless that
sentence is paraphrasing: e.g. "John said that Mary is beautiful."

#### Cycle preference.
For example, for language-generation, the wall-noun, wall-verb and
noun-verb should normally form a cycle. It's a mistake to break this
cycle, and extend one of the links to a different vertex (e.g. have two
different subjects).

Do we need to have explicit "must-form-a-cycle" rules?  Can we do this
statistically?
