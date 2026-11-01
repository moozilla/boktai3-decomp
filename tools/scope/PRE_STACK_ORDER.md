# Explaining PRE-generated stack-slot permutations

Tags: #matching #compiler #tooling

When two full functions differ only in SP-relative offsets, byte distance may
measure a permutation rather than remaining instruction structure. The
8,764-byte Golem reaction (`080C9C04`) and 6,372-byte Skeleton contact reaction
(`080C12FC`) both reached this state: 61 and 45 generated address slots,
respectively. Both now match in ordinary C.

## Establish the preconditions first

Use `scope.py FILE.c frame` and `scope.py FILE.c pre`, together with the complete
machine comparison and a same-byte compiler trace. A zero fuzzy shape score is
insufficient. Verify all of the following:

1. The complete function sizes and instruction offsets agree.
2. Every opcode, physical register, branch target, literal, and data row agrees
   after replacing only SP-relative slot numbers with a placeholder. Check both
   normalized and resolved operands.
3. Repeated uses identify a consistent one-to-one mapping between target and
   candidate slots. A majority vote across disagreeing uses is not proof.
4. The differing values are PRE-created field-address temporaries that actually
   spill. Local arrays, escaped objects, outgoing arguments, and later register
   evictions may follow different allocation rules.

If any condition fails, keep the result unresolved and diagnose the remaining
instruction, liveness, or allocation differences first.

## The observed compiler mechanism

In this agbcc version, `gcc/gcse.c` defines `alloc_expr_hash_table`,
`hash_expr_1`, `insert_expr_in_table`, and the PRE expression traversal. For an
address expression of this precise shape:

```text
(plus:SI (reg:SI R) (const_int O))
```

the hash simplifies to:

```text
bucket = (13772 + R + O) mod T
T = max(11, N // 2) | 1
```

Here `R` is the compiler pseudo-register number, `O` is the byte field offset,
and `N` counts real instruction, jump, and call RTL entries just before GCSE.
In scope's trace, count the `u.i.addressof` input, after address-of elimination.
An earlier CSE dump can omit real instructions introduced before this pass.

The compiler appends expressions within a hash chain. The ordering key is
therefore `(bucket, first-discovery expression ID)`, ascending in both parts.
Tie order must be rechecked after a source change. PRE creates its reaching
pseudos in this traversal order; the relevant spilled pseudos then receive
stack slots in creation order.

Before using this rule to reason about the ROM, sort the candidate's actual
address expressions and compare the predicted order with its measured
`names.txt` stack assignments. Reject the explanation if these disagree. The
constant 13772 is specific to the expression modes and this compiler's RTL
enumeration; it is not a generic GCC stack-layout constant.

## Turn the mismatch into constraints

The established slot bijection supplies the target order. For each adjacent
pair, require:

```text
(hash(left) mod T, discovery(left))
    <= (hash(right) mod T, discovery(right))
```

Hold the measured pseudo identities fixed and check feasible odd values of
`T`. If none satisfy the order, changing instruction count alone cannot solve
that source family. Look for a coherent change to an inline boundary or value
domain that changes the relevant expansion identities. Preserve alternative
source states even when they have the same or a worse byte distance.

Once `T` exceeds every nonnegative raw hash, modulo wrapping disappears and the
order remains fixed. A diagnostic should report this tail explicitly rather
than inventing an arbitrary upper search cutoff. Negative offsets or other RTL
shapes need the compiler's actual unsigned hashing rules and a new derivation.

The Skeleton predecessor had 131 differing bytes with `N=2573`, `T=1287`.
No table size alone satisfied its fixed 45-role ordering. Combining three
measured source changes produced `N=2534`, `T=1267`, the required pseudo
identities, and an exact ordinary compile. The composition was a prediction
until the whole source was compiled and verified.

The larger Golem experiment likewise retained a 152-byte candidate over a
69-byte alternative because the former had the useful relative ordering.
Byte count was not the objective once instruction structure agreed.

## With scope

`scope.py FILE.c preorder [FUNC]` does the steps above on the last compile. It reads the
expression table (`T`, each index's bucket and RTL) and PRE's reaching pseudos from
`u.i.gcse`, counts `N` in `u.i.addressof`, and checks both halves of the model on the
draft: every `(plus:SI reg const)` entry's bucket, and that PRE made its pseudos in
`(bucket, index)` order. It then takes the ROM's slot order from `frame`'s pairing
(spilled pseudos only; evicted ones get later slots), marks the adjacent pairs whose keys
do not rise, and searches:

* the table sizes that give the order with the draft's pseudo numbers, each with the
  insn counts that produce it (`T 853 [N 1704..1707 (+8..+11)]`), and the size from which
  no bucket wraps;
* one renumbering cut: every base pseudo from a named one on moved by `d` (the C before
  it makes `d` more or fewer pseudos), with `T` within 60 of the draft's. Parameters keep
  their numbers. Each cut names the base's first set (`line 189 of
  scene_actor_chase_step.h` for an inline copy).
* for the nearest table size, how far all the movable bases may shift together and keep
  the order; and when it needs fewer insns, the function body's lines that load a large
  field offset into a register before the add. Each one moved into an inline helper is one
  insn fewer with the same final code (README fact 30).

`frame` prints the same report, without the full key table, when the slot order is not
pseudo order. Expressions of other shapes (pool loads, register sums) are left out: their
hash includes symbol addresses or codes the model does not have. The search keeps the
draft's expression indices; a source change can reorder them, so compile every prediction.

## Source and acceptance discipline

Use types, record layout, operation ownership, and call/return boundaries that
describe the observed behavior. Preserve data widths, call effects, and memory
ordering. Dummy statements, unused parameters, compiler table overrides, and
binary patching do not establish a source reconstruction.

Even a readable exact source does not prove historical original C. Typed versus
generic receivers, layered getters, and macro/inline choices can remain
compiler-sensitive reconstruction decisions. Share known layouts where the
complete matches permit it, and describe remaining duplication honestly.

Accept only the complete ordinary-compiler byte match, then check the production
translation unit and combined ROM. Directed execution cases and ordering-model
checks explain and supplement that result.

The contact delivery includes the read-only `lab/second-root/pre_order.py`
diagnostic, retained source graphs, and the independent inequality model. Its
portable source histories require recompilation in the receiving checkout;
historical evaluations are evidence, not reusable current verdicts. See
[the contact completion report](../../notes/matching-search-contact_2026-10-29.md).
