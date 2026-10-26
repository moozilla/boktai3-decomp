#include "global.h"
#include "ram.h"
#define ScreenMote_Allocate      SpriteParticle_AllocateSlot
#define ScreenMote_CreateManager SpriteParticle_GetOrCreateManager
#define gRandomCursor            gEffectRandomTableIndex
#define gRandomValues            gUnk_086141BC
/* Tentative screen-space particle family; coordinates are packed signed halves. */
union ScreenMotePosition {
    struct {
        s16 x, y, z, pad;
    } axis;
    u32 words[2];
};
struct ScreenMoteSprite {
    u32 flags;
    u8 unknown4[0xb], mode, unknown10[8];
    union ScreenMotePosition position;
    u8 tail[8];
};
struct ScreenMoteState {
    union ScreenMotePosition position;
    u8 unknown8[8];
    union ScreenMotePosition *target;
    u16 options;
    u8 phase;
    s8 spriteIndex;
    u16 age, unknown1A, remaining;
    u8 delay, entering;
};
struct ScreenMoteManager {
    u8 unknown0[0x18];
    void *resource;
    struct ScreenMoteSprite sprites[1];
};
extern const u16 gUnk_086141BC[];
struct ScreenMoteManager *SpriteParticle_GetOrCreateManager(void);
struct ScreenMoteState *SpriteParticle_AllocateSlot(struct ScreenMoteManager *);
s32 Mod(s32, s32);
void ParticleSprite_Initialize(struct ScreenMoteSprite *, void *, u32);
void Effect_SetOrigin(struct ScreenMoteSprite *, s32, s32);
void Effect_SetPalette(struct ScreenMoteSprite *, u32);
void SpriteDrawable_SelectTileFrame(struct ScreenMoteSprite *, void *, u32);
static inline u32 NextRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
s32 ScreenMote_SpawnBatch(union ScreenMotePosition *origin, union ScreenMotePosition *target,
                          s16 *range, s32 count, u32 mode, u32 unused, u32 flags, u32 options)
{
    struct ScreenMoteManager *m;
    struct ScreenMoteState *s;
    struct ScreenMoteSprite *sprite;
    s32 i;
    if (!origin)
        return -1;
    m = gScreenMoteManager;
    if (!m) {
        m = SpriteParticle_GetOrCreateManager();
        if (!m)
            return -1;
    }
    for (i = 0; i < count; i++) {
        s = SpriteParticle_AllocateSlot(m);
        if (!s)
            return -1;
        sprite = &m->sprites[s->spriteIndex];
        s->position = *origin;
        s->target = target;
        s->entering = 1;
        if (range) {
            if (range[0])
                s->position.axis.x += Mod(NextRandom(), range[0]);
            if (range[1])
                s->position.axis.y += Mod(NextRandom(), range[1] * 2) - range[1];
        }
        ParticleSprite_Initialize(sprite, m->resource, 0);
        Effect_SetOrigin(sprite, -4, -4);
        SpriteDrawable_SelectTileFrame(sprite, m->resource, 2);
        sprite->flags |= 0x11 | flags;
        sprite->mode = mode;
        Effect_SetPalette(sprite, 1);
        s->phase = 1;
        s->options = options;
    }
    return 0;
}
