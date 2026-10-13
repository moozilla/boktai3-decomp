#include "global.h"
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
    u8 enabled;u8 pad[31];
};
extern struct Save *gUnk_02000710;
extern u32 gUnk_03003A50, gUnk_03005308;
extern u16 gUnk_0203B400[];
int Mod(int, int);
void sub_0822D808(int, struct ItemInfo*);
int sub_0822E618(int);
void sub_0822EA8C(int, int);
void sub_0822EACC(int);
void sub_08038170(struct P *input)
{
    struct ItemInfo info;
    int missing, usable;
    register struct P *p asm("r10") = input;
    register int start asm("r4");
    int i;
    register u32 mask asm("r5");
    register int offset asm("r8");
    register int moduloMask asm("r9");
    register int count asm("r1") = p->count;
    int other;
    register u32 *seed asm("r5");
    if (!count||!p->rightCount)return;
    i = 0;
    start = count;
    do {
        *(s16*)((u8*)gUnk_02000710+i*2+0x68) = -1;
        i++;
    } while (i<=7);
    start = Mod(gUnk_03003A50, start);
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
        register int n asm("r2") = start;
        register struct Save **slot asm("r12");
        register int limit asm("r6");
        int index;
        {
            register struct Save **t asm("r0") = &gUnk_02000710;
            asm("" : "+r"(t));
            slot = t;
        }
        limit = start+16;
        asm("" : "+r"(limit), "+r"(i));
        if (start<limit) {
            register int item asm("r1") = start;
            register int mod asm("r0") = moduloMask;
            register u32 bit asm("r3");
            asm("" : "+r"(item), "+r"(mod));
            item &= mod;
            bit = 1<<item;
            if (({
                register u32 available asm("r0") = mask;
                asm("" : "+r"(available) :: "r10");
                available&bit;
            })) {
                {
                    register struct Save **t asm("r2") = slot;
                    asm("" : "+r"(t));
                    *(s16*)((u8*)*t+offset+0x68) = item;
                }
                mask&=~bit;
                goto selected16;
            }
            while (1) {
                n++;
                if (n>=limit)break;
                item = n;
                mod = moduloMask;
                asm("" : "+r"(item), "+r"(mod));
                item &= mod;
                bit = 1<<item;
                if (({
                    register u32 available asm("r0") = mask;
                    asm("" : "+r"(available) :: "r10");
                    available&bit;
                })) {
                    {
                        register struct Save **t asm("r4") = slot;
                        asm("" : "+r"(t));
                        *(s16*)((u8*)*t+offset+0x68) = item;
                    }
                    mask&=~bit;
                    start = n;
                    break;
                }
            }
        }
selected16:
        if (*(s16*)((u8*)gUnk_02000710+offset+0x68)<0) {
            register int t asm("r2") = 1;
            asm("" : "+r"(t));
            missing = t;
        }
        else {
            sub_0822D808(i, &info);
            if (!info.enabled){
                register int t asm("r0") = 1;
                asm("" : "+r"(t));
                usable = t;
            }
        }
        index = (gUnk_03005308+1)&1023;
        gUnk_03005308 = index;
        {
            register int off asm("r0") = index*2;
            register int m asm("r2");
            asm("" : "+r"(off));
            off = *(u16*)((u8*)gUnk_0203B400+off);
            m = moduloMask;
            asm("" : "+r"(m));
            start+=off&m;
        }
        {
            register int two asm("r0") = 2;
            asm("" : "+r"(two));
            offset+=two;
        }
        i++;
    } while (i<=1);
    {
        register struct P *t asm("r1") = p;
        asm("" : "+r"(t));
        other = t->rightCount;
    }
    {
        register struct Save **fallback asm("r12");
        {
            register struct Save **t asm("r2") = &gUnk_02000710;
            asm("" : "+r"(t));
            fallback = t;
        }
        seed = &gUnk_03003A50;
        asm("" : "+r"(seed));
        if (({
            register int t asm("r0") = missing;
            asm("" : "+r"(t));
            t;
        })&& !({
            register int t asm("r1") = usable;
            asm("" : "+r"(t));
            t;
        })) {
            i = 0;
            {
                register struct Save **t asm("r2") = fallback;
                register struct Save *save asm("r0");
                register s16 *row asm("r1");
                asm("" : "+r"(t));
                save = *t;
                row = (s16*)((u8*)save+0x68);
                if (*row<0) *row = i;
                else {
                    while (1) {
                        s16 *row;
                        i++;
                        if (i>1) break;
                        {
                            register struct Save **t asm("r1") = fallback;
                            register struct Save *save asm("r0");
                            asm("" : "+r"(t));
                            save = *t;
                            asm("" : "+r"(save));
                            row = (s16*)(i*2+(u32)save+0x68);
                        }
                        if (*row<0) {
                            {
                                register u16 zero asm("r0") = 0;
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
        *(s16*)((u8*)gUnk_02000710+i*2+0x78) = -1;
        i++;
    } while (i<=7);
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
        register int n asm("r2") = start;
        register struct Save **slot asm("r12");
        register int limit asm("r6");
        int index;
        {
            register struct Save **t asm("r0") = &gUnk_02000710;
            asm("" : "+r"(t));
            slot = t;
        }
        limit = start+8;
        asm("" : "+r"(limit), "+r"(i));
        if (start<limit) {
            register int item asm("r1") = start;
            register int mod asm("r0") = moduloMask;
            register u32 bit asm("r3");
            asm("" : "+r"(item), "+r"(mod));
            item &= mod;
            bit = 1<<item;
            if (({
                register u32 available asm("r0") = mask;
                asm("" : "+r"(available) :: "r10");
                available&bit;
            })) {
                {
                    register struct Save **t asm("r2") = slot;
                    asm("" : "+r"(t));
                    *(s16*)((u8*)*t+offset+0x78) = item;
                }
                mask&=~bit;
                goto selected8;
            }
            while (1) {
                n++;
                if (n>=limit)break;
                item = n;
                mod = moduloMask;
                asm("" : "+r"(item), "+r"(mod));
                item &= mod;
                bit = 1<<item;
                if (({
                    register u32 available asm("r0") = mask;
                    asm("" : "+r"(available) :: "r10");
                    available&bit;
                })) {
                    {
                        register struct Save **t asm("r4") = slot;
                        asm("" : "+r"(t));
                        *(s16*)((u8*)*t+offset+0x78) = item;
                    }
                    mask&=~bit;
                    start = n;
                    break;
                }
            }
        }
selected8:
        {
            register struct Save **t asm("r0") = &gUnk_02000710;
            s16 *row;
            asm("" : "+r"(t));
            row = (s16*)((u8*)*t+offset+0x78);
            if (*row<0)missing = 1;
            else if (sub_0822E618(*row)==1){
                register int t asm("r0") = 1;
                asm("" : "+r"(t));
                usable = t;
            }
        }
        index = (gUnk_03005308+1)&1023;
        gUnk_03005308 = index;
        {
            register int off asm("r0") = index*2;
            register int m asm("r2");
            asm("" : "+r"(off));
            off = *(u16*)((u8*)gUnk_0203B400+off);
            m = moduloMask;
            asm("" : "+r"(m));
            start+=off&m;
        }
        {
            register int two asm("r0") = 2;
            asm("" : "+r"(two));
            offset+=two;
        }
        i++;
    } while (i<=1);
    {
        register struct Save **fallback asm("r12");
        {
            register struct Save **t asm("r1") = &gUnk_02000710;
            asm("" : "+r"(t));
            fallback = t;
        }
        if (({
            register int t asm("r2") = missing;
            asm("" : "+r"(t));
            t;
        })&& !({
            register int t asm("r4") = usable;
            asm("" : "+r"(t));
            t;
        })) {
            i = 0;
            {
                register struct Save **t asm("r1") = fallback;
                register struct Save *save asm("r0");
                register s16 *row asm("r1");
                asm("" : "+r"(t));
                save = *t;
                row = (s16*)((u8*)save+0x78);
                if (*row<0) *row = i;
                else {
                    while (1) {
                        s16 *row;
                        i++;
                        if (i>1) break;
                        {
                            register struct Save **t asm("r4") = fallback;
                            register struct Save *save asm("r0");
                            asm("" : "+r"(t));
                            save = *t;
                            asm("" : "+r"(save));
                            row = (s16*)(i*2+(u32)save+0x78);
                        }
                        if (*row<0) {
                            {
                                register u16 zero asm("r0") = 0;
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
            asm("" : "+r"(t)); (*t)->leftSelected = 0;
        }
    }
    asm("" ::: "r1");
    sub_0822EA8C(0, 0);
    i = 1;
    do {
        sub_0822EACC(i);
        i++;
    } while (i<=7);
    gUnk_02000710->extraSelected = 0;
}
