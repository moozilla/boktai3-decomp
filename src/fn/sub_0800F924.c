#include "global.h"
struct P {
    u8 pad[0x18];
    s32 w18;
    u32 w1c;
    u32 w20;
    s32 w24;
    u32 w28;
};
void sub_08177DEC(u32);
u32 Script_GetKeywordValue(u32, u32);
s32 Script_SeekToKeyword(s32);
u32 sub_08227E90(void);
s32 sub_08033690(u32, u32, u32, u32);
void TextWindow_SetScript(s32, u32);
void TextWindow_Refresh(s32, u32);
void sub_080337FC(s32);
s32 sub_0800F924(struct P *p)
{
    s32 r;
    sub_08177DEC(3);
    p->w1c = Script_GetKeywordValue(0x65, 0);
    p->w20 = 0;
    r = -1;
    p->w24 = r;
    if (Script_SeekToKeyword(0x73)) {
        p->w28 = sub_08227E90();
        if (p->w28 != 0) {
            p->w18 = sub_08033690(1, 0xd, 0x1c, 6);
            if (p->w18 >= 0) {
                TextWindow_SetScript(p->w18, p->w28);
                TextWindow_Refresh(p->w18, 0xb);
                sub_080337FC(p->w18);
                return 0;
            }
        }
    }
    return r;
}
