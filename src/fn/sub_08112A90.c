#include "global.h"
#include "ram.h"
struct PanelSprite {
    u8 data[0x1C];
    u32 flags;
    u8 pad20[3];
    u8 mode;
    u8 pad24[0x14];
    u8 clear[8];
    u8 pad40[8];
    u8 anim[0x14];
    u32 field5C;
};
struct PanelSpritePool {
    u8 pad0[0x18];
    struct PanelSprite slots[8];
    void *resource;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Memory_Clear(void *, u32);
s32 PanelSpritePool_Init(struct PanelSpritePool *p)
{
    s32 result;
    s32 i = 0;
    u8 *clear = p->slots[0].clear;
    u8 *anim = p->slots[0].anim;
    u8 *sprite = (u8 *)&p->slots[0].flags;
    struct PanelSprite *slot = p->slots;
    do {
        if (!AnimData_Load(slot, 0x409C)) {
            result = -1;
            goto done;
        }
        Sprite_Init(sprite, slot, 0);
        p->resource = ScriptProc_Start(0x922E, 0x1C22);
        Anim_SetSequence(anim, p->resource, 0, 0, 0);
        Memory_Clear(clear, 8);
        p->slots[i].field5C = 0;
        p->slots[i].flags |= 0x211;
        sprite[7] = 3;
        clear += 0x60;
        anim += 0x60;
        sprite += 0x60;
        slot++;
        i++;
    } while (i <= 7);
    gPanelSpritePool = p;
    result = 0;
done:
    return result;
}
