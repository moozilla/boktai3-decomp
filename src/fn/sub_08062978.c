#include "global.h"

void sub_08030BF8(void);
void sub_08033468(void);
void SceneInputPanel_ShowItemComparison(u8 *, u8 *, u8 *, s32, s32);
void SceneInputPanel_ShowAttributeComparison(u8 *, u8 *, u8 *, u8 *, s32);

void sub_08062978(u8 *p)
{
    *(u32 *)(p + 0xD10) |= 1;
    sub_08030BF8();
    sub_08033468();
    SceneInputPanel_ShowItemComparison(p + 0x14E8, p + 0x1608, p + 0x20, 0, -1);
    SceneInputPanel_ShowAttributeComparison(p + 0x1728, p + 0x1848, p + 0x1968, p + 0x20, -1);
}
