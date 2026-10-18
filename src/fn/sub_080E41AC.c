#include "global.h"

extern const u8 gUnk_08606204[];

struct S {
    u8 filler[648];
    const u8 *p;
};

void Dog_SetPrimaryBehaviorPreset(struct S *s)
{
    s->p = gUnk_08606204;
}
