# Axe palette transition

Tags: #matching #monsters #axe #080FB744

`Axe_UpdatePaletteTransition` matches all 420 bytes with the stock compiler.
The red update helper returns the element it updated, and the later channel
conversion reads that same element. This retains the red channel base without
introducing another advancing pointer in the loop.

The palette update binder initializes the actual flags, red, and delta handles
in execution order. Its output references establish delta, flags, and red as
the address-taken owners, giving the original spill order. This replaces the
previous mismatch between a native delta address and a later PRE red address.
There are no register pins, asm barriers, compiler overrides, or new selectors.

Independent stock compilation and standalone linking verified zero differing
bytes over the reviewed 420-byte extent, SHA-256
`117f1ad24598e837a3354b3e2527e4b3ee1e4c3ed58edccc4b03b04441ae0c74`.
The renamed caller at 080FD128 also remains byte-identical over its 48 bytes.
The orchestrator runs the combined ROM build during integration.

Scope's useful missing explanation was the origin of the address-taken owner
pseudos: the first address-taking operation, rather than local declaration
order alone, determined their spill identities. Returning the actual updated
red element also prevented the separate read from creating an unwanted GIV.
