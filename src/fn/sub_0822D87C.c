#include "global.h"
extern u8 *gUnk_02000710;
extern void *gUnk_02000580;
s32 Inventory_GetEquipmentId(s32);
void Inventory_SetEquipmentEntry(s32, const void *);
s32 Inventory_AddAssignedSlot(u32);
void Hud_RefreshInventoryDisplay(void);
void *Inventory_GetEquipmentEntry(s32);
void Player_BeginDescriptorAction(void *, void *);
#ifdef NONMATCHING
// fuzzy: 91.9%
#define Inventory_AddEquipmentFromRecord sub_0822D87C
// differs: register/instruction order of the slot-count loop setup (global address kept in r6).
s32 Inventory_AddEquipmentFromRecord(const void *record)
{
    s32 count, i;
    s16 *q;
    void *p;
    u8 **gp;
    s32 idx;
    for (i = 0; i <= 15; i++) {
        if (Inventory_GetEquipmentId(i) == 0xFF)
            break;
    }
    if (i > 15)
        return -1;
    Inventory_SetEquipmentEntry(i, record);
    idx = i;
    count = 0;
    gp = &gUnk_02000710;
    q = (s16 *)(*gp + 0x68);
    i = 7;
    do {
        s32 z = 0;
        if (*(s16 *)((u8 *)q + z) > -1)
            count++;
        q++;
        i--;
    } while (i >= 0);
    if (count <= 0) {
        *(u16 *)(*gp + 0x50) = 0;
        Inventory_AddAssignedSlot(idx);
        Hud_RefreshInventoryDisplay();
        p = gUnk_02000580;
        if (p)
            Player_BeginDescriptorAction(p, Inventory_GetEquipmentEntry(idx));
    }
    return idx;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0822D87C);
#endif
