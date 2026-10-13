// MATCHING_CONCESSION: register-pinning
#include "global.h"
int sub_0803972C(void);
int sub_08039744(void);
void sub_0821980C(void *, void *, int, int);
void sub_0803ACD0(u8 *p)
{
    int value = sub_0803972C();
    register int category asm("r2") = 0;
    if (value <= 60) {
        category = 2;
        if (value > 25)
            category = 1;
    }
    {
        register int offset asm("r3");
        register void *first asm("r0") = p + 480;
        register void *descriptor asm("r1");
        asm("" : "+r"(first));
        offset = 448;
        descriptor = p + offset;
        asm("" : "+r"(descriptor), "+r"(offset));
        offset -= 43;
        sub_0821980C(first, descriptor, category + offset, 0);
    }
    {
        register int category asm("r3");
        value = sub_08039744();
        category = 0;
        if (value <= 60) {
            category = 2;
            if (value > 25)
                category = 1;
        }
        {
            register int initialOffset asm("r1") = 576;
            register void *first asm("r0");
            register void *descriptor asm("r1");
            register int base asm("r4");
            register int value asm("r2");
            asm("" : "+r"(initialOffset));
            first = p + initialOffset;
            asm("" : "+r"(first));
            value = 448;
            asm("" : "+r"(value));
            descriptor = p + value;
            asm("" : "+r"(descriptor));
            base = 408;
            asm("" : "+r"(base));
            value = base;
            asm("" : "+r"(value));
            category |= value;
            asm("" : "+r"(category));
            sub_0821980C(first, descriptor, category, 0);
        }
    }
}
