#include "global.h"

void ModifierMenu_DecodeItemCursor(u8 *, u32);

void ModifierMenu_InitSavedCursors(u8 *p)
{
    u8 *s = p + 0x90;
    ModifierMenu_DecodeItemCursor(p + 0x24, *(s16 *)(s + 0x1c));
    ModifierMenu_DecodeItemCursor(p + 0x28, *(s16 *)(s + 0x24));
    ModifierMenu_DecodeItemCursor(p + 0x2c, *(s16 *)(s + 0x26));
}
