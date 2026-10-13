// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Vm {
    u8 pad[0x1C];
    const u8 *pc;
};
u16 sub_08032070(const u8 *);
int sub_080321CC(struct Vm *p)
{
    const u8 *s = p->pc;
    int n = 0;
    u32 c;
    goto test;
loop:
    c <<= 24;
    if ((s32)c < 0)
        goto wide;
    c >>= 24;
    if (c == '<') {
        do {
            s++;
            c = *s;
        } while (c != '>' && c && c != '\n');
        goto next;
    }
    if (c == 0x1F) {
        u32 code = (u16)sub_08032070(s);
        if (code != 0xFFFF) {
            {
                u32 mask = 0xFF;
                register u32 bits asm("r1");
                asm("" : "+r"(mask));
                mask <<= 8;
                asm("" : "+r"(mask));
                bits = mask;
                mask = code;
                mask &= bits;
                if (mask)
                    goto wide;
            }
            n++;
            goto nextWide;
        }
    }
    n++;
next:
    s++;
    goto limit;
wide:
    n += 2;
nextWide:
    s += 2;
limit:
    if (n > 254) {
        n = 255;
        goto done;
    }
test:
    c = *s;
    if (c)
        goto loop;
done:
    return n;
}
