#include "global.h"

void SceneActorAnimation_SelectDirectionalEntry(void *, u32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_080E1CB8(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 0x10);
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}
