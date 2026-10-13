// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Tentative result panel and drawable offsets.
struct Drawing {
    u8 pad[8];
    u32 flags;
    u8 padC[0x14];
    u16 x, y;
};
struct P {
    u8 pad[0x18];
    u32 timer;
    u8 state, entered;
    u8 pad1E[6];
    u8 result;
    u8 pad25[0x93];
    struct Drawing drawing;
};
extern u8 *gUnk_030053F8;
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_0822AE7C(int);
void sub_0821980C(void *, void *, int, int);

void sub_0803B01C(struct P *p)
{
    if (p->entered) {
        register int icon asm("r5");
        register int count asm("r1");
        register struct Drawing *drawing asm("r6");
        register u8 *result asm("r3");
        register u8 **save asm("r4");
        p->entered = 0;
        {
            register u8 *address asm("r0") = &p->result;
            register int resultValue asm("r1") = *address;
            asm("" : "+r"(resultValue));
            icon = 30;
            result = address;
            if (resultValue != 0) {
                icon = 32;
                if (resultValue == 1)
                    icon = 31;
            }
        }
        {
            register u8 **address asm("r0") = &gUnk_030053F8;
            register int maximum asm("r2");
            asm("" : "+r"(address));
            count = *(u16 *)(*address + 0x14) + 1;
            maximum = 9999;
            asm("" : "+r"(maximum));
            save = address;
            asm("" : "+r"(save));
            if (count > maximum)
                count = maximum;
            *(u16 *)(*save + 0x14) = count;
        }
        {
            int resultValue = *result;
            if (resultValue == 0) {
                sub_0822AE7C(0xB0);
                {
                    register int old asm("r0") = *(u16 *)(*save + 0x16);
                    asm("" : "+r"(old));
                    count = old + 1;
                }
                {
                    register int maximum asm("r0") = 9999;
                    asm("" : "+r"(maximum));
                    if (count > maximum)
                        count = maximum;
                }
                {
                    register u8 **address asm("r0") = &gUnk_030053F8;
                    asm("" : "+r"(address));
                    *(u16 *)(*address + 0x16) = count;
                }
            } else if (resultValue == 1) {
                sub_0822AE7C(0xB1);
            } else {
                sub_0822AE7C(0xB1);
            }
        }
        drawing = &p->drawing;
        drawing->flags &= ~1;
        {
            register u8 *descriptor asm("r1") = (u8 *)p + 0x50;
            register int resource asm("r2") = icon;
            asm("" : "+r"(descriptor), "+r"(resource));
            sub_0821980C(drawing, descriptor, resource, 0);
        }
        drawing->x = 0;
        drawing->y = 0;
    }
    if (p->timer <= 7) {
        register struct Drawing *drawing asm("r6") = &p->drawing;
        register u16 *table asm("r5") = gUnk_0203B400;
        register u32 *indexPointer asm("r2");
        register u32 index asm("r0");
        register u32 mask asm("r4");
        register int value asm("r1");
        register int smallMask asm("r3");
        asm("" : "+r"(drawing), "+r"(table));
        indexPointer = &gUnk_03005308;
        index = *indexPointer + 1;
        mask = 1023;
        asm("" : "+r"(mask));
        index &= mask;
        *indexPointer = index;
        value = table[index];
        smallMask = 3;
        value &= smallMask;
        value -= 2;
        drawing->x = value;
        index++;
        index &= mask;
        *indexPointer = index;
        index = table[index];
        index &= smallMask;
        index -= 2;
        drawing->y = index;
    } else if (p->timer == 8) {
        register struct Drawing *drawing asm("r6") = &p->drawing;
        asm("" : "+r"(drawing));
        drawing->x = 0;
        drawing->y = 0;
    }
    if (p->timer > 29) {
        register int state asm("r0") = 3;
        register int zero asm("r1") = 0;
        asm("" : "+r"(state), "+r"(zero));
        p->state = state;
        p->entered = 1;
        p->timer = zero;
    } else {
        p->timer++;
    }
}
