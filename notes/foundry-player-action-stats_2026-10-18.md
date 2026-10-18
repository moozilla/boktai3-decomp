# Foundry player action and stat candidates

Tags: #player #matching #foundry #stats

Processed the assigned functions sequentially; all six are exact Foundry
candidates under wip/foundry/, with result files under build/foundry/.
No production integration or code-percentage gain is claimed (0 percentage
points integrated). No register pins, instruction assembly, or commits.

| Address | Provisional name | Checks | Exact bytes |
| --- | --- | --- | --- |
| 08146478 | Player_UpdateTimedCuePhase | 1 | 252 |
| 08146870 | Player_UpdateBriefAction | 1 | 188 |
| 0814A01C | Player_UpdateStatBAction | 1 | 268 |
| 0814A360 | Player_UpdateStatBEffectAction | 1 | 336 |
| 0814A530 | Player_CalculateActionStat | 4 | 360 |
| 0814A6B8 | Player_CalculatePreviewActionStat | 2 | 180 |

The first four share Player action mode, phase, frame and stat-B fields.
08146478 tests the halfword at 0x98 and low two bits at 0xA0 before playing
a phase-one sound; these were carved from Player padding with tentative cue
names. Player hdrcheck preserved all 16 matched consumers.

0814A530 uses action definition bytes 0 and 3 (kind and tentative stat value),
then applies three callbacks at context+0x134. The callbacks formerly represented
as Player fields at 0xC78 are now inside its stat context at the same absolute
offsets. The inventory definition padding was split without moving baseValue.
Both changed headers passed all 16 matched consumers each. Kind 0x2F selects
level+5, and scene flag 0x1000 subtracts 15 for kind 0x24. Gameplay stat identity
remains uncertain. An unsigned count+1 switch with explicit cases 0/1/2/default
and a signed-magnitude shift helper reproduced the jump table and scaling.
One of its four checks was a compile failure caused by an existing callback
trampoline declaration; the draft now uses that declaration correctly.

0814A6B8 temporarily swaps context duration, mode and entry, calls the normal
calculator and restores all three. Mode four calls the alternate calculator
with two arguments. The signed snapshot load uses the established base+10
idiom at Player+0x32C to reproduce address calculation. All interpretations
are static evidence; no new runtime coverage is asserted.
