#include "global.h"

struct MessagePanelSprite {
    u8 pad0[8];
    u32 flags;
    u8 padC[0xE];
    u8 visible;
    u8 pad1B[0x45];
};
struct MessagePanel {
    u8 pad0[0x24];
    struct MessagePanelSprite sprites[23];
    u8 resources[0x20];
    u16 spriteIds[23];
    u16 selections[23];
    void *proc;
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void MessagePanel_InitSprites(struct MessagePanel *p)
{
    s32 i;
    s32 zero;
    u8 *sprite;
    p->proc = ScriptProc_Start(0xCB05, 0x1289);
    ResourceHeader_Relocate(p->resources, p->proc);
    i = 0;
    zero = 0;
    sprite = (u8 *)p->sprites;
    for (; i < 23; i++) {
        sub_0821983C(sprite, p->resources, p->spriteIds[i], 0x10, 1, zero, zero, (void *)zero);
        p->selections[i] = 0xFFFF;
        if (i == 5)
            p->sprites[5].visible = zero;
        sprite += 0x60;
    }
    p->sprites[7].flags |= 1;
}
