#include "global.h"
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
        TextWindow_Refresh(*handle, *(s16 *)((u8 *)entries + (*index << 1)));
        sub_080337FC(*handle);
        sub_08033468();
        ref = Script_ParseStringRef(*(u32 *)(s + 0x710));
        sub_0803343C(Text_LookupString(ref + *(s16 *)((u8 *)entries + (*index << 1))));
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
