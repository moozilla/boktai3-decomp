#include "global.h"
// confidence=low: refreshes inventory icons and equipped-slot markers.
#define PauseInventory_RefreshConsumableIcons sub_08170D78
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining: loop induction, marker pointers and register allocation.
struct PauseWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y, z, pad;
    u8 unknown28[0x38];
};
extern u8 *gUnk_02000710;
s32 Inventory_GetConsumableId(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Anim_LoadKeyframe(void *, void *, u16, u32);
void PauseMenu_SetSpritePalette(void *, u32);
void PauseMenu_ClearSpriteFlags(void *, u32, u32);
void PauseMenu_AddSpriteFlags(void *, u32, u32);
void PauseMenu_SetSpritePosition(void *, u32, s32, s32);
void PauseInventory_RefreshConsumableIcons(u8 *p)
{
    s32 i;
    u8 markers;
    struct PauseWidget *widget;
    typeof(i) c1;
    for (i = 0; i < 4; i++)
        p[0x4D98 + i] |= 255;

    markers = 0;
    i = 0;
    c1 = i;
    widget = (void *)(p + 0x17E0);
    do {
        s32 id = Inventory_GetConsumableId(c1);
        if (id == (-1))
            widget->flags |= 1;
        else {
            widget->flags &= ~1;
            Anim_LoadKeyframe(widget, p + 0xA4, (u16)(id + 0x121), 0);
            PauseMenu_SetSpritePalette(widget, 0x373);
            if (GameState_IsConsumableSlotEquipped(c1)) {
                s32 marker = markers + 30;
                PauseMenu_ClearSpriteFlags(p, marker, 1);
                PauseMenu_SetSpritePosition(p, marker, widget->x, widget->y);
                p[0x4D98 + markers] = c1;
                markers++;
            }
        }
        widget++;
        c1++;
    } while (c1 <= 15);
    for (; markers <= 15; markers++)
        PauseMenu_AddSpriteFlags(p, markers + 30, 1);

    widget = (void *)(p + 0x1DE0);
    for (i = 0; i < 4; i++) {
        s16 *slot = (void *)((gUnk_02000710 + 0x58) + (i * 2));
        s32 id;
        if (((*slot) < 0) || ((id = Inventory_GetConsumableId(*slot)) < 0)) {
            PauseMenu_AddSpriteFlags(p, i + 26, 1);
        } else {
            PauseMenu_ClearSpriteFlags(p, i + 26, 1);
            PauseMenu_SetSpritePosition(p, i + 26, (i * 24) + 8, 104);
            Anim_LoadKeyframe(widget, p + 0xA4, (u16)(id + 0x121), 0);
            PauseMenu_SetSpritePalette(widget, 0x373);
        }
        widget++;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08170D78);
#endif
