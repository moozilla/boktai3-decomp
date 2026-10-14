#include "global.h"
extern u8 *gUnk_030025FC;
struct IdTable {
    u8 pad[0x98];
    s32 ids[1];
};
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
s32 sub_0822B9E4(u8 *, s32);
/* Keyword 0x61: add each listed id that is not in the table yet (a negative
 * result is the free slot, as -1 - index). */
void sub_0822BA10(void)
{
    u8 *s = gUnk_030025FC;
    if (s && Script_SeekToKeyword(0x61)) {
        while (Script_GetPc()) {
            s32 id = Script_GetValue();
            s32 slot = sub_0822B9E4(s, id);
            if (slot < 0)
                ((struct IdTable *)s)->ids[-slot] = id;
        }
    }
}
