#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
void sub_0813D834(void *, s32, s32);
void sub_08249244(void *, s32, s32, void *);
void Inventory_ClearItemOverrideFlag(u32);
void PauseMenu_ClearItemSlot(u8 *, u32);

static inline u32 Tst(u32 *a, u32 m)
{
    return *a & m;
}

#define OWNER (*(u8 **)(p + 0xa40))

void PauseMenu_ApplyItemSlot(u8 *p, u32 slot)
{
    s32 item = Inventory_GetQuantityByStorage(p[0x4860], slot);
    s32 a, b;
    u8 mode = OWNER[0x418];

    if (mode == 1 || mode == 3 || mode == 4 || mode == 5) {
        if (item == 1 || item == 13 || item == 22 || item == 19 || item == 20 || item == 18 ||
            item == 25 || item == 2 || item == 23 || item == 24) {
            s32 dur = 0x708;
            if (OWNER && Tst((u32 *)(OWNER + 0x444), 0x20000))
                dur <<= 1;
            sub_0813D834(OWNER, 0, dur);
            *(u16 *)(p + 0x4866) = 0;
            asm("" : : "i"(0));
            goto clear;
        }
        if (item == 14) {
            s32 dur = 0x384;
            if (OWNER && Tst((u32 *)(OWNER + 0x444), 0x20000))
                dur <<= 1;
            sub_0813D834(OWNER, 1, dur);
            *(u16 *)(p + 0x4866) = 0;
            goto clear;
        }
    }
    a = gUnk_08E61280[item].valueA;
    b = gUnk_08E61280[item].valueB;
    if (OWNER[0x418] == 6 && item == 13)
        a = -a;
    if (OWNER && Tst((u32 *)(OWNER + 0x444), 0x20000)) {
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
