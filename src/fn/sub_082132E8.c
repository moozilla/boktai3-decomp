#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.8%
// 2 differing instruction runs; first diff at +0x3C (adds r1, r7, r1 vs adds r1, r1, r7)
u32 sub_08212F50(s32);
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, s32);
void sub_080337FC(u32);
u32 sub_082132E8(u8 *s)
{
    s16 *index = (s16 *)(s + 0x6FC);
    s32 offset = *index * 2;
    s16 *entries = (s16 *)(s + 0x6E0);
    u32 *handle;
    u32 ref;
    if (sub_08212F50(*(s16 *)((u8 *)entries + offset))) {
        handle = (u32 *)(s + 0x708);
        TextWindow_SetScript(*handle, *(u32 *)(s + 0x70C));
        TextWindow_Refresh(*handle, *(s16 *)((u32)entries + *index * 2));
        sub_080337FC(*handle);
        sub_08033468();
        ref = Script_ParseStringRef(*(u32 *)(s + 0x710));
        sub_0803343C(Text_LookupString(ref + *(s16 *)((u32)entries + *index * 2)));
    } else {
        handle = (u32 *)(s + 0x708);
        TextWindow_SetScript(*handle, *(u32 *)(s + 0x70C));
        TextWindow_Refresh(*handle, 10);
        sub_080337FC(*handle);
        sub_08033468();
        ref = Script_ParseStringRef(*(u32 *)(s + 0x714));
        offset = *index * 2;
        sub_0803343C(Text_LookupString(ref + *(s16 *)((u32)entries + offset)));
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_082132E8);
#endif
