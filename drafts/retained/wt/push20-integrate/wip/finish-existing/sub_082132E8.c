#include "global.h"
u32 sub_08212F50(s32);
void sub_0803386C(u32,u32);
void sub_08033924(u32,s32);
void sub_080337FC(u32);
u32 sub_082132E8(u8 *s)
{
    s16 *index=(s16 *)(s+0x6FC);
    s32 offset=*index*2;
    s16 *entries=(s16 *)(s+0x6E0);
    u32 *handle;
    u32 ref;
    if(sub_08212F50(*(s16 *)((u8 *)entries+offset))) {
        handle=(u32 *)(s+0x708);
        sub_0803386C(*handle,*(u32 *)(s+0x70C));
        sub_08033924(*handle,*(s16 *)((u32)entries+*index*2));
        sub_080337FC(*handle);
        sub_08033468();
        ref=Script_ParseStringRef(*(u32 *)(s+0x710));
        sub_0803343C(Text_LookupString(ref+*(s16 *)((u32)entries+*index*2)));
    } else {
        handle=(u32 *)(s+0x708);
        sub_0803386C(*handle,*(u32 *)(s+0x70C));
        sub_08033924(*handle,10);
        sub_080337FC(*handle);
        sub_08033468();
        ref=Script_ParseStringRef(*(u32 *)(s+0x714));
        offset=*index*2;
        sub_0803343C(Text_LookupString(ref+*(s16 *)((u32)entries+offset)));
    }
    return 0;
}
