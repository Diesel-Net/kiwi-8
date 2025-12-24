# Contributing

## C-style in Core (policy)

The `Core/` directory follows a "C-style in C++" policy: public APIs in `Core/`
should be implemented as C-style structs and free functions. Avoid introducing
C++ OO constructs (e.g., `class`, `virtual`, `template`, `operator`, `new`, `delete`, `std::`, or `#include <iostream>`) in `Core/`.

We enforce this via an automated linter and CI check.

How to run the check locally:

    bash .ci/check_no_oo.sh Core

If a file legitimately needs to contain C++ constructs (e.g., UI shims or OS-specific adapters), put it in `Core/` only after discussing and documenting why it must be an exception.
