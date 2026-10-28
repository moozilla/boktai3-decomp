// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"
struct OrbitVec {
    s16 x, y, z, pad;
};
struct OrbitW4 {
    u32 w[4];
};
struct OrbitPacked {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct OrbitShort3 {
    s16 x, y, z;
};
struct OrbitEffectState {
    u32 frame;
    u8 pad4[0x18 - 4];
    s16 mode;
    u8 pad1A[0x22 - 0x1a];
    s16 a22, a24, a26, a28;
    u8 pad2A[0x50 - 0x2a];
    struct OrbitVec position;
    u8 pad58[0xf4 - 0x58];
    s16 wide, count, phase;
};
extern const s16 gUnk_086149C4[];
extern const u32 gUnk_0824FE34[4];
extern const u32 gUnk_08613F88[4];
extern const u32 gUnk_08613F98[4];
void OrbitEffect_UpdateWideRadius(void *);
void OrbitEffect_UpdateNarrowRadius(void *);
void OrbitEffect_AdjustHeading(void *);
struct OrbitVec *Player_GetEffectivePosition(s32);
s32 Math_Atan2(s32, s32);
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, u16, u32, u32, u32);
void Projectile_SetSpriteParameters(void *, u32, u32, u32);
void Projectile_SetCollisionParameters(void *, s32, u32, s32, s32, s32);
void Projectile_SetBehaviorMode(void *, u16);
void Projectile_SpawnAtPosition(void *, void *);
void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);

u32 OrbitEffect_FireVolley(struct OrbitEffectState *p)
{
    if (p->wide == 0)
        OrbitEffect_UpdateWideRadius(p);
    else
        OrbitEffect_UpdateNarrowRadius(p);
    OrbitEffect_AdjustHeading(p);
    if (p->a28 == 0) {
        if (p->frame > 0x1d && p->count > 0 && p->mode == 2) {
            if (p->phase == 0) {
                u8 params[0x58];
                struct OrbitW4 sprites;
                register u32 dir asm("r4");
                u32 r;
                struct OrbitVec *target;
                struct OrbitVec *pos;
                struct OrbitVec spawn;
                struct OrbitVec *sv;
                sprites = *(const struct OrbitW4 *)gUnk_0824FE34;
                gRandomIndex = (gRandomIndex + 1) & 0x3FF;
                {
                    u32 tb = (u32)((const u16 *)RANDOM_TABLE_ADDR);
                    r = *(const u16 *)((gRandomIndex << 1) + tb);
                }
                r &= 3;
                target = Player_GetEffectivePosition(0);
                pos = &p->position;
                dir = Math_Atan2(target->x - pos->x, target->z - pos->z);
                asm("" : "+r"(dir));
                dir = (u8)dir;
                dir += 0xc0;
                dir = (u8)(dir + (p->count - 1) * 32);
                Projectile_InitParameters(params);
                Projectile_SetLaunchParameters(params, 0, 0xb4, dir, 0x12);
                Projectile_SetSpriteParameters(params, 0x5291, sprites.w[r], -1);
                Projectile_SetCollisionParameters(params, 0, 0, p->a22, p->a24, p->a26);
                Projectile_SetBehaviorMode(params, 2);
                *(u16 *)(params + 0x52) = 1;
                sv = &spawn;
                *sv = *pos;
                sv->y += 0x80;
                Projectile_SpawnAtPosition(params, &spawn);
                if (p->count == 5)
                    Sound_PlaySong(0x574);
            } else {
                u32 r;
                struct OrbitVec *target;
                s32 dir;
                struct OrbitVec vel;
                struct OrbitPacked pos;
                s32 k;
                struct OrbitVec *vp;
                struct OrbitVec *pp;
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
                pp = &p->position;
                dir = Math_Atan2(target->x - pp->x, target->z - pp->z);
                dir += p->count * 32 - sixty;
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
                pos = *(struct OrbitPacked *)&p->position;
                pos.y += 0x40;
                vel.y = 0x36;
                sub_08127590(&pos, &vel, 0xB07C, 0x99CB, gUnk_08613F88[r], 0x1e, 2, 0,
                             gUnk_08613F98[r] + 0x26b, 0, 0, p->a22, p->a24, 0, 0);
                if (p->count == 5)
                    Sound_PlaySong(0x575);
            }
            p->count--;
        }
        if (p->frame == 0x5a && p->phase == 1)
            p->count = 5;
    }
    return 0;
}
