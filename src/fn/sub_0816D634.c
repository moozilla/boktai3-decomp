#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
// confidence=low: applies the selected storage item effects and clears its menu slot.
#define PauseMenu_ApplyItemSlot sub_0816D634
struct ItemDef {
    u16 unknown0;
    u16 actionKind;
    u16 unknown4;
    u16 valueA;
    u16 valueB;
    u8 unknownA[6];
};
extern const struct ItemDef gUnk_08E61280[];
extern void *const gUnk_08611BF0[];
s32 Inventory_GetQuantityByStorage(u32, u32);
void sub_0813D834(struct Player *, s32, u32);
void sub_08249244(void *, s32, s32, void *);
void Inventory_ClearItemOverrideFlag(u32);
void PauseMenu_ClearItemSlot(struct SceneHudState *, u32);

static inline u32 Item_HasBonus(u32 *flags, u32 mask)
{
    return *flags & mask;
}

void PauseMenu_ApplyItemSlot(struct SceneHudState *p, u32 slot)
{
    s32 item = Inventory_GetQuantityByStorage(p->actionCategory, slot);
    s32 a, b;
    u8 mode = p->actionOwner->actionMode;

    if (mode == 1 || mode == 3 || mode == 4 || mode == 5) {
        if (item == 1 || item == 13 || item == 22 || item == 19 || item == 20 || item == 18 ||
            item == 25 || item == 2 || item == 23 || item == 24) {
            s16 *applied;
            s32 duration = 0x708;
            if (p->actionOwner && Item_HasBonus(&p->actionOwner->statBonusFlags, 0x20000))
                duration <<= 1;
            sub_0813D834(p->actionOwner, 0, duration);
            applied = &p->appliedStatB;
            *applied = 0;
            goto clear;
        }
        if (item == 14) {
            s32 duration = 0x384;
            if (p->actionOwner && Item_HasBonus(&p->actionOwner->statBonusFlags, 0x20000))
                duration <<= 1;
            sub_0813D834(p->actionOwner, 1, duration);
            p->appliedStatB = 0;
            goto clear;
        }
    }
    a = gUnk_08E61280[item].valueA;
    b = gUnk_08E61280[item].valueB;
    if (p->actionOwner->actionMode == 6 && item == 13)
        a = -a;
    if (p->actionOwner && Item_HasBonus(&p->actionOwner->statBonusFlags, 0x20000)) {
        if (item == 1 || item == 13 || item == 2 || item == 14) {
            a <<= 1;
            b <<= 1;
        } else if (item == 22 || item == 19 || item == 20 || item == 18 || item == 25 ||
                   item == 23 || item == 24)
            b <<= 1;
    }
    sub_08249244(p, a, b, gUnk_08611BF0[gUnk_08E61280[item].actionKind]);
    if (item == 9) {
        Inventory_ClearItemOverrideFlag(slot);
        if (Inventory_GetQuantityByStorage(0, slot) != 9)
            return;
        goto clear;
    }
    if ((u32)(item - 10) <= 1) {
    clear:
        PauseMenu_ClearItemSlot(p, slot);
    } else {
        PauseMenu_ClearItemSlot(p, slot);
    }
}
