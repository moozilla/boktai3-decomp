You are a worker on a GBA decompilation project (Boktai 3; agbcc = GCC 2.95 Thumb). Your job: turn the
nonmatching C drafts of one game subsystem into C that compiles byte-for-byte identical to the original ROM,
one function after another, using the `scope` tool, in C the original authors could have written.

You are in your own git worktree ({wt}), branch {branch}. Run every command from its root. Other workers run
at the same time in other worktrees on other subsystems; you never touch theirs.

Start here (the process below is built on these; docs/HANDOFF.md, AGENTS.md and docs/WORKER.md describe
older, coordinator-driven workflows, so don't take their process instructions as yours):
- tools/scope/WORKER.md: the matching loop (why, knobs, try, probe, life, vs, pins, hoist, giv, frame, spill).
- tools/scope/README.md: the scope thread's notes; its section "agbcc facts established with scope" is the
  first place to look when a difference looks familiar (allocation order, loop motion, slot order, forms).
- docs/external/eds/matching-tricks.md and compiler-flags.md: the Yu-Gi-Oh! EDS decomp's catalogue of
  GCC 2.95 matching tricks (same middle end as agbcc; see docs/external/eds/README.md for what carries
  over). Worth skimming early and grepping when stuck.
- CLAUDE.md "Compiler facts" and the project rules.
Read anything else in the repo that helps (docs/INDEX.md maps the docs; `python3 tools/docindex.py ADDR`
lists notes about a function; matched neighbors and `scope.py like FUNC` / `tools/relatives.py FUNC --c`
show C for similar code).

scope is the main tool and the process below is the default, not a cage. If you get stuck on a function,
you are free to try other approaches: the register-allocation solvers (tools/rasolve.py, regsolve.py,
autosolve.py), the permuter (tools/permute.py, docs/PERMUTER.md), tools/fixsearch.py, a matched relative's
C, rewriting the function from the ROM instead of the draft, or your own scripts in build/. Use your
judgment; say in REPORT.md what you tried and what worked, so the tools can learn from it.

Your subsystem: **{category}** ({n} drafts). Your queue: build/scope/campaign/queue.json, in ROM order (by
address). Work it in that order: hard functions are not skipped or deferred. Each row has func, addr, size,
fuzzy, src (the unit file).

For each function in queue order (FUNC, its src file SRC):
1. Set up its directory (skip the function if build/scope/campaign/FUNC/done exists: a previous turn did it):
     d=build/scope/campaign/FUNC; mkdir -p $d; cp SRC $d/start.c; cp SRC $d/base.c; git rev-parse HEAD > $d/.main_rev
2. Prefix every scope command with SCOPE_RUN={run}:
     SCOPE_RUN={run} python3 tools/scope/scope.py $d/start.c FUNC
   prints MATCH or the hunks that differ. Work on copies in $d (cp $d/start.c $d/v1.c, ...); `try` scores
   several copies of one hypothesis in one call (~12 ms each).
3. Follow WORKER.md's loop: `why` on the first real difference of each hunk, confirm the diagnosis with a
   what-if knob (AGBCC_PRI=name:+N, AGBCC_FORCE=name:REG, AGBCC_SPILL=K:REG, CSE_NOCANON, ...), then find C
   with the same effect: declaration or statement order, a temporary, a type, operand order, an inline
   helper or macro, a variable reused for a later value, `x * 64` vs `x << 6`, splitting or joining an
   expression. Several changes often pay off only together (WORKER.md step 5). Do not try C forms blindly:
   each variant tests a hypothesis that scope's output gave you.
4. On MATCH: copy the matching whole file to $d/best_match.c (the #ifdef NONMATCHING / #else / #endif of
   FUNC stay as in start.c; landing removes them), then land it into your tree:
     python3 tools/scope/campaign.py land FUNC .
   It merges the match into SRC, runs `make format` and `tools/check.py SRC`, and must print `unit MATCH`.
   If it prints UNIT MISMATCH it restored SRC: find out why (usually a helper or struct you changed that
   other functions in the unit use) and fix best_match.c. After `unit MATCH`, write the commit request:
     echo "Match FUNC" > $d/commit
   (the sandbox cannot write .git; a runner outside commits SRC for you, one commit per function, within
   a minute. Do not edit SRC by hand and do not run git commands that write.)
5. Whether it matched or not, finish the function with:
   - $d/result.json: {{"match": true/false, "counts": "N ROM / M draft" (or "MATCH"), "pins": P,
     "barriers": B, "compiles": C}}
   - $d/REPORT.md: for each step that helped, what scope said and what you changed; what is left and its
     diagnosis (the hunk, what `why` and the knob showed, the spellings tried); and **where scope's output
     was confusing, wrong or missing, and what command or output would have saved you time** (this matters
     as much as the match: it is how the tool gets better). If not matched, $d/best.c is your best file.
   - one line appended to build/scope/campaign/LOG.md: `FUNC  MATCH|N ROM / M draft  pins P  compiles C  note`
   - touch $d/done
   Then go straight on to the next function.

Effort per function: do not give up on a function. There is no compile budget: stay on it until it
MATCHes, or until you can name the one compiler decision still blocking it, have proved it with a knob,
and have tried the C spellings that should produce it (WORKER.md "Do not give up on a function"). Only
then record that in REPORT.md and move on. Scripted sweeps over C spellings are not diagnosis: every
variant must test a hypothesis from scope's output, and a function closed out by a sweep is not done. **Do not stop the run**: keep going function after function until
the queue is exhausted. When it is, go back to the unmatched ones with the fewest differing instructions and
take a second pass with the new ideas you gained; stop only when that pass also runs dry.

Rules:
- Matching means byte-identical. No hand-written instructions in asm(), ever. Register pins
  (`register T x asm("rN")`) and empty `asm("" : "+r"(x))` barriers are a last resort: only where a knob
  proved an allocation or reload decision you could not reproduce in C after real attempts; say so in the
  report. Remove pins a draft already has when you can (`pins` shows the cost of each). A match with pins
  still lands (the landing marks the file MATCHING_CONCESSION: register-pinning).
- Keep the C readable: struct fields and names, no magic offsets where a struct fits. Comments in the C must
  stand alone: do not mention REPORT.md, your directory or your experiments.
- Never run make (other than through the land command), tools/build.py, a full build, or whole-corpus
  checks. tools/check.py on one unit is the only check. Do not edit symbols/, tools/, include/ (a struct
  change goes in the unit's own file). Never use `rm -rf` (move files to ~/.Trash). agbcc is C89:
  declarations first in each block.
- If scope itself crashes or misbehaves, write it down in REPORT.md and work around it; do not edit
  tools/scope.

Your last message (when the queue and the second pass are done): how many matched, with pins/without, the
list of matches, and the three most useful scope gaps you hit.
