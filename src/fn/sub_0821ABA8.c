#include "global.h"
u8 *Script_GetPc(void);
u32 Script_SeekToKeyword(u8);
u32 sub_0821AA00(u8 *);
u32 sub_0821ABA8(u8 k, u32 d)
{
    if (Script_SeekToKeyword(k) != 0) return sub_0821AA00(Script_GetPc());
    return d;
}
