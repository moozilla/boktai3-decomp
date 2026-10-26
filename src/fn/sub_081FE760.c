#include "global.h"
#include "ram.h"
struct OrbConfig {
    u8 pad0[6];
    s8 angle;
    u8 pad7[0xF];
    u16 f16;
    s32 radius;
    void (*update)(void);
};
struct OrbSprite {
    u32 flags;
    u8 pad4[3];
    u8 f7;
    u8 pad8[4];
    void *resource;
    u16 frame;
    u8 pad12[0x1A];
};
struct Vec {
    s16 x, y, z;
};
struct Flags6 {
    u8 pad[6];
    u16 flags;
};
struct Pos {
    u32 a, b;
};
struct Orb {
    u32 f0;
    u8 pad4[8];
    struct Vec offset;
    u8 config[4];
    s16 mode;
    u8 config1A[0x1A];
    struct OrbSprite a;
    struct OrbSprite b;
    u8 pad8C[0xA0 - 0x8C];
};
struct OrbManager {
    u8 pad0[0x18];
    u32 mask;
};
extern const s16 gUnk_086149C4[];
void SunOrb_GetOrCreateManager(void);
u8 *SunOrb_FindFreeSlot(struct OrbManager *, s32 *);
void SunOrb_SetUpdateCallback(struct OrbConfig *, void (*)(void));
void OrbitEffect_UpdateExpansion(void);
void Memory_Copy(void *, void *, u32);
void Sprite_LinkToList(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void OrbitEffect_InitTrailSprites(void *);
void Sound_PlaySong(u32);
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
#define F(T, off) (*(T *)((u8 *)o + (off)))
// confidence=low: claims a free sun orb slot, copies the 0x20-byte config and starts it at pos.
s32 SunOrb_CreateAtPosition(struct OrbConfig *cfg, struct Pos *pos)
{
    struct OrbManager *m = gSlotController;
    s32 slot;
    struct Orb *o;
    struct OrbSprite *sa, *sb;
    s32 zero;
    if (!m) {
        SunOrb_GetOrCreateManager();
        return -1;
    }
    o = (struct Orb *)SunOrb_FindFreeSlot(m, &slot);
    if (!o)
        return -1;
    SunOrb_SetUpdateCallback(cfg, OrbitEffect_UpdateExpansion);
    Memory_Copy(o->config, cfg, 0x20);
    o->f0 = 0;
    {
        struct Vec *v = &o->offset;
        s32 angle = cfg->angle;
        s32 radius = cfg->radius;
        v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
        v->y = 0;
        v->z = scale(gUnk_086149C4[angle & 255] * radius);
    }
    zero = 0;
    F(struct Pos, 0x50) = *pos;
    F(struct Pos, 0x7C) = *pos;
    o->a.flags = ((o->a.flags | 2) & ~1) | 0x200;
    sa = &o->a;
    Sprite_LinkToList(sa, 0);
    o->b.flags = ((o->b.flags | 2) & ~1) | 0x400;
    sb = &o->b;
    Sprite_LinkToList(sb, 0);
    sa->frame = zero;
    sb->frame = 1;
    if (o->mode == 1) {
        AnimData_SelectPaletteBank(o->a.resource, 0x4F);
        AnimData_SelectPaletteBank(o->b.resource, 0x51);
    } else {
        AnimData_SelectPaletteBank(o->a.resource, 0x51);
        AnimData_SelectPaletteBank(o->b.resource, 0x51);
    }
    o->a.f7 = 2;
    o->b.f7 = 2;
    OrbitEffect_InitTrailSprites(o);
    F(u32, 0x3A4) = 0;
    F(u32, 0x3A8) = 0x2000;
    m->mask |= 1 << slot;
    {
        struct Flags6 *c = (struct Flags6 *)((u8 *)o + 0xA0);
        s32 mask = ~4;
        c->flags = mask & c->flags;
    }
    (*(u32 *)((u8 *)m + 0xEEC))++;
    (*(u32 *)((u8 *)m + 0xEF0))++;
    F(u16, 0xF6) = 5;
    {
        u16 v = cfg->f16;
        F(u16, 0xF8) = v;
    }
    F(u16, 0xFA) = 0;
    Sound_PlaySong(0x571);
    return slot;
}
