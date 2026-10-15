#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_LimitPlayerDistance(struct TetherActor *);
#define TetherActor_UpdateReverseDirectedMovement sub_0804D120
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void TetherActor_UpdateReverseDirectedMovement(struct TetherActor *actor)
{
    if (actor->directedTimer == 30) {
        s32 dx = actor->position.x - actor->directedTarget.x;
        s32 dz = actor->position.z - actor->directedTarget.z;
        if (dx || dz) {
            u32 heading = ((((Math_Atan2(dx, dz) + 16) & 255) >> 5) + 3) & 7;
            actor->directedHeading = heading;
            actor->heading = (heading + 4) & 7;
        }
    } else if (actor->directedTimer <= 29) {
        u32 angle;
        register const s16 *table asm("r2");
        {
            u32 value = (actor->directedHeading + 5) & 7;
            angle = value << 5;
        }
        table = gUnk_086149C4;
        actor->position.x += scale(table[(angle + 64) & 255] * 6);
        actor->position.z += scale(table[angle] * 6);
        actor->heading = ((u8)actor->directedHeading + 4) & 7;
        TetherActor_LimitPlayerDistance(actor);
        actor->moving = 1;
    }
    if (--actor->directedTimer == 0) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 sample = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
            sample &= 63;
            actor->directedTimer = sample + 45;
        }
    }
}
