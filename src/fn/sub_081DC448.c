#include "global.h"
extern u32 gScreenParams[];
void Anim_Remove(u8 *);
void ScreenMessage_Destroy(u8 *p)
{
    if (gScreenParams[2] == 1) {
        Anim_Remove(p + 0xa4);
        Anim_Remove(p + 0x1c);
    }
}
