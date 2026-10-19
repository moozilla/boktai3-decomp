#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.2%
// foundry draft (worker best 91.3%); what differs is mostly register allocation and instruction order
#define RecordMenu_InitializeSprites sub_0810EEFC
struct RecordMenuSprite {
    u8 unknown[0x3a];
    u16 flags;
    u8 pad3c[12];
    void *tiles;
    u8 pad4c[0x14];
};
struct RecordMenu {
    u8 unknown[0x24];
    struct RecordMenuSprite sprites[9];
    u8 resource[0x20];
    u8 rowResource[0x20];
    u16 spriteIds[9];
    u16 states[9];
    void *rowProc;
    void *cursorProc;
    u8 pad3f0[4];
    u16 palette[16];
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void sub_0810EEFC(struct RecordMenu *menu)
{
    s32 i;
    struct RecordMenuSprite *sprite;
    *(void **)((u8 *)menu + 0x3e8) = ScriptProc_Start(0xcb05, 0x1289);
    ResourceHeader_Relocate(menu->rowResource);
    i = 0;
    {
        s32 zero = 0;
        sprite = menu->sprites;
        for (; i < 8; i++) {
            sub_0821983C(sprite, menu->rowResource, menu->spriteIds[i], 16, zero, zero, zero,
                         (void *)zero);
            menu->states[i] = -1;
            sprite++;
        }
    }
    {
        s32 zero = 0;
        *(void **)((u8 *)menu + 0x3ec) = ScriptProc_Start(0xcb05, 0x530d);
        ResourceHeader_Relocate(menu->resource);
        sub_0821983C(&menu->sprites[8], menu->resource, menu->spriteIds[i], 17, zero, zero, zero,
                     (void *)zero);
    }
    menu->states[i] |= -1;
    CpuSet(menu->sprites[8].tiles, menu->palette, 0x4000008);
    menu->sprites[8].flags = 0xa000;
    menu->sprites[8].tiles = menu->palette;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810EEFC);
#endif
