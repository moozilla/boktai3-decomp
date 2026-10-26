#include "global.h"

struct G {
    u8 f[0x86c];
    s16 a;
};
extern struct G *gGameState;
s32 MessageWindow_GetSoundMode(void)
{
    return gGameState->a;
}
