#include "global.h"
extern const u32 gUnk_08605D68[8];
static inline s32 Divide128(s32 value)
{
    if (value >= 0)
        return value >> 7;
    return -((-value) >> 7);
}
static inline u8 TestFlags(u32 flags, u32 mask)
{
    if (flags & mask)
        return TRUE;
    return FALSE;
}
static inline u32 Read(const u32 *t, u32 i)
{
    return t[i];
}
u32 sub_08093094(u8 *p)
{
    u8 *state = *(u8 **)(p + 0x3D0);
    s32 sample;
    s32 index;
    sample = Divide128(*(u16 *)(state + 0x430));
    if (sample < 0)
        sample = -sample;
    index = sample;
    if (index > 7)
        index = 7;
    {
        u32 mask = 4;
        u32 flags = *(u32 *)(state + 0x3D4);
        if (TestFlags(flags, mask))
            return Read(gUnk_08605D68, index) + 6;
        return Read(gUnk_08605D68, index);
    }
}
