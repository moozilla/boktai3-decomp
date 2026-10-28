#include "global.h"
#include "player.h"
extern u8 *gGameState;
struct EquipEntry {
    u32 w[4];
};
struct EquipEntry *Inventory_GetEquipmentEntry(s32);
s32 Inventory_IsEquipmentAssigned(s32);
void Player_SetWeaponInstance(void *, void *);
void Inventory_SwapEquipmentEntries(s32 a, s32 b)
{
    struct EquipEntry *ea;
    struct EquipEntry *eb;
    struct EquipEntry tmp;
    s32 list[8];
    s32 i;
    s32 n;
    s32 pos;
    ea = Inventory_GetEquipmentEntry(a);
    eb = Inventory_GetEquipmentEntry(b);
    tmp = *ea;
    *ea = *eb;
    *eb = tmp;
    for (i = 0; i <= 7; i++) {
        u8 *g = gGameState;
        s16 *q = (s16 *)(i * 2 + (u32)g + 0x68);
        if (*q == a) {
            *q = b;
            Player_SetWeaponInstance((*(void **)gPlayers), eb);
        } else if (*q == b) {
            *q = a;
            Player_SetWeaponInstance((*(void **)gPlayers), ea);
        }
    }
    n = 0;
    for (i = 0; i <= 15; i++) {
        if (Inventory_IsEquipmentAssigned(i)) {
            list[n] = i;
            n++;
        }
    }
    pos = -1;
    if (*(s16 *)(gGameState + 0x50) <= 7) {
        for (i = 0; i < n; i++) {
            if (*(s16 *)(gGameState + 0x68 + *(s16 *)(gGameState + 0x50) * 2) == list[i]) {
                pos = i;
                break;
            }
        }
    }
    for (; n <= 7; n++)
        list[n] = -1;
    for (i = 0; i <= 7; i++)
        *(s16 *)(gGameState + i * 2 + 0x68) = list[i];
    if ((u32)pos <= 7)
        *(u16 *)(gGameState + 0x50) = pos;
}
