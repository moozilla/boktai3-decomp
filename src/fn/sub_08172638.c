#include "global.h"

/* tentative: redraws the solar-fruit and unlock icons, showing equipped entries */
#define PauseMenu_RefreshFruitAndUnlockIcons sub_08172638

struct Slot {
    u8 v : 8;
} __attribute__((packed));
struct Widget {
    u8 pad0[8];
    u32 flags;
    u8 pad1[0x14];
    s16 x;
    s16 y;
    u8 pad2[0x3C];
};
struct Menu {
    u8 pad0[0xA4];
    u8 resource[0x17E0 - 0xA4];
    struct Widget widgets[20];
    u8 pad1[0x4D80 - 0x17E0 - 20 * 0x60];
    struct Slot fruitSlots[8];
    struct Slot unlockSlots[8];
};
s32 SolarFruit_GetInventorySlot(s32);
s32 SolarFruit_IsInventorySlotEquipped(s32);
s32 GameState_GetUnlockEntry(s32);
s32 GameState_HasUnlockedInventoryEntry(s32);
s32 Anim_LoadKeyframe(void *, void *, u16, u8);
void PauseMenu_SetSpritePalette(void *, u16);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void PauseMenu_AddSpriteFlags(void *, s32, u32);
void PauseMenu_SetSpritePosition(void *, s32, s32, s32);

void PauseMenu_RefreshFruitAndUnlockIcons(struct Menu *p)
{
    s32 n;
    s32 i;
    struct Widget *w;
    s32 slot;

    {
        s32 j;
        for (j = 0; j < 8; j++) {
            p->fruitSlots[j].v |= 0xff;
            p->unlockSlots[j].v |= 0xff;
        }
    }
    i = n = 0;
    w = &p->widgets[0];
    for (; i <= 7; w++, i++) {
        slot = SolarFruit_GetInventorySlot(i);
        if (slot < 0) {
            w->flags |= 1;
            continue;
        }
        w->flags &= ~1;
        Anim_LoadKeyframe(w, p->resource, slot + 0xe7, 0);
        PauseMenu_SetSpritePalette(w, 0x373);
        if (SolarFruit_IsInventorySlotEquipped(i)) {
            PauseMenu_ClearSpriteFlags(p, n + 0x1e, 1);
            PauseMenu_SetSpritePosition(p, n + 0x1e, w->x, w->y);
            p->fruitSlots[n].v = i;
            n++;
        }
    }
    for (; n <= 7; n++)
        PauseMenu_AddSpriteFlags(p, n + 0x1e, 1);
    i = n = 0;
    w = &p->widgets[8];
    for (; i <= 11; w++, i++) {
        slot = GameState_GetUnlockEntry(i);
        if (slot == 0) {
            w->flags |= 1;
            continue;
        }
        w->flags &= ~1;
        Anim_LoadKeyframe(w, p->resource, slot + 0xd9, 0);
        PauseMenu_SetSpritePalette(w, 0x373);
        if (GameState_HasUnlockedInventoryEntry(i)) {
            PauseMenu_ClearSpriteFlags(p, n + 0x26, 1);
            PauseMenu_SetSpritePosition(p, n + 0x26, w->x, w->y);
            p->unlockSlots[n].v = i;
            n++;
        }
    }
    for (; n <= 7; n++)
        PauseMenu_AddSpriteFlags(p, n + 0x26, 1);
}
