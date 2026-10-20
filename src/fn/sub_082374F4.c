#include "global.h"
#include "engine.h"

struct MovingSpriteVec2 {
    u32 a, b;
};
struct MovingSpriteVec {
    s16 x, y, z, pad;
};
struct MovingSprite {
    u8 task[0x18];
    u8 sprite[0x2C];
    u8 anim[0x1C];
    struct MovingSpriteVec2 position;
    u8 pad68[2];
    u16 id;
    u16 angle;
    s16 velocityX, velocityY;
    u16 delay, duration;
    void (*phase)(void);
};
void MovingGlyph_SetState(void *, void (*)(void));
void MovingSprite_UpdateDelay(void);
void MovingSprite_UpdateMotion(void);
static inline s32 scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

s32 MovingSprite_Initialize(struct MovingSprite *p, u32 *position, u32 palette, u32 id, u32 angle,
                            u32 speed, u32 delay, u32 duration)
{
    u8 *sprite = p->sprite;
    u8 *anim = p->anim;
    u32 loaded;
    s32 index;
    u32 a;
    u32 b;
    AnimData_Load(anim, 0x210E);
    Sprite_Init(sprite, anim, 0);
    a = position[0];
    b = position[1];
    p->position.a = a;
    p->position.b = b;
    ((u32 *)sprite)[7] = a;
    ((u32 *)sprite)[8] = b;
    sprite[7] = 1;
    AnimData_SelectPaletteBank(anim, palette);
    {
        u16 *q = &p->id;
        *q = id;
        loaded = *q;
        *(u16 *)(sprite + 0x10) = loaded;
    }
    p->angle = angle;
    index = ((p->angle + 1) & 7) << 5;
    p->velocityX = -scale(gUnk_086149C4[(index + 64) & 255] * speed);
    p->velocityY = -scale(gUnk_086149C4[index << 0] * speed);
    p->delay = delay;
    p->duration = duration;
    if (p->delay) {
        *(u32 *)sprite |= 1;
        MovingGlyph_SetState(p, MovingSprite_UpdateDelay);
    } else {
        MovingGlyph_SetState(p, MovingSprite_UpdateMotion);
    }
    return 0;
}
