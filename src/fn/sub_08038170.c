#include "global.h"
#include "ram.h"
#include "engine_globals.h"
// Tentative panel/save fields for selecting two available entries in each group.
struct P {
    u8 pad[0x64];
    u32 count, leftCount, rightCount, leftMask, rightMask, extraMask;
};
struct Save {
    u8 pad[0x50];
    s16 selected[3];
    u8 pad2[0x12];
    s16 leftRows[8], extraRows[8];
};
struct ItemInfo {
    u8 enabled;
    u8 pad[31];
};
extern struct Save *gGameState;
int Mod(int, int);
void Inventory_GetEquippedDescriptor(int, struct ItemInfo *);
int GameState_GetUnlockEntry(int);
void GameState_MoveSlotEntry(int, int);
void SolarFruit_ClearEquippedSlot(int);
static inline u32 Random_Bits(u32 mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return ((u16 *)RANDOM_TABLE_ADDR)[index] & mask;
}
void WirelessGameMenu_InitItemChoices(struct P *p)
{
    struct ItemInfo info;
    int start, i, n, item, range, other;
    int missing, usable;
    u32 mask;
    int count = p->count;

    if (!count || !p->rightCount)
        return;
    i = 0;
    start = count;
    for (; i <= 7; i++)
        *(gGameState->leftRows + i) = -1;
    start = Mod(gSceneEffectPalettePhase, start);
    missing = 0;
    usable = 0;
    mask = p->leftMask;
    i = 0;
    range = 15;
    for (; i <= 1; i++) {
        for (n = start; n < start + 16; n++) {
            item = n & range;
            if (mask & (1 << item)) {
                *(gGameState->leftRows + i) = item;
                mask &= ~(1 << item);
                start = n;
                break;
            }
        }
        if (*(gGameState->leftRows + i) < 0)
            missing = 1;
        else {
            Inventory_GetEquippedDescriptor(i, &info);
            if (!info.enabled)
                usable = 1;
        }
        start += Random_Bits(range);
    }
    other = p->rightCount;
    if (missing && !usable) {
        for (i = 0; i <= 1; i++) {
            if (*(gGameState->leftRows + i) < 0) {
                *(gGameState->leftRows + i) = 0;
                break;
            }
        }
    }
    for (i = 0; i <= 7; i++)
        *(gGameState->extraRows + i) = -1;
    start = Mod(gSceneEffectPalettePhase, other);
    missing = 0;
    usable = 0;
    mask = p->extraMask;
    i = 0;
    range = 7;
    for (; i <= 1; i++) {
        for (n = start; n < start + 8; n++) {
            item = n & range;
            if (mask & (1 << item)) {
                *(gGameState->extraRows + i) = item;
                mask &= ~(1 << item);
                start = n;
                break;
            }
        }
        if (*(gGameState->extraRows + i) < 0)
            missing = 1;
        else if (GameState_GetUnlockEntry(*(gGameState->extraRows + i)) == 1)
            usable = 1;
        start += Random_Bits(range);
    }
    if (missing && !usable) {
        for (i = 0; i <= 1; i++) {
            if (*(gGameState->extraRows + i) < 0) {
                *(gGameState->extraRows + i) = 0;
                break;
            }
        }
    }
    *gGameState->selected = 0;
    GameState_MoveSlotEntry(0, 0);
    for (i = 1; i <= 7; i++)
        SolarFruit_ClearEquippedSlot(i);
    gGameState->selected[2] = 0;
}
