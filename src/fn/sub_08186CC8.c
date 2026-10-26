#include "global.h"
#ifdef NONMATCHING
// fuzzy: 88.6%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: behavior selection aligns the actor with the player and remembers attack origins.
// MATCHING_CONCESSION: register-pinning
// Remaining differences: decision-pointer lifetime, merged fallback calls and register allocation.
struct ArmorSelection {
    u8 pad0[0x48];
    s16 f48;
    s16 f4a;
    s16 f4c;
    u8 pad4e[0xae4];
    u16 fb32;
    u8 padb34[0x5];
    u8 fb39;
    u8 padb3a[0x2];
    u8 fb3c;
    u8 fb3d;
    u8 padb3e[0x5e2];
    u8 f1120;
    u8 pad1121[0x1];
    s16 f1122;
};
extern u8 *gGameState;
void ActorController_SetBehavior(void *, s32), ActorController_SetBehaviorIfFinished(void *, s32);
u32 Armor_IsNearSavedPosition(void *);
void Armor_SaveCurrentPosition(void *);
void Armor_SelectAlignedAttack(struct ArmorSelection *p)
{
    s32 x;
    s32 y;
    s32 z;
    register s32 ax asm("r5");
    s32 az;
    s32 playerY;
    s32 actorY;
    u8 flags;
    if (p->f1122 > 0)
        p->f1122--;
    if (((p->fb3d != 255) && (p->fb3c != 2)) && (p->fb3c != 19))
        return;
    x = (*((s16 *)(gGameState + 0x30))) - p->f48;
    playerY = *((s16 *)(gGameState + 0x32));
    actorY = p->f4a;
    z = (*((s16 *)(gGameState + 0x34))) - p->f4c;
    x = (x + 128) >> 8;
    z = (z + 128) >> 8;
    ax = x;
    y = playerY - actorY;
    az = z;
    if (ax < 0)
        ax = -ax;
    if (y < 0)
        y = -y;
    flags = p->fb32;
    if (z < 0)
        az = -z;
    if (p->fb32 & 1) {
        switch (p->f1120) {
            case 0:
                if ((u8)(p->fb32 & 2)) {
                    ActorController_SetBehavior(p, 12);
                    break;
                }
                if (y > 127)
                    goto far;
                if (x == 0) {
                    if (az > 1)
                        goto far;
                    if (Armor_IsNearSavedPosition(p))
                        goto near;
                    goto displaced;
                } else {
                    if ((z != 0) || (ax > 1))
                        goto far;
                    if (!Armor_IsNearSavedPosition(p))
                        goto displaced;
                }
            near:
                ActorController_SetBehavior(p, 3);

                goto save;
            displaced:
                ActorController_SetBehavior(p, 4);

            save:
                Armor_SaveCurrentPosition(p);

                break;
            far:
                ActorController_SetBehaviorIfFinished(p, 2);

                break;

            case 1:
                if ((p->f1122 <= 0) || (x && z))
                    p->f1120 = 0;
                else {
                    u32 action = p->fb3c;
                    if ((((u8)(action - 10)) > 1) && (action != 13))
                        ActorController_SetBehavior(p, 10);
                }
                break;
        }

    } else if (p->fb3c == 3) {
        if (y <= 127) {
            if (!x) {
                if ((z < 0) && (p->fb39 == 0xc0))
                    goto aligned;
                if (p->fb39 == 0x40)
                    goto aligned;
            } else if (!z) {
                if ((x < 0) && (p->fb39 == 0x80))
                    goto aligned;
                if (!p->fb39)
                    goto aligned;
            }
        }
        return;
        ActorController_SetBehaviorIfFinished(p, 2);
    aligned:
        ActorController_SetBehavior(p, 22);

    } else if ((y <= 127) && (((!x) && (az <= 1)) || ((!z) && (ax <= 1))))
        ActorController_SetBehavior(p, 3);
    else
        ActorController_SetBehaviorIfFinished(p, 2);
}

#else
INCLUDE_ASM("asm/nonmatching", Armor_SelectAlignedAttack);
#endif
