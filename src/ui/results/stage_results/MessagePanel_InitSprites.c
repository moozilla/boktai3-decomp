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
void AnimSprite_InitLinked(void *, void *, int, int, int, int, int, void *);
void MessagePanel_InitSprites(struct MessagePanel *p)
{
    s32 i;
    p->proc = ScriptProc_Start(0xCB05, 0x1289);
    ResourceHeader_Relocate(p->resources, p->proc);
    for (i = 0; i < 23; i++) {
        AnimSprite_InitLinked(&p->sprites[i], p->resources, p->spriteIds[i], 0x10, 1, 0, 0, 0);
        p->selections[i] = 0xFFFF;
        if (i == 5)
            p->sprites[5].visible = 0;
    }
    p->sprites[7].flags |= 1;
}
