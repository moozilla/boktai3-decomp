#include "global.h"
struct Ref { u8 bytes[4]; u16 first, index; };
u32 Script_GetValue(void);
void sub_0821AAD8(struct Ref *);
void sub_0821B4C8(struct Ref *, s32, s32);
s32 sub_0821E7EC(s16 *, u32, u32);
s32 sub_0821E860(void)
{
    s16 vector[4];
    struct Ref ref;
    s16 *saved;
    u32 first = Script_GetValue();
    u32 second = Script_GetValue();
    saved = vector;
    if (sub_0821E7EC(vector, first, second) < 0) return -1;
    sub_0821AAD8(&ref); sub_0821B4C8(&ref, 0, vector[0]);
    sub_0821AAD8(&ref); sub_0821B4C8(&ref, 0, saved[1]);
    sub_0821AAD8(&ref); sub_0821B4C8(&ref, 0, saved[2]);
    return 0;
}
