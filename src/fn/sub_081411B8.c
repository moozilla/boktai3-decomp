#include "global.h"
#include "player.h"
#include "directional_force.h"
#include "ram.h"

#define gSineTable gUnk_086149C4
s32 Player_GetForceDirection(struct Player *);
static inline u32 Player_IsActiveInSceneFive(struct Player *p)
{
    if (!p)
        return 0;
    if (p->active && p->sceneMode == 5)
        return 1;
    return 0;
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
    s32 z;
    if (p->tetherMode == 4)
        return;
    if (Player_IsActiveInSceneFive(p))
        return;
    if (p && Player_TestForceBlock(&p->statBonusFlags, 0x80))
        return;
    if (gPlayerBackgroundState < gPlayerDirectionLatch)
        return;
    if (gForceDirectionMode == 1)
        direction = Player_GetForceDirection(p);
    else
        direction = ((((gPlayerBackgroundMode + 0x70) & 0xFF) >> 5) + 1) << 5;
    x = gSineTable[(direction + 64) & 255];
    z = gSineTable[direction & 255];
    {
        s32 scaledX;
        scaledX = Player_ScaleForce(x * gPlayerBackgroundState);
        p->velocityX -= scaledX;
    }
    {
        s32 scaledZ = Player_ScaleForce(z * gPlayerBackgroundState);
        p->velocityZ -= scaledZ;
    }
}
