#include "global.h"

struct G {
    u8 f[0x86c];
    s16 a;
};
extern struct G *gUnk_02000710;
s32 MessageWindow_GetSoundMode(void)
{
    return gUnk_02000710->a;
}
