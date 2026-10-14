#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
#define TetherActor_UpdateWanderingState sub_0804C104
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_SetHeadingEntry(struct TetherActor *, u32);
void TetherActor_UpdateShake(struct TetherActor *);
void TetherActor_SelectInteractionState(struct TetherActor *);
u32 TetherActor_TestStepDelta(void *, void *, u32);
s32 Mod(s32, s32);
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void TetherActor_UpdateWanderingState(struct TetherActor *actor)
{
    if (actor->state != 4)
        TetherActor_SetState(actor, 4);
    if (gUnk_030053F4 & 512)
        return;
    if (actor->chargeReady) {
        actor->glowMode = 0;
        actor->roamFrame = 0;
        TetherActor_UpdateShake(actor);
        return;
    }
    actor->glowMode = 1;
    if (actor->roamFrame == 90) {
        if (actor->turnAfterBlock) {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            actor->heading = (actor->heading + Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], 5) + 2) & 7;
            actor->turnAfterBlock = 0;
        } else {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            actor->heading = (actor->heading + Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], 3) + 7) & 7;
        }
    } else if (actor->roamFrame > 90) {
        u32 angle;
        const s16 *table;
        struct ContactPackedVector delta;
        struct ContactPosition *position;
        {
            u32 value = (actor->heading + 5) & 7;
            angle = value << 5;
        }
        table = gUnk_086149C4;
        delta.x = scale(table[(angle + 64) & 255] * 3);
        delta.y = 0;
        delta.z = scale(table[angle] * 3);
        asm("" : : "r"(angle));
        position = &actor->position;
        if (TetherActor_TestStepDelta(position, &delta, 0x843)) {
            position->x += delta.x;
            actor->position.z += delta.z;
            actor->moving = 1;
        } else {
            actor->roamFrame = 30;
            actor->turnAfterBlock = 1;
        }
    }
    if (actor->kind == 5 && actor->moving)
        TetherActor_SetHeadingEntry(actor, 36);
    else
        TetherActor_SetEntry(actor, 0);
    {
        u16 *frame = &actor->roamFrame;
        if (++*frame > 119) {
            TetherActor_SelectInteractionState(actor);
            *frame = 0;
        }
    }
}
