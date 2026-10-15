#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
#define TetherActor_UpdateKindNineRecoil sub_0804CCB0
void TetherActor_SetState(struct TetherActor *, u32);
void sub_0821BC68(void *, void *);
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void TetherActor_UpdateKindNineRecoil(struct TetherActor *actor)
{
    if (actor->state != 11) {
        TetherActor_SetState(actor, 11);
        if (!actor->movementProbe) {
            sub_0821BC68(&actor->movementProbeStorage, &actor->position);
            actor->movementProbe = &actor->movementProbeStorage;
        }
        Sound_PlaySong(0x3F3);
    }
    if (!actor->stateFrame)
        actor->displayOffsetY = 300;
    {
        register u32 angle asm("r3") = actor->actionVariant;
        const s16 *table = gUnk_086149C4;
        actor->movementDelta.x = scale(table[(angle + 64) & 255] * actor->actionValue);
        actor->movementDelta.z = scale(table[angle] * actor->actionValue);
        asm("" : : "r"(angle));
    }
    actor->actionValue = (actor->actionValue * 3) >> 2;
    {
        register u16 *frame asm("r3") = &actor->stateFrame;
        u32 current;
        current = *frame;
        if (current > 3) {
            u16 *height = &actor->displayOffsetY;
            register u32 remaining asm("r2") = *height;
            u32 reduction = current << 3;
            if ((s32)remaining >= (s32)reduction)
                reduction = remaining - reduction;
            else
                reduction = 0;
            *height = reduction;
        }
    }
    ++actor->stateFrame;
    if (actor->actionValue <= 19 && !actor->displayOffsetY) {
        actor->displayOffsetY = 0;
        Sound_PlaySong(0x2A8);
        TetherActor_SetState(actor, 0);
    }
}
