# Round 14: volatile cleanup and remaining compiler-dead sites

Tags: #round14 #concessions #matching

## Completed volatile assignment

The seven affected files started with six volatile errors and five warnings.
They now have no errors or warnings, with compiler diagnostics enabled.

- The scene actor manager destructor, initializer, and factory use the existing
  `SceneActorManager` and `SceneActor` types. Their private copies had declared an
  unused minute field volatile. Initialization also clears its fields directly.
- `sub_080148CC` needs no volatility on either its particle flags or the flag
  helper. Removing both qualifiers preserves the exact function.
- The effect pool uses an ordinary occupancy word. Its allocator has the same
  instruction sequence as `SceneMovingParticle_AllocateEntry`; that sibling's
  ordinary indexed loop replaces the volatile pointer, constant temporary, and
  address-dereference expression. All 16 functions in the unit still match.
- `SceneMenu_WaitForConfirmation` uses an ordinary bounded `for` loop and passes
  its acknowledgement mask directly to the existing helper. The participant
  count is no longer volatile. The normal loop retains the original reads.

These changes are in `bdb97efda` through `ca23989e4`. Stock checks passed for all
changed units. The sole dependent of `particle_effect_pool.h` also matches.

## Stage 2 checkpoint

The assigned files are `sub_08038448.c`, `sub_0803B40C.c`, `sub_0805AC3C.c`,
`slime.c`, `dog.c`, and `spread_shot.c`. The before snapshot is `ca23989e4`.

| Kind | Before | After |
|---|---:|---:|
| dead-code | 14 | 8 |
| proved-zero | 1 | 1 |
| once-loop | 3 | 3 |
| stmt-expr | 2 | 2 |
| const-local | 33 | 33 |
| addr-deref | 6 | 6 |
| volatile-view | 2 | 0 |
| local-union | 2 | 2 |
| Total warnings | 63 | 55 |

Both snapshots have zero errors. No ignore comments or detector changes were
added. These are counts for the six assigned units, not the entire repository.

### ScenePanel_TickRig: fixed

Commit `ba319f43c` removes the two fabricated negative-value arms, two volatile
views, and the interval temporary reused for a different sample. Direct
comparisons of the shared schedule's frame and interval fields preserve the
original four timer reads. Stock agbcc produces the exact 220-byte function.

The first comparison detects expiry. The second checks the last 90 frames and
the blinking frame bit. No additional calls, masks, loops, or source-level
identities are needed. This removes two dead-code and two volatile-view sites.

### Dog_UpdateCardinalTurn: shared helper restored

Commit `34a3cdc12` removes the private `DogTurn_SelectAnimation` duplicate and
uses `SceneActorTurn_SelectAnimation`, already used by the Golem, Ghoul, Mummy,
and Skeleton turn routines. The 1,880-byte function and entire 37,088-byte Dog
unit match with stock agbcc.

The four reports were stores in generic playback-flag branches that become
unreachable for particular constant arguments. They were not fabricated game
logic. Those meaningful generic branches remain in the shared helper. The
reported count falls through removal of the private duplicate; this should not
be described as deleting four gameplay branches. No header was modified to
relocate code beyond the detector.

## Open decisions

### 08038448: WirelessGameMenu_InitEquipmentChoices

The three dead-code sites are fabricated: a negative test of an unsigned
halfword guards a call, arithmetic, and stores. They remain in the committed
source pending an exact ordinary replacement.

Deleting that block and using the shared `GameState` fields leaves eight
differing instructions. The differences are the unused saved high register and
the scratch register used for the random-index address. Using the actual
`gRandomTable` symbol restores both naturally, but the draft then keeps the
table address in that high register. The ROM instead reloads the address at
the sample. That candidate has three ROM/five draft differing instructions.

Tested alternatives include an explicit inner-loop limit, an offset-based
search loop, a named selected-bit value, signed random index, a private random
helper, and the existing `ParticleRandom_ReadDirectSample`,
`ParticleRandom_ReadSample`, and `ParticleRandom_NextSample` APIs. None matches.
The direct and shared sample helpers retain the same remaining table-hoist
difference. Disabling that hoist diagnostically also loses the unused save;
reload rotation alone does not solve it.

The next task is to recover the authentic random-table/loop dataflow that leaves
the saved register without retaining the hoisted table. Recreating a dead
carrier is not an acceptable solution.

Candidate store: `build/matchsearch/round14-equipment`. Useful nodes are
`ordinary-shared`, `table-symbol`, and `direct-shared`. Drafts are under
`build/round14-dead/equipment-*.c`.

### 0803B40C: WirelessResultPanel_Init

Three compiler-dead stores and one proved-zero pointer offset remain. The
existing source also contains bounded resource-pointer guards whose effects
are register-only, a pointer union, and a dead pointer read. These were all
explicitly identified as allocation machinery by the round-13 commit.

A candidate removes those forms while retaining the already-counted zero
temporaries for a separate cleanup. It differs by eleven ROM/nine draft
instructions. Two redundant source-copy moves disappear before the resource
block copies. The other differences concern the saved sprite-priority address,
the first sprite-flip access, and the final state stores.

Natural early-return checks restore both source-copy moves but move the shared
failure tail. Positive resource-success arms produce the same result as the
original goto form. Other tested alternatives include ordinary `memcpy`,
separate sequence/priority locals, an indexed widget loop, and the existing
`DisplaySequence_StepSprite` helper. None is exact. Diagnostic reload probes
confirm the first-flip and final-state differences independently.

The next task is the original resource-copy/error-control structure, followed
by the natural lifetime of the saved priority address and state stores. No
new zero temporary, union, guard, or dead read should be introduced.

Candidate store: `build/matchsearch/round14-result`. Useful nodes are
`no-scaffold`, `no-dead-existing-zeros`, `early-returns`, and `shared-step`.

### Slime_UpdateRandomWait

The one reported store is in the private generic animation helper's playback
flag branch, at a call whose flags argument is zero. The existing shared
`SceneActorTurn_SelectProfileAnimation` produces the same function except for
one unused high-register copy and alignment padding. Natural byte-typed local
flags and byte-typed parameter variants have the same difference. A fully
specialized zero-flags helper changes more of the emitted function.

`notes/round9-slime.md` explicitly says the private post-call byte flags were
chosen to preserve that copy. Consequently this warning is left visible:
neither a header move nor an ignore would establish the missing original
source evidence. The common animation helper does support the legitimacy of
the generic branch, but does not explain that particular narrowing.

Candidate store: `build/matchsearch/round14-slime`; useful node
`shared-animation`.

### SpreadShot_ResolveTerrain

The reported call is inside the bidirectional `Seek` helper at a call with
target equal to the current cell minus one. That invocation cannot take the
forward scan. Other invocations use the same helper for successor cells and
both row directions. This is an ordinary specialization of a real algorithm,
not a fabricated negative-value arm.

A backward-only helper for that known predecessor call removes the report but
changes 184 ROM/75 draft instructions. Sharing that backward helper with the
general `Seek` changes substantially more. Neither candidate was landed or
excused. The existing source's narrowing round trip and declaration placement
also remain separately documented allocation choices in
`notes/cloud-08122DF4.md`; removing the warning would not establish that the
whole function is free of such choices.

Candidate store: `build/matchsearch/round14-spread`; nodes `backward-call` and
`shared-backward`. The baseline remains exact at 8,964 bytes.

## Reproduction

Run the full linter on the six assigned source paths with compiler checking
enabled. All candidate stores and drafts are ignored working artifacts; the
notes above preserve the actionable conclusions without committing generated
compiler or ROM dumps. A combined ROM build belongs to integration and was not
run in this worker.
