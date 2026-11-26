#include "global.h"

void Task4E01_RemoveEntry(s32);

void Player_ReleaseActionEffectOnExit(u8 *p)
{
    s32 v = *(s32 *)(p + 0xC74);

    if (v >= 0) {
        Task4E01_RemoveEntry(v);
    }
}
