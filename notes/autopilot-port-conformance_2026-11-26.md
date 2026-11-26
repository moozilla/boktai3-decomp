# Autopilot checkpoint conformance, 2026-11-26

Tags: #port #autopilot #verification

Three 2,800-frame controller seeds ran from each of the 83 checkpoint saves.
All 249 movies and 1,257,166 sampled input ticks matched the original ROM
after the wasm ABI fix. The samples cover scene, position, life, energy,
HUD callbacks and page, previous input, and the random index. The baseline
seed 1 sweep matched 73 of 83 movies. The final seed 1 pass matched all 83,
a gain of 12.0 percentage points.

Nine baseline failures passed a full word to the halfword glyph index at
08219728. Checkpoint 0004, for example, passed 65,540 where the ROM used 4.
The renderer faulted after tick 4907. Checkpoint 0065 passed 16,777,215 to
the byte orbit angle at 0822223C and faulted after tick 4119. The sampled port state matched the ROM through every input tick before
those failures. Rewritten wasm calls omitted the defining narrow-argument
extension attributes. Keeping those attributes on direct calls and
indirect thunks fixed the ten replays.

`make port-verify` replays 13 checked-in controller recordings in 35 seconds
on the worker. One recording opens the inventory in checkpoint 0022 and
catches the glyph trap on the old wasm. The other recordings include area
smokes, equipment details and soft reset followed by Continue.

The matrix reached 81 functions from the initial 226-warning missing-return
scan. The pending overlay directory now supplies a checked source patch
for every reached function. Applying the overlays brought the Clang scan to
139 fallthrough warnings, with zero from the reached set. The combined build
passed the short suite and the full 249-movie matrix with 1,257,166 matching
ticks. Each movie on the combined build produced the same emulator script as
the ABI-fixed build and matched the original ROM samples byte for byte. The
sweep reached no additional fallthroughs.
StatusScreen_Destroy at 0800F1BC preserves the address one word past its BIOS
fill source in the port address space.
Modifier_OnTableTaskDestroy at 0811DEE0 preserves the retiring owner through
the matching published table pointer, confirmed by a ROM probe.
The pending patches can be applied to generated copies before the port's
source preparation. The source reorganization can integrate them by symbol
and recheck each unit separately.

The worktree report retains the complete save/seed table, baseline trap
ticks, reached functions and overlay verification results.

For follow-up port builds, `verify.py --rom-reference DIR --out-root NEW`
compares fresh port execution against saved emulator ticks when the ROM, save,
full harness script and tick dump agree with the reference hashes. It invokes
the emulator when any input changes or a capture is incomplete. Sealed Prison
produced identical tick dumps in both modes, with the comparison taking
18.4 seconds directly and 6.4 seconds with a reference on the worker.

Bank sweeps record a checksum for each successful comparison. The runner
checks the result, save, wasm, ROM and verifier inputs before skipping a
case under `--resume`. Checkpoint 0022 seed 1 took 15.6 seconds with a saved
ROM reference and 0.1 seconds when resuming the same comparison.

Recorded replays skip pixel rendering. The three equipment cases took
98.0 seconds with rendering enabled and 28.4 seconds headless on the
loaded worker. All 25,955 sampled ticks in the suite were identical.
The inventory recording still trapped before tick 1709 on the old wasm.
With reused ROM samples, the full headless suite took 145.9 seconds on
one worker and 80.1 seconds on two. The two-worker suite finishes the
in-flight pair after a failure, then exits before launching another pair.
