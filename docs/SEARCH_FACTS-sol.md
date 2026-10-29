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

* **Local allocation ranks quantities, not flow's pseudo lifetimes.** In the
  08203DF4 trial, flow reported 58 ticks while the table's local quantity covered
  birth 28 through death 126: 98 half-instructions. Its 4 refs gave priority 816;
  constant 24 had 3 refs over 36 ticks, priority 833. `priority` uses the actual
  `L` records and `local-alloc.c:QTY_CMP_PRI_BASE`, including quantity-number tie
  rules. Two fewer interval ticks crossed the boundary; that does not promise
  that one C edit preserves quantity grouping or hard-register constraints.
* **A named selector must be resolved before agbcc sees it.** `CSE_NOCANON`
  uses `atoi`, so passing an unresolved C name selects pseudo zero instead.
  Scope now resolves unique names for `CSE_NOCANON`/`CSE_KEEP`, as for PRI/FORCE;
  search rejects unsupported UID/event formats. `AGBCC_NOPRE` accepts one event,
  not a comma list. `AGBCC_SPILL_LABEL` requires a `label#occurrence:shift`.

* **A register mismatch can originate before allocation.** In the pin-free
  EffectPool_UpdateEntry investigation, CSE1 placed a constant mask second;
  `regmove.c:fixup_match_1` then overwrote a dead input with the AND result and
  rewrote its dying compare use. Changing priorities could not restore the
  missing copy. Typed byte extraction and sampled byte/halfword reads recovered
  the actual access boundaries. A signed-byte probe instead emitted LDRSB and
  merely traded instructions; a lower count was not a structural explanation.
* **A cached value can change a singleton conditional's subtraction.**
  `expr.c:expand_expr` recognizes `condition ? A - 1 : A`, copies A to a target,
  then conditionally updates it. In EffectPool, a named cached field left a
  three-operand SUB; selecting directly from the halfword field retained the
  two-operand decrement. A direct preincrement comparison also preserved the
  timer's duration-load order. These ordinary C changes completed an exact
  production match with zero register pins or asm barriers.
