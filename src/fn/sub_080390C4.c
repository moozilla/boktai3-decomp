// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
extern struct Menu *gUnk_020000FC;
extern struct Save *gUnk_02000710;
extern u8 gUnk_03003A50, gUnk_0203B400[];
extern u32 gUnk_03005308;
void *sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
void *Script_GetPc(void);
int Script_GetValue(void);
u8 *sub_08227F5C(int);
void sub_0822D83C(int, void *);
void sub_0822E650(int, int);
void sub_0822E9FC(int, int);
void sub_0822E0DC(int, int);
void sub_0822CE18(int);
void sub_0822CE34(int);
void sub_08038074(struct Menu *, int);
void *sub_08227E90(void);
int sub_080390C4(struct Menu *arg) {
    register struct Menu *p asm("r6") = arg;
    register u16 *leftBase asm("r9");
    register u16 *rightBase asm("r10");
    struct Locals {
        u8 *vec, *state, *entered, *v2b, *v2c, *v2d, *enabled, *v33, *counts;
    } local;
    int i;
    u8 *item;
    register u16 *cursor asm("r4");
    gUnk_020000FC = p;
    p->listener = sub_0821ABA8(101, 0);
    p->secondListener = sub_0821ABA8(114, 0);
    p->count = 0;
    p->leftMask = 0;
    if (Script_SeekToKeyword(119)) {
        leftBase = p->left;
        {
            register u32 offset asm("r1") = 0xBC;
            asm("" : "+r"(offset));
            rightBase = (u16 *)(offset+(u32)p);
        }
        {
            register u8 *a asm("r3") = &p->v2b;
            register u8 *b asm("r0");
            register u8 *c asm("r1");
            asm("" : "+r"(a));
            local.v2b = a;
            b = &p->v2c;
            asm("" : "+r"(b));
            local.v2c = b;
            c = &p->v2d;
            asm("" : "+r"(c));
            local.v2d = c;
            a += 8;
            asm("" : "+r"(a));
            local.v33 = a;
            b += 0xc0;
            asm("" : "+r"(b));
            local.vec = b;
            c += 5;
            asm("" : "+r"(c));
            local.enabled = c;
            a -= 11;
            asm("" : "+r"(a));
            local.state = a;
            b -= 0xc3;
            asm("" : "+r"(b));
            local.entered = b;
            c += 6;
            asm("" : "+r"(c));
            local.counts = c;
        }
        cursor = leftBase;
        do {
            if (Script_GetPc()) {
                *cursor = Script_GetValue();
                p->count++;
            } else *cursor = 0;
            cursor++;
            asm("" : "+r"(leftBase));
        } while ((s32)cursor <= (s32)(leftBase+15));
    } else {
        {
            register u32 offset asm("r3") = 0x7C;
            asm("" : "+r"(offset));
            leftBase = (u16 *)(offset+(u32)p);
        }
        {
            register u32 offset asm("r0") = 0xBC;
            asm("" : "+r"(offset));
            rightBase = (u16 *)(offset+(u32)p);
        }
        {
            register u8 *a asm("r1") = &p->v2b;
            register u8 *b asm("r3");
            register u8 *c asm("r0");
            asm("" : "+r"(a));
            local.v2b = a;
            b = &p->v2c;
            asm("" : "+r"(b));
            local.v2c = b;
            c = &p->v2d;
            asm("" : "+r"(c));
            local.v2d = c;
            a += 8;
            asm("" : "+r"(a));
            local.v33 = a;
            b += 0xc0;
            asm("" : "+r"(b));
            local.vec = b;
            c += 5;
            asm("" : "+r"(c));
            local.enabled = c;
            a -= 11;
            asm("" : "+r"(a));
            local.state = a;
            b -= 0xc3;
            asm("" : "+r"(b));
            local.entered = b;
            c += 6;
            asm("" : "+r"(c));
            local.counts = c;
            {
                register u16 *end asm("r1") = leftBase;
                register int zero asm("r2") = 0;
                register u16 *cursor asm("r0");
                asm("" : "+r"(end), "+r"(zero));
                cursor = (u16 *)(c+0x62);
                do {
                    *cursor = zero;
                    cursor--;
                } while ((s32)cursor >= (s32)end);
            }
        }
    }
    asm("" ::: "memory");
    {
        register u32 count asm("r1") = p->count;
        if (count) {
            register int r asm("r4");
            register int j asm("r5");
            r = gUnk_03003A50;
            j = 0;
            if ((u32)j<count) do {
                register int index asm("r2");
                register u32 mask asm("r1");
                item = sub_08227F5C(leftBase[j]);
                if (!*item) {
                    index = 0;
                    mask = p->leftMask;
                } else {
                    index = 15;
                    asm("" : "+r"(index));
                    index &= r;
                    if (!index)index = 1;
                    {
                        register u32 bit asm("r0") = 1 << index;
                        asm("" : "+r"(bit));
                        mask = p->leftMask;
                        if (bit&mask) {
                            register int small asm("r8");
                            register int one asm("r12");
                            register u32 used asm("r3");
                            {
                                register int x asm("r3") = 15;
                                asm("" : "+r"(x));
                                small = x;
                            }
                            {
                                register int x asm("r0") = 1;
                                asm("" : "+r"(x));
                                one = x;
                            }
                            used = mask;
                            do {
                                index = r;
                                asm("" : "+r"(index));
                                index &= small;
                                if (!index)index = 1;
                                r++;
                            } while ((one << index)&used);
                        }
                    }
                }
                p->leftMask = (1 << index)|mask;
                sub_0822D83C(index, item);
                {
                    register u32 *read asm("r1") = &gUnk_03005308;
                    register u32 *write asm("r3");
                    register u32 x asm("r0");
                    register u8 *table asm("r1");
                    asm("" : "+r"(read));
                    x = (*read+1)&1023;
                    write = &gUnk_03005308;
                    asm("" : "+r"(write));
                    *write = x;
                    x <<= 1;
                    asm("" : "+r"(x));
                    table = gUnk_0203B400;
                    r += *(u8 *)(x+(u32)table);
                }
                j++;
            } while ((u32)j<p->count);
        }
    }
    item = sub_08227F5C(0);
    sub_0822D83C(0, item);
    p->rightCount = 0;
    p->extraMask = 0;
    if (Script_SeekToKeyword(103)) {
        cursor = rightBase;
        do {
            if (Script_GetPc()) {
                *cursor = Script_GetValue();
                p->rightCount++;
            } else *cursor = 0;
            cursor++;
            asm("" : "+r"(rightBase));
        } while ((s32)cursor <= (s32)(rightBase+7));
    } else {
        register u16 *end asm("r1") = rightBase;
        register int zero asm("r2") = 0;
        register u16 *cursor asm("r0");
        asm("" : "+r"(end), "+r"(zero));
        cursor = rightBase+7;
        do {
            *cursor = zero;
            cursor--;
        } while ((s32)cursor >= (s32)end);
    }
    asm("" ::: "memory");
    {
        register u32 count asm("r1") = p->rightCount;
        if (count) {
            register int r asm("r4");
            register int j asm("r5");
            u16 *q;
            register u32 *random asm("r9");
            r = gUnk_03003A50;
            j = 0;
            if ((u32)j<count) {
                q = rightBase;
                {
                    register u32 *x asm("r3") = &gUnk_03005308;
                    asm("" : "+r"(x));
                    random = x;
                }
                do {
                    register int index asm("r2");
                    register u32 mask asm("r1");
                    if (*q == 1) {
                        index = 0;
                        mask = p->extraMask;
                    } else {
                        index = 7;
                        asm("" : "+r"(index));
                        index &= r;
                        if (!index)index = 1;
                        {
                            register u32 bit asm("r0") = 1 << index;
                            asm("" : "+r"(bit));
                            mask = p->extraMask;
                            if (bit&mask) {
                                register int small asm("r12");
                                register int one asm("r8");
                                register u32 used asm("r3");
                                {
                                    register int x asm("r0") = 7;
                                    asm("" : "+r"(x));
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
                                    if (!index)index = 1;
                                    r++;
                                } while ((one << index)&used);
                            }
                        }
                    }
                    p->extraMask = (1 << index)|mask;
                    asm("" : "+r"(q));
                    sub_0822E650(index, *q);
                    {
                        register u32 *read asm("r1") = random;
                        register u32 *write asm("r3");
                        register u32 x asm("r0");
                        register u8 *table asm("r1");
                        asm("" : "+r"(read));
                        x = (*read+1)&1023;
                        write = random;
                        asm("" : "+r"(write));
                        *write = x;
                        x <<= 1;
                        asm("" : "+r"(x));
                        table = gUnk_0203B400;
                        r += *(u8 *)(x+(u32)table);
                    }
                    asm("" : "+r"(q));
                    q++;
                    j++;
                } while ((u32)j<p->rightCount);
            }
        }
    }
    sub_0822E650(0, 1);
    sub_0822E9FC(0, 0);
    for (i = 1;i <= 7;i++)sub_0822E9FC(i, -1);
    {
        register struct Save **slot asm("r3");
        register int minus asm("r0");
        register int value asm("r2");
        i = 0;
        slot = &gUnk_02000710;
        minus = -1;
        asm("" : "+r"(slot), "+r"(minus));
        value = minus;
        do {
            register struct Save *save asm("r1") = *slot;
            register int offset asm("r0") = i << 1;
            asm("" : "+r"(offset));
            *(s16 *)(offset+(u32)save+0x58) = value;
            i++;
        } while (i <= 3);
    }
    for (i = 0;i <= 15;i++)sub_0822E0DC(i, -1);
    for (i = 0;i <= 15;i++)sub_0822CE18(i);
    for (i = 0;i <= 15;i++)sub_0822CE34(i);
    p->rightMask = 0xFCF;
    {
        register int zero asm("r2") = 0;
        register int byteZero asm("r0");
        register int one asm("r1");
        p->v18 = zero;
        byteZero = 0;
        asm("" : "+r"(byteZero));
        {
            register u16 half asm("r2") = 0;
            asm("" : "+r"(half));
            p->v1c = half;
            p->v1e = half;
        }
        p->flags = zero;
        {
            register u8 *q asm("r1") = local.v2b;
            asm("" : "+r"(q));
            *q = byteZero;
        }
        {
            register u8 *q asm("r3") = local.v2c;
            asm("" : "+r"(q));
            *q = byteZero;
        }
        {
            register u8 *q asm("r1") = local.v2d;
            asm("" : "+r"(q));
            *q = byteZero;
        }
        {
            register u8 *q asm("r3") = local.v33;
            asm("" : "+r"(q));
            *q = byteZero;
        }
        {
            register u8 *q asm("r1") = local.vec;
            asm("" : "+r"(q));
            *q = byteZero;
        }
        one = 1;
        asm("" : "+r"(one));
        {
            register u8 *q asm("r3") = local.enabled;
            asm("" : "+r"(q));
            *q = one;
        }
        {
            register u8 *q asm("r3") = local.state;
            asm("" : "+r"(q));
            *q = byteZero;
        }
        {
            register u8 *q asm("r0") = local.entered;
            asm("" : "+r"(q));
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
        register int zero asm("r4") = 0;
        int hundred = 100;
        register struct Tail *tail asm("r1") = (struct Tail *)&p->slots[0].a;
        register u8 *bytes asm("r3") = p->slots[0].counts;
        asm("" : "+r"(j));
        do {
            register u8 *destination asm("r2") = bytes+j;
            register int k asm("r0") = 3;
            // The original repeats this same per-slot byte store four times.
            do {
                *destination = zero;
            } while (--k >= 0);
            *((u8 *)tail) = hundred;
            *((u8 *)tail+1) = hundred;
            *((u8 *)tail+2) = zero;
            *((u8 *)tail+3) = zero;
            tail->timer = zero;
            asm("" : "+r"(tail), "+r"(bytes));
            tail = (struct Tail *)((u8 *)tail+12);
            bytes += 12;
            j++;
        } while (j <= 1);
    }
    {
        register u8 *base asm("r1") = local.counts;
        register int zero asm("r2") = 0;
        register u8 *c asm("r0");
        asm("" : "+r"(base), "+r"(zero));
        c = base+3;
        do {
            *c = zero;
            c--;
        } while ((s32)c >= (s32)base);
    }
    sub_08038074(p, 0);
    sub_08038074(p, 1);
    if (Script_SeekToKeyword(115)) {
        void *resource = sub_08227E90();
        p->resource = resource;
        if (resource) {
            asm("" : "+r"(p));
            p->delta = 0;
            p->weights[0] = (int)sub_0821ABA8(77, 11);
            if (Script_SeekToKeyword(112)) {
                cursor = p->weights+1;
                for (i = 3;i >= 0;i--) {
                    *cursor = Script_GetValue();
                    cursor++;
                }
                return 0;
            }
        }
    }
    return -1;
}
