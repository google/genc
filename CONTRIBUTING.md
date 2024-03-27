# Contributing to GenC

We'd love to accept your patches and contributions to this project. There are
just a few small guidelines you need to follow.

## Contributor License Agreement

Contributions to this project must be accompanied by a Contributor License
Agreement. You (or your employer) retain the copyright to your contribution;
this simply gives us permission to use and redistribute your contributions as
part of the project. Head over to <https://cla.developers.google.com/> to see
your current agreements on file or to sign a new one.

You generally only need to submit a CLA once, so if you've already submitted one
(even if it was for a different project), you probably don't need to do it
again.

## Code reviews

All submissions, including submissions by project members, require review. We
use GitHub pull requests for this purpose. Consult
[GitHub Help](https://help.github.com/articles/about-pull-requests/) for more
information on using pull requests.

## Community Guidelines

This project follows
[Google's Open Source Community Guidelines](https://opensource.google.com/conduct/).

## Code placement

While the code is evolving and the structure of the repo may change, here are
a handful of example pointers for common scenarios and general orientation:

*   **If you'd like to provide a runtime setup for a new environment**, please
    read the section about extensibility APIs in
    [api.md](generative_computing/docs/api.md),
    and the included tutorials, and review the
    [two example setups](generative_computing/cc/examples/executors/)
    we provided for use with the tutorials/examples included in this repo (one
    for Colab, and one for Android).

*   **If you'd like to add support for a new backend, custom operator, etc.**,
    please likewise read the section about extensibility APIs in
    [api.md](generative_computing/docs/api.md),
    and the tutorials, and review some of the examples of
    [existing operators](generative_computing/cc/intrinsics/)
    as well as the supporting
    [backend interop code](generative_computing/cc/interop).
    Also note that many simpler uses can be supported by defining a
    *custom function* (as shown in the tutorials).

*   **If you'd like to add support for a new frontend SDK**, or expand support
    for an existing one (like LangChain), please read the section on authoring
    in [api.md](generative_computing/docs/api.md) and the documentation on
    [architecture](generative_computing/docs/architecture.md) and
    [intermediate representation](generative_computing/docs/ir.md),
    and review the existing examples (e.g.,
    [the existing LangChain interop code](generative_computing/python/interop/langchain/))
    to see how we use the native authoring surface under the hood.

If the type of contribution you're interested in isn't listed above, or if you
have any questions or suggestions, please contact us! See also
[README.md](README.md) for the general instructions and pointers to the key
docs.
