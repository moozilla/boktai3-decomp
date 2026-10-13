# Dialogue interpreter and scrolling controller

Exact progress: 20.001% -> 20.230%, +0.229 percentage points;
36 additional functions and 5,520 reported code-span bytes.
Without register pinning: 16.016%; register-pinned: 4.214%, included above.

All named functions in 08031D40-080335D4 now match. Existing unnamed
assembly tails remain assembly; this does not claim those tails are recovered.
The 948-byte dialogue initializer at 0803198C remains an actual gap.

The text interpreter consumes narrow/wide glyphs, 0x1F escape codes and
angle-bracket commands. Command strings identify WAIT, PROC, NAME, BIKE,
LABEL, NUM and sound/voice controls. LABEL selects speaker portraits;
its string comparisons name Django, Sabata, Otenko, Rita and other characters.
Three Django variants share selector zero; NONE uses selector 255.
Numeric substitutions use the signed values at VM+0x88; string substitutions
use pointers at +0x110. Styles at +7 select the primary stream at +0x1C or
four temporary streams at +0x20..+0x2C, with a seven-entry stack at +0x109.
Tag parsing stores its value and length at +0x38 and +0x34, respectively.
These are observed views, not a claim that every field has been identified.

The scrolling task singleton is gUnk_020000E0. Its text VM begins at +0x18.
Flags +0x17C..+0x181 control visibility, requests, scrolling, delay and blocking.
The update count at +0x184 and reveal clock at +0x188 are distinct.
Columns +0x18C and rows +0x190 wrap at 32; +0x194 controls padding/wait glyphs
and +0x198 retains the script. The update processes wide glyphs on alternate
clock phases and clears rows in both drawing layers before revealing text.
The initializer stores a default script and show/hide callbacks manage the
shared visibility flag at gUnk_03004280.

Fast reviewed-extent checks passed for each promoted source. Combined ROM validation printed `build/boktai3.gba: OK`; no per-function full builds or new tools.
Register hints remain explicitly marked temporary concessions. The line-width
routine preserves the original discarded Text_FindChar return, rather than
silently correcting original behavior while trying to match it.

Remaining work: dialogue initializer 0803198C, choice initializer 080311F0,
input/window control 080313E4, then the adjacent multi-window manager at
080335F4 onward. The new user target is 25%, with the latest allowance overriding
older handoff floors. No delegation or additional tooling in this continuation.
