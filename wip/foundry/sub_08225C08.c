#define Terrain_UnpackHP sub_08225C08
#include "global.h"
void LZ77UnCompWram(const u32 *, void *);
/* HP resources can be used directly; compressed resources use the shared WRAM buffer. */
u8 *sub_08225C08(u8 *src)
{
    if (src[0] == 'H' && src[1] == 'P')
        return src;
    LZ77UnCompWram((const u32 *)src, (void *)0x02030400);
    return (u8 *)0x02030404;
}
