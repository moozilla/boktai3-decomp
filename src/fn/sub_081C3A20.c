#include "global.h"

s32 __divsi3(s32, s32);
void RegistryF3E4_UpdateVisibleIdle(void);

static inline void SetCallback(void (*fn)(void), void (**slot)(void))
{
    *slot = fn;
}

struct S {
    u8 pad00[0x34];
    u32 f34; /* 0x34 */
    u8 pad38[0x3c - 0x38];
    u8 scale[2]; /* 0x3c */
    u8 pad3e[0xd8 - 0x3e];
    u32 d8; /* 0xd8 */
    u8 padDC[0xf4 - 0xdc];
    void (*f4)(void); /* 0xf4 */
    u32 f8;           /* 0xf8 */
};

void SpriteController_AnimateGrowing(struct S *p)
{
    s32 counter = p->f8;
    s32 value;
    s32 limit2;

    if (counter > 0)
        goto checkUpper;
    value = 0x26;
    goto store;

checkUpper: {
    s32 limit = 8;
    if (counter < limit)
        goto interp;
}
    value = 0x40;
    goto store;

interp: {
    s32 k = 0xd0 << 5;
    s32 t = counter * k;
    s32 d = 8;
    s32 result;
    t = t / d;
    if (t >= 0)
        result = t >> 8;
    else
        result = -(-t >> 8);
    value = result;
    value += 0x26;
}

store:
    p->scale[0] = value;
    p->scale[1] = value;

    limit2 = 8;
    if ((s32)p->f8 >= limit2) {
        p->f34 &= -2;
        SetCallback(RegistryF3E4_UpdateVisibleIdle, &p->f4);
        p->f8 = 0;
        p->d8 = 0;
    } else {
        p->f8++;
    }
}
