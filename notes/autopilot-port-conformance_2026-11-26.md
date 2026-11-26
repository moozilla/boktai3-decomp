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
source preparation. The 85 patches were relocated against the main source
layout at `744333be7`. All 76 affected units passed individual ROM checks
and wasm32 syntax checks after applying the patches together. The relocation
preserves every added and removed C line. The source address and check
records accompany the patches in the worker archive.

A cold port build combined the worker changes with that source layout and
all 85 relocated patches. It linked with zero undefined symbols and zero
signature mismatches. The type generator preserved the full include paths
for nested headers and emitted the same set of 559 typed globals as before.
The one header diagnostic carried over from the previous layout.
The short suite matched 25,955 ticks across 13 recordings in 76.2 seconds
using two workers and validated ROM captures. Seed 1 at checkpoints 0022
and 0065 matched another 4,850 ticks each against the earlier ROM scripts
and captures. The artifact hashes are in the worker's
`build/return-layout-integration/verification.json`.

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

Each short recording now fixes the ROM's input masks using its verified
tick replay, while the port continues to take input by video frame.
Shortening the Sealed Prison Right press by one frame preserved the
total poll count and failed at tick 1690. The ROM script and capture
agreed byte for byte with the verified replay. The 13 unmodified
recordings passed in 109.8 seconds using two workers and retained ROM
samples. A focused seeded run also matched its earlier samples.

The comparison samples the emulated KEYINPUT register at each port poll.
Recorded fixtures check it against their fixed tick masks, and seeded runs
check it against the host masks sent to the ROM. All 13 recordings passed
in 102.6 seconds using the existing ROM samples. The shifted input still
failed at tick 1690, and checkpoint 0065 seed 1 matched all 4,850 ticks.
