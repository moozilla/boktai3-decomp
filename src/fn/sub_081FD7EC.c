#include "global.h"
#include "engine.h"
#include "ram.h"
#include "sun_orb_effect.h"

struct OrbitW4 {
    u32 w[4];
};
struct OrbitPacked {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct OrbitShort3 {
    s16 x, y, z;
};
extern const s16 gUnk_086149C4[];
extern const u32 gUnk_0824FE34[4];
extern const u32 gUnk_08613F88[4];
extern const u32 gUnk_08613F98[4];
void OrbitEffect_UpdateWideRadius(void *);
void OrbitEffect_UpdateNarrowRadius(void *);
void OrbitEffect_AdjustHeading(void *);
struct OrbitEffectPosition *Player_GetEffectivePosition(s32);
s32 Math_Atan2(s32, s32);
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, u16, u32, u32, u32);
void Projectile_SetSpriteParameters(void *, u32, u32, u32);
void Projectile_SetCollisionParameters(void *, s32, u32, s32, s32, s32);
void Projectile_SetBehaviorMode(void *, u16);
void Projectile_SpawnAtPosition(void *, void *);
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);

s32 OrbitEffect_FireVolley(struct OrbitEffect *p, s32 slot)
{
    if (p->volley.lowered == 0)
        OrbitEffect_UpdateWideRadius(p);
    else
        OrbitEffect_UpdateNarrowRadius(p);
    OrbitEffect_AdjustHeading(p);
    if (p->params.homing == 0) {
        if (p->frame > 0x1d && p->volley.count > 0 && p->params.kind == 2) {
            if (p->volley.phase == 0) {
                u8 params[0x58];
                struct OrbitW4 sprites;
                s32 targetHeading;
                u32 baseDirection;
                u8 launchAngle;
                u32 r;
                struct OrbitEffectPosition *target;
                struct OrbitEffectPosition *pos;
                struct OrbitEffectPosition spawn;
                struct OrbitEffectPosition *sv;
                sprites = *(const struct OrbitW4 *)gUnk_0824FE34;
                gRandomIndex = (gRandomIndex + 1) & 0x3FF;
                {
                    u32 tb = (u32)((const u16 *)RANDOM_TABLE_ADDR);
                    r = *(const u16 *)((gRandomIndex << 1) + tb);
                }
                r &= 3;
                target = Player_GetEffectivePosition(0);
                pos = &p->sprite.position;
                targetHeading = Math_Atan2(target->x - pos->x, target->z - pos->z);
                baseDirection = (u8)targetHeading + 0xC0;
                launchAngle = baseDirection + (p->volley.count - 1) * 32;
                Projectile_InitParameters(params);
                Projectile_SetLaunchParameters(params, 0, 0xb4, launchAngle, 0x12);
                Projectile_SetSpriteParameters(params, 0x5291, sprites.w[r], -1);
                Projectile_SetCollisionParameters(params, 0, 0, p->params.collisionX,
                                                  p->params.collisionY, p->params.collisionZ);
                Projectile_SetBehaviorMode(params, 2);
                *(u16 *)(params + 0x52) = 1;
                sv = &spawn;
                *sv = *pos;
                sv->y += 0x80;
                Projectile_SpawnAtPosition(params, &spawn);
                if (p->volley.count == 5)
                    Sound_PlaySong(0x574);
            } else {
                u32 r;
                struct OrbitEffectPosition *target;
                s32 dir;
                struct OrbitEffectPosition vel;
                struct OrbitPacked pos;
                s32 k;
                struct OrbitEffectPosition *vp;
                struct OrbitEffectPosition *pp;
                s32 v, t;
                s16 sixty;
                gRandomIndex = (gRandomIndex + 1) & 0x3FF;
                {
                    u32 tb = (u32)((const u16 *)RANDOM_TABLE_ADDR);
                    r = *(const u16 *)((gRandomIndex << 1) + tb);
                }
                r &= 3;
                target = Player_GetEffectivePosition(0);
                sixty = 0x60;
                pp = &p->sprite.position;
                dir = Math_Atan2(target->x - pp->x, target->z - pp->z);
                dir += p->volley.count * 32 - sixty;
                vp = &vel;
                k = 24;
                v = gUnk_086149C4[(dir + 0x40) & 0xff] * k;
                if (v >= 0)
                    t = v >> 12;
                else {
                    v = -v;
                    t = -(v >> 12);
                }
                vp->x = t;
                vp->y = 0;
                v = gUnk_086149C4[dir & 0xff] * k;
                if (v >= 0)
                    v = v >> 12;
                else {
                    v = -v;
                    v = -(v >> 12);
                }
                vp->z = v;
                pos = *(struct OrbitPacked *)&p->sprite.position;
                pos.y += 0x40;
                vel.y = 0x36;
                sub_08127590(&pos, &vel, 0xB07C, 0x99CB, gUnk_08613F88[r], 0x1e, 2, 0,
                             gUnk_08613F98[r] + 0x26b, 0, 0, p->params.collisionX,
                             p->params.collisionY, 0, 0);
                if (p->volley.count == 5)
                    Sound_PlaySong(0x575);
            }
            p->volley.count--;
        }
        if (p->frame == 0x5a && p->volley.phase == 1)
            p->volley.count = 5;
    }
    return 0;
}
