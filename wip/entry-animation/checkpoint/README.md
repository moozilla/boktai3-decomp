# Nonmatching candidates — do not merge

Source-only checkpoint after the entry point and contiguous subsystem batch.
All files here are **nonmatching** and excluded from production progress.
Resume with `build/venv/bin/python tools/check.py PATH --context 1`.

- Directed movement (`081E728C`): best saved near draft; two animation updates,
  turning, approach and a height-completion gate. Roughly 723 differing bytes
  in its earlier check; register allocation and object-pointer scheduling remain.
- Actor creation (`081EA4DC`): readable typed draft; palette copy and child
  initialization are understood, but register allocation/loop shape remain.
- Child visibility (`081E9BA4`): current candidate; parent/child addressing
  and live-register allocation are unresolved.
- Idle callback (`081E42C4`): previous near draft; do not repeat blind mutations.
- Spawn family (`08160EA4`): historical 3-byte register mismatch candidate.
  Empty register-constraint experiments did not improve it. Siblings remain WIP.
- Sound-test playback indicators (`081E2094`): two readable control-flow
  alternatives. The goto version had 154 differing bytes at the correct 192-byte
  extent; the other source is easier to read but compiles to 208 bytes.
  Local pins, signedness, case order and control-flow rewrites did not finish it.
  A 120-second existing-permuter run reduced its internal score from 2915 to
  1675, but no exact match. Its best mutation removed a case break, so it is
  not saved here as a behaviorally sound improvement. Do not count that score
  as semantic verification or exact progress.

The 764-byte sound-test initializer was resolved and is in production: separating
its first temporary asset variable, then clearing the vector through an inline
helper which accepts both source and destination, reproduced the original code.
Do not resume the obsolete initializer variants.

Other failed experiments remain in the local ignored working directory. This
checkpoint retains selected reusable sources rather than generated disassembly,
ROM bytes, build outputs or large permutations of the same idea.
