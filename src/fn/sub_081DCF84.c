#include "global.h"
#include "engine.h"
#include "ram.h"
void Menu_DrawText(u32, u32, const u8 *);
void Menu_EraseRect(u32, u32, u32, u32);
void TextEntry_DrawName(const void *src)
{
    u8 text[32];
    u8 *p;
    s32 i;
    CpuSet(src, text, 0x04000008);
    p = text;
    i = 4;
    do {
        if (!p[0] && !p[1]) {
            u8 *placeholder = Text_LookupString(
                Script_ParseStringRef((*(const void **)gNamePlaceholderScript)) + 0x54);
            p[0] = placeholder[0];
            p[1] = placeholder[1];
        }
        p += 2;
    } while (--i >= 0);
    Menu_EraseRect(18, 1, 10, 2);
    Menu_DrawText(18, 1, text);
}
