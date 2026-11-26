# Pending game-source fixes

Tags: #tooling #port #regression

Store reproducible source corrections here as repository-relative patch
files. Include the failing scenario, the original ROM behavior and the
byte-match check, or the inherited register evidence for a UBFIX branch.
Fixture setup errors belong to the fixture that produced them.

## Registry 1745 return

The scheduler-entry fixture successfully makes the link session. The
instrumented creator's success path otherwise falls off the end of a
pointer-returning function. The ROM at 0810B914 retains the Thumb return
address from the final call to Hud_SetMode (08177DEC). Procedure 6339 is
the observed caller and ignores the result. The port branch returns the
initialized singleton. The unguarded form differed by 34 bytes. The guarded
candidate passed `tools/check.py` at all 504 bytes and compiled for wasm with
UBFIX enabled. This patch is separate from the scheduler-entry fix.
