
#ifdef NONMATCHING
// fuzzy: 93.1%
// foundry draft (worker best 92.5%); what differs is mostly register allocation and instruction order
#define PlayerCue_ApplyColliderScale sub_0815DEEC
#include "global.h"
#include "player.h"
#include "game_state.h"
s32 Div(s32, s32);
u32 SunClock_GetPhase(void);
void Collider_SetContactMode(void *, u32);
static inline u32 ReadMaskedFlags(u32 mask, u32 *flags)
{
    return *flags & mask;
}
static inline u32 PlayerCue_TestFlags(struct Player *p, u32 mask)
{
    if (p == 0) return 0;
    return ReadMaskedFlags(mask, &p->actionFlags);
}
void sub_0815DEEC(struct Player *p, void *collider)
{
    s32 scale = 64;
    if (PlayerCue_TestFlags(p, 0x80000))
        scale -= Div(p->derivedStats[1], 5);
    if (PlayerCue_TestFlags(p, 0x40000))
        scale -= 6;
    if (PlayerCue_TestFlags(p, 0x1000000))
        scale += Div(13 * gUnk_02000710->unk876, 10);
    if (PlayerCue_TestFlags(p, 0x400000))
        scale -= Div(13 * gUnk_02000710->unk876, 10);
    switch (SunClock_GetPhase()) {
    case 1: case 2: case 3:
        if (PlayerCue_TestFlags(p, 0x100000))
            scale -= 13;
        if (PlayerCue_TestFlags(p, 0x2000000))
            scale += 13;
        break;
    case 0: case 4: case 5:
        if (PlayerCue_TestFlags(p, 0x200000))
            scale -= 13;
        if (PlayerCue_TestFlags(p, 0x4000000))
            scale += 13;
        break;
    }
    if (scale < 0) scale = 1;
    Collider_SetContactMode(collider, scale);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0815DEEC);
#endif
