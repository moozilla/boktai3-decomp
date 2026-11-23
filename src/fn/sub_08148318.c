#include "contact_swarm.h"
#include "display_sequence_inline.h"
#include "player.h"
#include "player_cue.h"
#include "player_cue_sequence.h"
#include "engine.h"
#include "ram.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"
// confidence=low: random neighboring directions with short hold durations.
u32 Player_ConsumeStateEntry(struct Player *);
s32 Player_GetSmoothedDirection(struct Player *);
void Player_SetDirectionalVelocity(struct Player *, s32);
static inline s32 WaveringMovement_NextSample(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index * 2);
}
void Player_UpdateWaveringMovement(struct Player *p)
{
    u8 *timer;
    if (Player_ConsumeStateEntry(p))
        p->directionHold = 0;
    p->statusFlags &= ~1;
    timer = &p->directionHold;
    if ((*timer) == 0) {
        s32 direction = Player_GetSmoothedDirection(p);
        s16 choice;
        if (direction & 1) {
            if (direction > 4) {
                p->heading = (8 - direction) >> 1;
                p->side = 1;
            } else {
                p->heading = direction >> 1;
                p->side = 0;
            }
            choice = WaveringMovement_NextSample() & 7;
            if (choice <= 5) {
                p->facing = direction;
                p->directionHold = 9;
            } else if (choice <= 6) {
                p->facing = (direction + 7) & 7;
                p->directionHold = 3;
            } else {
                p->facing = (direction + 9) & 7;
                p->directionHold = 3;
            }
        } else {
            u32 randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
            u32 sampleOrigin;
            choice = WaveringMovement_NextSample() & 7;
            sampleOrigin = randomOrigin;
            if (choice <= 5) {
                p->facing = direction;
                *timer = 9;
            } else {
                if (choice <= 6)
                    p->facing = (direction + 7) & 7;
                else
                    p->facing = (direction + 9) & 7;
                *timer = 3;
            }
            if (!(p->facing & 1)) {
                s32 sample =
                    ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
                s32 facing = p->facing;
                s32 offset = ((sample & 1) << 1) + 7;
                direction = (facing + offset) & 7;
            } else
                direction = p->facing;
            if (direction > 4) {
                p->heading = (8 - direction) >> 1;
                p->side = 1;
            } else {
                p->heading = direction >> 1;
                p->side = 0;
            }
        }
    } else
        --(*timer);
    Player_SetDirectionalVelocity(p, 9);
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.setupResource, 0, p->heading,
                     p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &p->cueCollider.sequence);
}
