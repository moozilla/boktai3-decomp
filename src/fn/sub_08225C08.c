#include "global.h"
#include "heap.h"
void LZ77UnCompWram(const void *src, void *dst);
/* Returns the terrain (HP) resource: as is when it is stored raw ("HP" tag),
   otherwise LZ77-decompressed into the EWRAM buffer at 0x02030400 and the
   data past its 4-byte header. The return value is spelled as the original
   computes it (heap-relative arithmetic that agbcc does not fold). */
u8 *sub_08225C08(u8 *src)
{
    u8 *a, *b;
    if (src[0] == 'H' && src[1] == 'P')
        return src;
    a = ((u8 *)&gHeapHead) + 0x02020400;
    b = ((u8 *)&gHeapHead);
    LZ77UnCompWram(src, (void *)0x02030400);
    return (u8 *)(a - b + 0x10004);
}
