# Autopilot port stress run, 2026-11-25

Tags: #hardware #tooling #menu #scene

The shutdown path at 08224FA4 calls RFU's timer polling loop before it can
reach SoftReset at 082485B4. During the poll, each write to RCNT used to
clear the source port's history of the timer read. The bus now clears
that history for stores covering the polled address. SWI 0 stops the
live wasm invocation, and the host restores its stack pointer before
reentering AgbMain in the same cartridge state. EEPROM, RTC, frame numbering
and agent hooks survive the reboot. The 3,374-tick A+B+Select+Start and
Continue movie from checkpoint 0010 agreed with the original ROM.

The pause menu's equipment details update at 08173470 ran during the
swords-page test after the iron armor scene. Both the before- and
after-armor inventory movies matched the ROM. The original sprite trap
did not recur in these inventories.

The worktree-local harness counts input ticks at the first Thumb literal
load after the input function's PUSH, allowing mGBA to run whole frames.
The instruction-stepped, frame-loop and headless frame-loop variants
produced identical RAM samples in the 24-tick equivalence test. The last
comparison took 12.68 seconds stepped and 7.96 seconds with headless frame
execution under workstation load. Coverage-free replay also avoids
88.14 MiB of output per movie.

The 2,800-frame seeded field and menu trial from checkpoint 0010 matched
all 4,850 input ticks including setup. Its ROM trace reached 1,240
functions and saved scenes 0, 1430 and 3091. The trial reached 18 functions
listed by the missing-return scan. The case found the script-table call
to Background_LoadMapsFromScript at 08001EE0 that a direct caller scan
could not see. The branch carries 16 pending one-unit patches, each with
ROM register evidence and a guarded unit check. The port build can apply
these to generated C during the source reorganization.

Run the short cases with `port/tests/verify.py`. The same tool runs a
resumable checkpoint sweep with `--all --resume`. The local run report and
movie artifacts are under `build/`.
