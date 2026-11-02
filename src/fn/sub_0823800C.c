#include "global.h"
struct PositionedSpritePosition {
    u32 words[2];
};
struct PositionedSpriteData {
    u8 unknown0[0x10];
    u16 frame;
    u8 unknown12[0xA];
};
struct PositionedSprite {
    u8 task[0x18];
    struct PositionedSpriteData sprite;
    struct PositionedSpritePosition position;
    u8 unknown3C[8];
    u8 resource[0x24];
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void PositionedSprite_Init(struct PositionedSprite *p, struct PositionedSpritePosition *position,
                           u32 frame, u32 palette)
{
    void *resource = p->resource;

    struct PositionedSpriteData *sprite;
    AnimData_Load(resource, 0x210e);

    sprite = &p->sprite;
    Sprite_Init(sprite, resource, 0);
    sprite->frame = frame;
    AnimData_SelectPaletteBank(resource, palette + 0x4b);
    p->position = *position;
}
