// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Pair {
    u32 x, y;
};
struct Vec {
    s16 x, y, z;
};
struct Sprite {
    u32 flags;
    u8 pad4[12];
    u16 value;
    u8 pad12[10];
    u32 x, y;
    u8 pad24[8];
};
struct Drawing {
    u8 pad0[8];
    u32 flags;
    u8 padc[0x14];
    u32 x, y;
    u8 pad28[0x38];
};
struct Item {
    u8 state, entered, unused, count, type, pad5;
    s16 velocity;
    u16 timer;
    u8 pada[2];
    struct Vec start;
    struct Vec end;
    u8 pad1c[8];
    struct Sprite sprite;
    struct Drawing drawing;
};
struct Owner {
    u8 pad0[0x18];
    u8 current[4], pending[4];
    u8 pad20[0x1c];
    u8 resource[32];
    struct Item items[16];
};
extern const u16 gUnk_0824DE7C[], gUnk_0824DE8C[];
void sub_0821980C(void *, void *, int, int);
void sub_08039968(void *, int, int);
void sub_082216F8(void *, void *, void *, int, int);
void sub_08039998(void *, int, int);
extern struct Owner *gUnk_02000100;
int sub_0803A618(int indexArg, int amountArg, int pendingArg)
{
    int index = indexArg;
    int amount = amountArg;
    int pendingFlag = pendingArg;
    struct Owner *owner = gUnk_02000100;
    struct Item *target;
    if (!owner)
        return -1;
    if (amount <= 0) {
        int result = 0;
        return result;
    }
    {
        int size = 0xb0;
        int scaled;
        register int extra asm("r2");
        int offset;
        scaled = index * size;
        extra = 0x89c;
        offset = scaled + extra;
        target = (struct Item *)((u8 *)owner + offset);
        if (target->count) {
            struct Item *current;
            register u8 *first asm("r1");
            u8 *second;
            register u8 *dest asm("r5");
            register int total asm("r2");
            extra = 0x5dc;
            offset = scaled + extra;
            current = (struct Item *)((u8 *)owner + offset);
            if (target->unused) {
                first = owner->pending;
                {
                    register int slot asm("r0") = index;
                    dest = first + slot;
                }
                total = *dest;
                total += target->count;
                second = first;
                if (total > 255)
                    total = 255;
                *dest = total;
                first = owner->current;
            } else {
                first = owner->current;
                {
                    register int slot asm("r2") = index;
                    dest = first + slot;
                }
                total = *dest;
                total += target->count;
                if (total > 255)
                    total = 255;
                *dest = total;
                second = owner->pending;
            }
            {
                int slot = index;
                register u8 *address asm("r0");
                int a;
                int b;
                register int one asm("r0");
                int zero;
                asm("" : "+r"(slot));
                address = first + slot;
                a = *address;
                address = second + slot;
                b = *address;
                one = 1;
                zero = 0;
                asm("" : "+r"(a), "+r"(b), "+r"(one), "+r"(zero));
                current->state = one;
                current->entered = one;
                current->timer = zero;
                {
                    register int value asm("r0") = a + b;
                    int clear;
                    if (value > 10)
                        value = 10;
                    clear = 0;
                    asm("" : "+r"(clear));
                    current->count = value;
                    value = 0;
                    current->unused = value;
                    target->count = clear;
                }
            }
        }
    }
    {
        register int value asm("r1") = amount;
        int zero;
        if (value > 10)
            value = 10;
        else if (value < 0)
            value = 0;
        zero = 0;
        target->count = value;
        value = pendingFlag;
        target->unused = value;
        target->state = 5;
        target->entered = 1;
        target->timer = zero;
        return zero;
    }
}
