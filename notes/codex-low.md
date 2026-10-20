
## Task 080DE6E0 (2026-10-20)

Tags: #matching #dog

- 080E059C: matched as Dog_ResumeAfterReaction (tentative interpretation). Entry consumes field2B2, selects animation 11, clears field2B3; completion resumes group control (flags2D4 bit 0x80) or idle. Existing Chase_SetCtl/Chase_SetBehavior reproduce the reset sequences exactly. Two necessary register pins control the motion mask and group flag insertion; depin retained both. Formatted source passed check.py with zero differing bytes (436 bytes).
- 080E4834: drafted 82.3% as Dog_ResumeRoute. Takes arrival, invokes control, attempts route toward context+0x6D8, then selects group chase or saved route. Uses shared chase/terrain helpers. Reviewed full-extent check reports 76 realigned differing instructions, 23 ignoring registers. Candidate is 12 bytes longer; helper expansion and literal/register placement remain. The queued commit title uses the preliminary candidate-length score 81.7%; the source's recorded full-extent score is authoritative.
- 080E25FC: inspected largest first with asmat, relatives and condensed listing. 1976 instructions; matched relative 080E4468 covers only 38%, largely repeated terrain lookups. Entry plays song 0x211 or 0x13E and tests motion/action flags. No complete candidate produced; stopped without a promising close port.
- 080E0750: inspected asmat, relatives and condensed listing. 1875 instructions; best relative covers 32%. Entry consumes field2B2 and quarters saved motion components before selecting directional animation 3. Four repeated terrain-boundary/height runs identified. No complete candidate produced; stopped without a promising close port.
- 080DE6E0: inspected full assembly. Animation-frame sound lookup from gUnk_086061E4; frame kinds 1/3 and frames 1/5/7 can spawn randomized SpriteEffectController effects behind actor direction. Vector stack construction and effect argument layout need reconstruction; no candidate retained.
- 080DEA7C: inspected full assembly and caller Dog_FinishFrame. Updates body/auxiliary collider positions and sizes, rounded to cardinal direction; context+0x678 bit 2 chooses enlarged offset/extents. Uses packed vector word updates on stack; no candidate retained.

Ghidra pseudo-C unavailable: gen/ghidra/decomp.c is absent. No full build or network operations performed. Only per-file check.py acceptance used; integration must validate combined ROM.
