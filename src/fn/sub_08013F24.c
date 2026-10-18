#include "global.h"
struct BF {
    u32 a : 16;
    u32 b : 16;
    u32 c : 16;
};
u32 Script_GetKeywordValue(u32, u32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 ShadowRegistry_SetEntryPosition(u32, struct BF *);
s32 ShadowRegistry_SetPositionFromScript(void)
{
    struct BF bf;
    u32 x = Script_GetKeywordValue(0x6e, 0);
    s32 r;
    if (Script_SeekToKeyword(0x70) == 0) {
        r = -1;
    } else {
        bf.a = Script_GetValue();
        bf.b = Script_GetValue();
        bf.c = Script_GetValue();
        r = ShadowRegistry_SetEntryPosition(x, &bf);
    }
    return r;
}
