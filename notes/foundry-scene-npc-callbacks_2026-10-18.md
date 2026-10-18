# Foundry: scene NPC callbacks, 2026-10-18

Tags: #matching #foundry #state-actor

Worked through 081CF490, 081CF84C, 081CFCA4, 081D1720, 081D1A44,
081D2118, 081D2544 and 081D25AC in order. Candidates remain under
`wip/foundry/`; result records are in the corresponding `build/foundry/`
directories. Nothing was committed or integrated, so production code progress
is unchanged (+0 percentage points).

Exact candidates: 081CF84C (animation 12), 081CFCA4 (projected idle),
081D1720 (current-position target), 081D2118 (oscillating palette blend),
081D2544 (requested-state entry), and 081D25AC (state tick and palette countdown).
Their final formatted candidates were checked against the completed shared header.

081CF490 retains a CORE-zero, 89.4% candidate: the phase mask needs an extra
register copy and the final comparison uses different registers. It explicitly
reloads the phase byte using a volatile-qualified read to preserve the observed
load; revisit this source choice along with allocation. 081D1A44 retains a
CORE-zero, 68.6% candidate: its facing conversion has identical operations but
different temporary and mask registers. No register pins or instruction bodies
were used. Budgets include final layout verification.

The existing `state_actor.h` describes the same command/state layout used by
these NPC callbacks. Extended its padding with observed facing bytes at 19–1B,
a tag subobject at 20, an angle at 65, and the animation callback at 2E4.
The tag's +8 word overlaps the +A halfword cleared by 081CF84C; represented
this as a union. Its +18 attachment is updated by 081D2544 and +1C palette
blend count is decremented by 081D25AC. Existing fields retain their offsets.
The final header check passed for the existing matched consumer.

081D1908 interpolates 5-bit RGB palette channels; the provisional alias is
SceneNpc_BlendPalette. 081D2118 feeds it alternating 60-frame ramps.
The state callback table at 0861265C is provisionally gSceneNpcStateCallbacks.
Names remain low confidence; animation and projection ownership come from
known callees, while state/command meanings follow the existing StateActor view.
