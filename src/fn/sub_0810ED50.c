#include "global.h"
struct S {
    u8 filler[0x14E0];
    u32 g;
    u8 pad[9];
    u8 c;
    u8 pad2[2];
    u32 d;
    u32 e;
    u32 f;
};
u32 sub_08033690(s32, s32, s32, s32);
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, u32);
void TextWindow_StartPlayback(u32);
void Registry33FF_CreateTextWindows(struct S *s)
{
    s->f = sub_08033690(1, 3, 0x1c, 2);
    s->d = sub_08033690(1, 6, 0xa, 2);
    s->e = sub_08033690(0x13, 6, 0xa, 2);
    TextWindow_SetScript(s->f, s->g);
    TextWindow_Refresh(s->f, s->c);
    TextWindow_StartPlayback(s->f);
}
