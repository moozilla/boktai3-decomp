# Player terrain, force, particles and camera foundry pass

Tags: #player #foundry #matching

Processed 08140748, 081408AC, 081411B8, 0814131C, 08141404 and 081414E4 in that order. Sources remain under `wip/foundry/`; result records are under `build/foundry/`. No commits or production promotion; no combined ROM build was run.

Exact candidates: 081408AC (5 checks), 0814131C (1), 08141404 (3), 081414E4 (4, including one compile failure when the camera fields were found already named in the shared header).

Retained fuzzy candidates: 08140748 at 79.5% after 5 checks (timer address formation and allocation); 081411B8 at 95.7% after 5 checks (CORE zero, allocation and a redundant null-test branch target). Fuzzy scores are candidate comparisons, not exact progress.

All six use `struct Player` in `include/player.h`. Added only padding replacements: recovery terrain cell at 0x28A; particle emission count at 0x442; action latch bytes at 0x464/0x465; recovery phase/active bytes at 0x47A/0x47B; action phase/active at 0x488/0x489; particle interval counter at 0x4CA. Each header edit was followed by foundry hdrcheck; all 16 matched consumers remained exact. Existing cameraOffsetX/Y/Z fields cover 0x4B0/0x4B2/0x4B4.

Names are provisional aliases at the top of each candidate. 081408AC derives a facing direction from the recovery terrain cell relative to terrainTarget. Separate coordinate subtraction statements prevent unwanted reassociation. Separate shift and increment statements reproduce the facing calculation. 081411B8 subtracts a sine-table force from velocity with signed magnitude rounding. 0814131C tests game-state feature flags and a positional predicate, whose meaning remains unknown. 08141404 emits one particle burst after 15 eligible ticks, using the collider position in modes 3/4 and an elevated player position otherwise; signed packed coordinates reproduce the vertical shift. 081414E4 clamps camera X/Z offset sum and difference to +/-800 relative to half the Y offset, using the original pre-adjustment values for both phases.
