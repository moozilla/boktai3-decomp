#include "global.h"
s32 PauseMenu_GetEquipmentRowY(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    s32 t = e * 3;
    t = 0x88 - t;
    return t - b * 24;
}
