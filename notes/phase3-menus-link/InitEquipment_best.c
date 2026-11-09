#include "global.h"
#include "ram.h"
#include "engine_globals.h"

// Tentative panel/save fields used to choose four distinct available entries.
struct P {
    u8 pad[0x1E];
    u16 round;
    u8 pad2[0x54];
    u32 rightMask;
};
struct Save {
    u8 pad[0x4C];
    u32 selection;
    s16 leftSelected, rightSelected;
    u8 pad2[0x34];
    s16 rightRows[16];
};
extern struct Save *gGameState;
void GameState_AddEquipment(int);
int Mod(int, int);

void WirelessGameMenu_InitEquipmentChoices(struct P *p)
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
        start += Mod(gRandomTable[index], 12);
        if (start > 11)
            start -= 12;
    }
    gGameState->rightSelected = 0;
}
