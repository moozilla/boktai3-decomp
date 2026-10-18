#include "global.h"

struct S {
    u8 pad[0x68];
    s32 f68;
};
void Registry6A37_UpdateBackgroundScroll(struct S *);
void Registry6A37_StartSoundPhase(struct S *);

void Registry6A37_UpdateFadeIn(struct S *p)
{
    Registry6A37_UpdateBackgroundScroll(p);
    p->f68++;
    if (p->f68 > 0x3f) {
        Registry6A37_StartSoundPhase(p);
    }
}
