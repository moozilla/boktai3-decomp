# Scene attachment, palette, and pool cleanup

Tags: #matching #types #phase3 #scene

These units remain 100% byte-identical, a change of zero percentage points.
The three objects together have 7 fewer report lines (8.0%) and 10 fewer
charged constant uses (11.0%), with zero linter errors.

| Object | Report lines | Charged uses |
| --- | ---: | ---: |
| Attachment | 51 → 46 | 54 → 47 |
| Palette and animation | 21 → 21 | 20 → 20 |
| Actor pool | 16 → 14 | 17 → 14 |

- The attachment slot reset (0806466C) clears four typed arrays in one
  indexed loop. The response replacement and return (0807C72C, 0807CA44)
  name the declared chase and route fields. The retarget callback
  (0807D220) uses the actor's declared tracking, motion and contact fields.
  The hit callback (0807D2E0) uses native drawable and position types and
  shares defeat recording and state entry.
- The 0x288-byte registry manager contains palette transitions at +0x64 and
  +0x118, with the existing subsystem still at +0x280. The affected
  08071E00–08072204 callbacks now use the real manager and drawable
  fields. All 433 dependents matched after the layout change and all 17
  palette-header dependents matched after removing its manager view.
- The pooled actor refresh uses indexed targets. Initial and requested
  actions use one typed transition. The free motion slot loop (08045748)
  reads the typed active mask and uses a regular counted loop. The full
  affected units match all 552, 2224 and 1376 bytes.
- The earlier 1b29b4a09 and b59f830c9 staging substitutions were reviewed.
  Direct stores change branch sharing or mask-load order, so the two
  source units retain their explicit masks. The b3ec0bde5 attachment change
  only reformatted source.

The direct home route at 0807CA44 changes register allocation and reloads.
The symbolic RNG table in the hit response at 0807D2E0 changes the burst
path's control and surrounding allocation. Removing the single-pass
animation flag computation at 080764C8 lets agbcc tie the wrong pseudo to
the flags register. Disabling that tie in scope matches. The clearest
ordinary C variants retain the tie, so the existing exact C remains.
The signed shift in the pool loop matters: the unsigned form reverses the
shift operand and changes seven bytes.
