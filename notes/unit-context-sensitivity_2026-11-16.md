# Functions that match alone but not inside a unit

Tags: #units #selector-macros #scene-actors 08087018 08087A70 080ACBE0 080B2580 080CF3E4 080D2194 080D2584 080D5778 080D6608 080DA97C 080DC228 080E059C 080E0750 080E25FC 080E4B34

Fifteen monster functions stayed in `src/fn` during the class consolidation
(`notes/object-cleanup_2026-11-15.md`) because each one matched alone but not in the
merged unit. I first read this as agbcc carrying state from earlier declarations or
bodies. That was wrong. Every case is the preprocessor: a `#define` that has to come
before a header ends up after it.

## The mechanism

Thirteen of the functions set chase selector macros before their includes, for
example `#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH 1` and then
`#include "scene/motion/scene_actor_chase_reaction.h"`. That header picks between two bodies of
several shared inlines by these macros, and it is include-guarded. Inside a unit the
selector stops working in two ways:

* `tools/merge_unit.py` hoists every `#include` to the top of the unit, above the
  file's own `#define`s. The define then comes after the header it was meant to steer.
* An earlier file in the unit has already included the header, so the guard skips it.

Either way the function inlines the default variant and compiles different code. The
fourteenth, 080ACBE0, works the same way through an alias macro,
`#define gSineTable gUnk_086149C4`, written before its includes and moved below them by
the hoist. The fifteenth, 080E059C, never changed at all: the differing bytes were in its
neighbor 080E0750, and unitize blamed the wrong function.

So the sensitivity says nothing about the original files. It is an artifact of the
selector macros, which the project rules forbid; once they are gone these functions
should merge like the rest.

## Per function

Tests (scratch scripts, kept with the session):

1. The function alone.
2. Prepended: a redundant unrelated prototype, ten of them, an extern variable, a
   typedef, a struct definition, an unused static inline body, blank lines, and the
   file's own first prototype repeated. Each was tried after the includes and directly
   before the body.
3. The preceding file, reduced (delta debugging, comparing only the function's own
   bytes) to the smallest set of items that still changes it.
4. The real merge with the following file, to see what lands in front of the function.

For 08087A70 none of the test-2 additions break it: not prototypes, typedefs, structs,
inline bodies, blank lines, nor a repeat of its own prototype. The earlier claim, in the
object-cleanup notes, that an identical redundant `s32 Mod(s32, s32);` broke it does not
reproduce. That run had the chase header included earlier.

| Function | Its selectors | Smallest content that breaks it | Doesn't break it |
|---|---|---|---|
| 08087018 | TYPED_GROUP | the next file's `#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH 1` placed before it | Bat_RestoreSavedState and the rest of bat_restore.c before it |
| 08087A70 | WIDE_MAP_WIDTH, FRACTION_ACCESSOR, DIRECT_SAVED_CONTROL | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` | everything in test 2 |
| 080ACBE0 | (`#define gSineTable gUnk_086149C4`) | merged with the next file 080ACCD4: the hoist moves its alias define below the headers (31 bytes differ) | its own content in its own order |
| 080B2580 | JOIN_ROUTE_FAILURES | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` (vorn_main.c) | vorn_main.c's 46 function bodies |
| 080CF3E4 | SCALAR_CELLS | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` (golem_main_2.c) | golem_main_2.c's bodies |
| 080D2194 | SCALAR_CELLS | the merge hoisting the chase header above its define | mummy_entry.c before it |
| 080D2584 | six selectors | the preceding file's `#define SCENE_ACTOR_CHASE_SCALAR_CELLS 1` | |
| 080D5778 | SCALAR_CELLS, ROUTE_COUNT_ACCESSOR, DIRECT_SAVED_CONTROL | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` (mummy_defeat.c) | mummy_defeat.c's bodies |
| 080D6608 | DIRECT_SAVED_CONTROL | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` (mummy_path.c, hoisted) | mummy_pursuit.c before it |
| 080DA97C | TYPED_GROUP, WIDE_MAP_WIDTH, DIRECT_ROUTE_FIELDS | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` (mummy_path.c) | mummy_path.c's bodies |
| 080DC228 | DIRECT_SAVED_CONTROL | the preceding file's `#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH 1` | |
| 080E059C | none | nothing: it matches in the merge; the 88 differing bytes were in 080E0750 | |
| 080E0750 | WIDE_MAP_WIDTH | merged with the next file 080E17D8: the hoist puts its define after `scene_actor_chase_reaction.h` (88 bytes differ) | 080E17D8's whole content appended after it |
| 080E25FC | WIDE_MAP_WIDTH | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` (dog_scripted.c) | |
| 080E4B34 | DIRECT_SAVED_CONTROL, SCALAR_CELLS | an earlier `#include "scene/motion/scene_actor_chase_reaction.h"` (dog_scripted.c) | |

No case needed a preceding function body. The cases where a body looked necessary all
came from placing a later function's body ahead of the one under test, which ROM order
never does.

## What follows

* Replace the chase selectors with real code. Each class that needs a variant gets its
  own copy of the inline under its own name, as `src/monsters/golem/golem_main.c` already does with
  `Golem_CellIndex`; the headers then keep only the default bodies. This is item 1 of
  the object cleanup.
* Then rerun the consolidation over these functions; they should join their
  neighbors.
* `tools/merge_unit.py` should refuse, or warn on, a `#define` that comes before an
  `#include` in an input, because hoisting the include reorders the two.

## Status (2026-11-16, later)

* 6d08b413b removed all ten chase selectors (`tools/unselect.py`; the files keep class copies).
* 080ACBE0 now names `gUnk_086149C4` directly instead of an alias `#define`.
* Not done yet: rerunning the consolidation over these fifteen functions so they join their
  neighbors' units (`unitize` over each class range); merge_resolve should then fold the
  identical class copies into one per unit.

## Consolidation rerun (branch claude/units-after-selectors, from main after PR 1038)

With the selectors gone, 16 of the 17 held-out functions joined units, as predicted:

* Bat: 08087018, 08087A70 and bat_defeat.c into `src/monsters/bat_restore.c`.
* Vorn: 080B2580 joins vorn_main.c and vorn_main_2.c.
* Golem: 080CBF24 into golem_2.c (with golem_3.c); 080CF3E4 into golem_main_2.c (with
  golem_main_3.c).
* Mummy: all six, and 080D1A98 onward is one unit, `src/monsters/mummy/mummy_entry.c`.
* Dog: all five, and the whole class is one unit, `src/monsters/dog.c`.

The one left out, 080ACBE0, is a different case: its neighbor 080ACCD4 is compiled with
old_agbcc (`// COMPILER: old_agbcc`), so neither can share a unit with vorn.c. Joining
golem_main.c, golem_2.c and golem_main_2.c stops on `sub_08014634`, declared two ways.
