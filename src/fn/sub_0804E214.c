#include "area_effect.h"
#define AreaEffect_InitializeEntry sub_0804E214
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectTiles(void *, u32);
u32 sub_0822C034(void *);
void AreaEffect_InitializeEntry(struct AreaEffectEntry *entry, s32 kind,
                                struct AreaEffectPosition *position, s32 rotated, u32 graphicsId)
{
    u32 mirror;
    u8 *kindField;
    u8 *graphics;
    struct Sprite {
        u8 unknown0[0x10];
        u16 priority;
    };
    struct Sprite *sprite;
    entry->position = *position;
    mirror = rotated ? 4 : 0;
    kindField = &entry->kind;
    *kindField = kind;
    graphics = entry->graphics[0];
    AnimData_Load(graphics, 0xCBE1);
    sprite = (struct Sprite *)&entry->spriteFlags;
    Sprite_Init(sprite, graphics, mirror);
    sprite->priority = *kindField;
    AnimData_SelectTiles(graphics, graphicsId);
    entry->displayPosition = entry->position;
    entry->renderMode = 3;
    entry->hasSprite = 1;
    sub_0804E180(entry, kind, position, rotated);
    entry->cachedValue = sub_0822C034(entry);
}
