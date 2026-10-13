// MATCHING_CONCESSION: register-pinning
#include "global.h"

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
extern struct Save *gUnk_02000710;
extern u32 gUnk_03003A50, gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_0822E498(int);
int Mod(int, int);

void sub_08038448(struct P *p)
{
    register u32 mask asm("r8") = 0;
    register int start asm("r4");
    register int i asm("r5");
    register u32 *random asm("r9");
    gUnk_02000710->selection = mask;
    sub_0822E498(0);
    {
        register u32 t asm("r3") = 1;
        asm volatile("" : "+r"(t));
        mask = t;
    }
    if ((p->rightMask & 2) && p->round > 3) {
        sub_0822E498(1);
        {
            register u32 t asm("r0") = 3;
            asm volatile("" : "+r"(t));
            mask = t;
        }
    }
    if ((p->rightMask & 4) && p->round > 3) {
        sub_0822E498(2);
        {
            register u32 t asm("r1") = mask;
            asm volatile("" : "+r"(t));
            t |= 4;
            mask = t;
        }
    }
    if (p->rightMask & 64) {
        sub_0822E498(6);
        {
            register u32 t asm("r3") = mask;
            asm volatile("" : "+r"(t));
            t |= 64;
            mask = t;
        }
    }
    if (p->rightMask & 128) {
        sub_0822E498(7);
        {
            register u32 t asm("r0") = mask;
            asm volatile("" : "+r"(t));
            t |= 128;
            mask = t;
        }
    }
    if (p->rightMask & 256) {
        sub_0822E498(8);
        {
            register u32 t asm("r1") = mask;
            asm volatile("" : "+r"(t));
            t |= 256;
            mask = t;
        }
    }
    if (p->rightMask & 512) {
        sub_0822E498(9);
        {
            register u32 t asm("r3") = mask;
            asm volatile("" : "+r"(t));
            t |= 512;
            mask = t;
        }
    }
    if (p->rightMask & 1024) {
        sub_0822E498(10);
        {
            register u32 t asm("r0") = mask;
            asm volatile("" : "+r"(t));
            t |= 1024;
            mask = t;
        }
    }
    if (p->rightMask & 2048) {
        sub_0822E498(11);
        {
            register u32 t asm("r1") = mask;
            asm volatile("" : "+r"(t));
            t |= 2048;
            mask = t;
        }
    }
    start = Mod(gUnk_03003A50, 12);
    i = 0;
    {
        register u32 *t asm("r0") = &gUnk_03005308;
        asm volatile("" : "+r"(t));
        random = t;
    }
    do {
        register int n asm("r3");
        register int limit asm("r6");
        int offset;
        int index;
        register int byteOffset asm("r2");
        {
            register struct Save **slot asm("r0") = &gUnk_02000710;
            register struct Save *save asm("r1");
            register s16 *row asm("r1");
            asm volatile("" : "+r"(slot));
            save = *slot;
            byteOffset = i * 2;
            row = (s16 *)(byteOffset + (u32)save + 0x88);
            asm volatile("" : "+r"(row), "+r"(byteOffset));
            *row = -1;
        }
        n = start;
        offset = byteOffset;
        limit = start + 12;
        i++;
        asm volatile("" : "+r"(n), "+r"(limit), "+r"(offset));
        goto condition;
    next:
        n++;
    condition:
        if (n < limit) {
            register int item asm("r1") = n;
            register u32 bit asm("r2");
            asm volatile("" : "+r"(n), "+r"(limit), "+r"(offset) :: "r10", "r12");
            if (item > 11)
                item -= 12;
            bit = 1 << item;
            asm volatile("" : "+r"(bit));
            if (({
                register u32 t asm("r0") = mask;
                asm volatile("" : "+r"(t));
                t & bit;
            }) != 0) {
                {
                    register struct Save **slot asm("r0") = &gUnk_02000710;
                    register struct Save *save asm("r0");
                    asm volatile("" : "+r"(slot), "+r"(offset), "+r"(limit), "+r"(n), "+r"(bit) :: "memory");
                    save = *slot;
                    *(s16 *)(offset + (u32)save + 0x88) = item;
                }
                {
                    register u32 t asm("r0") = mask;
                    asm volatile("" : "+r"(t));
                    t &= ~bit;
                    mask = t;
                }
                start = item;
                goto selected;
            }
            goto next;
        }
    selected:
        {
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
    gUnk_02000710->rightSelected = 0;
}
