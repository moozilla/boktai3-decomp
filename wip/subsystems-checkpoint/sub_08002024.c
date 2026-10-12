#include "global.h"
#define Background_LoadMapFromScript sub_08002024
struct Resource { u32 header[3],maps,tiles,extra; };
s32 sub_0821ABA8(s32,s32);
s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
s32 sub_0821AA00(u8 *);
u8 *sub_08216E54(u16);
s32 sub_08001EB4(struct Resource *,u16);
void sub_082164AC(u16,void *,u16);
static inline u8 tryIndex(u32 *value) {
    if(Script_SeekToKeyword('i')) {u8 *p=Script_GetPc(); if(p){*value=(u16)sub_0821AA00(p);return TRUE;}}
    *value=0;return FALSE;
}
s32 Background_LoadMapFromScript(void)
{
    struct Resource resource;
    u16 layer=sub_0821ABA8('p',0);
    u32 index;
    u8 *base=sub_08216E54(sub_0821ABA8('d',0));
    resource=*(struct Resource *)base;
    resource.maps+=(u32)base;resource.tiles+=(u32)base;resource.extra+=(u32)base;
    if(!tryIndex(&index) && Script_SeekToKeyword('c')) {
        u8 *p=Script_GetPc();
        if(p) index=(u16)sub_08001EB4(&resource,sub_0821AA00(p));
    }
    sub_082164AC(layer,base,index);
    return index;
}
