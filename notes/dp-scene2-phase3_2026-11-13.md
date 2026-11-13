# Scene actors and NPCs: second-half concession removal

Tags: phase3, actor-animation-controller, scope, 080F3230, 081AFD10, 081B08B8, 081B66E0, 081E34D0–081F0F68

The worker removes concessions in ROM order and checks each changed unit with the stock compiler. Integration owns the combined ROM build and progress totals.

## Matching forms

- `PhaseObject_ConvergeEntries` (080F3230): a byte-sized enable value removes the extra QImode constant and recovers reload rotation without DEBUG_NOP. Entry presentation fields replace position offsets.
- `PairedSceneActor_UpdateHalfPresentation` (081AFD10): existing actor fields, direct sine-table references, destination-first unsigned vector sums, and a block-local mask recover allocation and tail sharing without 19 pins and nine barriers.
- `ActorStatus_UpdatePlayerApproach` (081B08B8): repeat the completion stores in the earlier case. Reload sees both tails and rotates its scratch choice; jump2 merges them. `AGBCC_SPILL=13:2` isolated the original two-row difference.
- `VanargandEffect_Initialize` (081B66E0): palette groups 0/1 and 2/default each join before shared inline parameter setup. The joins stop unwanted field-address propagation and later move2add reuse; jump2 shares the setup. Scope fact 130 records the evidence.
- Actor palette and phase-entry routines use the shared mode and phase bitfields directly. The controller owner at offset 0x18 is a `Player *`, with unchanged layout. Both including units match.
- The variable phase setter (081E4B0C) uses the shared `Actor_SetState` helper. A direct expansion ranks the phase-field pointer just above the controller (5384 versus 5342); `AGBCC_PRI=g:+43` confirms the swap. The shared inline form matches naturally.
- The curve-to-position initializer (081E4DB4) matches through ordinary owner position fields, removing its volatile pointer reads.

## Tool evidence

Scope now explains same-base address reuse by `reload_cse_move2add`. It requires the earlier physical copy/add, matching pseudo base, correct immediate difference, and uninterrupted register history. Labels, calls, jumps, register writes and implicit address updates invalidate the evidence. Unit tests cover positive and negative histories. Both required test suites pass.

The relatives inventory is reused across the queue: generating full progress independently for each query dominated runtime. No source or compiler result is cached in place of the per-function stock checks.

## Controller field recovery and final queue items

The phase helpers now use the named phase bitfield, clearing their callers' inherited raw-state concessions. The controller's owner remains at the same offset with type `Player *`; releaseControl, blinkTimer and blinkInterval replace previously anonymous storage without changing layout. Both header users pass the stock unit check. Curve and script entry functions use the existing sprite, position, resource and handle fields, preserving load/store order. All touched units pass tools/check.py.

`SingletonSlot7_UpdateEndingScene` (081EF914) loses both r5 animation-mask pins when the playback wrapper copies its state parameter to a local. The caller's state and inline helper's state get distinct pseudos and recover the ROM allocation. Two original transition-zero pins remain.

`SingletonSlot7_InitDisplaySprites` (081F0F68) uses that same local-pointer playback wrapper and separates foreground/accent state and priority lifetimes. It matches without all eight pins and five barriers. The existing DepartureActor layout supplies every former raw offset, and its duplicate drawable type is removed.

The queue audit finds 44 of 48 functions free of concessions (91.67% by function count, a 91.67 percentage-point gain within this assigned queue). The other four retain five original pins in total: blink disappearance and finishing animation each keep an unused-zero pin; foreground display keeps its horizontal-sum pin; ending scene keeps two transition-zero pins. The blink and finishing functions each lost one flags-value pin. Their best pin-free drafts and compiler evidence are in notes/phase3-holdouts.md under dp-scene2 (Codex), with the source drafts in notes/phase3-drafts/dp-scene2/.

Scope's move2add explanation passed 407 root tests (two skipped) and 127 tools tests. The known matchsearch socket fallback problem is avoided with --local; it was not changed. The coordinator must perform the combined ROM build at integration.
