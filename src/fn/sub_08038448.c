// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"

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
extern u16 gUnk_0203B400[];
void GameState_AddEquipment(int);
int Mod(int, int);

void WirelessGameMenu_InitEquipmentChoices(struct P *p)
{
    u32 mask = 0;
    register int start asm("r4");
    register int i asm("r5");
    u32 *random;
    gGameState->selection = mask;
    GameState_AddEquipment(0);
    {
        u32 t = 1;
        mask = t;
    }
    if ((p->rightMask & 2) && p->round > 3) {
        GameState_AddEquipment(1);
        {
            u32 t = 3;
            mask = t;
        }
    }
    if ((p->rightMask & 4) && p->round > 3) {
        GameState_AddEquipment(2);
        {
            u32 t = mask;
            t |= 4;
            mask = t;
        }
    }
    if (p->rightMask & 64) {
        GameState_AddEquipment(6);
        {
            u32 t = mask;
            t |= 64;
            mask = t;
        }
    }
    if (p->rightMask & 128) {
        GameState_AddEquipment(7);
        {
            u32 t = mask;
            t |= 128;
            mask = t;
        }
    }
    if (p->rightMask & 256) {
        GameState_AddEquipment(8);
        {
            u32 t = mask;
            t |= 256;
            mask = t;
        }
    }
    if (p->rightMask & 512) {
        GameState_AddEquipment(9);
        {
            u32 t = mask;
            t |= 512;
            mask = t;
        }
    }
    if (p->rightMask & 1024) {
        GameState_AddEquipment(10);
        {
            u32 t = mask;
            t |= 1024;
            mask = t;
        }
    }
    if (p->rightMask & 2048) {
        GameState_AddEquipment(11);
        {
            u32 t = mask;
            t |= 2048;
            mask = t;
        }
    }
    start = Mod(gSceneEffectPalettePhase, 12);
    i = 0;
    {
        register u32 *t asm("r0") = &gRandomIndex;
        asm volatile("" : "+r"(t));
        random = t;
    }
    do {
        int n;
        int limit;
        int offset;
        int index;
        int byteOffset;
        {
            struct Save **slot = &gGameState;
            struct Save *save;
            s16 *row;
            save = *slot;
            byteOffset = i * 2;
            row = (s16 *)(byteOffset + (u32)save + 0x88);
            *row = -1;
        }
        n = start;
        offset = byteOffset;
        limit = start + 12;
        i++;
        goto condition;
    next:
        n++;
    condition:
        if (n < limit) {
            register int item asm("r1") = n;
            register u32 bit asm("r2");
            asm volatile("" : "+r"(n), "+r"(limit), "+r"(offset)::"r10", "r12");
            if (item > 11)
                item -= 12;
            bit = 1 << item;
            if (({
                    u32 t = mask;
                    t & bit;
                }) != 0) {
                {
                    struct Save **slot = &gGameState;
                    struct Save *save;
                    save = *slot;
                    *(s16 *)(offset + (u32)save + 0x88) = item;
                }
                {
                    u32 t = mask;
                    t &= ~bit;
                    mask = t;
                }
                start = item;
                goto selected;
            }
            goto next;
        }
    selected: {
        register u32 *read asm("r1") = random;
        register u32 *write asm("r3");
        asm("" : "+r"(read));
        index = (*read + 1) & 1023;
        write = random;
        asm("" : "+r"(write));
        *write = index;
    }
        start += Mod(gUnk_0203B400[index], 12);
        if (start > 11)
            start -= 12;
    } while (i <= 3);
    gGameState->rightSelected = 0;
}
