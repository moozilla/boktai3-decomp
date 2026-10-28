#include "global.h"
#include "area_effect_link.h"

struct P {
    u8 f[0x1c];
    u8 a;
    u8 g[7];
    u8 b;
};
void Sound_FadeOutChannel10(s32);

static inline u8 chk(struct P *p)
{
    if (p->a == 0)
        return TRUE;
    return FALSE;
}

void Registry1745_SetSelectionResponse(u32 v)
{
    if (gLinkSession != 0) {
        struct P *p = gLinkSession;
        if (!chk(p)) {
            p->b = v;
            Sound_FadeOutChannel10(1);
        }
    }
}
