#include "global.h"
// fix.py best (sweep after the inlined-insn fix)
struct CloudPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
union CloudPackedPosition {
    struct CloudPosition position;
    struct {
        s32 x : 16, y : 16, z : 16, pad : 16;
    } packed;
};
struct CloudSprite {
    u8 pad0[0x18];
    struct CloudPosition position;
    u8 pad20[8];
};
struct CloudParticle {
    u8 kind, frame, timer, duration, angle, speed;
    u16 delay, lifetime, padA;
    s16 motionX, motionY, motionZ;
    u16 pad12;
    struct CloudPosition origin;
    struct CloudPosition *anchor;
    struct CloudSprite sprite;
};
struct CloudPlayer {
    u8 pad0[0x30];
    struct CloudPosition position;
};
struct CloudTask {
    u8 pad0[0x18];
    u32 ticks;
    u16 mode;
    u8 duration, stop;
    u32 frameBase;
    struct CloudPosition anchor;
    void *sheet;
    struct CloudParticle particles[40];
    struct CloudPlayer *player;
};
extern struct CloudTask *gUnk_02000298;
extern struct CloudPlayer *gUnk_02000580[];
extern u32 gRandomIndex;
extern const s16 gUnk_086149C4[];
void *AnimData_FindEntry(u32);
s32 Mod(s32, s32);
void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void Effect_SetPalette(void *, s32);
static inline u32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void SetPosition(struct CloudSprite *sprite, s32 angle, s32 radius)
{
    sprite->position.x = Truncate(gUnk_086149C4[(angle + 64) & 255] * radius);
    sprite->position.y = 0;
    sprite->position.z = Truncate(gUnk_086149C4[angle] * radius);
}
// confidence=low: initializes forty particles around the player, with one of two sheets.
#define PlayerParticleCloud_Initialize sub_081D33A0
s32 PlayerParticleCloud_Initialize(struct CloudTask *p, const union CloudPackedPosition *input,
                                   s32 mode)
{
    s32 i;
    union CloudPackedPosition position;
    struct CloudParticle *particle;
    u32 sheet;
    u32 frameBase;
    struct CloudPlayer *player;
    gUnk_02000298 = p;
    player = gUnk_02000580[0];
    p->player = player;
    position = *input;
    position.packed.y += 32;
    if (!player)
        return -1;
    switch (mode) {
        case 0:
            sheet = 0x1C1C;
            frameBase = 6;
            break;

        case 1:
            sheet = 0x1C1E;
            frameBase = 14;
            break;

        default:
            return -1;
    }

    p->frameBase = frameBase;
    p->mode = 3;
    p->stop = 0;
    p->anchor = p->player->position;
    p->anchor.y += 128;
    p->ticks = 0;
    p->sheet = AnimData_FindEntry(sheet);
    p->duration = 4;
    particle = p->particles;
    for (i = 0; i <= 39; i++, particle++) {
        const u16 *table;
        s32 angle;
        s32 radius;
        s32 speed;
        struct CloudSprite *sprite;
        void *sheetData;
        particle->kind = 1;
        particle->frame = 0;
        particle->timer = i & 3;
        table = (const u16 *)0x0203B400;
        particle->duration = (RandomValue(table) & 3) + 4;
        particle->angle = RandomValue(table);
        speed = RandomValue(table) | -128;
        particle->speed = speed;
        particle->delay = Mod(RandomValue(table), 100) + 600;
        particle->lifetime = 1200;
        sheetData = p->sheet;
        sprite = &particle->sprite;
        ParticleSprite_Initialize(sprite, sheetData, 0);
        SetPosition(sprite, particle->angle, RandomValue(table) & 31);
        Effect_SetOrigin(sprite, -4, -4);
        SpriteDrawable_SelectTileFrame(sprite, p->sheet, p->frameBase + particle->frame);
        if (mode == 1)
            Effect_SetPalette(sprite, 1);
        sprite->position.x += position.packed.x;
        sprite->position.y += position.packed.y;
        sprite->position.z += position.packed.z;
        particle->motionX = 0;
        particle->motionY = 0;
        particle->motionZ = 0;
        particle->origin = sprite->position;
        particle->anchor = &p->anchor;
        table = (const u16 *)0x0203B400;
    }

    return 0;
}
