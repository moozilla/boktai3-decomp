#include "global.h"
void Actor_RestoreSavedAnimationPlayback(u8 *p)
{
    u32 a = *(u8 *)(p + 0xF86);
    u32 b;
    *(u16 *)(p + 0xE2C) = a;
    b = *(u8 *)(p + 0xF87);
    *(u16 *)(p + 0xE32) = b;
}
