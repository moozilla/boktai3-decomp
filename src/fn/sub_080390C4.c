// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
#include "engine_globals.h"
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
struct Save {
    u8 pad[0x58];
    s16 values[4];
};
extern struct Menu *gWirelessTradeMenu;
extern u8 gSceneEffectPalettePhase, gUnk_0203B400[];
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
int WirelessTradeMenu_InitFromScript(struct Menu *arg)
{
    register struct Menu *p asm("r6") = arg;
    u16 *leftBase;
    u16 *rightBase;
    struct Locals {
        u8 *vec, *state, *entered, *v2b, *v2c, *v2d, *enabled, *v33, *counts;
    } local;
    int i;
    u8 *item;
    u16 *cursor;
    gWirelessTradeMenu = p;
    p->listener = Script_GetKeywordValue(101, 0);
    p->secondListener = Script_GetKeywordValue(114, 0);
    p->count = 0;
    p->leftMask = 0;
    if (Script_SeekToKeyword(119)) {
        leftBase = p->left;
        {
            register u32 offset asm("r1") = 0xBC;
            asm("" : "+r"(offset));
            rightBase = (u16 *)(offset + (u32)p);
        }
        {
            register u8 *a asm("r3") = &p->v2b;
            u8 *b;
            u8 *c;
            local.v2b = a;
            b = &p->v2c;
            local.v2c = b;
            c = &p->v2d;
            local.v2d = c;
            a += 8;
            local.v33 = a;
            b += 0xc0;
            local.vec = b;
            c += 5;
            local.enabled = c;
            a -= 11;
            local.state = a;
            b -= 0xc3;
            local.entered = b;
            c += 6;
            local.counts = c;
        }
        cursor = leftBase;
        do {
            if (Script_GetPc()) {
                *cursor = Script_GetValue();
                p->count++;
            } else
                *cursor = 0;
            cursor++;
            asm("" : "+r"(leftBase));
        } while ((s32)cursor <= (s32)(leftBase + 15));
    } else {
        {
            register u32 offset asm("r3") = 0x7C;
            asm("" : "+r"(offset));
            leftBase = (u16 *)(offset + (u32)p);
        }
        {
            u32 offset = 0xBC;
            rightBase = (u16 *)(offset + (u32)p);
        }
        {
            u8 *a = &p->v2b;
            register u8 *b asm("r3");
            u8 *c;
            local.v2b = a;
            b = &p->v2c;
            local.v2c = b;
            c = &p->v2d;
            local.v2d = c;
            a += 8;
            local.v33 = a;
            b += 0xc0;
            local.vec = b;
            c += 5;
            local.enabled = c;
            a -= 11;
            local.state = a;
            b -= 0xc3;
            local.entered = b;
            c += 6;
            local.counts = c;
            {
                u16 *end = leftBase;
                int zero = 0;
                register u16 *cursor asm("r0");
                cursor = (u16 *)(c + 0x62);
                do {
                    *cursor = zero;
                    cursor--;
                } while ((s32)cursor >= (s32)end);
            }
        }
    }
    {
        register u32 count asm("r1") = p->count;
        if (count) {
            int r;
            register int j asm("r5");
            r = gSceneEffectPalettePhase;
            j = 0;
            if ((u32)j < count)
                do {
                    int index;
                    register u32 mask asm("r1");
                    item = Equipment_GetDefinition(leftBase[j]);
                    if (!*item) {
                        index = 0;
                        mask = p->leftMask;
                    } else {
                        index = 15;
                        index &= r;
                        if (!index)
                            index = 1;
                        {
                            u32 bit = 1 << index;
                            mask = p->leftMask;
                            if (bit & mask) {
                                register int small asm("r8");
                                int one;
                                u32 used;
                                {
                                    register int x asm("r3") = 15;
                                    asm("" : "+r"(x));
                                    small = x;
                                }
                                {
                                    int x = 1;
                                    one = x;
                                }
                                used = mask;
                                do {
                                    index = r;
                                    index &= small;
                                    if (!index)
                                        index = 1;
                                    r++;
                                } while ((one << index) & used);
                            }
                        }
                    }
                    p->leftMask = (1 << index) | mask;
                    Inventory_SetEquipmentEntry(index, item);
                    {
                        register u32 *read asm("r1") = &gRandomIndex;
                        register u32 *write asm("r3");
                        u32 x;
                        u8 *table;
                        x = (*read + 1) & 1023;
                        write = &gRandomIndex;
                        *write = x;
                        x <<= 1;
                        table = gUnk_0203B400;
                        r += *(u8 *)(x + (u32)table);
                    }
                    j++;
                } while ((u32)j < p->count);
        }
    }
    item = Equipment_GetDefinition(0);
    Inventory_SetEquipmentEntry(0, item);
    p->rightCount = 0;
    p->extraMask = 0;
    if (Script_SeekToKeyword(103)) {
        cursor = rightBase;
        do {
            if (Script_GetPc()) {
                *cursor = Script_GetValue();
                p->rightCount++;
            } else
                *cursor = 0;
            cursor++;
            asm("" : "+r"(rightBase));
        } while ((s32)cursor <= (s32)(rightBase + 7));
    } else {
        u16 *end = rightBase;
        int zero = 0;
        u16 *cursor;
        cursor = rightBase + 7;
        do {
            *cursor = zero;
            cursor--;
        } while ((s32)cursor >= (s32)end);
    }
    {
        register u32 count asm("r1") = p->rightCount;
        if (count) {
            register int r asm("r4");
            register int j asm("r5");
            u16 *q;
            u32 *random;
            r = gSceneEffectPalettePhase;
            j = 0;
            if ((u32)j < count) {
                q = rightBase;
                {
                    register u32 *x asm("r3") = &gRandomIndex;
                    asm("" : "+r"(x));
                    random = x;
                }
                do {
                    int index;
                    u32 mask;
                    if (*q == 1) {
                        index = 0;
                        mask = p->extraMask;
                    } else {
                        index = 7;
                        index &= r;
                        if (!index)
                            index = 1;
                        {
                            u32 bit = 1 << index;
                            mask = p->extraMask;
                            if (bit & mask) {
                                int small;
                                register int one asm("r8");
                                u32 used;
                                {
                                    int x = 7;
                                    small = x;
                                }
                                {
                                    register int x asm("r3") = 1;
                                    asm("" : "+r"(x));
                                    one = x;
                                }
                                used = mask;
                                do {
                                    index = r;
                                    asm("" : "+r"(index));
                                    index &= small;
                                    if (!index)
                                        index = 1;
                                    r++;
                                } while ((one << index) & used);
                            }
                        }
                    }
                    p->extraMask = (1 << index) | mask;
                    asm("" : "+r"(q));
                    GameState_SetUnlockSlot(index, *q);
                    {
                        register u32 *read asm("r1") = random;
                        register u32 *write asm("r3");
                        u32 x;
                        u8 *table;
                        asm("" : "+r"(read));
                        x = (*read + 1) & 1023;
                        write = random;
                        asm("" : "+r"(write));
                        *write = x;
                        x <<= 1;
                        table = gUnk_0203B400;
                        r += *(u8 *)(x + (u32)table);
                    }
                    q++;
                    j++;
                } while ((u32)j < p->rightCount);
            }
        }
    }
    GameState_SetUnlockSlot(0, 1);
    SolarFruit_SetInventorySlot(0, 0);
    for (i = 1; i <= 7; i++)
        SolarFruit_SetInventorySlot(i, -1);
    {
        struct Save **slot;
        register int minus asm("r0");
        register int value asm("r2");
        i = 0;
        slot = &gGameState;
        minus = -1;
        asm("" : "+r"(slot), "+r"(minus));
        value = minus;
        do {
            register struct Save *save asm("r1") = *slot;
            int offset = i << 1;
            asm("" : "+r"(offset));
            *(s16 *)(offset + (u32)save + 0x58) = value;
            i++;
        } while (i <= 3);
    }
    for (i = 0; i <= 15; i++)
        Inventory_SetConsumableEntry(i, -1);
    for (i = 0; i <= 15; i++)
        Inventory_ClearItemSlot(i);
    for (i = 0; i <= 15; i++)
        Inventory_ClearKeyItemSlot(i);
    p->rightMask = 0xFCF;
    {
        register int zero asm("r2") = 0;
        int byteZero;
        int one;
        p->v18 = zero;
        byteZero = 0;
        {
            register u16 half asm("r2") = 0;
            asm("" : "+r"(half));
            p->v1c = half;
            p->v1e = half;
        }
        p->flags = zero;
        {
            register u8 *q asm("r1") = local.v2b;
            *q = byteZero;
        }
        {
            register u8 *q asm("r3") = local.v2c;
            *q = byteZero;
        }
        {
            register u8 *q asm("r1") = local.v2d;
            *q = byteZero;
        }
        {
            register u8 *q asm("r3") = local.v33;
            *q = byteZero;
        }
        {
            register u8 *q asm("r1") = local.vec;
            *q = byteZero;
        }
        one = 1;
        {
            register u8 *q asm("r3") = local.enabled;
            *q = one;
        }
        {
            register u8 *q asm("r3") = local.state;
            *q = byteZero;
        }
        {
            u8 *q = local.entered;
            *q = one;
        }
        p->timer = zero;
    }
    asm("" : "+r"(p));
    {
        struct Tail {
            u8 a, b, flag, kind;
            u32 timer;
        };
        register int j asm("r5") = 0;
        int zero = 0;
        int hundred = 100;
        struct Tail *tail = (struct Tail *)&p->slots[0].a;
        u8 *bytes = p->slots[0].counts;
        do {
            register u8 *destination asm("r2") = bytes + j;
            register int k asm("r0") = 3;
            // The original repeats this same per-slot byte store four times.
            do {
                *destination = zero;
            } while (--k >= 0);
            *((u8 *)tail) = hundred;
            *((u8 *)tail + 1) = hundred;
            *((u8 *)tail + 2) = zero;
            *((u8 *)tail + 3) = zero;
            tail->timer = zero;
            asm("" : "+r"(tail), "+r"(bytes));
            tail = (struct Tail *)((u8 *)tail + 12);
            bytes += 12;
            j++;
        } while (j <= 1);
    }
    {
        u8 *base = local.counts;
        int zero = 0;
        u8 *c;
        c = base + 3;
        do {
            *c = zero;
            c--;
        } while ((s32)c >= (s32)base);
    }
    WirelessExchange_ClearPeerCounts(p, 0);
    WirelessExchange_ClearPeerCounts(p, 1);
    if (Script_SeekToKeyword(115)) {
        void *resource = Script_GetTextSourcePc();
        p->resource = resource;
        if (resource) {
            asm("" : "+r"(p));
            p->delta = 0;
            p->weights[0] = (int)Script_GetKeywordValue(77, 11);
            if (Script_SeekToKeyword(112)) {
                cursor = p->weights + 1;
                for (i = 3; i >= 0; i--) {
                    *cursor = Script_GetValue();
                    cursor++;
                }
                return 0;
            }
        }
    }
    return -1;
}
