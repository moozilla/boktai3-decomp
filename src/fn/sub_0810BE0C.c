#define SlotText_CopySelectedString sub_0810BE0C
#include "global.h"
struct SlotText {
    u8 unknown0[0x130];
    u8 mode;
    u8 unknown131[0x47];
    const void *stringRef;
};
void Memory_Clear(void *, u32);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
void sub_0810BE0C(u8 *dst, struct SlotText *p)
{
    u8 *text;
    s32 i;
    Memory_Clear(dst, 0x20);
    text = Text_LookupString(Script_ParseStringRef(p->stringRef) + p->mode);
    if (text) {
        i = 0;
        do {
            u8 ch = text[i];
            if (!ch)
                break;
            dst[i] = ch;
            i++;
        } while (i < 0x20);
        dst[i] = 0;
    }
}
