// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u8 *gUnk_02000710;
extern void *gUnk_02000580;
s32 Inventory_GetEquipmentId(s32);
void Inventory_SetEquipmentEntry(s32, const void *);
s32 Inventory_AddAssignedSlot(u32);
void Hud_RefreshInventoryDisplay(void);
void *Inventory_GetEquipmentEntry(s32);
void Player_BeginDescriptorAction(void *, void *);
s32 Inventory_AddEquipmentFromRecord(const void *record)
{
    s32 count, i;
    s16 *q;
    void *p;
    register u8 **gp asm("r6");
    s32 idx;
    register s32 m1 asm("r3");
    for (i = 0; i <= 15; i++) {
        if (Inventory_GetEquipmentId(i) == 0xFF)
            break;
    }
    if (i > 15)
        return -1;
    Inventory_SetEquipmentEntry(i, record);
    idx = i;
    count = 0;
    {
        register u8 **t asm("r0") = &gUnk_02000710;
        q = (s16 *)*t;
        m1 = -1;
        asm("" : "+r"(m1));
        gp = t;
    }
    q = (s16 *)((u8 *)q + 0x68);
    i = 7;
    do {
        s32 z = 0;
        if (*(s16 *)((u8 *)q + z) > m1)
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
