
#ifdef NONMATCHING
// fuzzy: 95.7%
// foundry draft (worker best 95.7%); what differs is mostly register allocation and instruction order
#define Player_ApplyDirectionalForce sub_081411B8
#define Player_GetForceDirection     sub_08140F40
#define gForceMagnitude              gUnk_02000550
#define gForceThreshold              gUnk_0200052C
#define gForceAngle                  gUnk_0200053C
#define gForceDirectionMode          gUnk_02000558
#define gSineTable                   gUnk_086149C4
#include "global.h"
#include "player.h"
#include "directional_force.h"
extern u16 gUnk_02000558;
s32 sub_08140F40(struct Player *);
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
void sub_081411B8(struct Player *p)
{
    s32 x, z, direction;
    if (p->tetherMode == 4)
        return;
    if (p && Player_IsActiveInSceneFive(p))
        return;
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
        s32 scaledX = Player_ScaleForce(gForceMagnitude * x);
        p->velocityX -= scaledX;
    }
    {
        s32 scaledZ = Player_ScaleForce(gForceMagnitude * z);
        p->velocityZ -= scaledZ;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081411B8);
#endif
