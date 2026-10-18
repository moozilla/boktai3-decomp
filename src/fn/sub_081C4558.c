#include "global.h"

struct S {
    u8 pad[0x68];
    s32 f68;
};
void Registry6A37_UpdateBackgroundScroll(struct S *);
void Registry6A37_StartFadeOut(struct S *);

void Registry6A37_UpdateDisplayHold(struct S *p)
{
    Registry6A37_UpdateBackgroundScroll(p);
    if (p->f68 > 0xb3) {
        Registry6A37_StartFadeOut(p);
    }
    p->f68++;
}
