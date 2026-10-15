#include "global.h"

struct G {
    u8 filler[0x868];
    u32 f;
};
extern struct G *gUnk_02000710;
void AreaEffect_KeepLinkVisibility(void *);
void AreaEffect_UpdateLinkVisibility(void *);
void AreaEffect_UpdateLinkPlayerResources(void *);

s32 AreaEffect_TickLink(void *p)
{
    if (gUnk_02000710->f & 2)
        AreaEffect_KeepLinkVisibility(p);
    else
        AreaEffect_UpdateLinkVisibility(p);
    AreaEffect_UpdateLinkPlayerResources(p);
    return 0;
}
