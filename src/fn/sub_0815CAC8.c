#include "global.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
void PlayerCue_OverlayOff(u8 *);
void sub_0814F828(u8 *);
void sub_0815CAC8(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p) {
        PlayerCue_OverlayOff(p);
        sub_0814F828(p);
    }
}
