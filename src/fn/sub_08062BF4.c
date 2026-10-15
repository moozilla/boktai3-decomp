#include "global.h"

void SceneInputPanel_InitBackground(u8 *, s32, u8);

void sub_08062BF4(u8 *p, s32 a)
{
    SceneInputPanel_InitBackground(p, a, *(u8 *)(p + 0x1AB1));
}
