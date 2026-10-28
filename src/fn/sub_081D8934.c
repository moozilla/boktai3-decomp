// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "input.h"
void *ResourceRegistry_GetResource(u16);
void Task4E01_ClearTrailSprites(void *);
void Task4E01_SpawnTrailSprite(void *, void *);
void Projectile_SetCollisionSize(void *, s32, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
void Task4E01_UpdateTrailSprites(void *, void *);
void Task4E01_UpdateTimedSprite(void);

struct ImpactSprite {
    u32 flags;
    u8 pad4[0xC];
    u16 frame;
};
struct ImpactConfig {
    u8 pad0[6];
    s8 direction;
    u8 pad7[9];
    void *callback;
    u32 f14, f18, f1c, f20, f24, f28, f2c;
    u8 pad30[0xA];
    s16 f3a;
};
struct Pos8 {
    u32 a, b;
};
struct Impact {
    u32 tick;
    u32 flags;
    u8 pad8[0x10];
    struct ImpactConfig config;
};

s32 Task4E01_UpdateTrailImpact(u8 *p)
{
    struct ImpactConfig *config = (struct ImpactConfig *)(p + 0x18);
    void *group;
    u32 mask;
    group = ResourceRegistry_GetResource(0x4E01);
    if (!group)
        return -1;
    if ((*(u32 *)p & 0x1f) == 0x1f) {
        s16 *timer = (s16 *)(p + 0x1a4);
        *timer += 1;
        if (*timer > 3) {
            *timer = 3;
            Task4E01_ClearTrailSprites(group);
        }
    }
    {
        struct ImpactSprite *sprite = (struct ImpactSprite *)(p + 0x8c);
        u32 t;
        s32 v;
        sprite->flags &= ~1;
        t = *(u32 *)p & 7;
        if (t == 0) {
            v = *(s16 *)(p + 0x1a4);
            sprite->frame = v + 0x19;
        } else if (t == 3) {
            if (*(s16 *)(p + 0x1a4) == 0) {
                sprite->flags |= 1;
            } else {
                v = *(s16 *)(p + 0x1a4);
                sprite->frame = v + 0x18;
            }
        }
    }
    if ((*(u32 *)p & 1) && *(s16 *)(p + 0x1a4) <= 2)
        Task4E01_SpawnTrailSprite(group, p);
    {
        u32 clr = ~2;
        u32 f = *(u32 *)(p + 4);
        *(u32 *)(p + 4) = f & clr;
    }
    mask = (*(u16 *)gPadState) & 2;
    if (mask == 0) {
        s16 *timer = (s16 *)(p + 0x1a4);
        u8 *collider;
        {
            register s32 w asm("r3") = *timer * 4 + 8;
            Projectile_SetCollisionSize(config, w, 0x50);
        }
        collider = p + 0xb8;
        *(struct Pos8 *)(collider + 0x1c) = *(struct Pos8 *)&config->f28;
        {
            s32 d = config->direction;
            p[0xfa] = d;
        }
        {
            u32 v = ((u16)config->direction + 0x70) >> 5 & 7;
            *(u16 *)(p + 0x17c) = v;
        }
        if (*timer > 0)
            config->f1c = *timer * config->f1c;
        Collider_SetAttackProperties(collider, config->f1c, config->f20, config->f14, config->f18,
                                     config->f24);
        config->callback = Task4E01_UpdateTimedSprite;
        *(u32 *)p = mask;
        Task4E01_ClearTrailSprites(group);
        if (config->f3a == 0)
            Sound_PlaySong(0xCD);
    }
    Task4E01_UpdateTrailSprites(group, p);
    return 1;
}
