#include "engine/shared/global.h"
#include "engine/shared/ram.h"
#include "engine/shared/engine_globals.h"

// Tentative panel/save fields used to choose four distinct available entries.
struct WirelessEquipmentMenu {
    u8 pad[0x1E];
    u16 round;
    u8 pad2[0x54];
    u32 rightMask;
};
struct WirelessEquipmentSave {
    u8 pad[0x4C];
    u32 selection;
    union {
        struct {
            s16 left, right;
        } sides;
        u32 word;
    } selected;
    u32 field54, field58;
    u8 pad2[0x2C];
    s16 rightRows[16];
};
extern struct WirelessEquipmentSave *gGameState;
void GameState_AddEquipment(int);
int Mod(int, int);

void WirelessGameMenu_InitEquipmentChoices(struct WirelessEquipmentMenu *p)
{
    u32 mask = 0;
    int start, i, n, item;
    u32 index;
    gGameState->selection = mask;
    GameState_AddEquipment(0);
    mask = 1;
    if ((p->rightMask & 2) && p->round > 3) {
        GameState_AddEquipment(1);
        mask = 3;
    }
    if ((p->rightMask & 4) && p->round > 3) {
        GameState_AddEquipment(2);
        mask |= 4;
    }
    if (p->rightMask & 64) {
        GameState_AddEquipment(6);
        mask |= 64;
    }
    if (p->rightMask & 128) {
        GameState_AddEquipment(7);
        mask |= 128;
    }
    if (p->rightMask & 256) {
        GameState_AddEquipment(8);
        mask |= 256;
    }
    if (p->rightMask & 512) {
        GameState_AddEquipment(9);
        mask |= 512;
    }
    if (p->rightMask & 1024) {
        GameState_AddEquipment(10);
        mask |= 1024;
    }
    if (p->rightMask & 2048) {
        GameState_AddEquipment(11);
        mask |= 2048;
    }
    start = Mod(gSceneEffectPalettePhase, 12);
    {
        u32 raw = (u16)gGameState->selected.sides.right;
        if ((s32)raw < 0) {
            u32 v0 = gGameState->selection;
            u32 v1 = gGameState->selected.word;
            u32 v2 = gGameState->field54;
            u32 v3 = gGameState->field58;
            Mod(0, 12);
            if (v0) {
                u32 sum = v0 + v1 + v2 + v3;
                u32 other = v0 - v1;
                struct WirelessEquipmentSave *save = gGameState;
                save->selection = sum;
                save->selected.word = other;
            }
        }
    }
    for (i = 0; i <= 3; i++) {
        *(gGameState->rightRows + i) = -1;
        for (n = start; n < start + 12; n++) {
            item = n;
            if (item > 11)
                item -= 12;
            if (mask & (1 << item)) {
                *(gGameState->rightRows + i) = item;
                mask &= ~(1 << item);
                start = item;
                break;
            }
        }
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        start += Mod(((u16 *)RANDOM_TABLE_ADDR)[index], 12);
        if (start > 11)
            start -= 12;
    }
    gGameState->selected.sides.right = 0;
}
