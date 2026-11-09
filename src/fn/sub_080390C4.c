#include "global.h"
#include "game_state.h"
#include "engine_globals.h"
#include "ram.h"
// Tentative wireless selection/menu layout from observed offsets.
struct Slot {
    u8 counts[4];
    u8 a, b, flag, kind;
    u32 timer;
};
struct Menu {
    u8 task[0x18];
    u32 v18;
    u16 v1c, v1e;
    u32 flags, timer;
    u8 state, entered, pending, v2b, v2c, v2d, pad2e[4], enabled, v33, pad34[4], counts[4];
    struct Slot slots[2];
    u8 messages[12];
    void *window60;
    u32 count, leftCount, rightCount, leftMask, rightMask, extraMask;
    u16 left[16], middle[16], right[8];
    void *resource;
    int window;
    void *listener, *secondListener;
    int delta;
    u16 weights[5];
    u8 padea[2], vec;
};
void *Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
void *Script_GetPc(void);
int Script_GetValue(void);
u8 *Equipment_GetDefinition(int);
void Inventory_SetEquipmentEntry(int, void *);
void GameState_SetUnlockSlot(int, int);
void SolarFruit_SetInventorySlot(int, int);
void Inventory_SetConsumableEntry(int, int);
void Inventory_ClearItemSlot(int);
void Inventory_ClearKeyItemSlot(int);
void WirelessExchange_ClearPeerCounts(struct Menu *, int);
void *Script_GetTextSourcePc(void);
int WirelessTradeMenu_InitFromScript(struct Menu *p)
{
    int i, j, r, index;
    u8 *item;

    gWirelessTradeMenu = p;
    p->listener = Script_GetKeywordValue(101, 0);
    p->secondListener = Script_GetKeywordValue(114, 0);
    p->count = 0;
    p->leftMask = 0;
    if (Script_SeekToKeyword(119)) {
        for (i = 0; i <= 15; i++) {
            if (Script_GetPc()) {
                p->left[i] = Script_GetValue();
                p->count++;
            } else
                p->left[i] = 0;
        }
    } else {
        for (i = 0; i <= 15; i++)
            p->left[i] = 0;
    }
    if (p->count) {
        r = (u8)gSceneEffectPalettePhase;
        for (i = 0; i < p->count; i++) {
            item = Equipment_GetDefinition(p->left[i]);
            if (*item == 0)
                index = 0;
            else {
                index = r & 15;
                if (!index)
                    index = 1;
                while ((1 << index) & p->leftMask) {
                    index = r & 15;
                    if (!index)
                        index = 1;
                    r++;
                }
            }
            p->leftMask |= 1 << index;
            Inventory_SetEquipmentEntry(index, item);
            r += (u8)gRandomTable[gRandomIndex = (gRandomIndex + 1) & 1023];
        }
    }
    item = Equipment_GetDefinition(0);
    Inventory_SetEquipmentEntry(0, item);
    p->rightCount = 0;
    p->extraMask = 0;
    if (Script_SeekToKeyword(103)) {
        for (i = 0; i <= 7; i++) {
            if (Script_GetPc()) {
                p->right[i] = Script_GetValue();
                p->rightCount++;
            } else
                p->right[i] = 0;
        }
    } else {
        for (i = 0; i <= 7; i++)
            p->right[i] = 0;
    }
    if (p->rightCount) {
        r = (u8)gSceneEffectPalettePhase;
        for (i = 0; i < p->rightCount; i++) {
            if (p->right[i] == 1)
                index = 0;
            else {
                index = r & 7;
                if (!index)
                    index = 1;
                while ((1 << index) & p->extraMask) {
                    index = r & 7;
                    if (!index)
                        index = 1;
                    r++;
                }
            }
            p->extraMask |= 1 << index;
            GameState_SetUnlockSlot(index, p->right[i]);
            r += (u8)gRandomTable[gRandomIndex = (gRandomIndex + 1) & 1023];
        }
    }
    GameState_SetUnlockSlot(0, 1);
    SolarFruit_SetInventorySlot(0, 0);
    for (i = 1; i <= 7; i++)
        SolarFruit_SetInventorySlot(i, -1);
    for (i = 0; i <= 3; i++)
        *(gGameState->party + i) = -1;
    for (i = 0; i <= 15; i++)
        Inventory_SetConsumableEntry(i, -1);
    for (i = 0; i <= 15; i++)
        Inventory_ClearItemSlot(i);
    for (i = 0; i <= 15; i++)
        Inventory_ClearKeyItemSlot(i);
    p->rightMask = 0xFCF;
    p->v18 = 0;
    p->v1c = 0;
    p->v1e = 0;
    p->flags = 0;
    p->v2b = 0;
    p->v2c = 0;
    p->v2d = 0;
    p->v33 = 0;
    p->vec = 0;
    p->enabled = 1;
    p->state = 0;
    p->entered = 1;
    p->timer = 0;
    for (i = 0; i <= 1; i++) {
        struct Slot *s = &p->slots[i];
        for (j = 0; j <= 3; j++)
            s->counts[i] = 0;
        s->a = 100;
        s->b = 100;
        s->flag = 0;
        s->kind = 0;
        s->timer = 0;
    }
    for (i = 0; i <= 3; i++)
        p->counts[i] = 0;
    WirelessExchange_ClearPeerCounts(p, 0);
    WirelessExchange_ClearPeerCounts(p, 1);
    if (!Script_SeekToKeyword(115))
        return -1;
    p->resource = Script_GetTextSourcePc();
    if (!p->resource)
        return -1;
    p->delta = 0;
    p->weights[0] = (int)Script_GetKeywordValue(77, 11);
    if (Script_SeekToKeyword(112)) {
        u16 *w = &p->weights[1];
        for (i = 0; i <= 3; i++)
            *w++ = Script_GetValue();
        return 0;
    }
    return -1;
}
