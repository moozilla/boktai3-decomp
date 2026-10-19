#include "global.h"

#define SceneActorMotion_ToggleMode sub_081C3950

struct BlinkState {
    u8 pad00[0x34];
    u32 flags;         /* 0x34 */
    u8 pad38[0x9c - 0x38];
    u16 timer;         /* 0x9c */
    s16 mode;          /* 0x9e */
};

void sub_081C3950(struct BlinkState *p)
{
    s16 *mode;

    if (p->flags & 1)
        return;

    mode = &p->mode;
    switch (mode[0]) {
    case 0:
    {
        s16 ctr = ++p->timer;
        if (ctr <= 1)
            return;
        {
            s32 old = mode[0];
            s16 *flagsArr = (s16 *)&p->flags;
            flagsArr[8] = old;
        }
        mode[0] = 1;
        break;
    }
    case 1:
    {
        s16 ctr = ++p->timer;
        if (ctr <= 0)
            return;
        {
            s32 old = mode[0];
            s16 *flagsArr = (s16 *)&p->flags;
            flagsArr[8] = old;
        }
        mode[0] = 0;
        break;
    }
    default:
        return;
    }
    p->timer = 0;
}
