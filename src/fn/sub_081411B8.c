#include "global.h"
#include "player.h"
#include "directional_force.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning
#define gForceMagnitude gUnk_02000550
#define gForceThreshold gUnk_0200052C
#define gForceAngle     gUnk_0200053C
#define gSineTable      gUnk_086149C4
s32 Player_GetForceDirection(struct Player *);
static inline u8 Player_IsActiveInSceneFive(struct Player *p)
{
    u16 active = p->active;
    u8 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
static inline u32 Player_TestForceBlock(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline s32 Player_ScaleForce(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void Player_ApplyDirectionalForce(struct Player *p)
{
    s32 x, direction;
    register s32 z asm("r2");
    if (p->tetherMode == 4)
        return;
    if (p && Player_IsActiveInSceneFive(p))
        return;
    asm("" : "+r"(p));
    if (p && Player_TestForceBlock(&p->statBonusFlags, 0x80))
        return;
    if (gForceMagnitude < gForceThreshold)
        return;
    if (gForceDirectionMode == 1)
        direction = Player_GetForceDirection(p);
    else
        direction = ((((gForceAngle + 0x70) & 0xFF) >> 5) + 1) << 5;
    x = gSineTable[(direction + 64) & 255];
    z = gSineTable[direction & 255];
    {
        s32 scaledX;
        scaledX = Player_ScaleForce(gForceMagnitude * x);
        p->velocityX -= scaledX;
    }
    {
        s32 scaledZ = Player_ScaleForce(gForceMagnitude * z);
        p->velocityZ -= scaledZ;
    }
}
