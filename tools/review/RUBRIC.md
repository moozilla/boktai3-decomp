# Review rubric: would a person have written this?

The question for each function: **would a professional game programmer in 2005 plausibly have
written this function this way?** Every function in the tree already compiles to the original
bytes; a match can still be C that nobody would write, shaped only to steer the compiler. The
reviewer reads the C (and the definition of a small `static inline` it calls, when needed) and
judges it as a reader. The reviewer does not run tools and does not see the linter's output.

## Verdicts

* `natural`: a person could well have written it. Unknown field names (`unk4C`) and generic
  names do not count against it.
* `minor`: reads as human except for one or two small oddities.
* `contrived`: clearly shaped for the compiler.

When unsure between two verdicts, choose the harsher one and say so in the evidence.

## Tags

* `parked-constant`: a local that only holds a literal (`u32 mask = 4;`, `zero`, `one`).
* `odd-helper`: a tiny helper whose name or parameter type belongs to another object or
  mechanism, or a helper that only wraps one operator on an untyped pointer. A typed accessor
  named for its own object (`Skeleton_TestMotionFlags(actor, 0x2000)`) is natural.
* `pointer-temp`: a pointer local to a single field where the field would be written directly.
* `value-reuse`: a variable known to be zero (or another value) stored instead of the constant.
* `cast-noise`: casts or raw offsets a person would not need with the right type.
* `control-flow`: goto, `do { } while (0)`, duplicated branches, inverted or split conditions
  with no purpose for the reader.
* `decl-order`: bare blocks or temporaries that exist only to order evaluation.
* `union-or-alias`: a union or second struct view of one object used to get another access
  width.
* `magic-numbers`: bare flag masks or state numbers where a named constant is expected. Alone,
  at most `minor`.
* `naming`: placeholder names, or names for the mechanism instead of the game. Alone, at most
  `minor`.

## Output

One row per function: path, function, verdict, tags, and for `minor` and `contrived` one
sentence of evidence quoting the code.

## Accepted as natural

Constructs a rewrite could not remove and that are argued to be the original's (each needs the
compiler decision named, as in `notes/naturalize-pilot_2026-11-27.md`): a pointer local
assigned in both branches where the ROM computes the address in one branch; a named flag
variable the ROM keeps in a register across a loop; two statements where one expression would
let the compiler fold a constant.
