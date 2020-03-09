
Nothing here yet, just a sandbox of junk.

Do this:
```
$ guile
guile> (load "connect.scm")
```

At the moment, the idea is to create an
```
   (EvaluationLink
      (PredicateNode "connection")
      (ListLink
         connector-type
         left-side
         right-side)

```
when joining together connectors. The process repeats until there
are no more unconnected connectors...
