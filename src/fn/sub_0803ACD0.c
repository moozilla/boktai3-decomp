// MATCHING_CONCESSION: register-pinning
#include "global.h"
int sub_0803972C(void);
int sub_08039744(void);
void sub_0821980C(void *, void *, int, int);
void sub_0803ACD0(u8 *p)
{
    int value = sub_0803972C();
    int category = 0;
    if (value <= 60) {
        category = 2;
        if (value > 25)
            category = 1;
    }
    {
        register int offset asm("r3");
        void *first = p + 480;
        void *descriptor;
        offset = 448;
        descriptor = p + offset;
        offset -= 43;
        sub_0821980C(first, descriptor, category + offset, 0);
    }
    {
        int category;
        value = sub_08039744();
        category = 0;
        if (value <= 60) {
            category = 2;
            if (value > 25)
                category = 1;
        }
        {
            register int initialOffset asm("r1") = 576;
            void *first;
            void *descriptor;
            register int base asm("r4");
            register int value asm("r2");
            first = p + initialOffset;
            value = 448;
            asm("" : "+r"(value));
            descriptor = p + value;
            base = 408;
            asm("" : "+r"(base));
            value = base;
            asm("" : "+r"(value));
            category |= value;
            sub_0821980C(first, descriptor, category, 0);
        }
    }
}
