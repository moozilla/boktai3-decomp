#include "global.h"
/* Ivy_EnterDisabledState: Consumes the entry flag and switches drawable and collision flags to directional animation 20; disabled role tentative */
#define Ivy_EnterDisabledState sub_080ED484

void SceneActorAnimation_SelectDirectionalFlags(u8 *, u32);

static inline u8 Consume(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Or(u16 m, u16 *f)
{
    *f |= m;
}

void sub_080ED484(u8 *p)
{
    if (Consume(p + 0x2b2)) {
        (*(u8 **)(p + 4))[7] = 3;
        {
            u16 v = 4 | *(u16 *)(p + 0x13a);
            *(u16 *)(p + 0x13a) = v & 0xfff7;
        }
        Or(0x800, (u16 *)(p + 0x2c0));
        SceneActorAnimation_SelectDirectionalFlags(p, 20);
    }
}
