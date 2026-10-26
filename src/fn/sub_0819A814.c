#include "global.h"
#include "engine.h"
struct LinkVector {
    s16 x, y, z;
};
struct LinkEffect {
    u8 pad0[0x1C];
    u8 *manager;
    struct LinkVector *target;
    struct LinkVector start, end;
    s8 orb, otherOrb;
    s16 collision;
    u16 timer;
    u8 unknown3A, state, emitted;
};
s32 Mod(s32, s32);
void Sound_PlaySongChecked(u32);
u8 *SunOrb_GetPositionFromManager(u8 *, s32);
void RisingActorManager_BeginRaise(s32);
void SlotController_RequestReset(s32);
void SlotController_SetSlotFlag2(s32);
void Projectile_InitParameters(u8 *);
void Projectile_SpawnAtPosition(u8 *, struct LinkVector *);
void Projectile_SetBehaviorMode(u8 *, u16);
void Projectile_SetCollisionParameters(u8 *, u32, u32, u32, u32, u32);
void Projectile_SetHomingParameters(u8 *, u16, u32, u8);
void Projectile_SetLaunchParameters(u8 *, u16, u16, u8, u32);
void Projectile_SetScriptProcedure(u8 *, u32, u16);
void Projectile_SetSpriteParameters(u8 *, u32, u32, u32);
void LinkEffect_UpdateProjectileBurst(struct LinkEffect *p)
{
    u8 projectile[0x58];
    struct LinkVector position;
    switch (p->state) {
        case 0:
            p->state = 1;
            p->emitted = 0;
            RisingActorManager_BeginRaise(p->orb);
            Sound_PlaySongChecked(0x55B);
            break;
        case 1:
            if (p->emitted <= 5 && Mod(p->timer, 15) == 0) {
                u32 random;
                Projectile_InitParameters(projectile);
                gRandomIndex = (gRandomIndex + 1) & 1023;
                random = *(u8 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2);
                Projectile_SetLaunchParameters(projectile, 0, 180, random, 8);
                Projectile_SetHomingParameters(
                    projectile, 120, (u32)SunOrb_GetPositionFromManager(p->manager, p->orb), 4);
                Projectile_SetSpriteParameters(projectile, 0x5291, 0, -1);
                Projectile_SetScriptProcedure(projectile, 0x3F72, 2);
                Projectile_SetCollisionParameters(projectile, 0, 0, p->collision, 15, 30);
                Projectile_SetBehaviorMode(projectile, 2);
                {
                    struct LinkVector *v = &position;
                    *v = *p->target;
                    v->y += 128;
                    Projectile_SpawnAtPosition(projectile, v);
                }
                p->emitted++;
            }
            if (Mod(p->timer, 300) == 180)
                SlotController_RequestReset(p->orb);
            if (p->timer <= 179)
                SlotController_SetSlotFlag2(p->orb);
            if (p->timer > 240)
                p->state = 255;
            break;
    }
}
