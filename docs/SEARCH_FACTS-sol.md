# Non-obvious agbcc search facts

Tags: #matching #compiler #tooling

Read the symptom you hit; the model supplies ordinary C knowledge. Compiler links
below name the instrumented source in `build/scope/agbcc/gcc/`; use `trace` for
current file:line references.

* **A priority change cannot admit a register outside the preferred class.**
  `find_reg`'s `used1` combines class exclusions, fixed/call restrictions and
  live conflicts. A pointer can change from `LO_REGS` to `STACK_REG` when CSE
  changes its address uses; the latter excludes all low registers. Inspect
  `regclass.c:regclass`, `global.c:find_reg`, and `thumb.h:REG_CLASS_CONTENTS`.
  Compare operand uses and cost classes before adding artificial live ranges.
* **When an aggregate becomes addressable can decide its construction shape.**
  In the 08022878 draft trial, taking the address of a packed vector before
  assigning its halves emitted halfword memory stores. Constructing a scalar
  vector first and passing its address later retained word-level merges before
  addressof spilled it. `function.c:purge_addressof` is the relevant boundary;
  the observation does not prove the original local declarations.
* **An induction multiplier need not be a literal.** A loop-invariant register
  multiplier is a real GIV in `loop.c:strength_reduce`. Replacing it with its
  known constant can remove a secondary induction value yet initially worsen
  the diff because allocation changes too. `compiler --view giv` now reports
  RTL-valued multipliers; use the UID knob to confirm, then repair the C and
  retain the structural branch while solving allocation.
* **Register-erased listings lose dependencies.** `r0 += r1` and `r0 += r0` can
  look alike after blanking registers. Search regions use local value IDs and
  reaching definitions instead. Equal local shape still leaves global input
  bindings, aliasing, branch correspondence and semantics to establish.
