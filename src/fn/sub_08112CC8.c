#include "global.h"
struct PanelSpriteInstance {
    u8 data[0x1C];
    u32 flags;
    u8 pad20[3];
    u8 mode;
    u8 pad24[0x14];
    u8 clear[8];
    u8 pad40[8];
    u8 anim[0x10];
    void *resource;
};
struct PanelSpriteOwner {
    struct PanelSpriteInstance *sprite;
    u8 pad4[0x17];
    u8 state;
};
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
s32 PanelSprite_Create(struct PanelSpriteOwner *owner, u32 data, u32 key, u32 seq, u32 row,
                       u32 flags, u32 spriteFlags, u32 mode)
{
    struct PanelSpriteInstance *p;
    owner->sprite = Heap_Alloc(0x5C);
    if (!owner->sprite)
        return -1;
    Memory_Clear(owner->sprite, 0x5C);
    owner->state = 0;
    p = owner->sprite;
    if (!AnimData_Load(p, data))
        return -1;
    Sprite_Init(&p->flags, p, 0);
    p->resource = ScriptProc_Start(0x922E, (u16)key);
    Anim_SetSequence(p->anim, p->resource, (u16)seq, (u8)row, (u8)flags);
    Memory_Clear(p->clear, 8);
    p->flags = spriteFlags | 0x11;
    p->mode = mode;
    return 1;
}
