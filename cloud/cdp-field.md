## Your assignment: take the last concessions out of Field objects and hazards

Your queue is `cloud/cdp-field.json`: every function in Field objects and hazards that still holds a concession on
main, in ROM order (17 functions). Each row has func, addr, size, score (share of its lines without
a concession), src (the unit file) and unit. Only you edit these src files tonight.

**Read `cloud/P3_BRIEF.md` now.** It is the brief four Claude workers used on this same work three
days ago, and it is your method: the loop, how hard to try, the forms that matched recently, the
tool gaps. Where it differs from this file (SendMessage reports, `git commit`, its queue path, its
fact numbers), this file wins. Since it was written, the workers added README facts up to 113 in
`tools/scope/README.md`; read 84-113 (the newest) before you start, they are the levers that worked:
the class-head lever (85), repeating a post-join statement in each arm (91), a missing return keeps
r0 live (95), a union initializer's clobber blocks loop hoisting (93), move2add after a label (100),
cross-jump labels (101), loop rotation stops at a return (102), the zero `orrs` (103), giv order
(112), goto loops vs while loops (113). Shared helpers that removed many pins:
`include/display_sequence_sprite.h` (DisplaySequence_StepSprite), `ParticleRandom_ReadSample` /
`ReadDirectSample` in `include/monster_particle_random.h` (table-first random draws).

`notes/phase3-holdouts.md` and `notes/phase3-drafts/` hold the notes and best pin-free drafts for
functions an earlier pass set aside (many in your queue): start from them, don't redo what they
record, and look for the C form with the knob's effect. `notes/round11-c-holdouts.md` and
`notes/round11-d-holdouts.md` have older notes.

Per function, in queue order:
1. `python3 tools/scope/scope.py rom FUNC`, `scope.py like FUNC`, `tools/relatives.py FUNC --c`,
   `scope.py SRC pins FUNC` (the cost of each pin and a pin-free draft in build/scope/drafts/).
2. `scope.py DRAFT.c FUNC` shows the differing hunks with the C line and pass behind each; `why UID`
   explains one; confirm with a what-if knob. The knob that matches is the diagnosis, not the answer:
   find ordinary C with the same effect. Work on copies under `build/cx/work/FUNC/`.
3. Land with `python3 tools/scope/scope.py land FUNC` (refuses anything that adds a concession), then
   `python3 tools/check.py SRC` must MATCH, then write a commit request (message `FUNC: match without
   concessions`, or `FUNC: drop N pins` for a partial that removes some and adds none; name the
   C form that decided it in the message body).
4. Not matched after a real, deep effort (see P3_BRIEF "how hard to try"; there is no compile budget,
   and a scripted sweep over spellings is not diagnosis): save the best pin-free draft to
   `notes/phase3-drafts/cdp-field/FUNC.c`, add or update its section under a `## cdp-field (Codex)` heading
   in `notes/phase3-holdouts.md` (blocking decision, matching knob, what you tried, next idea), commit
   both, and move on. A partial that removes some concessions and adds none is worth landing.

Tool improvements count as much as matches: when scope's output was confusing, missing or wrong,
fix scope (`tools/scope/`), add a spelling to fix.py, or a hint, with a unit test (`tests/`), as its
own commit; run `build/venv/bin/python -m unittest discover -s tests` and `-s tests/tools` after.
New compiler facts go in `tools/scope/README.md` numbered 310-319 (your block; other workers have
other blocks).
