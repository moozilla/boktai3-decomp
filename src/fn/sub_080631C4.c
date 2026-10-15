#include "global.h"

void SceneInputPanel_RequestAction(u8 *, s32 (*)(u8 *));
s32 sub_080631E8(u8 *);

s32 sub_080631C4(u8 *p)
{
    if (*(u16 *)(p + 0x1AEE) > 0x1f)
        SceneInputPanel_RequestAction(p, sub_080631E8);
    return 0;
}
