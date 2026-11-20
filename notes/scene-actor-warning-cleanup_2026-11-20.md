# Scene actor warning cleanup checkpoint

Tags: #scene-actors #phase3 #matching #0801E000 #080297E8

The first six ranges in the scene-actor queue were checked with stock agbcc.
Every changed source and every changed header's dependents matched the ROM.

- The scene-object and common actor controllers use the recovered slot, pool,
  pose, and event packet layouts. The ambient particle sampler uses a halfword
  cursor and the existing random-table reader.
- The graphics and animation ranges share the actor's playback resource, widget,
  action fields, facing, and timed recovery. Route, travel, lunge, and recoil
  handlers reuse the charge-vector projection.
- The reaction phase and timer are shared by Vampire damage, death, and sway.
  The grab handler uses the initializer's 26-word target table. The Vampire
  presentation callback uses one four-byte blend control record.
- Direct flag literals often move before or after the field load. These sites
  remain open where there is no shared game operation. The early inline
  parameter work is inventoried in the worker report for review.
- The linter now identifies the preexisting duplicate flag-assignment arms in
  the contracting shake handler at `08027AD4`. The stock match survives
  only with the branch in place so far.
