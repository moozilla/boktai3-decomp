#include "global.h"

#define NameEntry_AppendChar sub_081DDA78

struct NameEntry {
    u8 pad0[0x348];
    s32 which;
    s32 base;
    s32 count;
    u8 pad354[0x374 - 0x354];
    u8 buf[1];
};

extern const void *gUnk_02000328[];
int Script_ParseStringRef(const void *);
u8 *Text_LookupString(int);
void TextEntry_DrawName(void *);

u32 NameEntry_AppendChar(struct NameEntry *p)
{
    u8 *dst = &p->buf[p->count * 2];
    u8 *src;
    u32 r;
    const void *ref;
    if (p->which == 0)
        ref = gUnk_02000328[0];
    else
        ref = gUnk_02000328[1];
    src = Text_LookupString(Script_ParseStringRef(ref) + p->base);
    dst[0] = src[0];
    dst[1] = src[1];
    if (p->count <= 3) {
        p->count++;
        r = 0;
    } else
        r = 1;
    TextEntry_DrawName(p->buf);
    return r;
}
