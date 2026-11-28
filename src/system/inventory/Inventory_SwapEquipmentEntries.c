#include "system/game_state/game_state.h"
#include "player/player.h"
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
    for (i = 0; i < 8; i++) {
        if (*(i + gGameState->inventory) == a) {
            *(i + gGameState->inventory) = b;
            Player_SetWeaponInstance(gPlayers[0], eb);
        } else if (*(i + gGameState->inventory) == b) {
            *(i + gGameState->inventory) = a;
            Player_SetWeaponInstance(gPlayers[0], ea);
        }
    }
    n = 0;
    for (i = 0; i < 16; i++) {
        if (Inventory_IsEquipmentAssigned(i)) {
            list[n] = i;
            n++;
        }
    }
    pos = -1;
    if ((s16)gGameState->unk50 < 8) {
        for (i = 0; i < n; i++) {
            if (*((s16)gGameState->unk50 + gGameState->inventory) == list[i]) {
                pos = i;
                break;
            }
        }
    }
    for (; n < 8; n++)
        list[n] = -1;
    for (i = 0; i < 8; i++)
        *(i + gGameState->inventory) = list[i];
    if ((u32)pos <= 7)
        gGameState->unk50 = pos;
}
