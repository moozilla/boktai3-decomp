#include "global.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
// Tentative panel/save fields for selecting two available entries in each group.
struct P {
    u8 pad[0x64];
    u32 count, leftCount, rightCount, leftMask, rightMask, extraMask;
};
struct Save {
    u8 pad[0x50];
    s16 leftSelected, rightSelected, extraSelected;
    u8 pad2[0x12];
    s16 leftRows[16];
};
struct ItemInfo {
    u8 enabled;
    u8 pad[31];
};
extern struct Save *gGameState;
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
int Mod(int, int);
void Inventory_GetEquippedDescriptor(int, struct ItemInfo *);
int GameState_GetUnlockEntry(int);
void GameState_MoveSlotEntry(int, int);
void SolarFruit_ClearEquippedSlot(int);
void WirelessGameMenu_InitItemChoices(struct P *input)
{
    struct ItemInfo info;
    int missing, usable;
    register struct P *p asm("r10") = input;
    int start;
    int i;
    u32 mask;
    int offset;
    register int moduloMask asm("r9");
    int count = p->count;
    int other;
    register u32 *seed asm("r5");
    if (!count || !p->rightCount)
        return;
    i = 0;
    start = count;
    do {
        *(s16 *)((u8 *)gGameState + i * 2 + 0x68) = -1;
        i++;
    } while (i <= 7);
    start = Mod(gSceneEffectPalettePhase, start);
    missing = 0;
    usable = 0;
    mask = p->leftMask;
    i = 0;
    {
        register int t asm("r1") = 15;
        asm("" : "+r"(t));
        moduloMask = t;
    }
    {
        register int t asm("r2") = 0;
        asm("" : "+r"(t));
        offset = t;
    }
    do {
        int n = start;
        register struct Save **slot asm("r12");
        int limit;
        int index;
        {
            struct Save **t = &gGameState;
            slot = t;
        }
        limit = start + 16;
        if (start < limit) {
            int item = start;
            int mod = moduloMask;
            u32 bit;
            item &= mod;
            bit = 1 << item;
            if (({
                    u32 available = mask;
                    available & bit;
                })) {
                {
                    register struct Save **t asm("r2") = slot;
                    *(s16 *)((u8 *)*t + offset + 0x68) = item;
                }
                mask &= ~bit;
                goto selected16;
            }
            while (1) {
                n++;
                if (n >= limit)
                    break;
                item = n;
                mod = moduloMask;
                item &= mod;
                bit = 1 << item;
                if (({
                        u32 available = mask;
                        available & bit;
                    })) {
                    {
                        register struct Save **t asm("r4") = slot;
                        *(s16 *)((u8 *)*t + offset + 0x68) = item;
                    }
                    mask &= ~bit;
                    start = n;
                    break;
                }
            }
        }
    selected16:
        if (*(s16 *)((u8 *)gGameState + offset + 0x68) < 0) {
            int t = 1;
            missing = t;
        } else {
            Inventory_GetEquippedDescriptor(i, &info);
            if (!info.enabled) {
                int t = 1;
                usable = t;
            }
        }
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        {
            int off = index * 2;
            register int m asm("r2");
            off = *(u16 *)((u8 *)gUnk_0203B400 + off);
            m = moduloMask;
            asm("" : "+r"(m));
            start += off & m;
        }
        {
            int two = 2;
            asm("" : "+r"(two));
            offset += two;
        }
        i++;
    } while (i <= 1);
    {
        register struct P *t asm("r1") = p;
        other = t->rightCount;
    }
    {
        register struct Save **fallback asm("r12");
        {
            register struct Save **t asm("r2") = &gGameState;
            asm("" : "+r"(t));
            fallback = t;
        }
        seed = &gSceneEffectPalettePhase;
        if (({
                register int t asm("r0") = missing;
                asm("" : "+r"(t));
                t;
            }) &&
            !({
                int t = usable;
                t;
            })) {
            i = 0;
            {
                register struct Save **t asm("r2") = fallback;
                struct Save *save;
                s16 *row;
                save = *t;
                row = (s16 *)((u8 *)save + 0x68);
                if (*row < 0)
                    *row = i;
                else {
                    while (1) {
                        s16 *row;
                        i++;
                        if (i > 1)
                            break;
                        {
                            register struct Save **t asm("r1") = fallback;
                            struct Save *save;
                            save = *t;
                            row = (s16 *)(i * 2 + (u32)save + 0x68);
                        }
                        if (*row < 0) {
                            {
                                u16 zero = 0;
                                asm("" : "+r"(zero));
                                *row = zero;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    i = 0;
    do {
        *(s16 *)((u8 *)gGameState + i * 2 + 0x78) = -1;
        i++;
    } while (i <= 7);
    start = Mod(*seed, other);
    missing = 0;
    usable = 0;
    mask = p->extraMask;
    i = 0;
    {
        register int t asm("r1") = 7;
        asm("" : "+r"(t));
        moduloMask = t;
    }
    {
        register int t asm("r2") = 0;
        asm("" : "+r"(t));
        offset = t;
    }
    do {
        int n = start;
        register struct Save **slot asm("r12");
        register int limit asm("r6");
        int index;
        {
            struct Save **t = &gGameState;
            slot = t;
        }
        limit = start + 8;
        asm("" : "+r"(limit), "+r"(i));
        if (start < limit) {
            int item = start;
            int mod = moduloMask;
            u32 bit;
            item &= mod;
            bit = 1 << item;
            if (({
                    u32 available = mask;
                    available & bit;
                })) {
                {
                    register struct Save **t asm("r2") = slot;
                    *(s16 *)((u8 *)*t + offset + 0x78) = item;
                }
                mask &= ~bit;
                goto selected8;
            }
            while (1) {
                n++;
                if (n >= limit)
                    break;
                item = n;
                mod = moduloMask;
                item &= mod;
                bit = 1 << item;
                if (({
                        u32 available = mask;
                        asm("" : "+r"(available)::"r10");
                        available & bit;
                    })) {
                    {
                        register struct Save **t asm("r4") = slot;
                        *(s16 *)((u8 *)*t + offset + 0x78) = item;
                    }
                    mask &= ~bit;
                    start = n;
                    break;
                }
            }
        }
    selected8: {
        struct Save **t = &gGameState;
        s16 *row;
        row = (s16 *)((u8 *)*t + offset + 0x78);
        if (*row < 0)
            missing = 1;
        else if (GameState_GetUnlockEntry(*row) == 1) {
            int t = 1;
            usable = t;
        }
    }
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        {
            int off = index * 2;
            register int m asm("r2");
            off = *(u16 *)((u8 *)gUnk_0203B400 + off);
            m = moduloMask;
            asm("" : "+r"(m));
            start += off & m;
        }
        {
            int two = 2;
            asm("" : "+r"(two));
            offset += two;
        }
        i++;
    } while (i <= 1);
    {
        register struct Save **fallback asm("r12");
        {
            struct Save **t = &gGameState;
            fallback = t;
        }
        if (({
                int t = missing;
                t;
            }) &&
            !({
                register int t asm("r4") = usable;
                asm("" : "+r"(t));
                t;
            })) {
            i = 0;
            {
                register struct Save **t asm("r1") = fallback;
                struct Save *save;
                s16 *row;
                save = *t;
                row = (s16 *)((u8 *)save + 0x78);
                if (*row < 0)
                    *row = i;
                else {
                    while (1) {
                        s16 *row;
                        i++;
                        if (i > 1)
                            break;
                        {
                            register struct Save **t asm("r4") = fallback;
                            struct Save *save;
                            save = *t;
                            row = (s16 *)(i * 2 + (u32)save + 0x78);
                        }
                        if (*row < 0) {
                            {
                                u16 zero = 0;
                                asm("" : "+r"(zero));
                                *row = zero;
                            }
                            break;
                        }
                    }
                }
            }
        }
        {
            register struct Save **t asm("r4") = fallback;
            (*t)->leftSelected = 0;
        }
    }
    asm("" ::: "r1");
    GameState_MoveSlotEntry(0, 0);
    i = 1;
    do {
        SolarFruit_ClearEquippedSlot(i);
        i++;
    } while (i <= 7);
    gGameState->extraSelected = 0;
}
